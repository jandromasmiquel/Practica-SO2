/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include  "directorios.h"
#define TAMFILA 100
#define TAMBUFFER (TAMFILA*1000) //suponemos un máx de 1000 entradas, aunque debería ser SB.totInodos
int main(int argc, char **argv) {

     char* camino ;
    unsigned int flag =0;
    char tipo='d';
    

    if (argc != 4 && argc !=3 ) {
        fprintf(stderr, RED "Sintaxis: ./mi_ls -l <disco> </ruta> o ./mi_ls <disco> </ruta>\n");
        return FALLO;
    }

    //version extendida// 
    if (argc == 4 ) {

        if(strcmp(argv[1], "-l")!=0){
        fprintf(stderr, RED"Sintaxis: ./mi_ls -l <disco> </ruta> \n");
        return FALLO;
        }

    flag = 1;
    camino = argv[3];
    // Montar el dispositivo
    if (bmount(argv[2]) == FALLO) {
        fprintf(stderr, RED"Error al montar el dispositivo\n");
        return FALLO;
    }

 } else{
    camino = argv[2];
     // Montar el dispositivo
     if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED"Error al montar el dispositivo\n");
        return FALLO;
    }
 }

 if(camino[strlen(camino)-1]!='/'){
    tipo='f';
 }

 // Preparar buffer
 char buffer[TAMBUFFER];
 memset(buffer, 0, sizeof(buffer));

 // Llamar a mi_dir() 
 int entradas = mi_dir(camino, buffer, tipo,flag);

 if (entradas < 0) {
     // El error ya debería haberse informado dentro de mi_dir()
     bumount();
     return EXIT_FAILURE;
 }

 if (flag && entradas > 0) {
     //Formato extendido
     if(tipo!='f'){
     fprintf(stderr,"Total: %d\n", entradas);
     }
     fprintf(stderr,"Tipo   Permisos    mTime               Tamaño      Nombre\n");
     fprintf(stderr,"-----------------------------------------------------------\n");
 } else if (entradas > 0) {
    //Formato simple 
    if(tipo!='f'){
        fprintf(stderr,"Total: %d\n", entradas);
        }
 }

 fprintf(stderr,"%s\n", buffer);
 
 fprintf(stderr,RESET);

 // Desmontar
 if (bumount() < 0) {
     perror("Error desmontando disco");
     return EXIT_FAILURE;
 }


return EXITO;

}