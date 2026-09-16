/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"
void mostrar_buscar_entrada(char *camino, char reservar){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    printf("\ncamino: %s, reservar: %d\n", camino, reservar);
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
      mostrar_error_buscar_entrada(error);
    }
    printf("**********************************************************************\n");
    return;
  }


int main(int argc, char **argv)
{
    


    //struct superbloque SB;
    struct superbloque SB;
    bmount(argv[1]);
    bread(posSB,&SB);

    // imprimimos los datos del superbloque//
    printf("****DATOS DEL SUPER BLOQUE****\n\n");

    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf( "posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
    printf( "posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    printf( "posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf( "posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf( "posInodoRaiz = %d\n", SB.posInodoRaiz);

    printf( "posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf( "cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf( "cantInodosLibres = %d\n", SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf( "totInodos = %d\n\n", SB.totInodos);

    printf( "sizeof struct superbloque= %ld\n", sizeof(struct superbloque));
    printf( "sizeof struct inodo = %ld\n\n", sizeof(struct inodo));

   

    /*#if DEBUGN3
    struct tm *ts;
    struct inodo inodo;
    char atime[80];
    char mtime[80];
    char ctime[80];
    char btime[80];
    int ninodo;
   
    int bloqueliberado = reservar_bloque();
    printf( "****RESERVAMOS UN BLOQUE PARA LUEGO LIBERARLO****\n");
    printf("Hemos reservado el bloque n:%d\n",bloqueliberado);
    bread(posSB,&SB);
    printf("SB.cantBloquesLibres: %d\n",SB.cantBloquesLibres);
    liberar_bloque(bloqueliberado);
    bread(posSB,&SB);
    printf("Liberamos el bloque -> SB.cantBloquesLibres: %d\n\n",SB.cantBloquesLibres);

   int posbyte;
   int posbyteAjustado;
   int posbit;
   int nbloqueabs;
   int nbloqueMB;

   fprintf(stderr, BLUE "****MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS****\n\n");

     // primer bit SB
    posbyte =  posSB / 8;
    posbit = posSB % 8;
    posbyteAjustado= posbyte%BLOCKSIZE;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;
   
    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado):%d , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",posSB,posbyte,posbyteAjustado,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "posSB: %d → leer_bit(%d) = %d\n",posSB, posSB,leer_bit(posSB));



    //primer bit MB
    posbyte =  SB.posPrimerBloqueMB / 8;
    posbit = SB.posPrimerBloqueMB % 8;
    posbyteAjustado= posbyte%BLOCKSIZE;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado):%d , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posPrimerBloqueMB,posbyte,posbyteAjustado,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posPrimerBloqueMB: %d → leer_bit(%d) = %d\n",SB.posPrimerBloqueMB,SB.posPrimerBloqueMB ,leer_bit(SB.posPrimerBloqueMB));

    //ultimo bit MB
    posbyte =  SB.posUltimoBloqueMB / 8;
    posbit = SB.posUltimoBloqueMB % 8;
    posbyteAjustado= posbyte%BLOCKSIZE;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado):%d , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posUltimoBloqueMB,posbyte,posbyteAjustado,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posUltimoBloqueMB: %d → leer_bit(%d) = %d\n",SB.posUltimoBloqueMB,SB.posUltimoBloqueMB ,leer_bit(SB.posUltimoBloqueMB));

    //primer bit AI
    posbyte =  SB.posPrimerBloqueAI / 8;
    posbit = SB.posPrimerBloqueAI % 8;
    posbyteAjustado= posbyte%BLOCKSIZE;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;
   
    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado):%d , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posPrimerBloqueAI,posbyte,posbyteAjustado,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posPrimerBloqueAI: %d → leer_bit(%d) = %d\n",SB.posPrimerBloqueAI,SB.posPrimerBloqueAI ,leer_bit(SB.posPrimerBloqueAI));

    //ultimo bit AI
    posbyte =  SB.posUltimoBloqueAI / 8;
    posbit = SB.posUltimoBloqueAI % 8;
    posbyteAjustado= posbyte%BLOCKSIZE;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado):%d , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posUltimoBloqueAI,posbyte,posbyteAjustado,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posUltimoBloqueAI: %d → leer_bit(%d) = %d\n",SB.posUltimoBloqueAI,SB.posUltimoBloqueAI ,leer_bit(SB.posUltimoBloqueAI));

    //primer bit bloqueDatos
    posbyte =  SB.posPrimerBloqueDatos / 8;
    posbit = SB.posPrimerBloqueDatos % 8;
    posbyteAjustado= posbyte%BLOCKSIZE;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado):%d , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posPrimerBloqueDatos,posbyte,posbyteAjustado,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posPrimerBloqueDatos: %d → leer_bit(%d) = %d\n",SB.posPrimerBloqueDatos,SB.posPrimerBloqueDatos ,leer_bit(SB.posPrimerBloqueDatos));
    
    //ultimo bit bloqueDatos
    posbyte = SB.posUltimoBloqueDatos / 8;
    posbit = SB.posUltimoBloqueDatos % 8;
    posbyteAjustado= posbyte%BLOCKSIZE;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado):%d , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posUltimoBloqueDatos,posbyte,posbyteAjustado,posbit,nbloqueMB,nbloqueabs);
    //fprintf(stderr, BLUE "SB.posUltimoBloqueDatos: %d → leer_bit(%d) = %d\n\n",SB.posUltimoBloqueDatos,SB.posUltimoBloqueDatos ,leer_bit(SB.posUltimoBloqueDatos));

   
    ninodo=0;

    fprintf(stderr, BLUE "****DATOS DEL DIRECTORIO RAIZ****\n");
    leer_inodo(ninodo, &inodo);

    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.btime);
    strftime(ctime, sizeof(btime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("Tipo:%c\n Permisos:%d\n ID: %d\n ATIME: %s\n MTIME: %s\n CTIME: %s\n BTIME: %s\n nlinks:%d\n Tamenbyteslog:%d\n numBloquesOcupados:%d\n"
        ,inodo.tipo,inodo.permisos,ninodo, atime, mtime, ctime, btime,inodo.nlinks,inodo.tamEnBytesLog,inodo.numBloquesOcupados);

    printf(RESET);
    #endif

        
    #if DEBUGN4   
        ninodo= reservar_inodo('f',6);
        bread(posSB,&SB);
        printf(BLUE "INODO %d TRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 468.750\n",ninodo);
        traducir_bloque_inodo(ninodo,8,'1');
        printf("\n");
        traducir_bloque_inodo(ninodo,204,'1');
        printf("\n");
        traducir_bloque_inodo(ninodo,30004,'1');
        printf("\n");
        traducir_bloque_inodo(ninodo,400004,'1');
        printf("\n");
        traducir_bloque_inodo(ninodo,468750,'1');
        printf("\n");
       
        printf(BLUE "****DATOS DEL INODO RESERVADO RAIZ****\n");
        leer_inodo(ninodo, &inodo);
    
        ts = localtime(&inodo.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.btime);
        strftime(ctime, sizeof(btime), "%a %Y-%m-%d %H:%M:%S", ts);
        printf("Tipo:%c\n Permisos:%d\n ID: %d\n ATIME: %s\n MTIME: %s\n CTIME: %s\n BTIME: %s\n nlinks:%d\n Tamenbyteslog:%d\n numBloquesOcupados:%d\n"
            ,inodo.tipo,inodo.permisos,ninodo, atime, mtime, ctime, btime,inodo.nlinks,inodo.tamEnBytesLog,inodo.numBloquesOcupados);

        printf("SB.posPrimerInodoLibre:%d\n",SB.posPrimerInodoLibre);    
    
        printf(RESET);
    #endif    
   
   // bumount();


    #if DEBUGN7
    mostrar_buscar_entrada("pruebas/", 1); //ERROR_CAMINO_INCORRECTO
    mostrar_buscar_entrada("/pruebas/", 0); //ERROR_NO_EXISTE_ENTRADA_CONSULTA
    mostrar_buscar_entrada("/pruebas/docs/", 1); //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
    mostrar_buscar_entrada("/pruebas/", 1); // creamos /pruebas/
    mostrar_buscar_entrada("/pruebas/docs/", 1); //creamos /pruebas/docs/
    mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
    mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1);  
    //ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
    mostrar_buscar_entrada("/pruebas/", 1); //ERROR_ENTRADA_YA_EXISTENTE
    mostrar_buscar_entrada("/pruebas/docs/doc1", 0); //consultamos /pruebas/docs/doc1
    mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //ERROR_ENTRADA_YA_EXISTENTE
    mostrar_buscar_entrada("/pruebas/casos/", 1); //creamos /pruebas/casos/
    mostrar_buscar_entrada("/pruebas/docs/doc2", 1); //creamos /pruebas/docs/doc2

    #endif*/


}
