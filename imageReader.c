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

/**
* Funcion principal
* @param argc cantidad de parametros recibidos desde el proceso anterior
* @param argv arreglo de parametros recibidos
* @return entero
*/
int main(int arg, char** argv){
  pid_t pid;
  int status;
  int pipeline[2];
  bmpInfoHeader infoHeader;
  bmpFileHeader fileHeader;
  unsigned char* dataImg;
  char* charPipeline=(char*)malloc(sizeof(char)*2);
  dataImg=loadImage(argv[0],&infoHeader,&fileHeader);//se carga la imagen a memoria principal
  pipe(pipeline);
  pid=fork();
  if (pid<0){
    printf("Error: child process not created\n");
    return 0;
  }
  if(pid==0){
  	sprintf(charPipeline,"%d",pipeline[0]);
    //se pasan los parametros al sig. proceso
    char* arguments[]={argv[1],argv[2],argv[3],charPipeline,argv[4],argv[5],argv[6],argv[0],NULL};
    execv("./imageToGrayScale",arguments);
  }
  else{
    close(pipeline[0]);//se cierra la lectura del pipe porque solo se escribira

    //se escribe el fileheader
    write(pipeline[1],&fileHeader.size,sizeof(uint32_t));
    write(pipeline[1],&fileHeader.resv1,sizeof(uint16_t));
    write(pipeline[1],&fileHeader.resv2,sizeof(uint16_t));
    write(pipeline[1],&fileHeader.offset,sizeof(uint32_t));

    //se escribe el infoheader
    write(pipeline[1],&infoHeader.headersize,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.width,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.height,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.planes,sizeof(uint16_t));
    write(pipeline[1],&infoHeader.bpp,sizeof(uint16_t));
    write(pipeline[1],&infoHeader.compress,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.imgsize,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.bpmx,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.bpmy,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.colors,sizeof(uint32_t));
    write(pipeline[1],&infoHeader.imxtcolors,sizeof(uint32_t));

    //se escribe la data de la imagen
    for(int i = 0;i < infoHeader.imgsize;i++){
      write(pipeline[1],&dataImg[i],sizeof(unsigned char));
    }
    waitpid(pid, &status, 0);//Esperamos hasta que el proceso pid (hijo) cambie de estado
  }
  return 0;
}
