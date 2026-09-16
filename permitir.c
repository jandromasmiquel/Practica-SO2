/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "ficheros.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, RED "Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n" RESET);
        return FALLO;
    }

    // Extraer los argumentos
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]);

    // Montar el dispositivo
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED "Error al montar el dispositivo\n" RESET);
        return FALLO;
    }

    // Cambiar permisos del inodo
    if (mi_chmod_f(ninodo, permisos) == FALLO) {
        fprintf(stderr, RED "Error al cambiar permisos del inodo\n" RESET);
        bumount();
        return FALLO;
    }

    printf(GREEN "Permisos del inodo %d cambiados a %o correctamente.\n" RESET, ninodo, permisos);

    // Desmontar el dispositivo
    if (bumount() == FALLO) {
        fprintf(stderr, RED "Error al desmontar el dispositivo\n" RESET);
        return FALLO;
    }

    return 0;
}
