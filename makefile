CC=gcc

all: main.o functions.o
		$(CC) main.o functions.o -Wall -o pipeline

functions.o: functions.c functions.h
		$(CC) functions.c -Wall -c -o functions.o

main.o: main.c
		$(CC) main.c -Wall -c -o main.o

clean:
	rm pipeline *.o output*.bmp
