/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
    mi_waitSem();
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO){
        mi_signalSem();
         return FALLO;
    }
       

    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, RED "No hay permisos de escritura\n");
        fprintf(stderr, RESET);
        mi_signalSem();
        return FALLO;
    }

    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE];
    memset(buf_bloque, 0, BLOCKSIZE);
    unsigned int nbfisico;
    unsigned int bytes_escritos = 0;

    if (primerBL == ultimoBL)
    {
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (nbfisico == FALLO){
            mi_signalSem();
             return FALLO;
        }
           
        if (bread(nbfisico, buf_bloque) == FALLO){
             mi_signalSem();
             return FALLO;
        }
           
        memcpy(buf_bloque + desp1, buf_original, nbytes);
        if (bwrite(nbfisico, buf_bloque) == FALLO){
             mi_signalSem();
             return FALLO;
        }
           
        bytes_escritos = nbytes;
    }
    else
    {
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (nbfisico == FALLO){
            mi_signalSem();
             return FALLO;
        }
           
        if (bread(nbfisico, buf_bloque) == FALLO){
            mi_signalSem();
             return FALLO;
        }
           
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if (bwrite(nbfisico, buf_bloque) == FALLO){
            mi_signalSem();
            return FALLO;
        }
            
        bytes_escritos += BLOCKSIZE - desp1;

        // BLOQUES LÓGICOS INTERMEDIOS//
        for (unsigned int bl = primerBL + 1; bl < ultimoBL; bl++)
        {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 1);
            if (nbfisico == FALLO){
                 mi_signalSem();
                 return FALLO;
            }
               
            if (bwrite(nbfisico, buf_original + bytes_escritos) == FALLO){
                 mi_signalSem();
                return FALLO;
            }
                
            bytes_escritos += BLOCKSIZE;
        }

        // ÚLTIMO BLOQUE LÓGICO //
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
        if (nbfisico == FALLO){
            mi_signalSem();
             return FALLO;
        }
           
        if (bread(nbfisico, buf_bloque) == FALLO){
            mi_signalSem();
            return FALLO;
        }
            
        memcpy(buf_bloque, buf_original + nbytes - desp2 - 1, desp2 + 1);
        if (bwrite(nbfisico, buf_bloque) == FALLO){
            mi_signalSem();
            return FALLO;
        }
            
        bytes_escritos += desp2 + 1;
    }

    if (leer_inodo(ninodo, &inodo) == FALLO){
        mi_signalSem();
        return FALLO;
    }
        

    if (offset + nbytes > inodo.tamEnBytesLog)
        inodo.tamEnBytesLog = offset + nbytes;
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);

    if (escribir_inodo(ninodo, &inodo) == FALLO){
         mi_signalSem();
         return FALLO;
    }
       
     mi_signalSem();
    return bytes_escritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    mi_waitSem();
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO){
        mi_signalSem();
        return FALLO;
    }
       

    
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, "No hay permisos de lectura\n");
         mi_signalSem();
        return FALLO;
    }
  //
    inodo.atime = time(NULL);
    if (escribir_inodo(ninodo, &inodo) == FALLO) {
        mi_signalSem();
        return FALLO;
    }

    // Fin de la sección crítica
    mi_signalSem();

    if (offset >= inodo.tamEnBytesLog)
        return 0;

    if (offset + nbytes > inodo.tamEnBytesLog)
        nbytes = inodo.tamEnBytesLog - offset;

    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE];
    unsigned int nbfisico;
    unsigned int bytes_leidos = 0;

    if (primerBL == ultimoBL)
    {
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico != FALLO)
        {
            if (bread(nbfisico, buf_bloque) == FALLO)
                return FALLO;
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        bytes_leidos += nbytes;
    }
    else
    {
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico != FALLO)
        {
            if (bread(nbfisico, buf_bloque) == FALLO)
                return FALLO;
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }
        bytes_leidos += BLOCKSIZE - desp1;

        for (unsigned int bl = primerBL + 1; bl < ultimoBL; bl++)
        {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 0);
            if (nbfisico != FALLO)
            {
                if (bread(nbfisico, buf_original + bytes_leidos) == FALLO)
                    return FALLO;
            }
            bytes_leidos += BLOCKSIZE;
        }

        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
        if (nbfisico != FALLO)
        {
            if (bread(nbfisico, buf_bloque) == FALLO)
                return FALLO;
            memcpy(buf_original + bytes_leidos, buf_bloque, desp2 + 1);
        }
        bytes_leidos += desp2 + 1;
    }


    return bytes_leidos;
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;

    // Leemos el inodo del sistema de archivos
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error al leer el inodo\n");
        return FALLO;
    }

    // Copiamos la metainformación relevante al struct STAT
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->btime = inodo.btime;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return EXITO;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    mi_waitSem();
    struct inodo inodo;

    // Leemos el inodo correspondiente
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, RED "Error al leer el inodo\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    // Asignamos los nuevos permisos
    inodo.permisos = permisos;

    // Actualizamos el ctime porque hemos modificado metadatos
    inodo.ctime = time(NULL);

    // Guardamos el inodo actualizado
    if (escribir_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, RED "Error al escribir el inodo\n" RESET);
         mi_signalSem();
        return FALLO;
    }
    mi_signalSem();
    return EXITO;
}
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{
    struct inodo inodo;

    // Leer el inodo
    if (leer_inodo(ninodo, &inodo) == -1)
    {
        return FALLO;
    }

    // Comprobar permisos de escritura
    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, "Error: permisos denegados de escritura\n");
        return FALLO;
    }

    // No se puede truncar más allá del EOF
    if (nbytes >= inodo.tamEnBytesLog)
    {
        fprintf(stderr, "Error: no se puede truncar más allá del EOF\n");
        return FALLO;
    }

    // Calcular primer bloque lógico a liberar
    unsigned int primerBL;
    if (nbytes % BLOCKSIZE == 0)
    {
        primerBL = nbytes / BLOCKSIZE;
    }
    else
    {
        primerBL = nbytes / BLOCKSIZE + 1;
    }
    // Liberar bloques desde primerBL hasta el final
    int bloques_liberados = liberar_bloques_inodo(primerBL, &inodo);
    inodo.numBloquesOcupados -= bloques_liberados;

    // Actualizar metadatos
    inodo.tamEnBytesLog = nbytes;
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);

    // Escribir el inodo actualizado
    if (escribir_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    return bloques_liberados;
}
