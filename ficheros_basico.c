/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "fichero_basico.h"

int bitSobrantes(int n); // Funcion que devuelve valor decimal de los bits sobrantes//

/*Función que devuelve en numero de bloques necesarios para el Mapa de Bits */
int tamMB(unsigned int nbloques)
{
    int tamMB = (nbloques / 8) / BLOCKSIZE;

    if (((nbloques / 8) % BLOCKSIZE) == 0)
    {
        return tamMB;
    }
    else
    {
        return tamMB + 1;
    }
}

/*Función que calcula el tamaño de bloques del array de inodos*/
// paso de parametros desde mi_mkfs ninodos=nbloques/4//
int tamAI(unsigned int ninodos)
{

    int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
    if (((ninodos * INODOSIZE) % BLOCKSIZE) == 0)
    {
        return tamAI;
    }
    else
    {
        return tamAI + 1;
    }
}

/*Función para inicializar el super bloque*/
int initSB(unsigned int nbloques, unsigned int ninodos)
{

    struct superbloque SB;

    // Posiciones del Mapa de bits //
    SB.posPrimerBloqueMB = 0 + 1; // posSB = 0 posMB = 1

    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;

    // Posiciones del array de inodos//
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;

    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;

    // Posiciones del bloque de datos//
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;

    SB.posUltimoBloqueDatos = nbloques - 1;

    // posiciones de inodos//
    SB.posInodoRaiz = 0;

    SB.posPrimerInodoLibre = 0;

    // Cantidad de bloques e inodos libres  inicialmente//

    SB.cantBloquesLibres = nbloques;

    SB.cantInodosLibres = ninodos;

    // Cantidad totales de inodos y bloques //

    SB.totBloques = nbloques;

    SB.totInodos = ninodos;

    // Escribimos el superbloque en el disco virtual//
    // se puede escribir de esta manera ya que el super bloque ocupa 1 bloque//
    if (bwrite(posSB, &SB) == FALLO)
    {
        perror(RED "Error al escribir el SB");
        printf(RESET);
        return FALLO;
    };

    return 0;
}

int initMB()
{
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO)
    {
        fprintf(stderr, "Error leyendo el superbloque\n");
        return -1;
    }

    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB, 0, BLOCKSIZE); // Inicializar todo el buffer a 0

    // Cantidad de bloques a poner a 1
    int bloquesOcupados = tamAI(SB.totInodos) + tamMB(SB.totBloques) + tamSB;
    int bytes1 = bloquesOcupados / 8;
    int bits1 = bloquesOcupados % 8;

    int bloquesa1 = bytes1 / BLOCKSIZE;
    int bytesrestantes1 = bytes1 % BLOCKSIZE;

    // todo el buffer a 1
    memset(bufferMB, 255, BLOCKSIZE);

    // mas de 1 bloque a 1
    for (int i = 0; i < bloquesa1; i++)
    {
        if (bwrite(SB.posPrimerBloqueMB + i, bufferMB) == FALLO)
        {
            fprintf(stderr, "Error actualizando el superbloque\n");
            return FALLO;
        }
    }

    memset(bufferMB, 0, BLOCKSIZE); // Inicializar todo el buffer a 0
    for (int i = 0; i < bytesrestantes1; i++)
    {
        bufferMB[i] = 255;
    }
    // añadimos los bits que faltan
    if (bits1 > 0)
    {
        bufferMB[bytesrestantes1] = bitSobrantes(bits1);
    }

    if (bwrite(SB.posPrimerBloqueMB + bloquesa1, bufferMB) == FALLO)
    {
        fprintf(stderr, "Error actualizando el superbloque\n");
        return FALLO;
    }

    // Actualizar superbloque con la cantidad de bloques libres
    SB.cantBloquesLibres -= bloquesOcupados;
    if (bwrite(posSB, &SB) == FALLO)
    {
        fprintf(stderr, "Error actualizando el superbloque\n");
        return FALLO;
    }

    return 0;
}

