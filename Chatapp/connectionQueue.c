#include <stdio.h>
#include <stdlib.h>

#include "connectionQueue.h"

//ConnectionNode -> nodes of queue and ConnectionQueue front, rear and num of nodes
struct ConnectionQueue *connectedQueue;

void InitialiseConQueue(){
	connectedQueue = (struct ConnectionQueue*)malloc(sizeof(struct ConnectionQueue));
	connectedQueue -> front = NULL;
	connectedQueue -> rear = NULL;
	connectedQueue -> numOfNodes = 0;

}

void connectionNodeEnqueue(int socketID, struct myThread* thread){
	struct ConnectionNode* newCon = (struct ConnectionNode*)malloc(sizeof(struct ConnectionNode));
	if(!newCon){
		printf("New connection Node was not created!");
	}

	newCon -> socketID = socketID;
	newCon -> thread = thread;
	newCon -> next = NULL;
	
	//No node is present
	if(!connectedQueue -> rear){
		//printf("aa");
		connectedQueue -> rear = newCon;
		connectedQueue -> front = newCon;
	}else{
		//printf("bb");
		//else we add it to end of queue
		connectedQueue -> rear -> next = newCon;
		connectedQueue -> rear = newCon;
	}
	printf("Enqueued. Num of nodes: %d\n", connectedQueue -> numOfNodes);

	//Increase number of connections
	connectedQueue -> numOfNodes += 1;	
}

int connectedQueuePresent(int socketID){
	struct ConnectionNode* newCon = connectedQueue -> front;
	while(newCon){
		if(newCon -> socketID == socketID){
		 	printf("found!");	
		 	return 1;
		 }
		newCon = newCon -> next;
	}
	return 0;
}
struct ConnectionNode* connectionNodeDequeue(){
	if(!connectedQueue -> rear){
		printf("No current connected connections to be dequeued");
		return NULL;
	}
	
	struct ConnectionNode* temp = (struct ConnectionNode*)malloc(sizeof(struct ConnectionNode*));
	temp = connectedQueue -> front;
	//int socketID = connectedQueue -> front -> socketID;
	connectedQueue -> front = connectedQueue -> front -> next;

	connectedQueue -> numOfNodes -= 1;
	
	printf("Dequeued. No of nodes: %d", connectedQueue -> numOfNodes);
	return temp;
}


void removeConnectionNode(int socketID){
	struct ConnectionNode* temp = connectedQueue -> front;
	struct ConnectionNode* tempPrev = NULL;

	if(temp != NULL && temp -> socketID == socketID){
		connectedQueue -> front = temp -> next;
		free(temp);
		return ;
	}

	while(temp != NULL && temp -> socketID != socketID){
		tempPrev = temp;
		temp = temp -> next;
	}

	if(temp == NULL){
		printf("The connection Thread with given socket id was not found");
		return ;
	}

	if(temp -> socketID == socketID){
		tempPrev = temp -> next;
	}
	
	connectedQueue -> numOfNodes -= 1;
	free(temp);
}


