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
  for(i=0;i<bInfoHeader.height;i++){
			for(j=0;j<bInfoHeader.width;j++){
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

/**
* saveImage: funcion que abre una imagen (crea si no existe) y guarda en memoria secundaria toda su informacion,
*             es utilizada para guardar la imagen en escala de grises.
* @param array: arreglo de caracteres con los datos de la imagen a guardar
* @param bInfoHeader: puntero a la estrucutura que guarda informacion de la imagen
* @paramheader: puntero a la estructura que contiene el header de la imagen
* @param filename: nombre de la imagen a cargar
* @return -
*/
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

/**
* Funcion principal
* @param argc cantidad de parametros recibidos desde el proceso anterior
* @param argv arreglo de parametros recibidos
* @return entero
*/
int main(int argc,char* argv[]) {
    pid_t pid;
    int status;
    int pipelineGS[2];

    bmpInfoHeader infoHeader;
    bmpFileHeader fileHeader;
    int pipeline = atoi(argv[3]);
    char* nextPipe = (char*)malloc(sizeof(char)*2);
    
    //se lee la imagen desde el pipe------------------------------
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

    unsigned char* dataImg=(unsigned char*)malloc(sizeof(unsigned char)*infoHeader.imgsize);
    for(int i = 0;i < infoHeader.imgsize;i++){
        read(pipeline,&dataImg[i],sizeof(unsigned char));
    }
    //-------------------------------------------------------------


    rgbToGrayScale(dataImg,infoHeader);//se convierte la imagen leida en proceso anterior a escala de grises
    saveImageGS(dataImg,infoHeader,fileHeader,argv[5]);//se escribe en memoria secundaria la imagen en escala de grises

    pipe(pipelineGS);
    pid=fork();
    if (pid<0){
        printf("Error: child process not created\n");
        return 0;
    }
    if(pid==0){
        sprintf(nextPipe,"%d",pipelineGS[0]);
        //se crea un arreglo de argumentos que se pasan al siguiente proceso
        char* arguments[] = {argv[0],argv[1],argv[2],nextPipe,argv[4],argv[5],argv[6],argv[7],NULL};
        execv("./imageBinarizer",arguments);
    }
    else{
        //se cierra la lectura del pipe ya que solo se escribira en el------------------
        close(pipelineGS[0]);
        write(pipelineGS[1],&fileHeader.size,sizeof(uint32_t));
        write(pipelineGS[1],&fileHeader.resv1,sizeof(uint16_t));
        write(pipelineGS[1],&fileHeader.resv2,sizeof(uint16_t));
        write(pipelineGS[1],&fileHeader.offset,sizeof(uint32_t));

        write(pipelineGS[1],&infoHeader.headersize,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.width,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.height,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.planes,sizeof(uint16_t));
        write(pipelineGS[1],&infoHeader.bpp,sizeof(uint16_t));
        write(pipelineGS[1],&infoHeader.compress,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.imgsize,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.bpmx,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.bpmy,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.colors,sizeof(uint32_t));
        write(pipelineGS[1],&infoHeader.imxtcolors,sizeof(uint32_t));

        for(int i=0;i<infoHeader.imgsize;i++){
            write(pipelineGS[1],&dataImg[i],sizeof(unsigned char));
        }
        //--------------------------------------------------------------------------------------
        
        waitpid(pid, &status, 0);//Esperamos hasta que el proceso pid (hijo) cambie de estado
    }
  return 0;
}
