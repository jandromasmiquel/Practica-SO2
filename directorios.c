/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

static struct UltimaEntrada UltimasEntradas[CACHE_SIZE];

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{

    // Verificar que el camino comienza con '/'
    if (camino[0] != '/')
    {
        return -1; // error: no comienza con '/'
    }

    const char *camino_final = strchr(camino + 1, '/'); // buscamos el segundo '/'

    if (camino_final != NULL)
    {
        // Hay al menos dos '/': es un directorio
        size_t len = camino_final - (camino + 1); // longitud del nombre entre los dos '/'
        strncpy(inicial, camino + 1, len);
        inicial[len] = '\0'; // terminamos la cadena

        strcpy(final, camino_final); // copiamos el resto del camino incluyendo '/'
        *tipo = 'd';
    }
    else
    {
        // Solo hay un '/': es un fichero
        strcpy(inicial, camino + 1); // copiamos desde después del primer '/'
        strcpy(final, "");           // cadena vacía
        *tipo = 'f';
    }

    return 0; // éxito
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
                   unsigned int *p_inodo, unsigned int *p_entrada,
                   char reservar, unsigned char permisos)
{
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial) + 1];
    char tipo;
    struct superbloque SB;
    bread(posSB, &SB);

    // Caso base: camino es raíz "/"
    if (strcmp(camino_parcial, "/") == 0)
    {
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }

    // Extraer la siguiente parte del camino
    if (extraer_camino(camino_parcial, inicial, final, &tipo) == -1)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

// Mensaje de debug:
#if DEBUGN7
    printf("[buscar_entrada()→ inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
#endif
    // Leer el inodo del directorio actual
    if (leer_inodo(*p_inodo_dir, &inodo_dir) == -1)
        return -1;

    if (!(inodo_dir.permisos & 4))
        return ERROR_PERMISO_LECTURA;

    struct entrada buffer;
    int cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    int num_entrada_inodo = 0;

    if (cant_entradas_inodo > 0)
    {

        mi_read_f(*p_inodo_dir, &buffer, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada));

        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, buffer.nombre) != 0)
        {
            num_entrada_inodo++;
            mi_read_f(*p_inodo_dir, &buffer, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada));
        }
    }

    // no son iguales
    if (strcmp(inicial, buffer.nombre) != 0 && num_entrada_inodo == cant_entradas_inodo)
    {

        switch (reservar)
        {
        case 0:
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;

            break;
        case 1:

            if (inodo_dir.tipo == 'f')
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;

            if (!(inodo_dir.permisos & 2))
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {

                strncpy(buffer.nombre, inicial, sizeof(buffer.nombre) - 1);
                buffer.nombre[sizeof(buffer.nombre) - 1] = '\0';

                if (tipo == 'd')
                {

                    if (strcmp(final, "/") == 0)
                    {
                        buffer.ninodo = reservar_inodo('d', permisos);
#if DEBUGN7
                        fprintf(stderr, BLUE "[buscar_entrada()→ reservado inodo %d tipo d con permisos %d para %s]\n",
                                buffer.ninodo, permisos, inicial);
                        fprintf(stderr, RESET);
#endif
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    buffer.ninodo = reservar_inodo('f', permisos);
#if DEBUGN7
                    fprintf(stderr, BLUE "[buscar_entrada()→ reservado inodo %d tipo f con permisos %d para %s]\n",
                            buffer.ninodo, permisos, inicial);
                    fprintf(stderr, RESET);
#endif
                }
                // Escribimos entrada en el directorio padre//
                int escritos = mi_write_f(*p_inodo_dir, &buffer, inodo_dir.tamEnBytesLog, sizeof(struct entrada));
                if (escritos == FALLO)
                {
                    if (buffer.ninodo != -1)
                    {
                        // OJO AQUI
                        liberar_inodo(buffer.ninodo);
                    }
                    return -1;
                }
            }
            break;
        }
#if DEBUGN7
        fprintf(stderr, BLUE "[buscar_entrada()→ creada entrada: %s, %d]\n", inicial, buffer.ninodo);
        fprintf(stderr, RESET);
#endif
    }

    // ¿Es el final del camino?
    if (strcmp(final, "/") == 0 || strlen(final) == 0)
    {

        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
            // modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        *p_inodo = buffer.ninodo;
        *p_entrada = num_entrada_inodo;
        return EXITO;
    }
    else
    {
        *p_inodo_dir = buffer.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }

    return EXITO;
}
void mostrar_error_buscar_entrada(int error)
{
    // fprintf(stderr, "Error: %d\n", error);
    switch (error)
    {
    case -2:
        fprintf(stderr, RED "Error: Camino incorrecto.\n");
        fprintf(stderr, RESET);
        break;
    case -3:
        fprintf(stderr, RED "Error: Permiso denegado de lectura.\n");
        fprintf(stderr, RESET);
        break;
    case -4:
        fprintf(stderr, RED "Error: No existe el archivo o el directorio.\n");
        fprintf(stderr, RESET);
        break;
    case -5:
        fprintf(stderr, RED "Error: No existe algún directorio intermedio.\n");
        fprintf(stderr, RESET);
        break;
    case -6:
        fprintf(stderr, RED "Error: Permiso denegado de escritura.\n");
        fprintf(stderr, RESET);
        break;
    case -7:
        fprintf(stderr, RED "Error: El archivo ya existe.\n");
        fprintf(stderr, RESET);
        break;
    case -8:
        fprintf(stderr, RED "Error: No es un directorio.\n");
        fprintf(stderr, RESET);
        break;
    }
}