/*Funcion para inicializar el array de inodos*/
int initAI()
{

    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    struct superbloque SB;
    int contInodos;

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error al leer el SB");
        printf(RESET);
        return FALLO;
    }

    contInodos = SB.posPrimerInodoLibre + 1; // si hemos inicializado SB.posPrimerInodoLibre = 0

    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    { // para cada bloque del AI
        // leer el bloque de inodos i en el dispositivo virtual
        if (bread(i, &inodos) == FALLO)
        {
            perror(RED "Error al leer el SB");
            printf(RESET);
            return FALLO;
        }

        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {                         // para cada inodo del bloque
            inodos[j].tipo = 'l'; // libre
            if (contInodos < SB.totInodos)
            {                                               // si no hemos llegado al último inodo del AI
                inodos[j].punterosDirectos[0] = contInodos; // enlazamos con el siguiente
                contInodos++;
            }
            else
            { // hemos llegado al último inodo
                inodos[j].punterosDirectos[0] = UINT_MAX;
                break;
                // hay que salir del bucle, el último bloque no tiene por qué estar completo !!!
            }
        }

        // escribimos el bloque de inodos en el disco virtual//
        if (bwrite(i, &inodos) == FALLO)
        {
            perror(RED "Error al escribir AI en el SB");
            printf(RESET);
            return FALLO;
        }
    }

    return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit)
{

    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    }

    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // leemos el bloque del disco//
    if (bread(nbloqueabs, bufferMB) == FALLO)
    {
        perror(RED "Error al leer byte ");
        printf(RESET);
        return FALLO;
    }

    // actualizamos la posicion de byte//
    posbyte = posbyte % BLOCKSIZE;

    unsigned char mascara = 128; // 10000000

    mascara >>= posbit; // desplazamiento de bits a la derecha

    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara; // ponemos bit a 1
    }
    else
    {
        bufferMB[posbyte] &= ~mascara; // ponemos bit a 0
    }

    // escribimos el byte modificado en el disco virtual//
    if (bwrite(nbloqueabs, bufferMB) == FALLO)
    {

        perror(RED "Error al escribir byte en disco virtual");
        printf(RESET);
        return FALLO;
    }

    return 0;
}

char leer_bit(unsigned int nbloque)
{
    // repetimos proceso como en escribir bit//
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    }

    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // leemos el bloque del disco //
    if (bread(nbloqueabs, bufferMB) == FALLO)
    {
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    }

    // actualizamos la posicion de byte//
    posbyte = posbyte % BLOCKSIZE;
    unsigned char mascara = 128;  // 10000000
    mascara >>= posbit;           // desplazamiento de bits a la derecha, los que indique posbit
    mascara &= bufferMB[posbyte]; // operador AND para bits
    mascara >>= (7 - posbit);     // desplazamiento de bits a la derecha
                                  // para dejar el 0 o 1 en el extremo derecho y leerlo en decimal
    return mascara;
}

