#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "metro.h"

int wich_attribute(char *pal) {
    if (strcmp(pal, "estacion") == 0) return ESTACION;
    if (strcmp(pal, "linea") == 0) return LINEA;
    if (strcmp(pal, "transbordo") == 0) return TRANSBORDO;
    if (strcmp(pal, "adyacentes") == 0) return ADYACENTES;
    return DESCONOCIDO;
}

void skip_spaces(FILE *fptr, int *c) {
    while (isspace(*c)) *c = getc(fptr);
}

void formatear_estacion(estacion *est){

  printf("Nombre : %s| \n Líneas : [ %d, %d, %d ] \n transbordo : %d \n numero adyacentes:  %d\n",est->nombre, est->lineas[0] , est->lineas[1] , est->lineas[2],est->transbordo,est->num_ady );
}
void formatear_estacion2(estacion *est){

  printf("Nombre : %s| \n Puntero : %p \n Líneas : [ %d, %d, %d ] \n transbordo : %d \n numero adyacentes:  %d\n  adyacentes : ["
         ,est->nombre,est, est->lineas[0] , est->lineas[1] , est->lineas[2],est->transbordo,est->num_ady);
  for (int i = 0;i < est->num_ady;i ++) {
    printf("\n \t %s -> %p,",(est->adyacentes[i])->nombre,est->adyacentes[i]);
  }
  printf("\n \t]\n");
}



