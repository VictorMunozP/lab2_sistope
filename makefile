CC=gcc

all: main.o functions.o imageReader.o imageWriter.o
		$(CC) main.o functions.o imageReader.o imageWriter.o -Wall -o pipeline
		rm *.o

functions.o: functions.c functions.h
		$(CC) functions.c -Wall -c -o functions.o

imageReader.o: imageReader.c functions.h
		$(CC) imageReader.c -Wall -c -o imageReader.o

imageWriter.o: imageWriter.c functions.h
		$(CC) imageWriter.c -Wall -c -o imageWriter.o

main.o: main.c
		$(CC) main.c -Wall -c -o main.o

clean:
	rm pipeline  output*.bmp