int reservar_bloque()
{

    struct superbloque SB;
    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }
    // comrpobamos que haya bloques libres//
    if (SB.cantBloquesLibres == 0)
    {
        perror("No quedan bloques libres");
        return FALLO;
    }

    unsigned char bufferMB[BLOCKSIZE];  // buffer para leer los bloques de MB
    unsigned char bufferAux[BLOCKSIZE]; // buffer para comparar los bloques y encontrar uno no ocupado//
    memset(bufferAux, 255, BLOCKSIZE);  // inicializamos el buffer //

    int nbloqueMB;

    // leemos los bloques del MB para encontrar alguno que tenga un 0
    for (nbloqueMB = SB.posPrimerBloqueMB; nbloqueMB <= SB.posUltimoBloqueMB; nbloqueMB++)
    {
        if (bread(nbloqueMB, bufferMB) == FALLO)
        {
            perror("Error al leer el MB");
            return FALLO;
        }

        // hacemos la comparación
        if (memcmp(bufferMB, bufferAux, BLOCKSIZE) != 0)
        {
            break;
        }
    }

    int posbyte;
    // recorremos los bytes para encontrar uno con un 0
    for (posbyte = 0; posbyte < BLOCKSIZE; posbyte++)
    {
        if (bufferMB[posbyte] != 255)
        {
            break;
        }
    }

    int posbit = 0;
    unsigned char mascara = 128;
    // Recorremos los bits para encontrar  el 0
    while (bufferMB[posbyte] & mascara)
    {
        bufferMB[posbyte] <<= 1;
        posbit++;
    }

    // Determinamos el numero del bloque a reservar y escribimos un 1 en la posicion correspondiente del MB
    int nbloque = (nbloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE * 8 + (posbyte * 8) + posbit;
    escribir_bit(nbloque, 1);

    // actualizamos bloques libres
    SB.cantBloquesLibres--;
    bwrite(posSB, &SB);

    unsigned char bufferDatos[BLOCKSIZE];
    memset(bufferDatos, 0, BLOCKSIZE);
    bwrite(SB.posPrimerBloqueDatos + nbloque, bufferDatos);
    // Guardamos los cambios en el SB
    if (bwrite(posSB, &SB) == FALLO)
    {
        perror(RED "Error al actualizar el superbloque");
        printf(RESET);
        return FALLO;
    }
    return nbloque;
}

int liberar_bloque(unsigned int nbloque)
{

    struct superbloque SB;
    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error al leer el superbloque");
        printf(RESET);
        return FALLO;
    }

    // Escribimos un 0
    escribir_bit(nbloque, 0);

    // actualizamos bloques libres
    SB.cantBloquesLibres++;
    if (bwrite(posSB, &SB) == FALLO)
    {
        perror(RED "Error al actualizar el superbloque");
        printf(RESET);
    }
    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo)
{

    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error al leer el superbloque");
        printf(RESET);
        return FALLO;
    }

    int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;

    int nbloqueabs = nbloqueAI + SB.posPrimerBloqueAI;

    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);

    // Leer el bloque actual de inodos
    if (bread(nbloqueabs, inodos) == FALLO)
    {
        perror(RED "Error al leer el array de inodos");
        printf(RESET);
        return FALLO;
    }

    //  Solo preservar el btime si el nuevo no está inicializado
    // Solo preservar btime si el valor nuevo no es válido (== 0)
    if (inodo->btime == 0) {
        inodo->btime = inodos[posinodo].btime;
    }


    inodos[posinodo] = *inodo;

    // Escribir el bloque actualizado
    if (bwrite(nbloqueabs, inodos) == FALLO)
    {
        perror(RED "Error al escribir el array de inodos");
        printf(RESET);
        return FALLO;
    }

    return EXITO;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{

    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }

    int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
    // obtenemos la posicion absoluta del bloque//
    int nbloqueabs = nbloqueAI + SB.posPrimerBloqueAI;

    // obtenemos la posicion del inodo dentro del buffer
    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);

    // Leer el bloque actual de inodos
    if (bread(nbloqueabs, inodos) == FALLO)
    {
        perror(RED "Error al leer el superbloque");
        printf(RESET);
        return FALLO;
    }

    *inodo = inodos[posinodo];

    return EXITO;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos)
{

    struct superbloque SB;
    struct inodo inodoReservado;
    int posInodoReservado;

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }
    // comrpobamos que haya inodos libres//
    if (SB.cantInodosLibres == 0)
    {
        perror("No quedan inodos libres");
        return FALLO;
    }

    posInodoReservado = SB.posPrimerInodoLibre;
    // Actualizamos posicion del primer inodo libre
    SB.posPrimerInodoLibre++;

    // leemos el inodoReservado
    if (leer_inodo(posInodoReservado, &inodoReservado) == FALLO)
    {
        perror("Error");
        return FALLO;
    }

    // Actualizar la lista enlazada de inodos libres
    SB.posPrimerInodoLibre = inodoReservado.punterosDirectos[0];
    // Actualizamos la cantidad de inodos libres
    SB.cantInodosLibres--;
    // inicializamos los campos del inodo reservado
    inodoReservado.tipo = tipo;
    inodoReservado.permisos = permisos;
    inodoReservado.nlinks = 1;
    inodoReservado.tamEnBytesLog = 0;
    inodoReservado.atime = time(NULL);
    inodoReservado.btime = time(NULL);
    inodoReservado.ctime = time(NULL);
    inodoReservado.mtime = time(NULL);
    inodoReservado.numBloquesOcupados = 0;
    for (int i = 0; i < 12; i++)
    {
        inodoReservado.punterosDirectos[i] = 0;
    }
    for (int i = 0; i < 3; i++)
    {
        inodoReservado.punterosIndirectos[i] = 0;
    }


    // escribimos el inodo
    if (escribir_inodo(posInodoReservado, &inodoReservado) == FALLO)
    {
        perror("Error");
        return FALLO;
    }
    // Guardamos los cambios en el SB
    if (bwrite(posSB, &SB) == FALLO)
    {
        perror("Error al actualizar el superbloque");
        return FALLO;
    }
    return posInodoReservado;
}

