#include "structs.h"
#include "functions.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/**
* Funcion principal
* @param argc cantidad de parametros recibidos por consola
* @param argv arreglo de parametros recibidos
* @return entero
*/
int main(int argc, char** argv){
  int a,pid,status,numImages,umbralBin,umbralClas,i,imprimir=0;
  char* charUmbralBin=(char*)malloc(sizeof(char)*16);
  char* charUmbralClas=(char*)malloc(sizeof(char)*16);
  char* charImprimir=(char*)malloc(sizeof(char)*16);
  char* charNumImages=(char*)malloc(sizeof(char)*16);

  //Ciclo while para uso de funcion getopt
  while ((a = getopt (argc, argv, "bc:u:n:")) != -1){
    switch(a){
      case 'c':
        if(isInt(optarg)){
          numImages=atoi(optarg);
          //printf("The number of images to process are: %i\n",numImages);
        }
        else{
          printf("Not valid input -c (only integer digits!)\n");
          return 0;
        }
        break;
      case 'u':
        if(isInt(optarg)){
          umbralBin=atoi(optarg);
          if(umbralBin<0 || umbralBin>255){
            printf("Not valid input -u (only integer [0,255] numbers!)\n");
            return 0;
          }
          //printf("UMBRAL to Bin is: %i\n",umbralBin);
        }
        else{
          printf("Not valid input -u (only integer digits!)\n");
          return 0;
        }
        break;
      case 'n':
        if(isInt(optarg)){
          umbralClas=atoi(optarg);
          if(umbralClas<0 || umbralClas>100){
            printf("Not valid input -n (only integer [0,100] numbers!)\n");
            return 0;
          }
          //printf("UMBRAL to classify is:%i\n",umbralClas);
        }
        else{
          printf("Not valid input -n (only integer digits!)\n");
          return 0;
        }
        break;
      case 'b':
        imprimir=1;
        break;
      case '?':
        if (optopt == 'c' || optopt == 'u' || optopt == 'n')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknow option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Option with unknown character `\\x%x'.\n",optopt);
          return 1;
      default:
        abort();
    }//fin del switch
  }//fin del while

  //ciclo for para procesar las imagenes, una a la vez
  for(i=1;i<numImages+1;i++){

    char* nombreEntrada=malloc(sizeof(char)*22);
    nombreEntrada=setNameInput(i); //setNameInput() function in file functions.c 
    char* nombreSalidaGS=malloc(sizeof(char)*22);
    nombreSalidaGS=setNameOutputGS(i);
    char* nombreSalidaBin=malloc(sizeof(char)*22);
    nombreSalidaBin=setNameOutputBin(i);

    //Si no existe la imagen a procesar el programa termina-------
    FILE* fp=fopen(nombreEntrada,"r");
    if(fp){
      fclose(fp);
    }else{
      printf("File '%s' not found, execution canceled.\n",nombreEntrada);
      return 0;}
    //------------------------------------------------------------
      
    pid=fork();
    if (pid<0){
        printf("Error: child not created\n");
        return 0;
    }
    if(pid==0){
      sprintf(charUmbralBin, "%d", umbralBin);
      sprintf(charUmbralClas, "%d", umbralClas);
      sprintf(charImprimir, "%d", imprimir);
      sprintf(charNumImages, "%d", i);
      //se crea un arreglo de char* para pasar los parametros al siguiente proceso
      char* arguments[]={nombreEntrada,charUmbralBin,charUmbralClas,charImprimir,charNumImages,nombreSalidaGS,nombreSalidaBin,NULL};
      execv("./imageReader",arguments);
    }
    else{
      waitpid(pid, &status, 0);//Esperamos hasta que el proceso pid (hijo) cambie de estado
    }
    free(nombreEntrada);
    free(nombreSalidaGS);
    free(nombreSalidaBin);
  }//fin del ciclo for para procesar las imagenes
  return 0;
}
