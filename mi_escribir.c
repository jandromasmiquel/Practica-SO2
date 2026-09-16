/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return -1;
    }

    // Montar el sistema de archivos
    if (bmount(argv[1]) == -1) {
        perror("Error en bmount");
        return -1;
    }

    // Parámetros
    char *camino = argv[2];
    char *texto = argv[3];
    unsigned int offset = atoi(argv[4]);
    int nbytes = strlen(texto);

    // Comprobación: ¿es un directorio?
    if (camino[strlen(camino) - 1] == '/') {
        fprintf(stderr, "Error: la ruta se corresponde a un directorio\n");
        bumount();
        return -1;
    }

    // Escritura
    fprintf(stderr, "longitud texto: %d\n", nbytes);
    int bytes_escritos = mi_write(camino, texto, offset, nbytes);

    if (bytes_escritos < 0) {
        switch (bytes_escritos) {
            case -1: fprintf(stderr, "Error: fallo en mi_write()\n"); break;
            case -2: fprintf(stderr, "Error: permisos insuficientes de escritura\n"); break;
            default: fprintf(stderr, "Error desconocido: %d\n", bytes_escritos);
        }
        bumount();
        return -1;
    }

    fprintf(stderr, "Bytes escritos: %d\n", bytes_escritos);

    // Desmontar sistema de archivos
    bumount();

    return 0;
}
