#makefile for signal.c

CC = gcc
CFLAGS = 
OBJ = func

all:
	$(CC) $(CFLAGS) signal.c -o $(OBJ)
	echo "make complete"

exec:
	./func
	
clean:
	rm -rf func
	echo "clean complete"