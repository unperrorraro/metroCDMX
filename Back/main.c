#include "estaciones.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "USAGE: %s [in.json]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  estacion **estaciones = crear_estaciones(argv[1]);
  for (int i = 0; i < NUM_EST; i++) {
    formatear_estacion(estaciones[i]);
  }
  free_estaciones(estaciones);
}
