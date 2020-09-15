//
//  main.c
//  Actividad 5: IPC
//  Reforzar los conocimientos relacionados con los mecanismos básicos de comunicación entre procesos (pipes) en el lenguaje C.
//  Created by Sabrina Santana Lazos on 14/09/20.
//  Copyright © 2020 Sabrina Santana Lazos. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <math.h>

//Estructura de tubos
typedef struct{
	int * tubo;
} pipes;

void leerescribir(pipes * fd, long int pid);

int main(int argc, char **argv) {

    int estado,c, index, cant=5,cant2=-1;
    char *cvalue = NULL;
    opterr = 0;

    //Para recibir como argumento en la línea de comandos el número de procesos del Token Ring
    while ((c = getopt (argc, argv, "n:")) != -1){
			printf("Actividad 5: IPC. Ingrese un valor por consola con la opcion -n ENTERO, el valor es el numero de procesos para el Token Ring\n\n");
			printf("El Token Ring dará un máximo de 5 vueltas\n\n");
        switch (c)
    {
        case 'n':
            cvalue = optarg;
            //Validar que el valor especificado sea un número entero
            if(isdigit(*cvalue)!=0){
              cant2 = atoi(optarg);
            } else{
              printf("El argumento no es un numero entero \n");
            }
            break;
        case '?':
            if (optopt == 'n')
                fprintf (stderr, "Opción -%c requiere un argumento.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Opción desconocida '\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
    }
	}

		//Cuando la opcion no es -n
    for (index = optind; index < argc; index++)
        printf ("El argumento no es una opción válida %s, Opción válida: -n entero\n", argv[index]);

		if(cant2>0){
			//Aputadores para los pids y los tubos
			pid_t * pids = (pid_t *)malloc(sizeof(pid_t)*cant2);
			pid_t * inicio = pids;
			pid_t * fin = pids + cant2 - 1;
			pipes * tubos = (pipes *)malloc(sizeof(pipes)*cant2);
			pipes * iniciotubos = tubos;
			pipes * finaltubos = tubos + cant2;

			//Crear todos los tubos
			while(iniciotubos<finaltubos){
				iniciotubos->tubo = (int*)malloc(sizeof(int)*2);
				pipe(iniciotubos->tubo);
				iniciotubos++;
			}

			//Generar caracter testigo (T)
			char testigo = 'T';

			iniciotubos = tubos;

			//Crear procesos hijos
			while(inicio<fin){

					//Duplicar al proceso padre
					* inicio = fork();

					//Error al generar proceso hijo
					if (*inicio == -1) {
							sleep(1);
							printf("\nError al crear hijo \n");
							break; //Para no crear mas procesos hijos
					}
					//Cada hijo lee hasta que recibe al testigo
					else if (*inicio == 0) {
						while(cant>0){
							int pidactual = getpid();
							leerescribir(iniciotubos, pidactual);
							cant--;
						}
						exit(0);
					}
					inicio++;
					iniciotubos++;
			}

			//El padre inicia el ciclo y continúa el ciclo cuando recibe al testigo
			while(cant>0){
				char ultimoT;
				printf("\nSoy el proceso que inicia el ciclo con PID = %d y mando el testigo = %c \n", getpid(), testigo);
				write(*((tubos->tubo)+1), &testigo, sizeof(char));
				close(*((iniciotubos->tubo)+1));
				read(*(iniciotubos->tubo), &ultimoT, sizeof(char));
				printf("\nSoy el proceso que termina el ciclo con PID = %d y recibi el testigo = %c. , el cual tendré por 5 segundos \n", getpid(), ultimoT);
				printf("\nTermina vuelta: %d\n",cant);
				sleep(5);
				cant--;
			}

			printf("\n\n");

			//Liberar memoria
			free(pids);
			iniciotubos = tubos;
			while(iniciotubos<finaltubos){
				free(iniciotubos->tubo);
				iniciotubos++;
			}
			free(tubos);
		} else{
			printf("Para que el programa se ejecute incluya el argumento -n mayor a 0. Ejemplo: ./a.out -n 5 \n");
		}
    return 0;
}

void leerescribir(pipes * fd, long int pid)
{
    char c;
		//Leer el testigo y mantenerlo por 5 segundos
    close(*((fd->tubo)+1));
    read(*(fd->tubo), &c, sizeof(char));
    printf("\nSoy el proceso con PID = %ld y recibi el testigo = %c, el cual tendré por 5 segundos \n", pid, c);
		sleep(5);
		//Mandar el testigo al siguiente proceso en el anillo
    printf("\nSoy el proceso con PID = %ld y acabo de enviar el testigo = %c \n", pid, c);
    pipes * aux = fd;
    aux++;
		close(*(aux->tubo));
    write(*((aux->tubo)+1), &c, sizeof(char));
}
