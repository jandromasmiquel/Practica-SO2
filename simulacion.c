/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "directorios.h"
#include "semaforo_mutex_posix.h"
#include "simulacion.h"

int acabados = 0;
char simul_dir[100];



void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        acabados++;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Sintaxis: ./simulacion <disco>\n");
        exit(EXIT_FAILURE);
    }

    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "Error en bmount\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, reaper);

    // Crear nombre del directorio simul_aaaammddhhmmss
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(simul_dir, "/simul_%04d%02d%02d%02d%02d%02d/",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    
    int error = mi_creat(simul_dir, 6);
    
    if (error < 0)
    {
        fprintf(stderr, "Error al crear el directorio de simulación\n");
        mostrar_error_buscar_entrada(error);
        bumount();
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Directorio de simulación: %s\n", simul_dir);

    for (int i = 0; i < NUMPROCESOS; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // HIJO
            if (bmount(argv[1]) < 0)
                exit(EXIT_FAILURE);

            char dir_proceso[200];
            sprintf(dir_proceso, "%sproceso_%d/", simul_dir, getpid());
            int error = mi_creat(dir_proceso, 6);
            if (error < 0) {
                fprintf(stderr,"Error al crear directorio del proceso: %s\n", dir_proceso);
                mostrar_error_buscar_entrada(error);
                bumount();
                exit(EXIT_FAILURE);
            }


            char fichero[300];
            sprintf(fichero, "%sprueba.dat", dir_proceso);

            error = mi_creat(fichero, 6);
            if (error < 0) {
                fprintf(stderr,"Error al crear fichero del proceso: %s\n", fichero);
                mostrar_error_buscar_entrada(error); 
                bumount();
                exit(EXIT_FAILURE);
            }

           //mi_truncar(fichero, 0);

            srand(time(NULL) + getpid());

            struct REGISTRO reg;
            for (int j = 1; j <= NUMESCRITURAS; j++)
            {
                reg.fecha = time(NULL);
                reg.pid = getpid();
                reg.nEscritura = j;
                reg.nRegistro = rand() % REGMAX;

                off_t offset = reg.nRegistro * sizeof(struct REGISTRO);
                mi_waitSem();
                mi_write(fichero, &reg, offset, sizeof(struct REGISTRO));
                mi_signalSem();

                //fprintf(stderr, "[simulacion.c → Escritura %d en %s]\n", j, fichero);
                usleep(50000); // 0.05 segundos
            }

            fprintf(stderr, "\n[Proceso %d: Completadas %d escrituras en %s]\n",
                    (i+1), NUMESCRITURAS, fichero);


            bumount();
            exit(0);
        }
        usleep(150000); // 0.15 segundos entre procesos
    }

    // PADRE
    while (acabados < NUMPROCESOS)
    {
        pause(); // Espera a que llegue SIGCHLD
    }

    bumount();
    return 0;
}