int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
{
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }
    else if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }
    else if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }
    else
    {
        *ptr = 0;
        fprintf(stderr, "Error: Bloque lógico fuera de rango\n");
        return -1;
    }
}

int obtener_indice(unsigned int nblogico, int nivel_punteros)
{
    if (nblogico < DIRECTOS)
    {
        return nblogico; // Caso de bloques directos
    }
    else if (nblogico < INDIRECTOS0)
    {
        return nblogico - DIRECTOS; // Caso de indirecto simple
    }
    else if (nblogico < INDIRECTOS1)
    {
        if (nivel_punteros == 2)
        {
            return (nblogico - INDIRECTOS0) / NPUNTEROS; // Índice de nivel 2
        }
        else if (nivel_punteros == 1)
        {
            return (nblogico - INDIRECTOS0) % NPUNTEROS; // Índice de nivel 1
        }
    }
    else if (nblogico < INDIRECTOS2)
    {
        if (nivel_punteros == 3)
        {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS); // Índice de nivel 3
        }
        else if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS; // Índice de nivel 2
        }
        else if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS; // Índice de nivel 1
        }
    }

    return -1; // Error: fuera de rango
}

/**
 * Traduce un bloque lógico a un bloque físico en un inodo.
 */
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, unsigned char reservar)
{

    struct inodo inodo;
    unsigned int ptr = 0, ptr_ant = 0, salvar_inodo = 0;
    int nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];

    if (leer_inodo(ninodo, &inodo) < 0)
        return FALLO;

    nRangoBL = obtener_nRangoBL(&inodo, nblogico, &ptr);
    if (nRangoBL == -1)
        return FALLO; // Error: bloque fuera de rango

    nivel_punteros = nRangoBL;
    while (nivel_punteros > 0)
    {
        if (ptr == 0)
        { // No existe el bloque de punteros
            if (reservar == 0)
                return FALLO;
            ptr = reservar_bloque();
            if (ptr == -1)
                return FALLO;
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            salvar_inodo = 1;

            if (nivel_punteros == nRangoBL)
            {
                inodo.punterosIndirectos[nRangoBL - 1] = ptr;
#if DEBUGN4 || DEBUGN5
                fprintf(stderr, GRAY "[traducir_bloque_inodo()→ inodo.punterosIndirectos[%d] = %d (reservado BF %d para punteros_nivel%d)]\n",
                        nRangoBL - 1, ptr, ptr, nivel_punteros);
                printf(RESET);
#endif
            }
            else
            {
                buffer[indice] = ptr;
                if (bwrite(ptr_ant, buffer) < 0)
                    return FALLO;
#if DEBUGN4 || DEBUGN5
                fprintf(stderr, GRAY "[traducir_bloque_inodo()→ punteros_nivel%d [%d] = %d (reservado BF %d para punteros_nivel%d)]\n",
                        nivel_punteros + 1, indice, ptr, ptr, nivel_punteros);
                printf(RESET);
#endif
            }
            memset(buffer, 0, BLOCKSIZE);
        }
        else
        {
            if (bread(ptr, buffer) < 0)
                return FALLO;
        }

        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }

    if (ptr == 0)
    { // No existe bloque de datos
        if (reservar == 0)
            return FALLO;
        ptr = reservar_bloque();
        if (ptr == -1)
            return -1;
        inodo.numBloquesOcupados++;
        inodo.ctime = time(NULL);
        salvar_inodo = 1;

        if (nRangoBL == 0)
        {
            inodo.punterosDirectos[nblogico] = ptr;
#if DEBUGN4 || DEBUGN5
            fprintf(stderr, GRAY "[traducir_bloque_inodo()→ inodo.punterosDirectos[%d] = %d (reservado BF %d para BL %d)]\n",
                    nblogico, ptr, ptr, nblogico);
            printf(RESET);
#endif
        }
        else
        {
            buffer[indice] = ptr;
            if (bwrite(ptr_ant, buffer) < 0)
                return FALLO;
#if DEBUGN4 || DEBUGN5
            fprintf(stderr, GRAY "[traducir_bloque_inodo()→ punteros_nivel1 [%d] = %d (reservado BF %d para BL %d)]\n",
                    indice, ptr, ptr, nblogico);
            printf(RESET);
#endif
        }
    }

    if (salvar_inodo)
        escribir_inodo(ninodo, &inodo);
    return ptr;
}

