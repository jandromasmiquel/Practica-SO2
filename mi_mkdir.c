/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include  "directorios.h"




int main(int argc, char **argv) {

    unsigned int npermisos = atoi(argv[2]);
    const char *camino = argv[3];

    if (argc != 4) {
        fprintf(stderr, RED "Sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    //comprobamos que los permisos sean validos// 
    if(npermisos<0 || npermisos >7){
        fprintf(stderr, RED "permisos no validos\n");
        return FALLO;
    }

    // Montar el dispositivo
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED"Error al montar el dispositivo\n");
        return FALLO;
    }

    mi_creat(camino,npermisos);
    bumount();
    return EXITO;
}