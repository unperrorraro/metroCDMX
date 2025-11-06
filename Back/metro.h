typedef struct estacion {

  char nombre[128];
  int lineas[3];
  int transbordo;
  int num_ady;
  struct estacion** adyacentes;
  
} estacion;


enum Atribs { ESTACION, LINEA, TRANSBORDO, ADYACENTES, DESCONOCIDO };
int wich_attribute(char *pal);
void formatear_estacion(estacion *est);

#define NUM_EST 35
