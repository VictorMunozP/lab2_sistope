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
* rgbToGrayScale: funcion que procesa los pixeles de una imagen segun la ecuacion de luminiscencia (e.l)
* @param array: arreglo de caracteres con los datos de la imagen (pixeles) a procesar
* @param bInfoHeader: estrucutura que contiene informacion de la imagen
* @return mismo arreglo de caracteres entrante pero con nuevos valores "Y"(resulantes de e.l.) en los pixeles
*/
unsigned char* rgbToGrayScale(unsigned char* array, bmpInfoHeader bInfoHeader){
  int i,j,y,azul,verde,rojo,indice=0;
  //SE TRATA LA IMAGEN CON LA FORMULA
  //Se recorre segun ancho y largo
  for(i=0; i < bInfoHeader.height; i++){
			for(j=0; j < bInfoHeader.width; j++){
			     //los componentes alfa, r*0.3, g*0.59 ,b*0.11 de un pixel
           azul=array[indice]*0.11;//azul
           indice++;
           verde=array[indice]*0.59;//verde
           indice++;
           rojo=array[indice]*0.3;//rojo
           y=(azul+verde+rojo);
           indice++;//estoy en alpha
           array[indice-3]=y;
           array[indice-2]=y;
           array[indice-1]=y;
           indice++;//estoy en blue del proximo j
		  }
	}
  return array;
}
void saveImageGS(unsigned char* array, bmpInfoHeader bInfoHeader, bmpFileHeader header, char* filename){
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

int main(int argc,char* argv[]) {
    pid_t pid;
    int status;
    int tuberiaC[2];

    bmpInfoHeader binformacion;
    bmpFileHeader bcabecera;
    int tuberia = atoi(argv[3]);
    char *cambio = (char *)malloc(2 * sizeof(char));
    //lectura de la data en el pipe
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

    unsigned char *data_imagen = (unsigned char*)malloc(binformacion.imgsize* sizeof(unsigned char));
    for(int i = 0;i < binformacion.imgsize;i++){
        read(tuberia,&data_imagen[i],sizeof(unsigned char));
    }
    unsigned char* data_grisaseos=(unsigned char*)malloc(sizeof(unsigned char)*binformacion.imgsize);

    data_grisaseos = data_imagen;
    rgbToGrayScale(data_grisaseos,binformacion);
    saveImageGS(data_grisaseos,binformacion,bcabecera,argv[5]);

    pipe(tuberiaC);
    pid = fork();
    if (pid < 0){
        printf("Error al crear proceso hijo \n");
        return 0;
    }
    if(pid == 0){
        sprintf(cambio,"%d",tuberiaC[0]);
        char *arreglos[] = {argv[0],argv[1],argv[2],cambio,argv[4],argv[5],argv[6],NULL};
        execv("./binarizarImagen",arreglos);
        //printf("Soy el hijo %d de conversorGris (%d)",getpid(),getppid());
    }
    else{
        close(tuberiaC[0]);
        write(tuberiaC[1],&bcabecera.size,sizeof(uint32_t));
        write(tuberiaC[1],&bcabecera.resv1,sizeof(uint16_t));
        write(tuberiaC[1],&bcabecera.resv2,sizeof(uint16_t));
        write(tuberiaC[1],&bcabecera.offset,sizeof(uint32_t));

        write(tuberiaC[1],&binformacion.headersize,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.width,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.height,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.planes,sizeof(uint16_t));
        write(tuberiaC[1],&binformacion.bpp,sizeof(uint16_t));
        write(tuberiaC[1],&binformacion.compress,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.imgsize,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.bpmx,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.bpmy,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.colors,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.imxtcolors,sizeof(uint32_t));


        for(int i = 0;i < binformacion.imgsize;i++){
            write(tuberiaC[1],&data_imagen[i],sizeof(unsigned char));
        }
        free(data_grisaseos);
        waitpid(pid, &status, 0);
    }
  return 0;
}
