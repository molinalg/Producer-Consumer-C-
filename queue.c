
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"



//To create a queue
queue* queue_init(int size){

	queue * q = (queue *)malloc(sizeof(queue));
	
	// We set the initial values for the parameters
	q -> front = 0;
	q -> back = 0;
	q -> size = size;
	q -> length = 0;
	int space = size * sizeof(struct object);
	q -> values = malloc(space);
	return q;
}


// To Enqueue an element
int queue_put(queue *q, struct object* x) {
	
	int current = q -> front;
	int size = q -> size;
	int length = q -> length;
	int next;

	if (length > size - 1) {
		printf("The queue cant hold any more values");
		return -1;
	}

	else if (current + 1 > size - 1) {
		next = 0;
	}
	
	else {
		next = current + 1;
	}
	
	q -> length = length + 1;
	q -> values[current] = *x;
	q -> front = next;
	
	return 0;
}


// To Dequeue an element.
struct object* queue_get(queue *q) {
	struct object* element;
	
	if (queue_empty(q) == 0) {
		printf("The list is empty, you can't take any data from it");
		return -1;
	}
	
	else {
		int back = q -> back;
		element = &(q -> values[back]);
		
		//Change the position of the back of the queue
		int size = q -> size;
		if (back == size - 1) {
			q -> back = 0;
		}
		
		else {
			q -> back = back + 1;
		}
		
		int length = q -> length;
		q -> length = length  - 1;
	}
	
	return element;
}


//To check queue state
int queue_empty(queue *q){
	int length = q -> length;
	if (length != 0) {
		return -1;
	}
	return 0;
}

int queue_full(queue *q){
	int length = q -> length;
	int size = q -> size;
	if (length != size) {
		return -1;
	}
	return 0;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
	//We free every memory space we occupied with each malloc
	free(q -> values);
	free(q);
	return 0;
}
