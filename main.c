#include "structs.h"
#include "functions.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/**
* Funcion principal
* @param argc cantidad de parametros recibidos por consola
* @param argv arreglo de parametros recibidos
* @return entero
*/
int main(int argc, char** argv){
  int a, numImages,umbralBin,umbralClas,i,imprimir=0;
  bmpInfoHeader bInfoHeader;
	bmpFileHeader header;

  //Ciclo while para uso de funcion getopt
  while ((a = getopt (argc, argv, "bc:u:n:")) != -1){
    switch(a){
      case 'c':
        if(isInt(optarg)){
          numImages=atoi(optarg);
          //printf("The number of images to process are: %i\n",numImages);
        }
        else{
          printf("Not valid input -c (only integer digits!)\n");
          return 0;
        }
        break;
      case 'u':
        if(isInt(optarg)){
          umbralBin=atoi(optarg);
          //printf("UMBRAL to Bin is: %i\n",umbralBin);
        }
        else{
          printf("Not valid input -u (only integer digits!)\n");
          return 0;
        }
        break;
      case 'n':
        if(isInt(optarg)){
          umbralClas=atoi(optarg);
          //printf("UMBRAL to classify is:%i\n",umbralClas);
        }
        else{
          printf("Not valid input -n (only integer digits!)\n");
          return 0;
        }
        break;
      case 'b':
        imprimir=1;
        break;
      case '?':
        if (optopt == 'c' || optopt == 'u' || optopt == 'n')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknow option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Option with unknown character `\\x%x'.\n",optopt);
          return 1;
      default:
        abort();
    }//fin del switch
  }//fin del while


  if(imprimir==1){//la variable imprimir es seteada en 1 segun la bandera -b
    printf("|    image   | nearly black  |\n");
    printf("|------------|---------------|\n");
  }

  //ciclo for para procesar las imagenes, una a la vez
  for(i=1;i<numImages+1;i++){

    //Se construye nombre de entrada de las imagenes segun enunciado
    char* nombreEntrada=malloc(sizeof(char)*22);
    nombreEntrada=setNameInput(i);


    //GS= GrayScale
    unsigned char* imagen = loadImage(nombreEntrada, &bInfoHeader, &header);
    char* nombreSalidaGS=malloc(sizeof(char)*22);
    nombreSalidaGS=setNameOutputGS(i);
    rgbToGrayScale(imagen,bInfoHeader);
    saveImage(imagen, bInfoHeader, header,nombreSalidaGS);

    //Bin= Binarize
    unsigned char* imagen2 = loadImage(nombreEntrada, &bInfoHeader, &header);
    char* nombreSalidaBin=malloc(sizeof(char)*22);
    nombreSalidaBin=setNameOutputBin(i);
    binarizeImage(imagen2,bInfoHeader,umbralBin);
    saveImage(imagen2, bInfoHeader, header,nombreSalidaBin);

    if(imprimir==1){
      char* answerNB=malloc(sizeof(char)*4);
      answerNB=nearlyBlack(imagen2,bInfoHeader,umbralClas);
      printf("|  imagen_%d  |      %s      |\n",i,answerNB);
    }
    free(imagen);
    free(imagen2);

  }//fin del ciclo for para procesar las imagenes

  return 0;
}
