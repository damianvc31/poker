#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
	
void results(char * jugada)//esta funcion imprime una jugada en pantalla
{
 switch(jugada[0])//segun el primer caracter del array jugada
	{
	 case 1:
		printf("Carta Alta - %c%c%c%c%c\n",valor_a_carta(jugada[1]),valor_a_carta(jugada[2]),valor_a_carta(jugada[3]),valor_a_carta(jugada[4]),valor_a_carta(jugada[5]));
		break;
	 case 2:
		printf("Par Simple - %c%c %c%c%c\n",valor_a_carta(jugada[1]),valor_a_carta(jugada[2]),valor_a_carta(jugada[3]),valor_a_carta(jugada[4]),valor_a_carta(jugada[5]));
		break;			 
	 case 3:
		printf("Par Doble - %c%c %c%c %c\n",valor_a_carta(jugada[1]),valor_a_carta(jugada[2]),valor_a_carta(jugada[3]),valor_a_carta(jugada[4]),valor_a_carta(jugada[5]));
		break;
	 case 4:
		printf("Trio - %c%c%c %c %c\n",valor_a_carta(jugada[1]),valor_a_carta(jugada[2]),valor_a_carta(jugada[3]),valor_a_carta(jugada[4]),valor_a_carta(jugada[5]));
		break;
	 case 5:
		printf("Escalera - %c%c%c%c%c\n",valor_a_carta(jugada[5]),valor_a_carta(jugada[4]),valor_a_carta(jugada[3]),valor_a_carta(jugada[2]),valor_a_carta(jugada[1]));
		break;
	 case 6:
		printf("Color - %c%c%c%c%c\n",valor_a_carta(jugada[1]),valor_a_carta(jugada[2]),valor_a_carta(jugada[3]),valor_a_carta(jugada[4]),valor_a_carta(jugada[5]));
		break;
	 case 7:
		printf("Full House - %c%c%c %c%c\n",valor_a_carta(jugada[1]),valor_a_carta(jugada[2]),valor_a_carta(jugada[3]),valor_a_carta(jugada[4]),valor_a_carta(jugada[5]));
		break;
	 case 8:
		printf("Poker - %c%c%c%c %c\n",valor_a_carta(jugada[1]),valor_a_carta(jugada[2]),valor_a_carta(jugada[3]),valor_a_carta(jugada[4]),valor_a_carta(jugada[5]));
		break; 
	 case 9:
		if(jugada[1]==14)
			printf("Escalera Real - %c%c%c%c%c\n",valor_a_carta(jugada[5]),valor_a_carta(jugada[4]),valor_a_carta(jugada[3]),valor_a_carta(jugada[2]),valor_a_carta(jugada[1]));
		else
			printf("Escalera de Color - %c%c%c%c%c\n",valor_a_carta(jugada[5]),valor_a_carta(jugada[4]),valor_a_carta(jugada[3]),valor_a_carta(jugada[2]),valor_a_carta(jugada[1]));			
		break;
	}
}

char valor_a_carta(char c)//esta funcion convierte un valor numerico de una carta a caracter 
{
 switch(c)
	{
	 case 1:
		return 'A';
	 case 2:
		return '2';
	 case 3:
		return '3';
	 case 4:
		return '4';
	 case 5:
		return '5';
	 case 6:
		return '6';
	 case 7:
		return '7';
	 case 8:
		return '8';
	 case 9:
		return '9';
	 case 10:
		return 'T';	
	 case 11:
		return 'J';
	 case 12:
		return 'Q';
	 case 13:
		return 'K';
	 case 14:
		return 'A';
	}
 return 0;
}

void print_cards(CARTA * C,int n)//esta funcion imprime las cartas
{
 int i;
 for(i=0;i<n;i++)
	printf("%c de %s\n",C[i].carta,C[i].palo);
}

void print_puntos(int pyo,int prival,int ptempyo,int ptemprival)//esta funcion imprime el estado actual de la mesa
{
 printf("\nUd->%d___%d_________________%d___%d<-Rival\n",pyo,ptempyo,ptemprival,prival);
}  

