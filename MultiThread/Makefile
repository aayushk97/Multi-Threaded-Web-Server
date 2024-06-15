all:  runc
	./ws
	

runc:
	gcc -c connectionQueue.c -o connectionQueue.o
	ar -rcs libconnectionQueue.a -o connectionQueue.o
	gcc ws.c -pthread -lconnectionQueue -L. -o ws

clean:	
	rm connectionQueue.o ws