int liberar_inodo(unsigned int ninodo)
{
    struct inodo inodo;
    struct superbloque SB;

    // Leer el inodo
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    // Liberar todos los bloques del inodo
    int bloques_liberados = liberar_bloques_inodo(0, &inodo);
    inodo.numBloquesOcupados -= bloques_liberados;

    // Marcar el inodo como libre
    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;

    // Leer el superbloque
    if (bread(0, &SB) == FALLO)
    {
        return FALLO;
    }

    // Actualizar lista de inodos libres
    inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = ninodo;
    SB.cantInodosLibres++;

    // Escribir el superbloque actualizado
    if (bwrite(0, &SB) == FALLO)
    {
        return FALLO;
    }

    // Actualizar ctime y escribir el inodo
    inodo.ctime = time(NULL);
    if (escribir_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    return ninodo;
}

int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{
    unsigned int nBL = primerBL, ultimoBL, ptr = 0;
    int nRangoBL, nivel_punteros, liberados = 0, eof = 0;

    if (inodo->tamEnBytesLog == 0)
        return 0;

    if (inodo->tamEnBytesLog % BLOCKSIZE == 0)
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE - 1;
    }
    else
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }
#if DEBUGN6
    fprintf(stderr, "[liberar_bloques_inodo()\u2192 primer BL: %d, ", primerBL);
#endif
    while (!eof)
    {
        nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr);
        nivel_punteros = nRangoBL;

        if (nRangoBL == 0)
        {
            liberados += liberar_directos(&nBL, ultimoBL, inodo, &eof);
        }
        else
        {
            liberados += liberar_indirectos_recursivo(&nBL, primerBL, ultimoBL, inodo, nRangoBL,
                                                      nivel_punteros, &ptr, &eof);
        }
    }

#if DEBUGN6
    fprintf(stderr, "[liberar_bloques_inodo()\u2192 total bloques liberados: %d]", liberados);
#endif
    return liberados;
}

