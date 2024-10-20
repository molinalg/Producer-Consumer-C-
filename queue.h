#ifndef HEADER_FILE
#define HEADER_FILE


struct object {
	//Type of the machine
	int type; 
	//Time using the machine
	int minutes; 
};

typedef struct queue {
	// Define the struct yourself
	int front;
	int back;
	int size;
	int length;
	struct object *values;
	
}queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct object* elem);
struct object * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);

#endif
