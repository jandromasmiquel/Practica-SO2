/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "ficheros.h"
#define  tamBuffer 1500

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, RED "Sintaxis: leer <nombre_dispositivo> <ninodo>\n");
        return FALLO;
    }

    
    int ninodo = atoi(argv[2]);

    // Montar el dispositivo
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED"Error al montar el dispositivo\n");
        return FALLO;
    }

    struct STAT stat;
    if (mi_stat_f(ninodo, &stat) == FALLO) {
        fprintf(stderr, RED "Error al obtener información del inodo\n");
        bumount();
        return FALLO;
    }

    // Buffer para la lectura
    char buffer_texto[tamBuffer];// Tamaño del buffer de lectura para leer bloque a bloque 
    char texto[tamBuffer];
    memset(texto, 0, tamBuffer);
    int offset = 0;
    int bytesLeidos = 0;
    int totBytesLeidos = 0;

    // Leer el contenido del inodo en bloque
        memset(buffer_texto, 0, tamBuffer);  // Limpiar el buffer antes de cada lectura
        bytesLeidos = mi_read_f(ninodo, buffer_texto, offset, tamBuffer);
        totBytesLeidos +=bytesLeidos;
        
        if (bytesLeidos == FALLO) {
            fprintf(stderr,RED "Error al leer del inodo\n");
            bumount();
            return FALLO;
        }

        while (bytesLeidos > 0 ){

            write(1,buffer_texto,bytesLeidos);
            memset(buffer_texto, 0, tamBuffer);  // Limpiar el buffer antes de cada lectura
            offset += tamBuffer;
            bytesLeidos=mi_read_f(ninodo, buffer_texto, offset, tamBuffer);
            if (bytesLeidos == FALLO) {
                fprintf(stderr, RED"Error al leer del inodo\n");
                bumount();
                return FALLO;
            }
            totBytesLeidos +=bytesLeidos;
         }
         
     // Mostrar estadísticas por stderr para que no afecte a la redirección
    fprintf(stderr, BLUE"\n\ntotal_leidos %d\ntamEnBytesLog %d\n",totBytesLeidos, stat.tamEnBytesLog);
         

    // Desmontar el dispositivo
    if (bumount() == FALLO) {
        fprintf(stderr, RED"Error al desmontar el dispositivo\n");
        return FALLO;
    }

    return EXITO;
}