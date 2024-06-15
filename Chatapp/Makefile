all: runc
	./cs
	

runc:
	gcc -c connectionQueue.c -o connectionQueue.o
	ar -rcs libconnectionQueue.a -o connectionQueue.o
	gcc chatS.c -pthread -lconnectionQueue -L. -o cs

clean:	
	rm connectionQueue.o ws


