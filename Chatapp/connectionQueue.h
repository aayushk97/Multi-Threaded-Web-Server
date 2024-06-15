

struct ConnectionNode{
	int socketID;
	struct myThread* thread;
	struct ConnectionNode* next;
};
struct ConnectionQueue *connectedQueue;
struct ConnectionQueue{
	struct ConnectionNode* front;
        struct ConnectionNode* rear;
	int numOfNodes;	
};

void InitialiseConQueue();
void connectionNodeEnqueue(int, struct myThread*);
struct ConnectionNode* connectionNodeDequeue();
void removeConnectionNode(int);
int connectedQueuePresent(int);