int showdown(CARTA * jug1,CARTA * jug2,CARTA * mesa,int * pjug1,int * pjug2,int pot,int id,int p,char * ip, int port)/*esta funcion lleva a cabo el showdown*/
{
 char jugada1[7],jugada2[7];
 int r;
 printf("\nSe muestran las cartas:\n");
 if(id==1)
	{
	 printf("\nCartas de Ud:\n\n");
	 print_cards(jug1,2);
	 printf("\nCartas del rival:\n\n");
	 print_cards(jug2,2);
	}
 else
	{
	 printf("\nCartas de Ud:\n\n");
	 print_cards(jug2,2);
	 printf("\nCartas del rival:\n\n");
	 print_cards(jug1,2);
	}
 if(p<2) //dependiendo de cuando se termino all in se muestran algunas cartas o se omiten 
	{
 	 printf("\nPresione <Enter> para ver flop...");
	 getchar();
	 printf("\nFlop:\n\n");
	 print_cards(mesa,3);
	}
 if(p<3)
	{
	 if(p==2)
	 	{
		 printf("\nFlop:\n\n");
	 	 print_cards(mesa,3);
		}
 	 printf("\nPresione <Enter> para ver turn...");
	 getchar();	 
	 printf("\nTurn:\n\n");
	 print_cards(mesa+3,1);
	}
 if(p<4)
	{
	 if(p==3)
		{
	 	 printf("\nFlop:\n\n");
	 	 print_cards(mesa,3);
	 	 printf("\nTurn:\n\n");
	 	 print_cards(mesa+3,1);
		}
 	 printf("\nPresione <Enter> para ver river...");
	 getchar();
	 printf("\nRiver:\n\n");
	 print_cards(mesa+4,1);
	}
 if(p==4)
	{
 	 printf("\nFlop:\n\n");
	 print_cards(mesa,3);
	 printf("\nTurn:\n\n");
	 print_cards(mesa+3,1);	
	 printf("\nRiver:\n\n");
	 print_cards(mesa+4,1);
	} 
 printf("\nPresione <Enter> para ver resultado...");
 getchar(); //pausa antes de ver el resultado
 if(!recibir_result(ip,port,jugada1,jugada2))
	return 0; //recibe resultados del servidor
 if(!(r=recibir_ganador(ip,port)))
	return 0; //recibe el ganador del servidor
 if(id==1)//muestra los resultados a ambos jugadores dependiendo de su ID
	{
	 printf("\nUd tiene ");
	 results(jugada1); 
	 printf("Rival tiene ");
	 results(jugada2);
	}
 else if(id==2)
	{
	 printf("\nUd tiene ");
	 results(jugada2);
	 printf("Rival tiene ");
	 results(jugada1);
	}
 if(r!=3) //si no hay empate
	if(r==id)//se informa el ganador
	 	printf("\nUd gana con ");
 	else
		printf("\nRival gana con ");
 else if(r==3) //si hay empate se informa
	 printf("\nEmpatan con ");
 results(jugada1); //se muestra la jugada ganadora
 if(r==1)//dependiendo del ganador se suman el pozo al jugador correspondiente
	 *pjug1 = *pjug1 + pot;
 else if(r==2)
	 *pjug2 = *pjug2 + pot;
 else if(r==3) /* si hay empate se divide el pozo (si el numero es impar una ficha se desprecia (ejemplo pozo 75, 37 fichas cada jugador, 1 se pierde)*/
 	{
	 *pjug1 = *pjug1 + (pot/2);
	 *pjug2 = *pjug2 + (pot/2);
	}	
 if(r==id)//se informa el resultado 
	printf("Ud gana el pozo (%d)\n",pot);
 else if(r!=3)
	printf("Rival gana el pozo (%d)\n",pot);
 else if(r==3)
	printf("Dividen el pozo(%d)\n",pot);
 return 1;
}

void no_showdown(int * pjug1,int * pjug2,int pot,int g,int id)/*esta funcion lleva a cabo el 'no showdown' es decir cuando un jugador se retira*/
{
 if(g==1)//se suma el pozo al jugador correspondiente
 	*pjug1 = *pjug1 + pot;	
 else if(g==2)
	 *pjug2 = *pjug2 + pot;
 if(id==g)//se informan los resultados
 	printf("\nUd gana el pozo (%d)\n",pot);
 else
	printf("\nRival gana el pozo(%d)\n",pot);
}
