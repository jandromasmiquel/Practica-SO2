/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "ficheros.h"

int initSB(unsigned int nbloques, unsigned int ninodos);

int main(int argc, char **argv){

    //obtenemos el número de bloques //
    int nbloques = atoi(argv[2]);

    //obtenemos el número de inodos//
    int ninodos = nbloques/4;

    //inicializamos el buffer que utilizaremos para escribir//
    unsigned char buff[BLOCKSIZE];
    memset(buff, 0, BLOCKSIZE);

    //montamos nuestro disco// 
    if(bmount(argv[1])==FALLO){
        perror(RED "Error al montar el disco");
        printf(RESET);
        return FALLO;

    };

    //Inicializamos los bloques en el disco virtual//
    for (size_t i = 1; i <= nbloques; i++)
    {
        bwrite(i,buff);
    }
  
    //inicializamos el superbloque, Array de inodos y Mapa de bits //
    initSB(nbloques,ninodos);
    initMB();
    initAI();
    //Reservamos un inodo
    reservar_inodo ('d', 7);
    //Desmontamos el disco//
    bumount();
    return 0;

}