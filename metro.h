typedef struct estacion {

  char* nombre;
  int* lineas;
  int transbordo;
  struct estacion** adyacentes;
  
} estacion;
