#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define TAM_BUFFER 1024

int main(int argc, char *argv[]) {

    int archivo;
    int tamTrozo;
    int numTrozos;
    int tuberia[2];
    int i;
    pid_t pid;

    char buffer[TAM_BUFFER];
    char nombreArchivo[300];

    if(argc != 3) {
        printf("ARGUMENTOS INCORRECTOS\n");
        exit(-1);
    }

    tamTrozo = atoi(argv[2]);

    if(tamTrozo <= 0) {
        printf("El tamaño debe ser un número positivo\n");
        exit(-1);
    }

    archivo = open(argv[1], O_RDONLY);

    if(archivo < 0) {
        perror("Error al abrir el archivo");
        exit(-1);
    }


    /* Obtenemos el tamaño total del archivo */

    int tamArchivo = lseek(archivo, 0, SEEK_END);

    lseek(archivo, 0, SEEK_SET);


    /* Calculamos cuántos trozos son necesarios */

    numTrozos = tamArchivo / tamTrozo;

    if(tamArchivo % tamTrozo != 0) {
        numTrozos++;
    }


    for(i = 0; i < numTrozos; i++) {

        if(pipe(tuberia) < 0) {
            perror("Error al crear la tuberia");
            exit(-1);
        }


        pid = fork();


        if(pid < 0) {

            perror("Error en fork");
            exit(-1);

        } else if(pid == 0) {

            /* HIJO */

            close(tuberia[1]);
            close(archivo);


            sprintf(nombreArchivo, "%s.h%02d", argv[1], i);


            int archivoSalida;

            archivoSalida = open(nombreArchivo,
                                 O_WRONLY | O_CREAT | O_TRUNC,
                                 0666);

            if(archivoSalida < 0) {
                perror("Error al crear el archivo");
                exit(-1);
            }


            int leidos;

            while((leidos = read(tuberia[0],
                                 buffer,
                                 TAM_BUFFER)) > 0) {

                write(archivoSalida,
                      buffer,
                      leidos);
            }


            close(tuberia[0]);
            close(archivoSalida);

            exit(0);


        } else {

            /* PADRE */

            close(tuberia[0]);


            int enviados = 0;
            int leidos;


            while(enviados < tamTrozo) {

                int queda = tamTrozo - enviados;

                int cantidad;

                if(queda > TAM_BUFFER) {
                    cantidad = TAM_BUFFER;
                } else {
                    cantidad = queda;
                }
                leidos = read(archivo,
                              buffer,
                              cantidad);

                if(leidos <= 0) {
                    break;
                }
                write(tuberia[1],
                      buffer,
                      leidos);
                enviados = enviados + leidos;
            }
            close(tuberia[1]);
            wait(NULL);
        }
    }
    close(archivo);
    return 0;
}