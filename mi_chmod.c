/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

int main(int argc, char **argv)
{

    if (argc != 4)
    {
        fprintf(stderr, RED "./mi_chmod <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    
    unsigned int npermisos = atoi(argv[2]);
     const char *camino = argv[3];
    // Montar el dispositivo
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, RED "Error al montar el dispositivo\n");
        return FALLO;
    }

    // comprobamos que los permisos sean validos//
    if (npermisos < 0 || npermisos > 7)
    {
        fprintf(stderr, RED "permisos no validos\n");
        return FALLO;
    }

    mi_chmod(camino, npermisos);
    bumount();
    return EXITO;
}