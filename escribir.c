/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/

#include "ficheros.h"




int main(int argc, char **argv) {

    int OFFSETS[5] = {9000, 209000, 30725000, 409605000, 480000000};

    if (argc != 4) {
        fprintf(stderr,RED "Sintaxis: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        printf(RESET);
        return FALLO;
    }

    char *texto = argv[2];
    int diferentes_inodos = atoi(argv[3]);
   
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar el dispositivo\n");
        return FALLO;
    }

    int tamText = strlen(texto);
    printf(BLUE"Longuitud del texto: %d\n\n", tamText);    
    int ninodo;
    struct STAT stat;
    char bufferTexto[tamText];
    memset(bufferTexto,0,tamText);
    
    strcpy(bufferTexto, texto);
    if (diferentes_inodos == 0) {
        // Reservar un solo inodo para todos los offsets
        ninodo = reservar_inodo('f', 6);
        if (ninodo == FALLO) {
            fprintf(stderr, RED "Error al reservar inodo\n");
            printf(RESET);
            bumount();
            return FALLO;
        }
        for (int i = 0; i < 5; i++) {
            printf(BLUE"Num inodo reservado: %d\n", ninodo);
            printf(BLUE"offset: %u\n", OFFSETS[i]);
            // Escribir el texto en el offset actual
            int bytesEscritos = mi_write_f(ninodo, bufferTexto, OFFSETS[i], tamText);

            if (bytesEscritos == FALLO) {
                fprintf(stderr, RED "Error al escribir en el inodo\n");
                printf(RESET);
                return FALLO;
            }
            printf(BLUE"Bytes escritos: %d\n", bytesEscritos);
            
            // Obtener información del inodo
            if (mi_stat_f(ninodo, &stat)) {
                fprintf(stderr, RED"Error al obtener stat del inodo\n");
                printf(RESET);
            }
            printf(BLUE"stat.tamEnBytesLog=%d\n", stat.tamEnBytesLog);
            printf(BLUE"stat.numBloquesOcupados=%d\n\n", stat.numBloquesOcupados);
            printf(RESET);
        }
    } else {
        // Reservar un inodo diferente para cada offset
        for (int i = 0; i <  5; i++) {
            ninodo = reservar_inodo('f', 6);
            if (ninodo == FALLO) {
                fprintf(stderr, RED "Error al reservar inodo para offset %u\n", OFFSETS[i]);
                printf(RESET);
            }
            printf(BLUE"Nº inodo reservado: %d\n", ninodo);
            printf("offset: %u\n", OFFSETS[i]);
            printf(RESET);
            
            // Escribir el texto en el offset actual
            int bytes_escritos = mi_write_f(ninodo, bufferTexto, OFFSETS[i], tamText);
            if (bytes_escritos == FALLO) {
                fprintf(stderr, RED "Error al escribir en el inodo\n");
                printf(RESET);
            }
            printf(BLUE"Bytes escritos: %d\n", bytes_escritos);
            printf(RESET);
            // Obtener información del inodo
            if (mi_stat_f(ninodo, &stat)) {
                fprintf(stderr, RED "Error al obtener stat del inodo\n");
                printf(RESET);
            }
            printf(BLUE"stat.tamEnBytesLog=%u\n", stat.tamEnBytesLog);
            printf("stat.numBloquesOcupados=%u\n\n", stat.numBloquesOcupados);
            printf(RESET);
        }
    }

    // Desmontar el dispositivo
    if (bumount() == FALLO) {
        fprintf(stderr, RED"Error al desmontar el dispositivo\n");
        printf(RESET);
        return FALLO;
    }

    return 0;
}