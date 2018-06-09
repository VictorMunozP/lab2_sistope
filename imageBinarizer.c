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
* binarizeImage: funcion que procesa los pixeles de una imagen convirtiendolos en negros o blancos segun el umbralBin ingresado
* @param array: arreglo de caracteres con los datos de la imagen (pixeles) a procesar
* @param bInfoHeader: estrucutura que contiene informacion de la imagen
* @param umbralBin: numero entero que es ingresado por el usuario, corresponde al umbralBin de binarizado
* @return mismo arreglo de caracteres entrante pero con nuevos valores en los pixeles, para que representen un pixel negro o blanco
*/
unsigned char* binarizeImage(unsigned char* array, bmpInfoHeader bInfoHeader,int umbralBin){
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
           if(prom>umbralBin){
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
* Funcion principal
* @param argc cantidad de parametros recibidos desde el proceso anterior
* @param argv arreglo de parametros recibidos
* @return entero
*/
int main(int argc,char *argv[]) {
    pid_t pid;
    int status,umbralBin;
    int pipelineBin[2];

    bmpInfoHeader infoHeader;
    bmpFileHeader fileHeader;
    
    int pipeline=atoi(argv[3]);
    char *nextPipe=(char*)malloc(sizeof(char)*2);
    
    //se lee desde el pipe la informacion de la imagen----------------------------------
    read(pipeline,&fileHeader.size,sizeof(uint32_t));
    read(pipeline,&fileHeader.resv1,sizeof(uint16_t));
    read(pipeline,&fileHeader.resv2,sizeof(uint16_t));
    read(pipeline,&fileHeader.offset,sizeof(uint32_t));

    read(pipeline,&infoHeader.headersize,sizeof(uint32_t));
    read(pipeline,&infoHeader.width,sizeof(uint32_t));
    read(pipeline,&infoHeader.height,sizeof(uint32_t));
    read(pipeline,&infoHeader.planes,sizeof(uint16_t));
    read(pipeline,&infoHeader.bpp,sizeof(uint16_t));
    read(pipeline,&infoHeader.compress,sizeof(uint32_t));
    read(pipeline,&infoHeader.imgsize,sizeof(uint32_t));
    read(pipeline,&infoHeader.bpmx,sizeof(uint32_t));
    read(pipeline,&infoHeader.bpmy,sizeof(uint32_t));
    read(pipeline,&infoHeader.colors,sizeof(uint32_t));
    read(pipeline,&infoHeader.imxtcolors,sizeof(uint32_t));

    unsigned char* dataBin=(unsigned char*)malloc(sizeof(unsigned char)*infoHeader.imgsize);
    for(int i = 0;i < infoHeader.imgsize;i++){
        read(pipeline,&dataBin[i],sizeof(unsigned char));
    }
    //----------------------------------------------------------------------------------------
    umbralBin = atoi(argv[0]);
    
    binarizeImage(dataBin,infoHeader,umbralBin); //se binariza la imagen
    pipe(pipelineBin);
    pid = fork();
    if (pid < 0){
        printf("Error: child process not created\n");
        return 0;
    }
    if(pid == 0){

        sprintf(nextPipe,"%d",pipelineBin[0]);
        //se crea un arreglo de argumentos para pasarselos al sig. proceso
        char* arguments[] = {argv[1],argv[2],nextPipe,argv[4],argv[5],argv[6],argv[7],NULL};
        execv("./nearlyBlack",arguments);
    }
    else{
        //se cierra la lectura del pipe pues solo se necesita escribir la informacion de la imagen-----------------
        close(pipelineBin[0]);
        
        write(pipelineBin[1],&fileHeader.size,sizeof(uint32_t));
        write(pipelineBin[1],&fileHeader.resv1,sizeof(uint16_t));
        write(pipelineBin[1],&fileHeader.resv2,sizeof(uint16_t));
        write(pipelineBin[1],&fileHeader.offset,sizeof(uint32_t));

        write(pipelineBin[1],&infoHeader.headersize,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.width,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.height,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.planes,sizeof(uint16_t));
        write(pipelineBin[1],&infoHeader.bpp,sizeof(uint16_t));
        write(pipelineBin[1],&infoHeader.compress,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.imgsize,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.bpmx,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.bpmy,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.colors,sizeof(uint32_t));
        write(pipelineBin[1],&infoHeader.imxtcolors,sizeof(uint32_t));

        for(int i = 0;i < infoHeader.imgsize;i++){
            write(pipelineBin[1],&dataBin[i],sizeof(unsigned char));
        }
        //------------------------------------------------------------------------------------------------------------
        waitpid(pid, &status, 0);//Esperamos hasta que el proceso pid (hijo) cambie de estado
    }
  return 0;
}
