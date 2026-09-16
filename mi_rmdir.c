
/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, RED "Sintaxis: ./mi_rmdir <disco> </ruta_directorio>\n" RESET);
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO) return FALLO;
    
    char ruta[1024];
    strncpy(ruta, argv[2], sizeof(ruta));
    if (ruta[strlen(ruta) - 1] != '/') {
        strcat(ruta, "/");
    }

    
    struct STAT stat;
    if (mi_stat(ruta, &stat) == FALLO) {
        fprintf(stderr, RED "Error: No se pudo obtener información del directorio\n" RESET);
        bumount();
        return FALLO;
    }

    if (stat.tipo != 'd') {
        fprintf(stderr, RED "Error: La ruta no corresponde a un directorio\n" RESET);
        bumount();
        return FALLO;
    }

    if (stat.tamEnBytesLog > 0) {
        fprintf(stderr, RED "Error: El directorio no está vacío\n" RESET);
        bumount();
        return FALLO;
    }


    if (mi_unlink(ruta) == FALLO) {
        fprintf(stderr, RED "Error al eliminar el directorio\n" RESET);
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}
    