# Sistema de Ficheros UNIX en C (estilo EXT)

Implementación en **C** de un **Sistema de Ficheros emulado** sobre un archivo de disco virtual, basado en la arquitectura tradicional de UNIX/EXT (superbloque, inodos, mapa de bits de bloques e inodos, directorio raíz, bloques directos e indirectos, y enlaces).

Proyecto desarrollado para la asignatura de **Sistemas Operativos II**.

---

## 🏗️ Arquitectura del Sistema

El sistema está estructurado modularmente en capas:

1. **Capa de Bloques (`bloques.c` / `bloques.h`)**:
   Abstracción de bajo nivel del dispositivo virtual. Lee y escribe bloques de tamaño fijo (1024 bytes) en el fichero que actúa como disco.
2. **Capa de Ficheros Básicos (`ficheros_basico.c` / `fichero_basico.h`)**:
   Gestión del Superbloque, bitmaps de inodos y bloques, reserva/liberación de bloques/inodos y traducción de bloques lógicos a bloques físicos (apuntadores directos, indirectos 0, 1 y 2).
3. **Capa de Ficheros (`ficheros.c` / `ficheros.h`)**:
   Operaciones de lectura, escritura y truncado a nivel de inodo según offsets y permisos de acceso.
4. **Capa de Directorios (`directorios.c` / `directorios.h`)**:
   Traducción de rutas tipo UNIX (`/dir/fichero`), gestión de entradas de directorio y enlaces (*hard links*).
5. **Capa de Concurrencia y Comandos (`simulacion.c`, `verificacion.c`, `mi_*`)**:
   Simulación de múltiples procesos escribiendo de forma concurrente con semáforos POSIX y utilidades de consola.

---

## 🛠️ Comandos Disponibles

- **`mi_mkfs`**: Formatea un archivo como disco virtual con el sistema de ficheros.
- **`mi_mkdir`**: Crea un directorio en una ruta especificada.
- **`mi_touch`**: Crea un fichero vacío.
- **`mi_ls`**: Lista el contenido e información de un directorio.
- **`mi_chmod`**: Modifica los permisos de un fichero o directorio.
- **`mi_stat`**: Muestra los metadatos detallados de un inodo.
- **`mi_escribir` / `mi_cat`**: Escribe texto en un fichero y lee su contenido.
- **`mi_link`**: Crea un enlace duro (*hard link*).
- **`mi_rm` / `mi_rmdir`**: Elimina ficheros o directorios.

---

## 🚀 Compilación y Pruebas

### Compilar
```bash
make
```

### Limpiar ejecutables y archivos temporales
```bash
make clean
```

### Scripts de Verificación
- **`./scripte1.sh`**: Valida formateo, operaciones de lectura/escritura de bajo nivel, permisos y truncado.
- **`./scripte2.sh`**: Prueba exhaustivamente todos los comandos del sistema de ficheros (`mi_*`).
- **`./scripte3.sh`**: Realiza pruebas de concurrencia simulando procesos paralelos y verifica la integridad del sistema de ficheros.

---

## ✒️ Autores
- Carlos López Mihi
- Alejandro Masmiquel Casado
