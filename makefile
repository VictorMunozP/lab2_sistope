CC=gcc

all: main.o functions.o imageReader.o imageToGrayScale.o imageBinarizer.o nearlyBlack.o imageWriter.o
		$(CC) main.o functions.o imageReader.o imageToGrayScale.o imageBinarizer.o nearlyBlack.o imageWriter.o -Wall -o pipeline
		rm *.o

main.o: main.c
				$(CC) main.c -Wall -c -o main.o

functions.o: functions.c functions.h
		$(CC) functions.c -Wall -c -o functions.o

imageReader.o: imageReader.c functions.h
		$(CC) imageReader.c -Wall -c -o imageReader.o

imageToGrayScale.o: imageToGrayScale.c functions.h
		$(CC) imageToGrayScale.c -Wall -c -o imageToGrayScale.o

imageBinarizer.o: imageBinarizer.c functions.h
		$(CC) imageBinarizer.c -Wall -c -o imageBinarizer.o

nearlyBlack.o: nearlyBlack.c functions.h
		$(CC) nearlyBlack.c -Wall -c -o nearlyBlack.o

imageWriter.o: imageWriter.c functions.h
		$(CC) imageWriter.c -Wall -c -o imageWriter.o

clean:
	rm pipeline  output*.bmp
