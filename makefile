compile:
	gcc -o pipeline main.c functions.c -Wall
	gcc -o lectorImagen imageReader.c -Wall
	gcc -o conversorGris imageToGrayScale.c -Wall
	gcc -o binarizarImagen imageBinarizer.c -Wall
	gcc -o analisisPropiedad nearlyBlack.c -Wall
	gcc -o crearImagen imageWriter.c -Wall


	@echo " ./pipeline -c num imgs -u umbral -n umbral -b "

clean:
	rm pipeline lectorImagen conversorGris binarizarImagen analisisPropiedad crearImagen
