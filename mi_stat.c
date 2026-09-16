/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

int main(int argc, char **argv)
{

    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    char btime[80];
    struct STAT stat;

    if (argc != 3)
    {
        fprintf(stderr, RED "./mi_stat <disco> </ruta>\n");
        return FALLO;
    }

    // Montar el dispositivo
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, RED "Error al montar el dispositivo\n");
        return FALLO;
    }
    const char *camino = argv[2];

    if (mi_stat(camino, &stat) == FALLO)
    {
        bumount();
        return FALLO;
    }
    ts = localtime(&stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);

    ts = localtime(&stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);

    ts = localtime(&stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    ts = localtime(&stat.btime);
    strftime(btime, sizeof(btime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("N inodo:%d\n Tipo:%c\n Permisos:%d\n ID: %d\n ATIME: %s\n MTIME: %s\n CTIME: %s\n BTIME: %s\n nlinks:%d\n Tamenbyteslog:%d\n numBloquesOcupados:%d\n", stat.ninodo, stat.tipo, stat.permisos, stat.ninodo, atime, mtime, ctime, btime, stat.nlinks, stat.tamEnBytesLog, stat.numBloquesOcupados);

    printf(RESET);
    bumount();

}