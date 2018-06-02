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
    int tuberiaB[2];

    bmpInfoHeader binformacion;
    bmpFileHeader bcabecera;
    //bitmaptotal totalPixeles;
    //int totalPixeles
    int tuberia = atoi(argv[3]);
    int UMBRAL;
    char *cambio = (char *)malloc(2 * sizeof(char));
    /*
    read(tuberia,&bcabecera.tamano,sizeof(uint32_t));
    read(tuberia,&bcabecera.reservado1,sizeof(uint16_t));
    read(tuberia,&bcabecera.reservado2,sizeof(uint16_t));
    read(tuberia,&bcabecera.offsetBit,sizeof(uint32_t));

    read(tuberia,&binformacion.alto,sizeof(uint32_t));
    read(tuberia,&binformacion.ancho,sizeof(uint32_t));
    read(tuberia,&binformacion.coloresImportantes,sizeof(uint32_t));
    read(tuberia,&binformacion.colorPixel,sizeof(uint16_t));
    read(tuberia,&binformacion.compresion,sizeof(uint16_t));
    read(tuberia,&binformacion.direcciones,sizeof(uint32_t));
    read(tuberia,&binformacion.tamano,sizeof(uint32_t));
    read(tuberia,&binformacion.tamanoImagen,sizeof(uint32_t));
    read(tuberia,&binformacion.totalBit,sizeof(uint32_t));
    read(tuberia,&binformacion.XResolporMetros,sizeof(uint32_t));
    read(tuberia,&binformacion.YResolporMetros,sizeof(uint32_t));
    */
    read(tuberia,&bcabecera.size,sizeof(uint32_t));
    read(tuberia,&bcabecera.resv1,sizeof(uint16_t));
    read(tuberia,&bcabecera.resv2,sizeof(uint16_t));
    read(tuberia,&bcabecera.offset,sizeof(uint32_t));

    read(tuberia,&binformacion.headersize,sizeof(uint32_t));
    read(tuberia,&binformacion.width,sizeof(uint32_t));
    read(tuberia,&binformacion.height,sizeof(uint32_t));
    read(tuberia,&binformacion.planes,sizeof(uint16_t));
    read(tuberia,&binformacion.bpp,sizeof(uint16_t));
    read(tuberia,&binformacion.compress,sizeof(uint32_t));
    read(tuberia,&binformacion.imgsize,sizeof(uint32_t));
    read(tuberia,&binformacion.bpmx,sizeof(uint32_t));
    read(tuberia,&binformacion.bpmy,sizeof(uint32_t));
    read(tuberia,&binformacion.colors,sizeof(uint32_t));
    read(tuberia,&binformacion.imxtcolors,sizeof(uint32_t));

    unsigned char* data_binarizado = (unsigned char*)malloc(binformacion.imgsize * sizeof(unsigned char));
    for(int i = 0;i < binformacion.imgsize;i++){
        read(tuberia,&data_binarizado[i],sizeof(unsigned char));
    }
    UMBRAL = atoi(argv[0]);
    //Los parametros que son recibidos como char, se transforma en enteros para su posterior utilizacion
    //unsigned char *binarizar_color;
    binarizeImage(data_binarizado,binformacion,UMBRAL);
    pipe(tuberiaB);
    pid = fork();
    if (pid < 0){
        printf("Error: child process not created\n");
        return 0;
    }
    if(pid == 0){

        sprintf(cambio,"%d",tuberiaB[0]);
        char *arreglos[] = {argv[1],argv[2],cambio,argv[4],argv[5],argv[6],NULL};
        execv("./analisisPropiedad",arreglos);
    }
    else{
        close(tuberiaB[0]);
        /*
        write(tuberiaB[1],&bcabecera.tamano,sizeof(uint32_t));
        write(tuberiaB[1],&bcabecera.reservado1,sizeof(uint16_t));
        write(tuberiaB[1],&bcabecera.reservado2,sizeof(uint16_t));
        write(tuberiaB[1],&bcabecera.offsetBit,sizeof(uint32_t));

        write(tuberiaB[1],&binformacion.alto,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.ancho,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.coloresImportantes,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.colorPixel,sizeof(uint16_t));
        write(tuberiaB[1],&binformacion.compresion,sizeof(uint16_t));
        write(tuberiaB[1],&binformacion.direcciones,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.tamano,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.tamanoImagen,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.totalBit,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.XResolporMetros,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.YResolporMetros,sizeof(uint32_t));

        write(tuberiaB[1],&totalPixeles.totalBlancos,sizeof(int));
        write(tuberiaB[1],&totalPixeles.totalNegros,sizeof(int));
        */

        write(tuberiaB[1],&bcabecera.size,sizeof(uint32_t));
        write(tuberiaB[1],&bcabecera.resv1,sizeof(uint16_t));
        write(tuberiaB[1],&bcabecera.resv2,sizeof(uint16_t));
        write(tuberiaB[1],&bcabecera.offset,sizeof(uint32_t));

        write(tuberiaB[1],&binformacion.headersize,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.width,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.height,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.planes,sizeof(uint16_t));
        write(tuberiaB[1],&binformacion.bpp,sizeof(uint16_t));
        write(tuberiaB[1],&binformacion.compress,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.imgsize,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.bpmx,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.bpmy,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.colors,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.imxtcolors,sizeof(uint32_t));

        for(int i = 0;i < binformacion.imgsize;i++){
            write(tuberiaB[1],&data_binarizado[i],sizeof(unsigned char));
        }
        waitpid(pid, &status, 0);
    }
  return 0;
}
