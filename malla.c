#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

void crearMalla(const int x, const int y) {
    bool esHijo = false;
    int i, j;

    for(i = 0; i < x && !esHijo; i++) {
        pid_t pid = fork();

        if(pid < 0) {
            perror("Error en fork"); 
            exit(-1);
        } else  if (pid == 0) {
            esHijo = true; 
            bool esPadre = false; 
            for(j = 1; j < y && !esPadre; j++) {
                pid_t pidHijo = fork();

                if(pidHijo < 0) {
                    perror("Error en fork"); 
                    exit(-1); 
                } else if(pidHijo > 0) {
                    esPadre = true; 
                    wait(NULL); // Espera a que el hijo termine antes de continuar
                    printf("Soy el proceso %d y muero\n", getpid()); 
                }
            }
            if(!esPadre) {
                printf("Soy el proceso final, mi pid es %d\n", getpid());
                sleep(15); 
            }
            exit(0); // Asegurarse de que el proceso hijo termine después de crear sus hijos
        }
        // El proceso padre continúa el bucle para crear más hijos
    }
    if(!esHijo) {
        // El proceso padre espera a que todos los hijos terminen
        for(i = 0; i < x; i++) {
            wait(NULL);
        }
        printf("Soy malla %d, y muero\n", getpid());
    }
} 


int main(int argc, char *argv[]) {
    int x, y = 0; 
    bool correcto = false; 

    if(argc == 3) {
        x = atoi(argv[1]);
        y = atoi(argv[2]);
        correcto = true;
    if(correcto) {
        printf("Soy malla, mi pid es %d\n", getpid());
        crearMalla(x, y);
    }
    } else {
        printf("ARGUMENTOS INCORRECTOS\n");
        exit(-1);
    }
    return 0; 
}