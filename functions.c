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

/**
* rgbToGrayScale: funcion que procesa los pixeles de una imagen segun la ecuacion de luminiscencia (e.l)
* @param array: arreglo de caracteres con los datos de la imagen (pixeles) a procesar
* @param bInfoHeader: estrucutura que contiene informacion de la imagen
* @return mismo arreglo de caracteres entrante pero con nuevos valores "Y"(resulantes de e.l.) en los pixeles
*/
unsigned char* rgbToGrayScale(unsigned char* array, bmpInfoHeader bInfoHeader){
  int i,j,y,azul,verde,rojo,indice=0;
  //SE TRATA LA IMAGEN CON LA FORMULA
  //Se recorre segun ancho y largo
  for(i=0; i < bInfoHeader.height; i++){
			for(j=0; j < bInfoHeader.width; j++){
			     //los componentes alfa, r*0.3, g*0.59 ,b*0.11 de un pixel
           azul=array[indice]*0.11;//azul
           indice++;
           verde=array[indice]*0.59;//verde
           indice++;
           rojo=array[indice]*0.3;//rojo
           y=(azul+verde+rojo);
           indice++;//estoy en alpha
           array[indice-3]=y;
           array[indice-2]=y;
           array[indice-1]=y;
           indice++;//estoy en blue del proximo j
		  }
	}
  return array;
}


/**
* binarizeImage: funcion que procesa los pixeles de una imagen convirtiendolos en negros o blancos segun el umbral ingresado
* @param array: arreglo de caracteres con los datos de la imagen (pixeles) a procesar
* @param bInfoHeader: estrucutura que contiene informacion de la imagen
* @param umbral: numero entero que es ingresado por el usuario, corresponde al umbral de binarizado
* @return mismo arreglo de caracteres entrante pero con nuevos valores en los pixeles, para que representen un pixel negro o blanco
*/
unsigned char* binarizeImage(unsigned char* array, bmpInfoHeader bInfoHeader,int umbral){
  int i,j,prom,azul,verde,rojo,indice=0;
  //SE TRATA LA IMAGEN CON LA FORMULA
  //Se recorre segun ancho y largo
  for(i=0; i < bInfoHeader.height; i++){
			for(j=0; j < bInfoHeader.width; j++){
           azul=array[indice];//azul
           indice++;
           verde=array[indice];//verde
           indice++;
           rojo=array[indice];//rojo
           prom=(azul+verde+rojo)/3;
           indice++;//estoy en alpha
           if(prom>umbral){
             array[indice-3]=255;
             array[indice-2]=255;
             array[indice-1]=255;
           }
           else{
             array[indice-3]=0;
             array[indice-2]=0;
             array[indice-1]=0;
           }
           indice++;//estoy en blue del proximo j
		  }
	}
  return array;
}


/**
* nearlyBlack: funcion que evalua si el porcentaje de pixeles negros es mayor o no que un umbral ingresado
* @param array: arreglo de caracteres con los datos de la imagen (pixeles) a evaluar
* @param bInfoHeader: estrucutura que contiene informacion de la imagen
* @param umbralPorcentaje: numero entero que es ingresado por el usuario, corresponde al umbral de negrura
* @return "yes" si la tasa de pixeles negros es mayor que el porcentaje ingresado, "no" de lo contrario.
*/
char* nearlyBlack(unsigned char* array, bmpInfoHeader bInfoHeader,int umbralPorcentaje){
  int i,j,prom,azul,verde,rojo,indice=0;
  float negro,blanco;
  float newUmbral= umbralPorcentaje;
  negro=0;
  blanco=0;
  //Se recorre segun ancho y largo
  for(i=0; i < bInfoHeader.height; i++){
			for(j=0; j < bInfoHeader.width; j++){
           azul=array[indice];//azul
           indice++;
           verde=array[indice];//verde
           indice++;
           rojo=array[indice];//rojo
           prom=(float)(azul+verde+rojo)/3;
           indice++;//estoy en alpha
           if(prom==0){
             negro++;
           }
           else{
             blanco++;
           }
           indice++;//estoy en blue del proximo j
		  }
	}
  if((negro/(negro+blanco))>=(newUmbral/100)){
      //printf("%f >?< %f",(negro/(negro+blanco)),(newUmbral/100));
      return "yes";
  }
  else{
    //printf("%f >?< %f",(negro/(negro+blanco)),(newUmbral/100));
    return "no";
  }
}
