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

int main(int arg, char** argv){
  pid_t pid;
  int status;
  int pipeline[2];
  bmpInfoHeader binformacion;
  bmpFileHeader bcabecera;
  unsigned char* img_data;
  char* charPipeline = (char*)malloc(sizeof(char)*2);
  img_data = loadImage(argv[0],&binformacion,&bcabecera);
  pipe(pipeline);
  pid = fork();
      if (pid < 0){
          printf("Error: child process not created\n");
          return 0;
      }
      if(pid == 0){
  	     sprintf(charPipeline,"%d",pipeline[0]);
         char *arguments[] = {argv[1],argv[2],argv[3],charPipeline,argv[4],argv[5],argv[6],argv[0],NULL};
         execv("./imageToGrayScale",arguments);
       }
       else{
         close(pipeline[0]);
         write(pipeline[1],&bcabecera.size,sizeof(uint32_t));
         write(pipeline[1],&bcabecera.resv1,sizeof(uint16_t));
         write(pipeline[1],&bcabecera.resv2,sizeof(uint16_t));
         write(pipeline[1],&bcabecera.offset,sizeof(uint32_t));

         write(pipeline[1],&binformacion.headersize,sizeof(uint32_t));
         write(pipeline[1],&binformacion.width,sizeof(uint32_t));
         write(pipeline[1],&binformacion.height,sizeof(uint32_t));
         write(pipeline[1],&binformacion.planes,sizeof(uint16_t));
         write(pipeline[1],&binformacion.bpp,sizeof(uint16_t));
         write(pipeline[1],&binformacion.compress,sizeof(uint32_t));
         write(pipeline[1],&binformacion.imgsize,sizeof(uint32_t));
         write(pipeline[1],&binformacion.bpmx,sizeof(uint32_t));
         write(pipeline[1],&binformacion.bpmy,sizeof(uint32_t));
         write(pipeline[1],&binformacion.colors,sizeof(uint32_t));
         write(pipeline[1],&binformacion.imxtcolors,sizeof(uint32_t));

         for(int i = 0;i < binformacion.imgsize;i++){
           write(pipeline[1],&img_data[i],sizeof(unsigned char));
         }
         waitpid(pid, &status, 0);
       }
  return 0;
}