int mi_creat(const char *camino, unsigned char permisos)
{
    mi_waitSem();
    unsigned int p_inodo;
    unsigned int p_inodo_dir = 0;
    unsigned int p_entrada;
    int error;

    if (strcmp(camino, "/") == 0)
    {
        mi_signalSem();
        return FALLO;
    }
    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return error;
    }

    mi_signalSem();
    return EXITO;
}

int mi_truncar(const char *camino, unsigned int nbytes)
{
    fprintf(stderr, "camino: [%s]\n", camino);
    unsigned int p_inodo;
    unsigned int p_inodo_dir = 0;
    unsigned int p_entrada;

    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    fprintf(stderr, "la\n");

    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    return mi_truncar_f(p_inodo, nbytes);
}


int mi_dir(const char *camino, char *buffer, char tipo, char flag)
{

    unsigned int p_inodo;
    unsigned int p_inodo_dir = 0;
    unsigned int p_entrada;
    unsigned char permisos = 0;
    struct inodo inodo;
    int numentradas;
    struct entrada buffer_entradas[BLOCKSIZE / sizeof(struct entrada)];
    int total_entradas_leidas = 0;
    struct tm *tm;
    char tmp[100];

    int error;

    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos);

    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    if (leer_inodo(p_inodo, &inodo) == FALLO)
    {
        fprintf(stderr, RED "Error al leer el inodo.\n");
        return FALLO;
    }

    // Comprobar tipo
    if ((inodo.tipo != tipo))
    {
        fprintf(stderr, RED "Error: la sintaxis no concuerda con el tipo\n");
        return FALLO;
    }

    // Comprobar permisos de lectura
    if (!(inodo.permisos & 4))
    {
        fprintf(stderr, RED "Error: Permiso denegado de lectura.\n");
        return FALLO;
    }

    // Directorio
    if (inodo.tipo == 'd')
    {
        numentradas = inodo.tamEnBytesLog / sizeof(struct entrada);
        int offset = 0;

        while (total_entradas_leidas < numentradas)
        {
            int n = mi_read_f(p_inodo, buffer_entradas, offset, sizeof(struct entrada));
            int leidas = n / sizeof(struct entrada);
            for (int i = 0; i < leidas; i++)
            {
                if (leer_inodo(buffer_entradas[i].ninodo, &inodo) < 0)
                {
                    fprintf(stderr, RED "Error al leer inodo de entrada.\n");
                    return FALLO;
                }


                if (flag)
                { // formato extendido
                    tipo = (inodo.tipo == 'd') ? 'd' : 'f';
                    if(tipo=='d'){
                            sprintf(tmp, MAGENTA "%c        ", tipo);
                    }else{
                        sprintf(tmp, ORANGE "%c        ", tipo);
                    }
                    strcat(buffer, tmp);

                    // Permisos
                    strcat(buffer, (inodo.permisos & 4) ? "r" : "-");
                    strcat(buffer, (inodo.permisos & 2) ? "w" : "-");
                    strcat(buffer, (inodo.permisos & 1) ? "x" : "-");
                    strcat(buffer, "        ");

                    // Fecha
                    tm = localtime(&inodo.mtime);
                    sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d   ",
                            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                            tm->tm_hour, tm->tm_min, tm->tm_sec);
                    strcat(buffer, tmp);

                    // Tamaño
                    sprintf(tmp, "%d        ", inodo.tamEnBytesLog);
                    strcat(buffer, tmp);

                    // Nombre
                    strcat(buffer, buffer_entradas[i].nombre);
                    strcat(buffer, "\n");
                }
                else
                { // formato simple
                    strcat(buffer, buffer_entradas[i].nombre);
                    strcat(buffer, "\t");
                }
            }

            total_entradas_leidas += leidas;
            offset += n;
        }

        return numentradas;
    }
    // Fichero
    else if (inodo.tipo == 'f')
    {
        if (flag)
        {
            // tipo
            tipo = 'f';
            sprintf(tmp, ORANGE "%c        ", tipo);
            strcat(buffer, tmp);

            // permisos
            strcat(buffer, (inodo.permisos & 4) ? "r" : "-");
            strcat(buffer, (inodo.permisos & 2) ? "w" : "-");
            strcat(buffer, (inodo.permisos & 1) ? "x" : "-");
            strcat(buffer, "        ");

            // fecha
            tm = localtime(&inodo.mtime);
            sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d   ",
                    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                    tm->tm_hour, tm->tm_min, tm->tm_sec);
            strcat(buffer, tmp);

            // tamaño
            sprintf(tmp, "%d        ", inodo.tamEnBytesLog);
            strcat(buffer, tmp);

            char *nombre = strrchr(camino, '/');
            if (nombre != NULL)
                nombre++;
            else
                nombre = (char *)camino;

            // nombre
            strcat(buffer, nombre);
            strcat(buffer, "\n");
        }
        else // formato simple
        {
            sprintf(tmp, ORANGE);
            strcat(buffer, tmp);
            char *nombre = strrchr(camino, '/');
            if (nombre != NULL)
                nombre++;
            else
                nombre = (char *)camino;

            strcat(buffer, nombre);
            strcat(buffer, "\t");
        }
        return 1;
    }
    else
    {
        fprintf(stderr, "Error: tipo desconocido.\n");
        return FALLO;
    }
}

