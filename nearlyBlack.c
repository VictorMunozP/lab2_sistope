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
      //printf("%f >?< %f\n",(negro/(negro+blanco)),(newUmbral/100));
      return("yes");
  }
  else{
    //printf("%f >?< %f\n",(negro/(negro+blanco)),(newUmbral/100));
    return("no");
  }
}

int main(int argc,char *argv[]) {
    pid_t pid;
    int status;
    int pipelineB[2];
    bmpInfoHeader binformacion;
    bmpFileHeader bcabecera;
    //bitmaptotal totalPixeles;
    int pipeline = atoi(argv[2]);
    int UMBRAL_F,bflag;
    char *cambio = (char *)malloc(2 * sizeof(char));
    
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

    unsigned char *data_analize = (unsigned char*)malloc(binformacion.imgsize * sizeof(unsigned char));
    for(int i = 0;i < binformacion.imgsize;i++){
        read(pipeline,&data_analize[i],sizeof(unsigned char));
    }
    UMBRAL_F = atoi(argv[0]);
    bflag = atoi(argv[1]);
    
    
    if(bflag == 1){
      if(argv[6][7]=='1'){
        printf("|   image    | nearly black  |\n");
        printf("|------------|---------------|\n");
      }
      
      //char* answerNB=;
      printf("|  imagen_%s  |      %-3s      |\n",argv[3],nearlyBlack(data_analize,binformacion,UMBRAL_F));
      
    }
    pipe(pipelineB);
    pid = fork();
    if (pid < 0){
        printf("Error al crear proceso hijo \n");
        return 0;
    }
    if(pid == 0){
        sprintf(cambio,"%d",pipelineB[0]);
        char *arreglos[] = {cambio,argv[5],NULL};
        execv("./imageWriter",arreglos);
    }
    else{
        close(pipelineB[0]);

        write(pipelineB[1],&bcabecera.size,sizeof(uint32_t));
        write(pipelineB[1],&bcabecera.resv1,sizeof(uint16_t));
        write(pipelineB[1],&bcabecera.resv2,sizeof(uint16_t));
        write(pipelineB[1],&bcabecera.offset,sizeof(uint32_t));

        write(pipelineB[1],&binformacion.headersize,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.width,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.height,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.planes,sizeof(uint16_t));
        write(pipelineB[1],&binformacion.bpp,sizeof(uint16_t));
        write(pipelineB[1],&binformacion.compress,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.imgsize,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.bpmx,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.bpmy,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.colors,sizeof(uint32_t));
        write(pipelineB[1],&binformacion.imxtcolors,sizeof(uint32_t));

        for(int i = 0;i < binformacion.imgsize;i++){
            write(pipelineB[1],&data_analize[i],sizeof(unsigned char));
        }
        waitpid(pid, &status, 0);
    }
  return 0;
}