int liberar_directos(unsigned int *nBL, unsigned int ultimoBL, struct inodo *inodo, int *eof)
{
    int liberados = 0;

    for (int d = *nBL; d < DIRECTOS && !(*eof); d++)
    {
        if (inodo->punterosDirectos[d] != 0)
        {

#if DEBUGN6
            fprintf(stderr, "[liberar_bloques_inodo()\u2192 liberado BF %d de datos para BL %d]\n",
                    inodo->punterosDirectos[d], *nBL);
#endif
            liberar_bloque(inodo->punterosDirectos[d]);
            inodo->punterosDirectos[d] = 0;
            liberados++;
        }
        (*nBL)++;
        if (*nBL > ultimoBL)
            *eof = 1;
    }
    return liberados;
}

int liberar_indirectos_recursivo(unsigned int *nBL, unsigned int primerBL, unsigned int ultimoBL, struct inodo *inodo, int nRangoBL, int nivel_punteros, unsigned int *ptr, int *eof)
{
    unsigned int bloquePunteros[NPUNTEROS], bloquePunteros_Aux[NPUNTEROS], bufferCeros[NPUNTEROS];
    memset(bufferCeros, 0, BLOCKSIZE);
    int liberados = 0, indice_inicial;

    if (*ptr)
    { // Si hay un bloque de punteros
        indice_inicial = obtener_indice(*nBL, nivel_punteros);
        if (indice_inicial == 0 || *nBL == primerBL)
        {
            if (bread(*ptr, bloquePunteros) == -1)
                return -1;
            memcpy(bloquePunteros_Aux, bloquePunteros, BLOCKSIZE);
        }

        for (int i = indice_inicial; i < NPUNTEROS && !(*eof); i++)
        {
            if (bloquePunteros[i] != 0)
            {
                if (nivel_punteros == 1)
                {
                    liberar_bloque(bloquePunteros[i]);

#if DEBUGN6
                    fprintf(stderr, "[liberar_bloques_inodo()→ liberado BF %d de datos para BL %d]\n", bloquePunteros[i], *nBL);
#endif
                    bloquePunteros[i] = 0;
                    liberados++;
                    (*nBL)++;
                }
                else
                {
                    liberados += liberar_indirectos_recursivo(nBL, primerBL, ultimoBL, inodo, nRangoBL, nivel_punteros - 1, &bloquePunteros[i], eof);
                }
            }
            else
            {
                switch (nivel_punteros)
                {
                case 1:
                    (*nBL)++;
                    break;
                case 2:
                    (*nBL) += NPUNTEROS;
                    break;
                case 3:
                    (*nBL) += NPUNTEROS * NPUNTEROS;
                    break;
                }
            }
            if (*nBL > ultimoBL)
                *eof = 1;
        }

        if (memcmp(bloquePunteros, bloquePunteros_Aux, BLOCKSIZE) != 0)
        {
            if (memcmp(bloquePunteros, bufferCeros, BLOCKSIZE) != 0)
            {
                bwrite(*ptr, bloquePunteros);
            }
            else
            {
                liberar_bloque(*ptr);

#if DEBUGN6
                fprintf(stderr, "[liberar_bloques_inodo()→ liberado BF %d de punteros_nivel%d correspondiente al BL %d]\n", *ptr, nivel_punteros, *nBL);
#endif
                *ptr = 0;
                if (nRangoBL == nivel_punteros)
                {
                    inodo->punterosIndirectos[nRangoBL - 1] = 0;
                }
                liberados++;
            }
        }
    }
    else
    {
        switch (nRangoBL)
        {
        case 1:
            *nBL = INDIRECTOS0;
            break;
        case 2:
            *nBL = INDIRECTOS1;
            break;
        case 3:
            *nBL = INDIRECTOS2;
            break;
        }
    }
    return liberados;
}
// Funcion que devuelve valor decimal de los bits sobrantes//
int bitSobrantes(int n)
{
    int sol = 0;
    for (int i = 7; n > 0; n--, i--)
    {
        sol |= 1 << i;
    }
    return sol;
}