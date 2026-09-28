#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pidEje, pidA, pidB, pidX, pidY, pidZ;
int tiempo; 
char nombreHoja; 

void controlAlarmaZ(int sig) {
    kill(pidA, SIGUSR1); 
}

void manejadorA(int sig) {
    pid_t pid = fork(); 

    if(pid < 0) {
        perror("Error en fork"); 
        exit(-1); 
    } else if(pid == 0) {
        execlp("pstree", "pstree", NULL); 
        perror("Error en execlp");
        exit(-1);
    } else {
        waitpid(pid, NULL, 0);
        kill(pidB, SIGUSR2);
    }
} 

void manejarDestruccionB(int sig) {
    kill(pidZ, SIGUSR2); 
    waitpid(pidZ, NULL, 0);

    kill(pidY, SIGUSR2);
    waitpid(pidY, NULL, 0);

    kill(pidX, SIGUSR2);
    waitpid(pidX, NULL, 0);
} 

void manejadorMuerteHoja(int sig) {
    printf("Soy %c (%d) y muero\n", nombreHoja, getpid()); 
    exit(0); 
}

void procesoHoja(char nombre) {
    nombreHoja = nombre;
   
    printf("Soy el proceso %c: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n",
           nombre, getpid(), pidB, pidA, pidEje);
    
    signal(SIGUSR2, manejadorMuerteHoja);

    if (nombre == 'Z') {
        signal(SIGALRM, controlAlarmaZ);
        alarm(tiempo); 
        pause(); 
    }

    pause(); 
}

pid_t crearProceso(char nombre) {
    pid_t pid = fork(); 

    if(pid < 0) {
        perror("Error en fork"); 
        exit(-1); 
    } else if(pid == 0) {
        procesoHoja(nombre); 
        exit(0); 
    } 

    return pid; 
} 

void procesoB() {
    pidB = getpid();

    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n",
           pidB, pidA, pidEje);

    signal(SIGUSR2, manejarDestruccionB);

    pidX = crearProceso('X');
    pidY = crearProceso('Y');
    pidZ = crearProceso('Z');

    pause(); 

    printf("Soy B (%d) y muero\n", pidB);
    exit(0);
}

void procesoA() {
    pidA = getpid(); 

    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", pidA, pidEje);

    signal(SIGUSR1, manejadorA); 

    pidB = fork(); 

    if(pidB < 0) {
        perror("Error en fork"); 
        exit(-1); 
    } else if(pidB == 0) {
        procesoB(); 
        exit(0); 
    } else {
        pause(); 
        waitpid(pidB, NULL, 0); 
        printf("Soy A (%d) y muero\n", pidA); 
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("ARGUMENTOS INCORRECTOS\n");
        exit(-1);
    } 

    tiempo = atoi(argv[1]); 
    if(tiempo <= 0) {
        printf("El tiempo debe ser un número positivo\n");
        exit(-1);
    } 

    setbuf(stdout, NULL);
    pidEje = getpid();
    printf("Soy el proceso ejec: mi pid es %d\n", pidEje);

    pidA = fork();

    if(pidA < 0) {
        perror("Error en fork"); 
        exit(-1); 
    } else if(pidA == 0) {
        procesoA(); 
        exit(0); 
    } else {
        waitpid(pidA, NULL, 0); 
        printf("Soy ejec (%d) y muero\n", pidEje); 
    }

    return 0; 
}