int esta_en_estaciones(const char* nombre , estacion** estaciones){
  
  for (int i = 0;i < NUM_EST;i ++) {
    if(strcmp(nombre, estaciones[i]->nombre) == 0){
     
      return i;}
  }
  return -1;

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s [in.json]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fptr = fopen(argv[1], "r");
    if (!fptr) {
        fprintf(stderr, "ERROR: no se pudo abrir el archivo %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int c = getc(fptr);
    skip_spaces(fptr, &c);

    if (c != '[') {
        fprintf(stderr, "ERROR: el archivo JSON debe comenzar con '['\n");
        exit(EXIT_FAILURE);
    }
    estacion* estaciones_array[NUM_EST];
    for (int i =0; i < NUM_EST;i++) {
      estaciones_array[i] = malloc(sizeof(estacion));
    }

    int estacion_num = 0;

    while ((c = getc(fptr)) != EOF) {
        skip_spaces(fptr, &c);
        if (c == ']') break;
        if (c != '{') continue;

        estacion* est = estaciones_array[estacion_num];
        estacion_num++;


        printf("\n=== Estación %d ===\n", estacion_num);

        while ((c = getc(fptr)) != EOF) {
            skip_spaces(fptr, &c);
            if (c == '}') break;
            if (c != '"') continue;

            // Leer clave
            char key[32];
            int klen = 0;
            while ((c = getc(fptr)) != EOF && c != '"') {
                if (klen < 31) key[klen++] = c;
            }
            key[klen] = '\0';
            skip_spaces(fptr, &c);

            if ((c = getc(fptr)) != ':') {
                fprintf(stderr, "ERROR: falta ':' despues de \"%s\"\n", key);
                exit(EXIT_FAILURE);
            }

            int attr = wich_attribute(key);
            c = getc(fptr);
            skip_spaces(fptr, &c);

            switch (attr) {
                case ESTACION: {
                    if (c != '"') {
                        fprintf(stderr, "ERROR: estacion debe ser string\n");
                        exit(EXIT_FAILURE);
                    }
                    char val[128];
                    int vlen = 0;
                    while ((c = getc(fptr)) != EOF && c != '"') {
                        if (vlen < 127) val[vlen++] = c;
                    }
                    val[vlen] = '\0';
                    printf("Nombre: %s\n", val);
                    strcpy(est->nombre ,val);
                    break;
                }

                case LINEA: {
                    if (c != '[') {
                        fprintf(stderr, "ERROR: linea debe ser lista\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("Líneas: ");
                    int first = 1;
                    int i_num = 0;
                    while ((c = getc(fptr)) != EOF) {
                        if (isdigit(c)) {
                            int num = c - '0';
                            
                            while (isdigit(c = getc(fptr)))
                                num = num * 10 + (c - '0');
                            if (!first) printf(", ");
                            (est->lineas)[i_num] = num;
                            i_num ++;
                            printf("%d", num);
                            first = 0;
                        }
                        if (c == ']') break;
                    }
                    printf("\n");
                    break;
                }

                case TRANSBORDO: {
                    if (c != '"') {
                        fprintf(stderr, "ERROR: transbordo debe ser string\n");
                        exit(EXIT_FAILURE);
                    }
                    c = getc(fptr);
                    if(c == 'F'){
                      printf("transbordo: false\n");
                     est->transbordo = 0;}
                    else if(c == 'T'){
                      printf("transbordo: true\n");       
                      est->transbordo = 1;}
                    else{  fprintf(stderr, "ERROR: transbordo debe ser true o false\n");
                        exit(EXIT_FAILURE);}

                    if(c = getc(fptr) != '"'){ fprintf(stderr, "ERROR: error al finalizar string\n");
                        exit(EXIT_FAILURE);}
                    break;}
                

                case ADYACENTES: {
                    if (c != '[') {
                        fprintf(stderr, "ERROR: adyacentes debe ser lista\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("Adyacentes: ");
                    int first = 1;
                    int i_ady = 0;
                    while ((c = getc(fptr)) != EOF) {
                        if (c == '"') {
                            char name[128];
                            int nlen = 0;
                            while ((c = getc(fptr)) != EOF && c != '"') {
                                if (nlen < 127) name[nlen++] = c;
                            }
                            name[nlen] = '\0';
                            if (!first) printf(", ");
                            printf("%s", name);
                            i_ady ++;
                            first = 0;
                        }
                        if (c == ']'){est->num_ady = i_ady;
                      est->adyacentes = malloc(sizeof(estacion*) * est->num_ady);
                      if (!est->adyacentes) {
                          fprintf(stderr, "ERROR: no se pudo reservar memoria para adyacentes de %s\n", est->nombre);
                          exit(EXIT_FAILURE);
                      }
                       break;}
                    }
                    printf("\n");
                    break;
                }

                default:
                    fprintf(stderr, "ADVERTENCIA: atributo desconocido \"%s\"\n", key);
                    break;
            }


            // Leer coma o cierre
            c = getc(fptr);
            skip_spaces(fptr, &c);
            if (c == ','){continue;}
            if (c == '}') {break;}
        }

            c = getc(fptr);
        skip_spaces(fptr, &c);
        if (c == ',') continue;
        if (c == ']') {
      //for (int j = 0; j < NUM_EST; j++) { formatear_estacion(estaciones_array[j]);}
      break;}
    }
  // Segunda vuelta para que funcione adyacentes
    rewind(fptr);

      estacion_num = 0;

    while ((c = getc(fptr)) != EOF) {
        skip_spaces(fptr, &c);
        if (c == ']') break;
        if (c != '{') continue;

        estacion* est = estaciones_array[estacion_num];
        estacion_num++;


     //   printf("\n=== Estación %d ===\n", estacion_num);

        while ((c = getc(fptr)) != EOF) {
            skip_spaces(fptr, &c);
            if (c == '}') break;
            if (c != '"') continue;

            // Leer clave
            char key[32];
            int klen = 0;
            while ((c = getc(fptr)) != EOF && c != '"') {
                if (klen < 31) key[klen++] = c;
            }
            key[klen] = '\0';
            skip_spaces(fptr, &c);

            if ((c = getc(fptr)) != ':') {
                fprintf(stderr, "ERROR: falta ':' despues de \"%s\"\n", key);
                exit(EXIT_FAILURE);
            }

            int attr = wich_attribute(key);
            c = getc(fptr);
            skip_spaces(fptr, &c);

            switch (attr) {
                       case ADYACENTES: {
                    if (c != '[') {
                        fprintf(stderr, "ERROR: adyacentes debe ser lista\n");
                        exit(EXIT_FAILURE);
                    }
                   // printf("Adyacentes: ");
                    int first = 1;
                    int i_ady = 0;
                    while ((c = getc(fptr)) != EOF) {
                        if (c == '"') {
                            char name[128];
                            int nlen = 0;
                            while ((c = getc(fptr)) != EOF && c != '"') {
                                if (nlen < 127) name[nlen++] = c;
                            }
                            name[nlen] = '\0';
                          //  if (!first) printf(", ");
                          //  printf("%s", name);
                            int index = esta_en_estaciones(name,estaciones_array);
                            if(index == -1){printf("ERROR");}
                            est->adyacentes[i_ady] = estaciones_array[index];
                            i_ady ++;
                            first = 0;
                        }
                        if (c == ']'){ break;}
                    }
                    break;
                }

                default:{
                   // fprintf(stderr, "ADVERTENCIA: atributo desconocido \"%s\"\n", key);
                    long ultima_coma = -1;
                    int c;
                    
                    while ((c = getc(fptr)) != '\n' && c != EOF) {
                        if (c == ',') {
                            ultima_coma = ftell(fptr) - 1;  // guarda posición de la coma
                        }
                    }
                    
                    // Si encontramos alguna coma, reposicionamos
                    if (ultima_coma != -1) {
                        fseek(fptr, ultima_coma, SEEK_SET);
                    }
                break;}
            }


            // Leer coma o cierre
            c = getc(fptr);
            skip_spaces(fptr, &c);
            if (c == ','){continue;}
            if (c == '}') {break;}
        }

            c = getc(fptr);
        skip_spaces(fptr, &c);
        if (c == ',') continue;
        if (c == ']') {for (int j = 0; j < NUM_EST; j++) { formatear_estacion2(estaciones_array[j]);
        }break;}
    }


    fclose(fptr);
    printf("\nArchivo JSON leído correctamente.\n");
    return EXIT_SUCCESS;
}

