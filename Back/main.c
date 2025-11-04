#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum Atribs { ESTACION, LINEA, TRANSBORDO, ADYACENTES, DESCONOCIDO };

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

    int estacion_num = 0;

    while ((c = getc(fptr)) != EOF) {
        skip_spaces(fptr, &c);
        if (c == ']') break;
        if (c != '{') continue;

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
                    break;
                }

                case LINEA: {
                    if (c != '[') {
                        fprintf(stderr, "ERROR: linea debe ser lista\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("Líneas: ");
                    int first = 1;
                    while ((c = getc(fptr)) != EOF) {
                        if (isdigit(c)) {
                            int num = c - '0';
                            while (isdigit(c = getc(fptr)))
                                num = num * 10 + (c - '0');
                            if (!first) printf(", ");
                            printf("%d", num);
                            first = 0;
                        }
                        if (c == ']') break;
                    }
                    printf("\n");
                    break;
                }

                case TRANSBORDO: {
                    char word[10];
                    int wlen = 0;
                    if (isalpha(c)) {
                        word[wlen++] = c;
                        while (isalpha(c = getc(fptr)) && wlen < 9)
                            word[wlen++] = c;
                        word[wlen] = '\0';
                        printf("Transbordo: %s\n", word);
                    } else {
                        fprintf(stderr, "ERROR: valor de transbordo inválido\n");
                        exit(EXIT_FAILURE);
                    }
                    break;
                }

                case ADYACENTES: {
                    if (c != '[') {
                        fprintf(stderr, "ERROR: adyacentes debe ser lista\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("Adyacentes: ");
                    int first = 1;
                    while ((c = getc(fptr)) != EOF) {
                        if (c == '"') {
                            char name[64];
                            int nlen = 0;
                            while ((c = getc(fptr)) != EOF && c != '"') {
                                if (nlen < 63) name[nlen++] = c;
                            }
                            name[nlen] = '\0';
                            if (!first) printf(", ");
                            printf("%s", name);
                            first = 0;
                        }
                        if (c == ']') break;
                    }
                    printf("\n");
                    break;
                }

                default:
                    fprintf(stderr, "ADVERTENCIA: atributo desconocido \"%s\"\n", key);
                    break;
            }

            // Leer coma o cierre
            skip_spaces(fptr, &c);
            if (c == ',') continue;
            if (c == '}') break;
        }

        skip_spaces(fptr, &c);
        if (c == ',') continue;
        if (c == ']') break;
    }

    fclose(fptr);
    printf("\nArchivo JSON leído correctamente.\n");
    return EXIT_SUCCESS;
}

