/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

#define TAMBUFFER BLOCKSIZE * 4 // para pruebas grandes

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: ./mi_cat <disco> </ruta_fichero>\n");
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO)
    {
        perror("Error en bmount");
        return FALLO;
    }

    const char *camino = argv[2];
    if (camino[strlen(camino) - 1] == '/')
    {
        fprintf(stderr, "Error: la ruta se corresponde a un directorio\n");
        bumount();
        return FALLO;
    }

    struct STAT stat;
    if (mi_stat(camino, &stat) == -1)
    {
        fprintf(stderr, "Error: no se pudo obtener stat del fichero\n");
        bumount();
        return FALLO;
    }

    if (stat.tipo != 'f')
    {
        fprintf(stderr, "Error: '%s' no es un fichero\n", camino);
        bumount();
        return FALLO;
    }

    char buffer[TAMBUFFER];
    memset(buffer, 0, sizeof(buffer));
    int offset = 0;
    int total_leidos = 0;
    int leidos;
    
    while ((leidos = mi_read(camino, buffer, offset, TAMBUFFER)) > 0)
    {
        write(1, buffer, leidos);
        total_leidos += leidos;
        offset += leidos;
        memset(buffer, 0, TAMBUFFER); // limpiar después de usarlo
    }

    fprintf(stderr, "\n\nTotal_leidos %d\n", total_leidos);

    if (leidos < 0)
    {
        fprintf(stderr, "Error en la lectura del fichero\n");
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}
