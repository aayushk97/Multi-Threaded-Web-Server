#include <ucontext.h>
struct myThread_attr{
	int stackSize;
	void* stackbase;
};

struct myThread{
	int threadID; 			//Thread Identification Number
	ucontext_t threadContext;	//Context of this thread
	struct myThread_attr attr;      //Stack attributes of thread
	void* retVal;			//return Value of function called by this
};

//Structure of each node of the Thread queue
struct threadQueueNode{
	struct myThread* threadStr;
	struct threadQueueNode* next;

};

//stucture to keep track of front and rear pointers alog with count
struct queueStr{
	struct threadQueueNode* front;
	struct threadQueueNode* rear;
	int numOfNodes;
};


struct myThread_mutex{
	int lock; 
};


//functions for thready library
int myThread_create(struct myThread*, struct myThread_attr*, void *(*)(void*), void*);

void myThread_exit(void*);

int myThread_cancel(struct myThread*);

int myThread_attr_init(struct myThread_attr*);

int myThread_attr_destroy(struct myThread_attr*);

struct myThread myThread_self(void);

int myThread_yield(void);

int myThread_join(struct myThread targetThread, void** retVal);

void myThreadInit(long);

void myThread_run(void*(*start_routine)(void*), void* arg);

//functions for thread queue, to implement Round Robin Scheduling

void queueInit(struct queueStr*);

int queueSize(struct queueStr*);

int enqueueThreadNode(struct queueStr*, struct myThread*);

struct myThread* dequeueThreadQueue(struct queueStr*);

struct myThread* getQueueThread(struct queueStr*, int);

int removeQueueThread(struct queueStr*, struct myThread*);


//functions to initialize and implement mutex
int myThread_mutex_init(struct myThread_mutex*);

int myThread_mutex_lock(struct myThread_mutex*);

int myThread_mutex_unlock(struct myThread_mutex*);


//functions for timer of Round Robin
void startTime();

void stopTime();

void scheduler();




