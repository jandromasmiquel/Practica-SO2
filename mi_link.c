/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include  "directorios.h"


int main(int argc, char **argv) {

    char *ruta_fichero_original =argv[2];
     char *ruta_enlace=argv[3];

  if (argc != 4) {
        fprintf(stderr, RED"Sintaxis: ./mi_link <disco> </ruta_fichero_original> </ruta_enlace>\n");
        fprintf(stderr, RESET);
        return FALLO;
    }

    //montamos el disco 
    if (bmount(argv[1]) == FALLO) return FALLO;

    
    if (mi_link(ruta_fichero_original, ruta_enlace) == FALLO ) {
        fprintf(stderr, RED"Error al crear el enlace físico\n");
        fprintf(stderr,RESET);
        return FALLO;
    }

    bumount();
return EXITO;
}