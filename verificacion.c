/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "verificacion.h"
#include "directorios.h"

#define RUTA_PRUEBA "/prueba.dat"
#define RUTA_INFORME "/informe.txt"
#define TAM_RUTA 1024

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: ./verificacion <nombre_dispositivo> <directorio_simulación/>\n");
        return EXIT_FAILURE;
    }

    if (bmount(argv[1]) < 0)
    {
        perror("Error en bmount");
        return EXIT_FAILURE;
    }

    char *dir_sim = argv[2];
    if (dir_sim[strlen(dir_sim) - 1] != '/')
    {
        fprintf(stderr, "Error: el directorio debe terminar en '/'\n");
        bumount();
        return EXIT_FAILURE;
    }

    struct STAT stat;
    if (mi_stat(dir_sim, &stat) < 0)
    {
        fprintf(stderr, "Error al hacer stat del directorio de simulación\n");
        bumount();
        return EXIT_FAILURE;
    }

    int numentradas = stat.tamEnBytesLog / sizeof(struct entrada);
    printf("dir_sim: %s\n", dir_sim);
    printf("numentradas: %d NUMPROCESOS: %d\n", numentradas, NUMPROCESOS);

    if (numentradas != NUMPROCESOS)
    {
        fprintf(stderr, "ERROR: El número de entradas no coincide con NUMPROCESOS\n");
        bumount();
        return EXIT_FAILURE;
    }

    // Crear informe.txt
char ruta_informe[TAM_RUTA];
snprintf(ruta_informe, sizeof(ruta_informe), "%sinforme.txt", dir_sim);

if (mi_creat(ruta_informe, 7) < 0) {
    fprintf(stderr, "Error al crear o truncar informe.txt\n");
    bumount();
    return EXIT_FAILURE;
}

    struct entrada entradas[NUMPROCESOS];
    if (mi_read_f(stat.ninodo, entradas, 0, sizeof(struct entrada) * NUMPROCESOS) < 0)
    {
        fprintf(stderr, "Error al leer entradas del directorio de simulación\n");
        bumount();
        return EXIT_FAILURE;
    }

    int contador = 1;

    for (int i = 0; i < NUMPROCESOS; i++)
    {

        struct INFORMACION info;
        memset(&info, 0, sizeof(info));

        if (strncmp(entradas[i].nombre, "proceso_", 8) != 0)
        {
            continue;
        }
        info.pid = atoi(entradas[i].nombre + 8);

        char ruta_fichero[TAM_RUTA];
        snprintf(ruta_fichero, sizeof(ruta_fichero), "%s%s%s", dir_sim, entradas[i].nombre, RUTA_PRUEBA);

        off_t offset = 0;
        struct REGISTRO reg;
        struct STAT st;
        int validas = 0, primera = 1;

        mi_stat(ruta_fichero, &st);
        off_t fin = st.tamEnBytesLog;

        while (offset < fin)
        {
            memset(&reg,0,sizeof(struct REGISTRO));
            if (mi_read(ruta_fichero, &reg, offset, sizeof(struct REGISTRO)) >0)
            {
                if (reg.pid == info.pid)
                {
                     validas++;

                    if (primera)
                    {
                        info.PrimeraEscritura = reg;
                        info.UltimaEscritura = reg;
                        info.MenorPosicion = reg;
                        info.MayorPosicion = reg;
                        primera = 0;
                    }
                    else
                    {
                        if (reg.nEscritura < info.PrimeraEscritura.nEscritura)
                            info.PrimeraEscritura = reg;
                        if (reg.nEscritura > info.UltimaEscritura.nEscritura)
                            info.UltimaEscritura = reg;
                        if (reg.nRegistro < info.MenorPosicion.nRegistro)
                            info.MenorPosicion = reg;
                        if (reg.nRegistro > info.MayorPosicion.nRegistro)
                            info.MayorPosicion = reg;
                    }

                    
                }
            }
           
            offset += sizeof(struct REGISTRO);
        }

        info.nEscrituras = validas;

        printf("[%d| %d escrituras validadas en %s]\n", contador++, validas, ruta_fichero);

        char salida[1024];
        char fecha[64];
        int bytes = 0;

        bytes += sprintf(salida + bytes, "\nPID: %d\n", info.pid);
        bytes += sprintf(salida + bytes, "Numero de escrituras: %d\n", info.nEscrituras);

        strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", localtime(&info.PrimeraEscritura.fecha));
        bytes += sprintf(salida + bytes, "Primera Escritura\t%2d\t%6d\t%s\n",
                         info.PrimeraEscritura.nEscritura, info.PrimeraEscritura.nRegistro, fecha);

        strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", localtime(&info.UltimaEscritura.fecha));
        bytes += sprintf(salida + bytes, "Ultima Escritura \t%2d\t%6d\t%s\n",
                         info.UltimaEscritura.nEscritura, info.UltimaEscritura.nRegistro, fecha);

        strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", localtime(&info.MenorPosicion.fecha));
        bytes += sprintf(salida + bytes, "Menor Posicion  \t%2d\t%6d\t%s\n",
                         info.MenorPosicion.nEscritura, info.MenorPosicion.nRegistro, fecha);

        strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", localtime(&info.MayorPosicion.fecha));
        bytes += sprintf(salida + bytes, "Mayor Posicion  \t%2d\t%6d\t%s\n",
                         info.MayorPosicion.nEscritura, info.MayorPosicion.nRegistro, fecha);

        struct STAT sti;
        if (mi_stat(ruta_informe, &sti) < 0)
        {
            fprintf(stderr, "Error al hacer stat del informe.txt\n");
            bumount();
            return EXIT_FAILURE;
        }
        mi_write(ruta_informe, salida, sti.tamEnBytesLog, bytes);

    }

    bumount();
    return EXITO;
}
