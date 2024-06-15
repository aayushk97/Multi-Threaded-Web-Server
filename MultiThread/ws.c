
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myThread.c"
#include "connectionQueue.h"

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#define SERVERPORT 8888
#define BUFFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 3
#define MAX_CONNECTION 5

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void* handleConnection(void* clientSocket);
int getCGIResource(char* ,char*,char*, int*, char*);


int main(){
	
	myThreadInit(5000);
	InitialiseConQueue();
		
	
	int numOfOpenConnection = 0;
	
	int serverSocket, clientSocket, addrSize;
	SA_IN server_addr, client_addr;
	
	//Create a socket on server side
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	//Initialise the address struct
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVERPORT);
	
	//Set socket options to allow connection to be terminated in connected socket.
	int option = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,&option, sizeof option );
	
	//Bind socket to port
	bind(serverSocket, (SA*)&server_addr, sizeof(server_addr));

	//Listen on socket
	listen(serverSocket, SERVER_BACKLOG);

	
	
	while(1){
		//printf("Waiting for connection..\n");

		addrSize = sizeof(SA_IN);

		//accept the request
		clientSocket = accept(serverSocket, (SA*)&client_addr, (socklen_t*)&addrSize);
		
		//printf("connected.\n");
		//printf("SocketID %d", clientSocket);
		//printf("NumOFConnectedComp %d", connectedQueue -> numOfNodes );
		//pthread_t tid;
		//pthread_create(&tid, NULL, handleConnection, (void*)&clientSocket);
		if(connectedQueue -> numOfNodes < MAX_CONNECTION){
			if(clientSocket != -1 && (connectedQueue -> numOfNodes == 0 ||!connectedQueuePresent(clientSocket))){
				printf("printOne");
			//printf("clientSocket %d", clientSocket);
				struct myThread* thread = (struct myThread*)malloc(sizeof(struct myThread));
				myThread_create(thread, NULL, handleConnection, (void*)&clientSocket);
				printf("Back again\n");	
				connectionNodeEnqueue(clientSocket, thread);
			}else if(clientSocket != -1 && connectedQueuePresent(clientSocket)){
				printf("printTwo");
		
				struct myThread* thread = (struct myThread*)malloc(sizeof(struct myThread));
				myThread_create(thread, NULL, handleConnection, (void*)&clientSocket);
				printf("Back again\n");	
			}
	
		}else{
			if(clientSocket != -1 && !connectedQueuePresent(clientSocket)){
				 struct ConnectionNode* removed = connectionNodeDequeue();
	 			close(removed->socketID);
	 			myThread_cancel(removed->thread);		
					
			
			struct myThread* thread = (struct myThread*)malloc(sizeof(struct myThread));
			myThread_create(thread, NULL, handleConnection, (void*)&clientSocket);
			printf("Back again\n");	
			connectionNodeEnqueue(clientSocket, thread);
				
			}
		}
		
	}

}

				
int getResource(char* path, int* resourceLength, char* response){
	
	path++;
	printf("New path: %s\n" ,path);
	FILE* file;
	int fl = 0;
	
	if(strcmp(path, "") == 0){
		file = fopen("index.html", "r");
		
	 	//printf("FILE PATH CHANGED\n");
	 }else if(strchr(path, '?')){
	  	//file = fopen(path, "r");
	  	getCGIResource("GET", path, NULL,resourceLength,response);
	  	
	  	return 1;
	  }else{
	  	file =  fopen(path, "r");	
	  }
	 
	
	printf("PATH is: %s\n", path);
	
	if(!file){
		printf("file not opened.\n");
		//Add error 404
	 	return 0;
	 }
	
	//Get the resourceLength
	fseek(file, 0, SEEK_END);
	*resourceLength = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	int readNum = fread(response,1, *resourceLength, file);
	
	//printf("RESPOSNE: %s\n read: %d", response, readNum);
	
	
	
	fclose(file);
	return 1;
}

int createERRORResponse(char* response, int error){

	//Sprintf will return the string according to formatted input.
	sprintf(response, "<html><Body>Error Code : %d </body></html>", error);
	return 1;

}