int mi_chmod(const char *camino, unsigned char permisos)
{

    int error;

    unsigned int p_inodo;
    unsigned int p_inodo_dir = 0;
    unsigned int p_entrada;

    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    mi_chmod_f(p_inodo, permisos);
    return 0;
}
 
int buscar_cache_LRU(const char *camino, int *p_inodo) {
    struct timeval ahora;
    gettimeofday(&ahora, NULL);

    int i;
    int indice_mas_antiguo = 0;
    long min_diferencia = LONG_MAX;

    for (i = 0; i < CACHE_SIZE; i++) {
        if (strcmp(UltimasEntradas[i].camino, camino) == 0) {
            *p_inodo = UltimasEntradas[i].p_inodo;
            UltimasEntradas[i].ultima_consulta = ahora;
            return 1; // encontrado
        }

        // buscar el más antiguo
        long diferencia = ahora.tv_sec - UltimasEntradas[i].ultima_consulta.tv_sec;
        if (diferencia < min_diferencia) {
            min_diferencia = diferencia;
            indice_mas_antiguo = i;
        }
    }

    return -indice_mas_antiguo - 1; // no encontrado, devolvemos índice negativo para reemplazo
}
int mi_stat(const char *camino, struct STAT *p_stat) {
    unsigned int p_inodo;
    unsigned int p_inodo_dir = 0;
    unsigned int p_entrada;

    int resultado = buscar_cache_LRU(camino, (int *)&p_inodo);

    if (resultado < 0) {
        // No está en la caché, hay que buscar la entrada
        int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
        if (error < 0) {
            mostrar_error_buscar_entrada(error);
            return FALLO;
        }

        // Actualizamos la caché LRU
        int indice_reemplazo = -resultado - 1;
        strcpy(UltimasEntradas[indice_reemplazo].camino, camino);
        UltimasEntradas[indice_reemplazo].p_inodo = p_inodo;
        gettimeofday(&UltimasEntradas[indice_reemplazo].ultima_consulta, NULL);

        #if DEBUGN9
        fprintf(stderr, "[mi_stat() → Actualizamos caché LRU en posición %d]\n", indice_reemplazo);
        #endif
    } else {
        #if DEBUGN9
        fprintf(stderr, "[mi_stat() → Entrada encontrada en caché LRU]\n");
        #endif
    }

    if (mi_stat_f(p_inodo, p_stat) == FALLO) {
        fprintf(stderr, RED "Error en mi_stat_f\n");
        return FALLO;
    }

    p_stat->ninodo = p_inodo;
    return 0;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes) {
    unsigned int p_inodo;
    unsigned int p_inodo_dir = 0, p_entrada;
    struct inodo inodo;
    int error;

    int resultado = buscar_cache_LRU(camino, (int *)&p_inodo);

    if (resultado < 0) {
        // No está en la caché
        error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
        if (error < 0) {
            mostrar_error_buscar_entrada(error);
            return FALLO;
        }

        // Actualizar la caché LRU
        int indice_reemplazo = -resultado - 1;
        strcpy(UltimasEntradas[indice_reemplazo].camino, camino);
        UltimasEntradas[indice_reemplazo].p_inodo = p_inodo;
        gettimeofday(&UltimasEntradas[indice_reemplazo].ultima_consulta, NULL);

        #if DEBUGN9
                fprintf(stderr, "[mi_write() → Actualizamos caché LRU en posición %d]\n", indice_reemplazo);
        #endif
    } else {
        #if DEBUGN9
                fprintf(stderr, "[mi_write() → Entrada encontrada en caché LRU]\n");
        #endif
    }

    if (leer_inodo(p_inodo, &inodo) == FALLO) return FALLO;
    if (inodo.tipo != 'f') return FALLO;

    return mi_write_f(p_inodo, buf, offset, nbytes);
}


