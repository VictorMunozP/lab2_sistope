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
void nearlyBlack(unsigned char* array, bmpInfoHeader bInfoHeader,int umbralPorcentaje){
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
      //printf("%f >?< %f",(negro/(negro+blanco)),(newUmbral/100));
      printf("yes");
  }
  else{
    //printf("%f >?< %f",(negro/(negro+blanco)),(newUmbral/100));
    printf("no");
  }
}

int main(int argc,char *argv[]) {
    pid_t pid;
    int status;
    int tuberiaB[2];
    bmpInfoHeader binformacion;
    bmpFileHeader bcabecera;
    //bitmaptotal totalPixeles;
    int tuberia = atoi(argv[2]);
    int UMBRAL_F,cantidadImagen,bflag;
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

    read(tuberia,&totalPixeles.totalBlancos,sizeof(int));
    read(tuberia,&totalPixeles.totalNegros,sizeof(int));
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

    unsigned char *data_analize = (unsigned char*)malloc(binformacion.imgsize * sizeof(unsigned char));
    for(int i = 0;i < binformacion.imgsize;i++){
        read(tuberia,&data_analize[i],sizeof(unsigned char));
    }
    UMBRAL_F = atoi(argv[0]);
    bflag = atoi(argv[1]);
    cantidadImagen = atoi(argv[3]);
    //Los parametros que son recibidos como char, se transforma en enteros para su posterior utilizacion
    if(bflag == 1){
      printf("cantidadImagen %d: ",cantidadImagen);
      nearlyBlack(data_analize,binformacion,UMBRAL_F);
      printf("\n");
    }
    pipe(tuberiaB);
    pid = fork();
    if (pid < 0){
        printf("Error al crear proceso hijo \n");
        return 0;
    }
    if(pid == 0){
        sprintf(cambio,"%d",tuberiaB[0]);
        char *arreglos[] = {cambio,argv[5],NULL};
        execv("./imageWriter",arreglos);
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
            write(tuberiaB[1],&data_analize[i],sizeof(unsigned char));
        }
        waitpid(pid, &status, 0);
    }
  return 0;
}
