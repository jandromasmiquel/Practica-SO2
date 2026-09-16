/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

int main(int argc, char **argv) {
    char* ruta = argv[2];
    if (argc != 3) {
        fprintf(stderr, RED"Sintaxis: ./mi_rm <disco> </ruta>\n");
        fprintf(stderr,RESET);
        return FALLO;
    }

    //montamos el disco 
    if (bmount(argv[1]) == FALLO) return FALLO;

    if ( mi_unlink(ruta)== FALLO ) {
        fprintf(stderr, RED"Error al borrar archivo/enlace\n");
        fprintf(stderr,RESET);
        return FALLO;
    }

    bumount();
    return EXITO;
}