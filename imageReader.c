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
/*
int main(int arg, char** argv){

  return 0;
}
*/
