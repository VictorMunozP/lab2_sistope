#include "structs.h"

//Se incluyen las cabeceras de las funciones utilizadas

int isInt(char* numero);

char* setNameInput(int a);

char* setNameOutputGS(int a);

char* setNameOutputBin(int a);

unsigned char* loadImage(char *filename, bmpInfoHeader *bInfoHeader, bmpFileHeader *header);

void saveImage(unsigned char* array, bmpInfoHeader bInfoHeader, bmpFileHeader header, char* filename);

unsigned char* rgbToGrayScale(unsigned char* array, bmpInfoHeader bInfoHeader);

unsigned char* binarizeImage(unsigned char* array, bmpInfoHeader bInfoHeader,int umbral);

char* nearlyBlack(unsigned char* array, bmpInfoHeader bInfoHeader,int umbral);
