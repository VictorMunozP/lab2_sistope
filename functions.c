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
* loadImage: funcion que lee y carga a memoria principal una imagen bmp
* @param filename: nombre de la imagen a cargar (mismo path que archivo ejecutable)
* @param bInfoHeader: puntero a la estrucutura que guarda informacion de la imagen
* @paramheader: puntero a la estructura que contiene el header de la imagen
* @return arreglo de caracteres con los datos de la imagen
*/
unsigned char* loadImage(char* filename, bmpInfoHeader* bInfoHeader, bmpFileHeader* header){
  int f;
  unsigned char* imgdata;   /* datos de la imagen */
  uint16_t type;        /* 2 bytes identificativos */

  f=open (filename, O_RDONLY);
  if (f<0){
    return NULL;
  }        //retorno nulo en caso de error

  //* Se lee los dos primeros bytes
  //read(int fildes, void *buf, size_t nbyte);  FORMATO
  read(f, &type, sizeof(uint16_t));
  if (type !=0x4D42){
      close(f);
      return NULL;
    }
  /*Se lee la cabecera del fichero*/
  read(f,header, sizeof(bmpFileHeader));
  /*Se lee la informacion del header*/
  read(f,bInfoHeader, sizeof(bmpInfoHeader));
  /*Se reserva la cantidad de memoria indicada por imgsize*/
  imgdata=(unsigned char*)malloc(bInfoHeader->imgsize);
  /*Se ubica donde comienzan los datos de la imagen segun el offset de la cabecera*/
  lseek(f, header->offset, SEEK_SET);
  /*Se leen los datos de la imagen*/
  read(f,imgdata, bInfoHeader->imgsize);
  /*Se cierra el archivo*/
  close(f);
  /*Se retorna la imagen */
  return imgdata;
}

/**
* saveImage: funcion que abre una imagen (crea si no existe) y guarda en memoria principal toda su informacion
* @param array: arreglo de caracteres con los datos de la imagen a guardar
* @param bInfoHeader: puntero a la estrucutura que guarda informacion de la imagen
* @paramheader: puntero a la estructura que contiene el header de la imagen
* @param filename: nombre de la imagen a cargar
* @return -
*/
void saveImage(unsigned char* array, bmpInfoHeader bInfoHeader, bmpFileHeader header, char* filename){
	//Se crea/abre el archivo de la imagen a escribir, con permisos de lectura/escritura para user y group, y de solo lectura para others
  int imagen= open(filename, O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if(imagen<0){
		printf("Error: %d.\n",errno);
		return;
	}
	//Se escribe el tipo de archivo BM
	uint16_t type = 0x4D42;
  if(write(imagen, &type, sizeof(uint16_t)) != sizeof(uint16_t) ){
      write(2,"There was an error writing to standard out\n", 44);
  }
  //Se escribe la cabecera del archivo
  write(imagen, &header, sizeof(bmpFileHeader) );
  //Se escribe la cabecera de info de la imagen
  write(imagen, &bInfoHeader, sizeof(bmpInfoHeader) );
  //Se ubica donde deben ir los datos, segun el offset del header
  //lseek(int fd, off_t offset, int whence); FORMATO
  if(lseek(imagen, header.offset, SEEK_SET)<0){
    printf("Error: se cae en lseek\n");
  }
  //Se escriben los datos de la imagen
  write(imagen, array, bInfoHeader.imgsize );
  //Se cierra el archivo
	if(close(imagen)<0){
    printf("Falla en close()\n");
  }
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
  if((negro/(negro+blanco))>=(float)(umbralPorcentaje/100)){
      return "yes";
  }
  else{
    return "no";
  }
}
