/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

int main(int argc, char **argv) {
    unsigned int npermisos = atoi(argv[2]);
    const char *camino = argv[3];

    if (argc != 4) {
        fprintf(stderr, RED "Sintaxis: ./mi_touch <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    // Comprobamos que los permisos sean válidos (0-7)
    if(npermisos < 0 || npermisos > 7) {
        fprintf(stderr, RED "Error: permisos no válidos (deben ser 0-7)\n");
        return FALLO;
    }

    // Verificamos que la ruta no termine en '/' (no es un directorio)
    if(camino[strlen(camino)-1] == '/') {
        fprintf(stderr, RED "Error: la ruta no puede terminar en '/' (mi_touch es solo para archivos)\n");
        return FALLO;
    }

    // Montar el dispositivo
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED "Error al montar el dispositivo\n");
        return FALLO;
    }

    // Crear el archivo
    int resultado = mi_creat(camino, npermisos);
    if (resultado < 0) {
        mostrar_error_buscar_entrada(resultado);
        bumount();
        return FALLO;
    }

    // Desmontar el dispositivo
    bumount();
    return EXITO;
}