int getCGIResource(char* method, char* path, char* requestBuffer, int* resourceLength, char* response){
	*resourceLength = 1024;
	if(path[0] == '/') path++;

	FILE* file;
	
	//PHP Query: php-cgi PHP/signIn.php name=aa email=aa%40gmail.com
	
	char name[512];
	char email[512];
	char pathTwo[2048];
	
	if(strcmp(method, "GET")==0){ 
		
		char php_query[4096] = "php-cgi ";
		printf("HERE!%s\n", path);
	
		int i = 0;
		for( i = 0; i < strlen(path); i++){
			if(path[i] == '?') path[i] = ' ';
			if(path[i] == '&') path[i] = ' ';
		}
	
		printf("PATH %s\n", path);
	
		sscanf(path, "%s %s %s ",pathTwo, name, email);
	
		//break path into tokens 
		strcat(php_query, pathTwo);
		strcat(php_query, " ");
		printf("%s %s\n", pathTwo, path);
		//printf("%s and %s ", name, email);
		
		/*for(i = 0; i < strlen(name); i++){
			if(name[i] == '+') name[i] = ' ';
		}*/
		strcat(php_query, name);
		strcat(php_query, " ");
		strcat(php_query, email);
		
		printf("%s\n",php_query);
		
		file = popen(php_query, "r");
		
		if(file == NULL){
			createERRORResponse(response, 404);
			return 0;
		}
		
		size_t len = 0;
		int ignoreLines = 2;
		char* line_content;
		
		while(getline(&line_content, &len, file)!= -1){
			if(ignoreLines-- > 0) continue;
			
			strcat(response, line_content);
		}
		return 0;
	}else{
		char php_query[4096] = "php ";
		char temp[1024];
		int i;
		
		int n = strlen(requestBuffer);
		
		for(i = 0; i < n; i++){
			if(requestBuffer[i] == '\r'){
				if(i+1<n && requestBuffer[i+1] == '\n')
					if(i+2<n && requestBuffer[i+2] == '\r')
						if(i+3<n && requestBuffer[i+3] == '\n')break;
			}
			
	//		printf("%d %c\n",i, requestBuffer[i]);
		
		}
		int j = 0;
		printf("\n%d / %d\n",i,n);
		i = i+4;
		while(i < n){
			pathTwo[j] = requestBuffer[i];
			j++; i++;
		}
		//sscanf(requestBuffer,"%s \n \n%s",temp, pathTwo);
		printf("%s\n", pathTwo);
		
		strcat(php_query, path);
		strcat(php_query, " '");
		strcat(php_query, pathTwo);
		strcat(php_query, "' ");
		
		printf("%s \n", php_query);
		
		file = popen(php_query, "r");
		
		/*for(i = 0; i < strlen(pathTwo); i++){
			if(pathTwo[i] == '&') pathTwo[i] = ' ';
		
		}
		sscanf(pathTwo, "%s %s", name, email);
		strcat(php_query, path);
		strcat(php_query, " ");
		printf("%s and %s\n", name, email);
		*/
		
		
		if(file == NULL){
			
			createERRORResponse(response, 404);
			return 0;
		}
		
		size_t len = 0;
		int ignoreLines = 2;
		char* line_content;
		
		while(getline(&line_content, &len, file)!= -1){
			if(ignoreLines-- > 0) continue;
			
			strcat(response, line_content);
		}
	}
	
	
	return 0;
	

}


void createResponse(char* ResourceType, char* path, char* response){
	char fileType[512];
	//extract the file type from the path
	if(strcmp(path, "/") == 0){
		strcpy(ResourceType, "text/html");
		
	}else{
		int i = 0;
		while(i < strlen(path) && path[i] != '.') i++;
		if(path[i] == '.'){
			int j = 0;
			while(j < strlen(path)){
				fileType[j] = path[i];
				j++; i++;
			}
		}
		printf("Filetype: %s\n", fileType);
		if(strcmp(fileType, ".jpg") == 0)strcpy(ResourceType, "image/jpg"); 
		else if(strcmp(fileType, ".css")==0)strcpy(ResourceType, "text/css"); 
		else strcpy(ResourceType, "text/html");
	
	}
	
	  
}

