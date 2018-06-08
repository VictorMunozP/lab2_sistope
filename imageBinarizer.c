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

int main(int argc,char *argv[]) {
    pid_t pid;
    int status;
    int pipelineBin[2];

    bmpInfoHeader binformacion;
    bmpFileHeader bcabecera;
    //bitmaptotal totalPixeles;
    //int totalPixeles
    int pipeline = atoi(argv[3]);
    int UMBRAL;
    char *nextPipe = (char*) malloc(2 * sizeof(char));
    
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

    unsigned char* dataBin = (unsigned char*)malloc(sizeof(unsigned char)*binformacion.imgsize);
    for(int i = 0;i < binformacion.imgsize;i++){
        read(pipeline,&dataBin[i],sizeof(unsigned char));
    }
    UMBRAL = atoi(argv[0]);
    //Los parametros que son recibidos como char, se transforma en enteros para su posterior utilizacion
    //unsigned char *binarizar_color;
    binarizeImage(dataBin,binformacion,UMBRAL);
    pipe(pipelineBin);
    pid = fork();
    if (pid < 0){
        printf("Error: child process not created\n");
        return 0;
    }
    if(pid == 0){

        sprintf(nextPipe,"%d",pipelineBin[0]);
        char* arguments[] = {argv[1],argv[2],nextPipe,argv[4],argv[5],argv[6],argv[7],NULL};
        execv("./nearlyBlack",arguments);
    }
    else{
        close(pipelineBin[0]);
        
        write(pipelineBin[1],&bcabecera.size,sizeof(uint32_t));
        write(pipelineBin[1],&bcabecera.resv1,sizeof(uint16_t));
        write(pipelineBin[1],&bcabecera.resv2,sizeof(uint16_t));
        write(pipelineBin[1],&bcabecera.offset,sizeof(uint32_t));

        write(pipelineBin[1],&binformacion.headersize,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.width,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.height,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.planes,sizeof(uint16_t));
        write(pipelineBin[1],&binformacion.bpp,sizeof(uint16_t));
        write(pipelineBin[1],&binformacion.compress,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.imgsize,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.bpmx,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.bpmy,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.colors,sizeof(uint32_t));
        write(pipelineBin[1],&binformacion.imxtcolors,sizeof(uint32_t));

        for(int i = 0;i < binformacion.imgsize;i++){
            write(pipelineBin[1],&dataBin[i],sizeof(unsigned char));
        }
        waitpid(pid, &status, 0);
    }
  return 0;
}