int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes) {
    unsigned int p_inodo;
    unsigned int p_inodo_dir = 0, p_entrada;
    struct inodo inodo;
    int error;

    int resultado = buscar_cache_LRU(camino, (int *)&p_inodo);

    if (resultado < 0) {
        error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
        if (error < 0) {
            mostrar_error_buscar_entrada(error);
            return FALLO;
        }

        int indice_reemplazo = -resultado - 1;
        strcpy(UltimasEntradas[indice_reemplazo].camino, camino);
        UltimasEntradas[indice_reemplazo].p_inodo = p_inodo;
        gettimeofday(&UltimasEntradas[indice_reemplazo].ultima_consulta, NULL);

#if DEBUGN9
        fprintf(stderr, "[mi_read() → Actualizamos caché LRU en posición %d]\n", indice_reemplazo);
#endif
    } else {
#if DEBUGN9
        fprintf(stderr, "[mi_read() → Entrada encontrada en caché LRU]\n");
#endif
    }

    if (leer_inodo(p_inodo, &inodo) == FALLO) return FALLO;
    if (inodo.tipo != 'f') return FALLO;

    return mi_read_f(p_inodo, buf, offset, nbytes);
}

int mi_link(const char *camino1, const char *camino2)
{

    mi_waitSem();
    unsigned int p_inodo1, p_inodo2, p_entrada2, p_entrada1;
    struct inodo inodo1;
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo_dir2 = 0;
    unsigned char permisos = 0;

    // Existencia camino1
    int error = buscar_entrada(camino1, &p_inodo_dir, &p_inodo1, &p_entrada1, 0, permisos);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    leer_inodo(p_inodo1, &inodo1);

    // Si no es fichero salimos
    if ((inodo1.tipo != 'f'))
    {
        mi_signalSem();
        return FALLO; // no se permite enlazar directorios
    }
    // permisos de lectura
    if ((inodo1.permisos & 4) != 4)
    {
        mi_signalSem();
        return FALLO;
    }
    // Crear entrada camino2
    error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6);
    if (error < 0)
    {
        // error si la entrada ya existe//
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    struct entrada entrada2;

    if (mi_read_f(p_inodo_dir2, &entrada2, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) < 0)
    {
        mi_signalSem();
        return FALLO;
    }
    entrada2.ninodo = p_inodo1;

    if (mi_write_f(p_inodo_dir2, &entrada2, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) < 0)
    {
        mi_signalSem();
        return FALLO;
    }
    // Liberar inodo reservado para la nueva entrada
    liberar_inodo(p_inodo2);

    // actualizamos los datos
    inodo1.nlinks++;
    inodo1.ctime = time(NULL);
    if (escribir_inodo(p_inodo1, &inodo1) == FALLO)
    {
        fprintf(stderr, RED "fallo al escribir el inodo\n");
        fprintf(stderr, RESET);
        mi_signalSem();
        return FALLO;
    };

    mi_signalSem();
    return EXITO;
}
int mi_unlink(const char *camino)
{
    mi_waitSem();
    unsigned int p_inodo_dir, p_inodo, p_entrada;
    p_inodo_dir = 0;
    struct inodo inodo, inodo_dir;
    struct entrada entrada;

    if (strcmp(camino, "/") == 0)
    {
        fprintf(stderr, RED "Error: No se puede borrar el directorio raíz\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    if (leer_inodo(p_inodo, &inodo) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    //  directorio no vacio
    if (inodo.tipo == 'd' && inodo.tamEnBytesLog > 0)
    {
        fprintf(stderr, RED "Error: El directorio no está vacío\n");
        fprintf(stderr, RESET);
        mi_signalSem();
        return FALLO;
    }else
    {

        if (leer_inodo(p_inodo_dir, &inodo_dir) == FALLO)
        {
            mi_signalSem();
            return FALLO;
        }
        int num_entradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);

        // si es la última entrada Truncamos
        if (p_entrada == (num_entradas - 1))
        {

            if (mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada)) == FALLO)
            {
                mi_signalSem();
                return FALLO;
            }
            // si no es la ultima entrada
            
        }
        else
        {
            // leemos última entrada
            if (mi_read_f(p_inodo_dir, &entrada, (num_entradas - 1) * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
            {
                mi_signalSem();
                return FALLO;
            }

            if (mi_write_f(p_inodo_dir, &entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
            {
                mi_signalSem();
                return FALLO;
            }

            // truncamos
            if (mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada)) == FALLO)
            {
                mi_signalSem();
                return FALLO;
            }
        }
        
    }

    // Actualizamos nlinks
    inodo.nlinks--;
    if (inodo.nlinks == 0)
    {
        liberar_inodo(p_inodo);
    }
    else
    {
        inodo.ctime = time(NULL);
        if (escribir_inodo(p_inodo, &inodo) == FALLO)
        {
            mi_signalSem();
            return FALLO;
        }
    }
    mi_signalSem();
    return EXITO;
}