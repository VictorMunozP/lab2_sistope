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

/**
* Funcion principal
* @param argc cantidad de parametros recibidos desde el proceso anterior
* @param argv arreglo de parametros recibidos
* @return entero
*/
int main(int argc,char *argv[]) {
    pid_t pid;
    int status;
    int pipelineNB[2];
    bmpInfoHeader infoHeader;
    bmpFileHeader fileHeader;
    
    int pipeline = atoi(argv[2]);
    int umbralNB,imprimir;
    char *nextPipe = (char *)malloc(2 * sizeof(char));
    
    //se lee desde el pipe la imagen ----------------------------------------------------
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

    unsigned char *dataImg=(unsigned char*)malloc(sizeof(unsigned char)*infoHeader.imgsize);
    for(int i=0;i<infoHeader.imgsize;i++){
        read(pipeline,&dataImg[i],sizeof(unsigned char));
    }
    //-------------------------------------------------------------------------------------

    umbralNB=atoi(argv[0]);
    imprimir=atoi(argv[1]);
    
    if(imprimir==1){//solo se imprime si el segundo argumento recibido por este proceso es 1 (si en bash se recibio la flag -b)
      if(argv[6][7]=='1'){//antes de escribir por pantalla el resultado de la primera imagen se printea la tabla, solo con fines visuales
        printf("|   image    | nearly black  |\n");
        printf("|------------|---------------|\n");
      }
      //se imprimen los resultados de la evaluacion nearlyBlack()
      printf("|  imagen_%s  |      %-3s      |\n",argv[3],nearlyBlack(dataImg,infoHeader,umbralNB)); 
    }

    pipe(pipelineNB);
    pid=fork();
    if (pid<0){
        printf("Error: child process not created \n");
        return 0;
    }
    if(pid==0){
        sprintf(nextPipe,"%d",pipelineNB[0]);
        //se crea un arreglo con 2 argumentos (el pipe y el nombre de la imagen a escribir) para el proceso final
        char *arguments[]={nextPipe,argv[5],NULL};
        execv("./imageWriter",arguments);
    }
    else{
        //se cierra la lectura para solo escribir la imagen----------------------------------------------------------
        close(pipelineNB[0]);

        write(pipelineNB[1],&fileHeader.size,sizeof(uint32_t));
        write(pipelineNB[1],&fileHeader.resv1,sizeof(uint16_t));
        write(pipelineNB[1],&fileHeader.resv2,sizeof(uint16_t));
        write(pipelineNB[1],&fileHeader.offset,sizeof(uint32_t));

        write(pipelineNB[1],&infoHeader.headersize,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.width,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.height,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.planes,sizeof(uint16_t));
        write(pipelineNB[1],&infoHeader.bpp,sizeof(uint16_t));
        write(pipelineNB[1],&infoHeader.compress,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.imgsize,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.bpmx,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.bpmy,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.colors,sizeof(uint32_t));
        write(pipelineNB[1],&infoHeader.imxtcolors,sizeof(uint32_t));

        for(int i=0;i<infoHeader.imgsize;i++){
            write(pipelineNB[1],&dataImg[i],sizeof(unsigned char));
        }
        //------------------------------------------------------------------------------------------------------------
        waitpid(pid, &status, 0);//Esperamos hasta que el proceso pid (hijo) cambie de estado
    }
  return 0;
}
