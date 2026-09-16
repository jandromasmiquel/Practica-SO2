/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/

#include "ficheros.h"


int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Sintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return FALLO;
    }
    
    // Montar dispositivo
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, "Error al montar el dispositivo\n");
        return FALLO;
    }
    
    unsigned int ninodo = atoi(argv[2]);
    unsigned int nbytes = atoi(argv[3]);
    
    int resultado;
    if (nbytes == 0) {
        resultado = liberar_inodo(ninodo);
        printf("Inodo %d liberado\n", resultado);
    } else {
        resultado = mi_truncar_f(ninodo, nbytes);
        printf("Bloques liberados: %d\n", resultado);
        
        // Mostrar información del inodo
        struct STAT stat;
        if (mi_stat_f(ninodo, &stat) == FALLO) {
            fprintf(stderr, "Error al obtener stat del inodo\n");
            return FALLO;
        }

        
        
        printf("tamEnBytesLog: %u\n", stat.tamEnBytesLog);
        printf("numBloquesOcupados: %u\n", stat.numBloquesOcupados);
    }
    
    // Desmontar dispositivo
    if (bumount() == FALLO) {
        fprintf(stderr, "Error al desmontar el dispositivo\n");
        return FALLO;
    }
    
    return 0;
}