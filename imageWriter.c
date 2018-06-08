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
* saveImage: funcion que abre una imagen (crea si no existe) y guarda en memoria secundaria toda su informacion
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

int main(int argc,char *argv[]) {
    bmpInfoHeader binformacion;
    bmpFileHeader bcabecera;
    int pipeline = atoi(argv[0]);
    read(pipeline,&bcabecera.size,sizeof(uint32_t));
    read(pipeline,&bcabecera.resv1,sizeof(uint16_t));
    read(pipeline,&bcabecera.resv2,sizeof(uint16_t));
    read(pipeline,&bcabecera.offset,sizeof(uint32_t));

    read(pipeline,&binformacion.headersize,sizeof(uint32_t));
    read(pipeline,&binformacion.width,sizeof(uint32_t));
    read(pipeline,&binformacion.height,sizeof(uint32_t));
    read(pipeline,&binformacion.planes,sizeof(uint16_t));
    read(pipeline,&binformacion.bpp,sizeof(uint16_t));
    read(pipeline,&binformacion.compress,sizeof(uint32_t));
    read(pipeline,&binformacion.imgsize,sizeof(uint32_t));
    read(pipeline,&binformacion.bpmx,sizeof(uint32_t));
    read(pipeline,&binformacion.bpmy,sizeof(uint32_t));
    read(pipeline,&binformacion.colors,sizeof(uint32_t));
    read(pipeline,&binformacion.imxtcolors,sizeof(uint32_t));

    unsigned char* data_writer = (unsigned char*)malloc(sizeof(unsigned char)*binformacion.imgsize);
    for(int i = 0;i < binformacion.imgsize;i++){
        read(pipeline,&data_writer[i],sizeof(unsigned char));
    }

    saveImage(data_writer,binformacion,bcabecera,argv[1]);
    return 0;
}