int respond(char* resourceType, int* resourceLength, int clientSocket, char* response){
	int status = 200;
	char http_status_message[5] = "OK"; 
	//Get the resourceLength
	
	int bodyLength = *resourceLength;
	char http_header[8000];
	
	memset(http_header, 0, sizeof(http_header));
	
	sprintf(http_header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\nServer: Webserver C\r\n\r\n",resourceType,bodyLength); 
	int headerLength = strlen(http_header);
	
	//printf("Header: %s\n", http_header);
	//printf("Header length: %d\n", headerLength);
	
	strcat(http_header, response);
	
	//free(response);
	//strcat(http_header, "\r\n");
	
	int totalLength = headerLength+bodyLength;
	
	
	int numSent = send(clientSocket, http_header,totalLength, 0);
//	send(clientSocket, "\r\n",sizeof("\r\n"),0);
	printf("sent %d\n", numSent);

	return 1;
}

void closeConnection(int socketID){
	close(socketID);
	removeConnectionNode(socketID);
}
void* handleConnection(void* p_clientSocket){
	printf("Handle Connection..");
	int clientSocket = *(int *)p_clientSocket;
	
	char requestBuffer[1024];
	size_t bytesRead;
	int msgSize = 0;
	int recvLength;
	//Read the clients request
	while(1){
	while(recvLength = recv(clientSocket, requestBuffer, sizeof(requestBuffer), 0) <=0);
	//if(recvLength > 0){
	//printf("Non existent size of buffer: %d\n", recvLength);
	//Print out the request
	printf("Request: %s %d\n", requestBuffer, clientSocket);
	fflush(stdout);
	
		
	//extract out info from request
	char method[10];
	char path[2048];
	char protocol[10];
	char resourceType[4];
	int resourceLength;
	
	sscanf(requestBuffer, "%s %s HTTP/%s", method, path, protocol);
	
	//printf("request Buffer param%s %s %s", method, path, protocol);
	int error = 0;
	//check if the protocol parameter is correct
	if(strcmp(protocol, "1.1") != 0 && strcmp(protocol, "1.0") != 0){
	 	error = 400 ;
	 	
	 }
	
	//check if the Method requested is correct
	if(strcmp(method, "GET") != 0 && strcmp(method, "POST") != 0 && strcmp(method, "HEAD") != 0){
	 error = 400;
	
	 }
	
	
	char response[8000];
	
	int status;
	if(strcmp(method, "GET") == 0 && error == 0){
	//	printf("request Buffer param%s %s %s", method, path, protocol);
		status = getResource(path, &resourceLength, response);
		createResponse(resourceType, path, response);
			
	}else if(strcmp(method, "POST") == 0 && error == 0){
		
		status = getCGIResource(method, path, requestBuffer, &resourceLength, response);
		createResponse(resourceType, path, response);			
		
	}
	
	//send the error encountered
	if(error != 0) createERRORResponse(response, error);
	
	printf("creating response\n");
	if(respond(resourceType, &resourceLength, clientSocket, response)) printf("back?\n");
	printf("After Sending");
	memset(response, 0, 8000);
	memset(path,0,1024);
	
	
	printf("Here");
	if(error != 0 ){
		//Close the connection and remove the client from queue
		closeConnection(clientSocket);
	}else{
		if(strcmp(protocol, "1.0") == 0){
			printf("closing 1.0");
			closeConnection(clientSocket);
		}else if(strcmp(protocol, "1.1") == 0){
			printf("closing 1.1\n");
			//closeConnection(clientSocket);
		
			//int temp;
			//myThread_exit(&temp);
		}
		}
		printf("HandleEnd %d \n", connectedQueue->numOfNodes);
	}
	
	printf("HandleEnd %d \n", connectedQueue->numOfNodes);
return 0;
}




