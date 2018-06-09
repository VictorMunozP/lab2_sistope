CC=gcc

all: main.o functions.o imageReader.o imageToGrayScale.o imageBinarizer.o nearlyBlack.o imageWriter.o
		$(CC) main.o functions.o -Wall -o pipeline
		$(CC) imageReader.o functions.o -Wall -o imageReader
		$(CC) imageWriter.o functions.o -Wall -o imageWriter
		$(CC) imageToGrayScale.o functions.o -Wall -o imageToGrayScale
		$(CC) imageBinarizer.o functions.o -Wall -o imageBinarizer
		$(CC) nearlyBlack.o functions.o -Wall -o nearlyBlack

main.o: main.c
		$(CC) main.c -Wall -c -o main.o

functions.o: functions.c functions.h
		$(CC) functions.c -Wall -c -o functions.o

imageReader.o: imageReader.c
		$(CC) imageReader.c -Wall -c -o imageReader.o

imageToGrayScale.o: imageToGrayScale.c
		$(CC) imageToGrayScale.c -Wall -c -o imageToGrayScale.o

imageBinarizer.o: imageBinarizer.c
		$(CC) imageBinarizer.c -Wall -c -o imageBinarizer.o

nearlyBlack.o: nearlyBlack.c
		$(CC) nearlyBlack.c -Wall -c -o nearlyBlack.o

imageWriter.o: imageWriter.c
		$(CC) imageWriter.c -Wall -c -o imageWriter.o

clean:
	rm pipeline imageReader imageToGrayScale imageBinarizer nearlyBlack imageWriter output* *.o
