#include <sys/time.h>
#include <ucontext.h>
ucontext_t mainContext;

struct myThread mainThread;
struct myThread *currentThread;

//long period_t;

struct itimerval timerVal;

int cancelCurrent = 0;
//int totalCount = 0;

struct queueStr readyQueue, finishQueue;


void startTime(){
	setitimer(ITIMER_REAL, &timerVal, 0);
}

void stopTime(){
	setitimer(ITIMER_REAL, 0, 0);
}

void queueInit(struct queueStr* queue){
	queue -> front = NULL;
       	queue -> rear = NULL;
	queue -> numOfNodes= 0;
}


int queueSize(struct queueStr *queue){
	return queue -> numOfNodes;
}

//We enqueue the given thread node into the list
int enqueueThreadQueue(struct queueStr *queue, struct myThread* thread){
	
	struct threadQueueNode* threadNode = (struct threadQueueNode *)malloc(sizeof(struct threadQueueNode));

	if(!threadNode){
		printf("Thread node was not created");
		return 0;
	}

	threadNode -> threadStr = thread;
	threadNode -> next = NULL;

	//if no node is present in ready queue 
	if(!queue -> rear){
		//printf("Insert in first\n");
		queue -> rear = threadNode;
		queue -> front = threadNode;
		
	}else{//otherwise add to end of list
		//printf("Insert in second\n");
		queue -> rear -> next = threadNode;
		queue -> rear = threadNode;
	}
	
	//increase number of nodes
	//queue -> numOfNodes += 1;
	//printf("Enqueued: %d\n", threadNode -> threadStr -> threadID);
	//printf("Number of nodes:%d\n", readyQueue.numOfNodes);
	return 1;
}


//Dequeue a node from the front
struct myThread* dequeueThreadQueue(struct queueStr* queue){
	if(queue -> front == NULL){
		printf("Cannot dequeue thread, Queue is Empty\n");
		if(getcontext(&mainContext) == -1){
			printf("Error switching to main");
		}
		return NULL;
	}
	
	struct threadQueueNode* threadNode = (struct threadQueueNode*)malloc((sizeof(struct threadQueueNode)));
	
	if(!queue -> front -> next){
		threadNode = queue -> front;
		queue -> front = NULL;
		queue -> rear = NULL;
	}else{
		threadNode = queue -> front;
		queue -> front = queue -> front -> next;
	}
	

	struct myThread* temp = threadNode -> threadStr;

	//queue -> numOfNodes -= 1;
	free(threadNode);
	//printf("Dequeued: %d\n", temp-> threadID);
	return temp;
}


//Search a thread with specific thread id
struct myThread* getQueueThread(struct queueStr* queue, int threadID){
	if(!queue -> rear){
//		printf("There are no nodes in queue");
		return NULL;
	} 

	struct threadQueueNode* temp = queue -> front;
	
	struct myThread* data = 0;

	while(temp){

		if(temp -> threadStr -> threadID  == threadID){
			data = temp -> threadStr;
			return data;
		}
		temp = temp -> next;
	}
	queue -> numOfNodes -= 1;
//	printf("The node with threadID %d was not found\n", threadID);
	return NULL;
}


//Remove any node that whos work is done
int removeQueueThread(struct queueStr* queue, struct myThread* thread){
	
	struct threadQueueNode* temp = queue -> front;
	struct threadQueueNode* tempPrev = NULL;

	if(temp != NULL && temp -> threadStr -> threadID == thread -> threadID){
		
		queue -> front = temp -> next;
		free(temp);
		return 1;
	}

	while(temp != NULL && temp -> threadStr -> threadID != thread -> threadID){
		tempPrev = temp;
		temp = temp -> next;
	}

	if(temp == NULL){
	//printf("The given node with thread ID : %d could not be found\n",thread-> threadID);
	//printf("Thread ID of current thread: %d\n", myThread_self().threadID);
		return 0;
	}
	
	//remove the node from the list
	if(temp -> threadStr -> threadID == thread -> threadID){ 
		//printf("Found\n");
		tempPrev -> next = temp -> next;
	}
	
	free(temp);
	//printf("Removed\n");
	return 1;
}

//Scheduler process
void scheduler(){
	//printf("No of nodes:%d\n", readyQueue.numOfNodes);
	//printf("scheduling: %d\n", currentThread -> threadID);
	struct myThread* prevThread = NULL;
	struct myThread* nextThread = NULL;
	if(readyQueue.numOfNodes > 0){
	//printf("SWITCH");
	if(getcontext(&mainContext) == -1){
		printf("There was an eeror getting the context");
		return;
	}
	
	prevThread = currentThread;
	//printf("aaa\n");
	if(!cancelCurrent){
		int succ = enqueueThreadQueue(&readyQueue, prevThread);
		//printf("cancelledCurrent\n");
		if(!succ) printf("The enqueue operation of thread was not successful");
		
	}
	
	if(cancelCurrent){
	 	readyQueue.numOfNodes -= 1;
	 	cancelCurrent = 0;
	 }
	//printf("bbbb\n");
	nextThread = dequeueThreadQueue(&readyQueue);
	//printf("The thread going to be dequed %d", nextThread -> threadID);
	//printf("Number of nodes: %d", readyQueue.numOfNodes);
	
	if(nextThread == NULL){
		printf("No thread was present to be dequeued");
		return;
	}

	currentThread = nextThread;

	startTime();

	if(swapcontext(&prevThread -> threadContext, &nextThread -> threadContext) == -1){
		printf("Swapping error\n");
	}
	}
	//printf("End of sch\n");

}
