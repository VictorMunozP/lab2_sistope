compile:
	gcc -o pipeline main.c functions.c -Wall
	gcc -o imageReader imageReader.c -Wall
	gcc -o imageToGrayScale imageToGrayScale.c -Wall
	gcc -o imageBinarizer imageBinarizer.c -Wall
	gcc -o nearlyBlack nearlyBlack.c -Wall
	gcc -o imageWriter imageWriter.c -Wall

clean:
	rm pipeline imageReader imageToGrayScale imageBinarizer nearlyBlack imageWriter
