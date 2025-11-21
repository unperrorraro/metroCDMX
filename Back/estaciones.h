#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct estacion {

  char nombre[128];
  int lineas[3];
  int transbordo;
  int num_ady;
  struct estacion **adyacentes;

} estacion;

enum Atribs { ESTACION, LINEA, TRANSBORDO, ADYACENTES, DESCONOCIDO };
int wich_attribute(char *pal);
void formatear_estacion(estacion *est);
int esta_en_estaciones(const char *nombre, estacion **estaciones);
estacion **crear_estaciones(char *in);
void free_estacion(estacion *e);
void free_estaciones(estacion **es);

#define NUM_EST 35
