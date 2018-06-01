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
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

/**
* isInt: funcion que verifica si el arreglo recibido solo contiene caracteres numericos
* @param numero: arreglo de caracteres a analizar
* @return entero: 1 si solo contiene caracteres numericos, 0 si no.
*/
int isInt(char* numero){
  /*VERIFICACION DE QUE EL ARGUMENTO numero SEA ENTERO*/
 for(int i=0;i<strlen(numero);i++){
   if(isdigit(numero[i])==0){
     return 0;
   }
 }
 return 1;
}


/**
* setNameInput: funcion que construye el nombre de las imagenes a procesar
* @param a: numero recibido para agregar al nombre de la imagen
* @return arreglo de caracteres con el nombre de la imagen
*/
char* setNameInput(int a){
  char* nombreEntrada=malloc(sizeof(char)*20);
  strcpy(nombreEntrada,"imagen_");

  char* buffer=malloc(sizeof(char)*12);
  sprintf(buffer,"%d",a);

  char* formato=".bmp";

  char* nombreDef=malloc(sizeof(char)*22);
  nombreDef=strcat(nombreEntrada,buffer);
  nombreDef=strcat(nombreDef,formato);
  return nombreDef;
}

/**
* setNameOutputGS: funcion que construye el nombre de las imagenes en GrayScale
* @param a: numero recibido para agregar al nombre de la imagen
* @return arreglo de caracteres con el nombre de la imagen
*/
char* setNameOutputGS(int a){
  char* nombreEntrada=malloc(sizeof(char)*20);
  strcpy(nombreEntrada,"outputGS_");

  char* buffer=malloc(sizeof(char)*12);
  sprintf(buffer,"%d",a);

  char* formato=".bmp";

  char* nombreDef=malloc(sizeof(char)*22);
  nombreDef=strcat(nombreEntrada,buffer);
  //printf("nombre parcial: %s\n",nombreDef);
  nombreDef=strcat(nombreDef,formato);
  //printf("%s\n",nombreDef);
  return nombreDef;
}

/**
* setNameOutputBin: funcion que construye el nombre de las imagenes binarizadas
* @param a: numero recibido para agregar al nombre de la imagen
* @return arreglo de caracteres con el nombre de la imagen
*/
char* setNameOutputBin(int a){
  char* nombreEntrada=malloc(sizeof(char)*20);
  strcpy(nombreEntrada,"outputBin_");

  char* buffer=malloc(sizeof(char)*12);
  sprintf(buffer,"%d",a);

  char* formato=".bmp";

  char* nombreDef=malloc(sizeof(char)*22);
  nombreDef=strcat(nombreEntrada,buffer);
  //printf("nombre parcial: %s\n",nombreDef);
  nombreDef=strcat(nombreDef,formato);
  //printf("%s\n",nombreDef);
  return nombreDef;
}
