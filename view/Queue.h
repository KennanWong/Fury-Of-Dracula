#include <stdbool.h>
#include "Places.h"

typedef struct QueueRep *Queue;

Queue newQueue (void);			// create new empty queue
void dropQueue (Queue);			// free memory used by queue
void showQueue (Queue);			// display as 3 > 5 > 4 > ...
void QueueJoin (Queue, PlaceId);	// add item on queue
PlaceId QueueLeave (Queue);		// remove item from queue
int QueueIsEmpty (Queue);		// check for no items
PlaceId lastIdInQueue(Queue Q); // returns last id in queue
