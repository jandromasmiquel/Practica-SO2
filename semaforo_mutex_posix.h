/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include <semaphore.h>

#ifndef SEMAFORO_MUTEX_POSIX_H
#define SEMAFORO_MUTEX_POSIX_H


#define SEM_NAME "/mymutex"
#define SEM_INIT_VALUE 1

sem_t *initSem();
void deleteSem();
void waitSem(sem_t *sem);
void signalSem(sem_t *sem);


#endif
