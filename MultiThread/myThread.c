#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "myThread.h"
#include "RRScheduler.c"
#define STACKSIZE 320000
#include <string.h>

struct itimerval timerVal;
struct sigaction sa;
int threadIDCount = 0;

//Function so that we don't have to write the exit in every thread function
void myThread_Run(void*(*start_routine)(void*), void* arg){
	myThread_exit(start_routine(arg));
	//printf("DoneTwo\n");
}

int myThread_create(struct myThread* thread, struct myThread_attr *attr, void *(*start_func)(void *), void *arg){
	
	
	if(attr == NULL){
		attr = (struct myThread_attr *)malloc(sizeof(struct myThread_attr));
		myThread_attr_init(attr);
	}
	
	thread -> threadID = threadIDCount++;

	if(getcontext(&thread -> threadContext) == -1){
		printf("getcontext");
		exit(EXIT_FAILURE);
	}
	
	//printf("Thread created!");
	
	thread -> threadContext.uc_stack.ss_sp = malloc(attr->stackSize);
	thread -> threadContext.uc_stack.ss_size =attr -> stackSize;
	thread -> threadContext.uc_stack.ss_flags = 0;
	thread -> threadContext.uc_link = &mainContext;

	//calls the threadRun from the context when switched	
	makecontext(&thread -> threadContext,(void(*)()) myThread_run ,2,start_func, arg);
	
	enqueueThreadQueue(&readyQueue, thread);
	readyQueue.numOfNodes += 1;
	return 0;
}

void myThreadInit(long period){
	
	queueInit(&readyQueue);
	queueInit(&finishQueue);
	
	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = scheduler;
        sa.sa_flags = SA_SIGINFO;	

	sigaction(SIGALRM, &sa, NULL);

	timerVal.it_value.tv_sec = 0;
	timerVal.it_value.tv_usec = 50000;
	timerVal.it_interval.tv_sec = 0;
        timerVal.it_interval.tv_usec = 50000;

	startTime();

	

	mainThread.threadID = -1;

	if(getcontext(&mainThread.threadContext) == -1){
		printf("error");
		exit(EXIT_FAILURE);
	}
	
	//Set the main as Current Thread
	currentThread = &mainThread;
	
}


void myThread_exit(void *retVal){
	currentThread -> retVal = retVal;
	enqueueThreadQueue(&finishQueue, currentThread);
//	finishQueue -> count += 1;
//	printf("enqueued on exit\n");
	struct myThread ret = myThread_self();
	myThread_cancel(&ret);
	
//	printf("DoneTwo\n");
}


//cancels the given thread 
int myThread_cancel(struct myThread* thread){
	//If its the current thread we need to cancel
	if(thread -> threadID == myThread_self().threadID){
		//printf("CancelledTwo\n");
		
		cancelCurrent = 1;
		scheduler();
		//readyQueue.numOfNodes -= 1;
		return 1;
	}

	if(removeQueueThread(&readyQueue, thread)){
		readyQueue.numOfNodes -= 1;
		//printf("CancelledOne\n");
		return 1;
	}	
	
	
	return 0;
}


int myThread_attr_init(struct myThread_attr* attr){
	attr -> stackSize = STACKSIZE;
	return 1;
}


int myThread_attr_destroy(struct myThread_attr *attr){
	attr -> stackSize = -1;
	return 1;
}

struct myThread myThread_self(void){
	return *currentThread;
}

void myThread_run(void*(*start_routine)(void*), void* arg){
	myThread_exit(start_routine(arg));
	return;

}

//Similar to pThread_yield causes the calling thread to relinquish CPU and place itself at back of queue
int myThread_yield(void){
	//Reset timer
	stopTime();
	startTime();
	//Will push it to the end of queue
	scheduler();
	return 0;
}





int myThread_join(struct myThread targetThread, void** retVal){
	
	struct myThread* thr = 0;

	//looping to check if thread is finished
		
	while(1){
		
		thr = getQueueThread(&finishQueue, targetThread.threadID);
		if(thr==NULL && !removeQueueThread(&finishQueue,&targetThread)){
			//Do nothing
		}else{
			//int* p = thr-> retVal;
			//printf("value in:%d",*p);
			if(retVal && *retVal) *retVal = thr -> retVal;
			return 1;
		}
			
		
		myThread_yield();
	
	}
}

int myThread_mutex_init (struct myThread_mutex *mutex) {
	mutex->lock = 0; 
	return 0;
}

int myThread_mutex_lock (struct myThread_mutex *mutex) {
	stopTime();
	while(mutex -> lock == 1){
		startTime();
		myThread_yield();
		stopTime();
	}
	mutex -> lock = 1;
	//while(!__sync_bool_compare_and_swap(&mutex->lock, 0, 1));
	//Try using inbuilt functions if time remains
	startTime();
	return 0;
}

int myThread_mutex_unlock(struct myThread_mutex *mutex) {
	mutex -> lock = 0;
	/*if(!__sync_bool_compare_and_swap(&mutex-> lock, 1, 0)){
		printf("The lock was unclocked without being locked");
	}*/
	return 0;
}
