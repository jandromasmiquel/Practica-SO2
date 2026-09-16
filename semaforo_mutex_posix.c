/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <unistd.h>
#include "semaforo_mutex_posix.h"

sem_t *initSem() {
    sem_unlink(SEM_NAME);
    return sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, SEM_INIT_VALUE);
}

void deleteSem() {
    sem_unlink(SEM_NAME);
}

void waitSem(sem_t *sem) {
    sem_wait(sem);
}

void signalSem(sem_t *sem) {
    sem_post(sem);
}
