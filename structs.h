#include<stdint.h>

#ifndef structs
#define structs

typedef struct bmpFileHeader
{
  /* 2 bytes */
  uint32_t size;        /* Tamaño de archivo*/
  uint16_t resv1;       /* Reservado */
  uint16_t resv2;       /* Reservado */
  uint32_t offset;      /* Offset a los datos de la imagen */
} bmpFileHeader;

typedef struct bmpInfoHeader
{
  uint32_t headersize;      /* Tamaño de la cabecera */
  uint32_t width;           /* Ancho */
  uint32_t height;          /* Alto */
  uint16_t planes;          /* Planos de color (Siempre 1) */
  uint16_t bpp;             /* bits por pixel */
  uint32_t compress;        /* compresion */
  uint32_t imgsize;         /* tamaño de los datos de imagen */
  uint32_t bpmx;            /* Resolucion X en bits por metro */
  uint32_t bpmy;            /* Resolucion Y en bits por metro */
  uint32_t colors;          /* Colores usados en la paleta */
  uint32_t imxtcolors;      /* Colores importantes. 0 si son todos */
} bmpInfoHeader;


#endif
