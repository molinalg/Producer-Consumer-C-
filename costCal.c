
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */

//Variables for the mutex and condition variables
pthread_mutex_t mutex;
pthread_cond_t notEmpty;
pthread_cond_t withSpace;

//Queue
struct queue *circQueue;

//Structure for the producers
struct set {
	int initialID;
	int operations;
};

//Arrays to store the data
int *types;
int *times;

//The variable where the result will be stored
int total;

//Method executed by producer threads
void *producer(struct set *group){
	//This function will need to create an object structure to group all the information of each operation as a group
	struct object current;
	
	//We create a loop to insert each operation
	for (int i = (group -> initialID)-1; i < ((group -> initialID)+(group -> operations))-1 ; i++) {
		//We introduce the data into the structure
		current.type = types[i];
		current.minutes = times[i];

		//Check if we are free to insert
		if (pthread_mutex_lock(&mutex) < 0){
			perror("Error when locking mutex");
			exit(-1); 
		}  
		//Make sure the queue is not full
		while (queue_full(circQueue) == 0){
			if (pthread_cond_wait(&withSpace,&mutex) < 0) {
				perror("Error (the queue is full condition variable)");
				exit(-1);
			}
		}	 
		
		//Introducing the data in the queue
		int producerControl;
		producerControl = queue_put(circQueue, &current);
		if (producerControl < 0) {
			perror("Error when introducing the data in the queue");
			exit(-1);
		}
		
		//We activate the condition variable that shows the queue is not empty
		producerControl = pthread_cond_signal(&notEmpty);
		if (producerControl < 0) {
			perror("Error (the queue is not empty condition variable)");
			exit(-1);
		}
		
		//Unlocking mutex
		producerControl = pthread_mutex_unlock(&mutex);
		if (producerControl < 0) {
			perror("Error unlocking mutex");
			exit(-1);
		}
	}
	pthread_exit(0);
} 

//Method executed by consumer threads
void *consumer(int *num){
	//Again we need a structure object to group the parameters
	struct object current;
	
	//For loop to go through every operation
	for (int i = 0; i < *num; i++) {
		
		//We lock mutex to avoid mixing	
		if (pthread_mutex_lock(&mutex) < 0) {
			perror("Error when locking mutex");
			exit(-1);
		}

		//The program must wait for the queue not to be empty so that it can "consume"
		while (queue_empty(circQueue) == 0) {
			if (pthread_cond_wait(&notEmpty, &mutex) < 0) {
				perror("Error when waiting for not empty queue");
				exit(-1);
			}
		}
		
		//Once it is not empty we obtain the first element in the queue
		struct object *current = queue_get(circQueue);

		//Variable to store the cost per minute
		int cost;
		
		//There can only be 3 types of "types" so we print an error if it is not 1, 2 or 3
		if (current -> type != 1 && current -> type != 2 && current -> type != 3) {			
			//printf("The type of a machine is not valid: type %i \n, time %i",current->type,current->minutes);
			cost = 0;
		} else if (current -> type == 1) {
			cost = 3;
		} else if (current -> type == 2) {
			cost = 6;
		} else if (current -> type == 3) {
			cost = 15;
		}
		total = total + ((current -> minutes)*cost);
		
		//We state that the queue is not not full (in case it was before) and unlock the mutex
		int consumerControl;
		
		consumerControl = pthread_cond_signal(&withSpace);
		if (consumerControl < 0) {
			perror("Error (the queue is not full condition variable)");
			exit(-1);
		}
		
		consumerControl = pthread_mutex_unlock(&mutex);
		if (consumerControl < 0) {
			perror("Error unlocking mutex");
			exit(-1);
		}
	}	
	pthread_exit(0);
} 
 

