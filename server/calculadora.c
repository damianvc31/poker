#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "defs.h"

void calculadora (void)//esta funcion muestra el menu de la calculadora y llama a la funcion que calcula probabilidades
{
 char opc;
 printf("\nCALCULADORA DE PROBABILIDADES\n");
 do 
	{
	 printf("\nElija una opción del menú\n");
	 printf("\n1)Probabilidad de una mano contra otra mano especifica\n2)Volver al menu principal\n"); 
	 printf("\nOpción: ");
	 scanf("%d",(int *)&opc);
	 while(opc<1 || opc>2)
		{
	 	 printf("\nEntrada inválida, ingrese nuevamente: ");
		 scanf("%d",(int *)&opc);
		}
	 getchar();
	 if(opc == 1)
		 calculadora_1();
	}while(opc!=2);
}

void calculadora_1(void)/*esta funcion simula una cantidad de manos a partir de dos manos iniciales e informa que porcentaje de veces ganara cada una*/
{
 NODE * mazo=NULL, *N=NULL;
 CARTA jug[2],opp[2],mesa[5];
 char valor[2],palo[2],valor_opp[2],palo_opp[2];
 long double sim,gjug=0,gopp=0,emp=0;
 int cant,i,r;
 long double eq_jug,eq_opp;
 time_t before,after;
 printf("\nIngrese la mano deseada para Ud ('A','1'...'T','J','Q',K' para el valor;'C'(Corazones),'D'(Diamantes),'T'(Treboles),'P'(Picas) para el palo)\n"); //ingreso de las cartas del jugador
 printf("\nCarta 1:\n");
 scanf("%c",valor);
 getchar();
 scanf("%c",palo);
 getchar();
 printf("\nCarta 2:\n");
 scanf("%c",valor+1);
 getchar();
 scanf("%c",palo+1);
 getchar();
 printf("\nIngrese la mano deseada para el oponente ('A','1'...'T','J','Q',K' para el valor;'C'(Corazones),'D'(Diamantes),'T'(Treboles),'P'(Picas) para el palo)\n"); //ingreso de las cartas del oponente
 printf("\nCarta 1:\n");
 scanf("%c",valor_opp);
 getchar();
 scanf("%c",palo_opp);
 getchar();
 printf("\nCarta 2:\n");
 scanf("%c",valor_opp+1);
 getchar();
 scanf("%c",palo_opp+1);
 getchar();
 before = time(NULL); //guarda el tiempo antes de empezar la simulacion
 printf("\nComienza la simulación!\n");
 for(sim=1;sim<=CANT_SIMULACIONES;sim++) /*bucle que se repetira tantas veces como este definido en el header (a mas de 50 mil repeticiones, hay un retraso mayor a un minuto*/
	{
	 fill_mazo(&mazo); //llena el mazo con todas las cartas
	 cant=count_nodes(mazo); //cuenta la cantidad de cartas
	 N = select_node_by_user(mazo,valor[0],palo[0]); /*selecciona las cartas definidas por el usuario,las quita del mazo y las agrega a la mano de cada jugador*/
	 detach_node(&mazo,N);
	 jug[0]=N->D;
	 free(N);
	 cant --;
	 N = select_node_by_user(mazo,valor[1],palo[1]);
	 detach_node(&mazo,N);
	 jug[1]=N->D;
	 free(N);
	 cant --;
	 N = select_node_by_user(mazo,valor_opp[0],palo_opp[0]);
	 detach_node(&mazo,N);
	 opp[0]=N->D;
	 free(N);
	 cant --;
	 N = select_node_by_user(mazo,valor_opp[1],palo_opp[1]);
	 detach_node(&mazo,N);
	 opp[1]=N->D;
	 free(N);
	 cant --;
	 mazo = shuffle_mazo (&mazo,cant); //mezcla el mazo y reparte las cartas de la mesa
	 for(i=0;i<5;i++) 
		{
		 N = mazo;
		 mesa[i]=N->D;
		 detach_node(&mazo,N);	 
		 free(N);
		}	 
	 r = realizar_sim(jug,opp,mesa); //calcula el ganador
	 switch(r)//dependiendo del ganador aumenta el contador correspondiente
		{
		 case 1:
			gjug++;
			break;
		 case 2:
			gopp++;
			break;
		 case 3:
			emp++;
			break;
		}	
	 delete_list(&mazo); //elimina el mazo
	}
 after=time(NULL); //guarda el tiempo al terminar
 eq_jug=(gjug*100)/CANT_SIMULACIONES+(emp*50)/CANT_SIMULACIONES; //calcula la probabilidad de victoria del jugador
 eq_opp=(gopp*100)/CANT_SIMULACIONES+(emp*50)/CANT_SIMULACIONES; //calcular la probabilidad de victoria del oponente
 printf("\n");
 printf("\nEquity de Ud: %Lf",eq_jug); //imprime los resultados
 printf("\nEquity del oponente: %Lf",eq_opp);
 printf("\n");
 printf("\nCalculado en %d segundos\n",(int)difftime(after,before)); //imprime el tiempo demorado en realizar las simulaciones
}

 
NODE * select_node_by_user(NODE * L,char valor,char palo)/*esta funcion selecciona el nodo correspondiente al valor y palo de la carta enviada y lo devuelve*/
{
 NODE * aux=L;
 while(aux->D.carta != valor || aux->D.palo[0] != palo)
	aux=aux->next;
 return aux;
} 

int realizar_sim (CARTA * jug1, CARTA * jug2, CARTA * mesa) /*esta funcion calcula la jugada de ambas manos y devuelve el ganador*/
{
 int r;
 char * jugada1,jugada2[7]; /*2 array de caracteres para almacenar las jugadas de ambos jugadores (jugada1 por malloc debido a errores inexplicables)*/
 jugada1=(char *)malloc(7);
 bzero(jugada1,7);
 bzero(jugada2,7);
 calcular_jugada1(jug1,mesa,jugada1); //se calcula la jugada de cada jugador
 calcular_jugada1(jug2,mesa,jugada2); 
 r=calcular_jugada4(jugada1,jugada2); //se compara y se obtiene al ganador
 free(jugada1); //liberacion de memoria
 return r; //devolucion del ganador
}