int main (int argc, const char *argv[] ) {
	//We initialize the mutex and the condition variables
	int controlMutex;
	controlMutex = pthread_mutex_init(&mutex, NULL);
	if (controlMutex < 0){
		perror("Error when initializing mutex");
    	exit(-1);
	}
	
	//Condition variable to check there is space in the queue
	int controlWithspace;
	controlWithspace = pthread_cond_init(&withSpace, NULL);
	if (controlWithspace < 0){
		perror("Error when initializing condition variable withSpace");
    		exit(-1);
	}
	
	//Condition variable to check if the queue is not empty
	int controlFull;
	controlFull = pthread_cond_init(&notEmpty, NULL);	
	if (controlFull < 0) {
		perror("Error when initializing condition variable notEmpty");
		exit(-1);
	}
		
	//Checking if the number of arguments is ok
	if (argc != 5) {
    		perror("Not enough or too many parameters");
    	exit(-1);
  	}
  	
  	//Checking if the file_name argument is ok
  	FILE *fd = fopen(argv[1],"r");
  	if (fd == NULL){
  		perror("The name of the file is not correct");
  		exit(-1);
	  }
	  
	//Checking if the number of operations parameter is ok
	//First take the supposed number. This is located in the first line of the file
	int numOper;
	int control;
	control = fscanf(fd, "%d", &numOper);
	if (control < 0){
    		printf("Error when finding the number of operations in the file");
    		exit(-1);
    }
    
    //We count the number of lines (excluding the first one) and check if it is the same or bigger
    int numLines = 0;
    int element;
	
    while (!feof(fd)) {
    	element = fgetc(fd);
    	//We go through each character in the file and add 1 to the number of lines when the character is the one used to jump to another line
   		if (element == '\n') {
    		numLines = numLines + 1;
    	}
  	}

  	if (numLines < numOper){
  		perror("The parameter for the number of operations is wrong");
  		exit(-1);
	  }
	
	//Now we check if the number of consumers or the number of producers given is wrong
	int numProd;
	int numCons;
	
	numProd = atoi(argv[2]);
	numCons = atoi(argv[3]);
	
	if (numProd <= 0){
		perror("The number of producers is not valid");
		exit(-1);
	} 
	
    if (numCons <= 0){
		perror("The number of consumers is not valid");
		exit(-1);
	} 
	
	//Finally we check the argument for the size of the buffer is ok
	int maxElem;
	maxElem = atoi(argv[4]);
	
	if (maxElem <= 0){
		perror("The queue size is too small");
		exit(-1);
	}
	
	//Going back to the beginning of the file and read the first line (we don't check if it works as we did it before and assume it will not change)
	rewind(fd);
	//Read the first line again to start in the first operation
	fscanf(fd, "%d", &numOper);

	//We reserve space for the information in the file
	int space;
	space = numOper * 4;
	types = (int *)malloc(space);
	times = (int *)malloc(space);
	
	//The information will be initially in the following variables:
	int iD;
	int ty;
	int ti;
		
	//Beginning in the second line, we start taking the information in the file (in each there are 3 numbers)
	for (int i=0; i<numOper; i++) {
		
		int resultControl;
		resultControl = fscanf(fd, "%d %d %d", &iD, &ty, &ti);
		
		if (resultControl < 0){
      			perror("Error when reading the information in the file");
      			exit(-1);
    		}
    	
    	//We only need the type and the time
		types[i]=ty;
		times[i]=ti;
	}

	//Creating the queue
	circQueue = queue_init(maxElem);

	//If we have less operations than producers, we will use the same producers as operations	
	if (numProd > numOper) {
		numProd = numOper;
	}
	
	//We calculate the operations each producer will do	
	int operEach;
	operEach = floor((numOper / numProd));
	
	//We check if the operEach is not the same for all
	int remaining;
	remaining = numOper - (operEach * numProd);
	
	//If we have less operations than consumers, we will use the same consumers as operations
	if (numCons > numOper) {
		numCons = numOper;
	}
	
	//We calculate the operations that each consumer takes
	int eachConsumer;
	eachConsumer = floor((numOper / numCons));

	//We check if the eachCons is not the same for all
	int remainingC;
	remainingC = numOper - (eachConsumer * numCons);

	//Creation of the consumer threads
	pthread_t consumers[numCons];	
	int controling;
	
	for (int j = 0;j < numCons - 1;j++) {
		controling = pthread_create(&consumers[j],NULL,(void *)consumer,&eachConsumer);	
		if (controling < 0){
			perror("Error when creating consumer thread");
			exit(-1);
		} 	
}

	//If there where remaining operations, we assign the remaining to the last thread
	int lastConsOper;
	lastConsOper = eachConsumer + remainingC;
		controling = pthread_create(&consumers[numCons - 1],NULL,(void *)consumer,&lastConsOper);
	if (controling < 0){
			perror("Error when creating consumer thread");
			exit(-1);
	} 			
	
	//Creation of the producer threads
	struct set group[numProd];
	pthread_t producers[numProd];
	int initID;
	initID = 1;	
	
	for (int i = 0;i < numProd - 1;i++) {
			group[i].initialID = initID;
			group[i].operations = operEach;
			initID = initID + operEach;
			
		controling = pthread_create(&producers[i],NULL,(void *)producer,&group[i]);
		if (controling < 0){
			perror("Error when creating producer thread");
			exit(-1);
		} 	
	}

	//If there where remaining operations, we asign the remaining to the last thread
	group[numProd - 1].initialID = initID;
	group[numProd - 1].operations = operEach + remaining;

	controling = pthread_create(&producers[numProd - 1],NULL,(void *)producer,&group[numProd - 1]);	
	if (controling < 0){
			perror("Error when creating producer thread");
			exit(-1);
	} 
	
	//Waiting for producer threads
	for (int i = 0; i < numProd; i++) {
    		if(pthread_join(producers[i], NULL) < 0){
      		perror("Error when waiting for a producer thread");
      		exit(-1);
    		}
  	}
  	
  	//Waiting for consumer threads
  	for (int j = 0; j < numCons; j++) {
    		if(pthread_join(consumers[j], NULL) < 0){
      		perror("Error when waiting for a consumer thread");
      		exit(-1);
    		}
  	}
	
  	//We print the result
  	printf("Total: %i euros. \n", total);

  	//Destroying the queue, mutex and condition variables
  	queue_destroy(circQueue);
  	
  	int controlDestroy;
  	controlDestroy = pthread_mutex_destroy(&mutex);
  	
  	if (controlDestroy < 0) {
  		perror("Error when destroying mutex");
  		exit(-1);
	}
	
  	controlDestroy = pthread_cond_destroy(&withSpace);
  	if (controlDestroy < 0) {
  		perror("Error when destroying condition variable withSpace");
  		exit(-1);
	}
	
	controlDestroy = pthread_cond_destroy(&notEmpty);
  	if (controlDestroy < 0) {
  		perror("Error when destroying condition variable notEmpty");
  		exit(-1);
	}
	
	//Closing the descriptor opened at the beggining 
	int controlClose;
	controlClose = fclose(fd);
	
	//Freeing the space reserved for the data
	free(types);
	free(times);	

	if (controlClose < 0){
		perror("Error when closing file descriptor");
		exit(-1);
	} 
    return 0;
}
