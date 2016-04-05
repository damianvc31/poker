#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "defs.h"

void print_cards(CARTA * C,int n) //funcion que imprime cartas
{
 int i;
 for(i=0;i<n;i++)
	printf("%c de %s\n",C[i].carta,C[i].palo);
}

int showdown(CARTA * jug1,CARTA * jug2,CARTA * mesa,int * pjug1,int * pjug2,int pot,int p,int socketfd1,int socketfd2) /*esta funcion lleva a cabo el showdown*/
{
 int r;
 if(p<2) //si se llego al showdown antes del river se muestran las cartas que faltaban
	{
	 printf("\nFlop:\n\n");
	 print_cards(mesa,3);
	}
 if(p<3)
	{	 
	 printf("\nTurn:\n\n");
	 print_cards(mesa+3,1);
	}
 if(p<4)
	{
	 printf("\nRiver:\n\n");
	 print_cards(mesa+4,1);
	}
 r=calcular_jugada(jug1,jug2,mesa,socketfd1,socketfd2);
 if(!r)
	return 0;
 if(r==1)
	 *pjug1 = *pjug1 + pot;
 else if(r==2)
	 *pjug2 = *pjug2 + pot;
 else if(r==3)
 	{
	 *pjug1 = *pjug1 + (pot/2);
	 *pjug2 = *pjug2 + (pot/2);
	}	
 if(r!=3)
	printf("\nJugador %d gana el pozo (%d)\n",r,pot);
 else
	printf("\nReparten el pozo (%d)\n",pot); //dependiendo del resultado se suman los puntos correspondienten al ganador
 return 1;
}

void no_showdown(int * pjug1,int * pjug2,int pot,int g) /*esta funcion lleva a cabo el 'no showdown' es decir cuando algun jugador se reitra se le da los puntos al otro*/
{
 if(g==1)
 	*pjug1 = *pjug1 + pot;	
 else if(g==2)
	 *pjug2 = *pjug2 + pot;
 printf("\nJugador %d gana el pozo (%d)\n",g,pot);
}

int calcular_jugada (CARTA * jug1, CARTA * jug2, CARTA * mesa,int socketfd1,int socketfd2)/*funcion principal para calcular una jugada con las 7 cartas disponibles*/
{
 int r;
 char * jugada1,jugada2[7]; /*2 array de caracteres para almacenar las jugadas de ambos jugadores (jugada1 por malloc debido a errores inexplicables)*/
 jugada1=(char *)malloc(7);
 bzero(jugada1,7);
 bzero(jugada2,7);
 calcular_jugada1(jug1,mesa,jugada1); //calcula la jugada del jugador 1, teniendo en cuenta sus cartas y la mesa
 calcular_jugada1(jug2,mesa,jugada2); //lo mismo para el jugador 2
 printf("\nJugador 1 tiene "); //impresion de los resultados
 results(jugada1); 
 printf("Jugador 2 tiene ");
 results(jugada2);
 if(!enviar_result(socketfd1,socketfd2,jugada1,jugada2))//envia las jugadas a los jugadores
	return 0;
 r=calcular_jugada4(jugada1,jugada2); //compara ambas jugadas y obtiene el ganador
 if(!enviar_ganador(socketfd1,socketfd2,r)) //envia el ganador a los jugadores
	return 0;
 free(jugada1);
 return r; //devuelve el ganador
}

void calcular_jugada1 (CARTA * jug,CARTA * mesa,char * jugada)/*esta funcion es otro paso para calcular una jugada, se encarga de formar todas las combinaciones de 5 cartas que cada jugador puede formar con la mesa y su mano*/
{
 CARTA C[7]={jug[0],jug[1],mesa[0],mesa[1],mesa[2],mesa[3],mesa[4]}; //array de cartas con las 7 cartas disponibles
 int i,j;
 for(i=0;i<6;i++)//se recorre el array con 2 indices, que determinaran las 2 cartas que no se tomaran en cuenta cada vez
	{
	 for(j=i+1;j<7;j++)
		 calcular_jugada2(C,jugada,i,j);//se envia a otra funcion los datos
	}
}
		 
void calcular_jugada2(CARTA * C,char * jugada,int i,int j)/*esta funcion recibe los datos de la anterior y genera cada combinacion de 5 cartas, para determinar el juego que forman*/
{
 int k,l;
 char jugada2[7];//array jugada auxiliar para comparar con la jugada ganadora hasta ese momento
 CARTA D[5];//array de 5 cartas definitivo
 for(k=0,l=0;k<5;l++)//se recorre las 7 cartas y solo se agregan al nuevo array las que corresponde
	{
	 if(l!=i && l!=j)
		{
		 D[k]=C[l];
		 k++;
		}
	}
 calcular_jugada3(D,jugada2);/*se envia la combinacion de 5 cartas a una nueva funcion que pondra en el array 'jugada2' el juego resultante*/
 calcular_jugada4(jugada,jugada2);/*se compara la jugada obtenida con este array de 5 cartos con la ganadora mediante esta funcion*/
}
	 
void calcular_jugada3(CARTA * D,char * jugada2)/*esta funcion calcula el juego de una combinacion de 5 cartas comprobando las repeticiones*/
{
 CARTA aux;
 int i,j,k;
 int rep1[5]={1,-1,-1,-1,-1},rep2[4]={1,-1,-1,-1};
 int * rep=rep1;
 for(i=0;i<4;i++)
	{
	 for(j=i+1;j<5;j++)
		{
		 if(carta_a_valor(D[i].carta) < carta_a_valor(D[j].carta))
			{
			 aux=D[i];
			 D[i]=D[j];
			 D[j]=aux;
			}
		}
	}
 for(i=0,k=1;i<4;i++)
	{
	 if(D[i].carta==D[i+1].carta)
		{
		 rep[0]++;
		 rep[k]=i;
		 rep[k+1]=i+1;
		 k++;
		}
	 else if(rep1[0]>1)
		{
		 rep=rep2;
		 k=1;
		}
	}
 if(rep1[0]==1) //si no hubo repeticiones
	{
	 jugada2[0]=1; // carta alta
	 for(i=1;i<6;i++) //se llena el array con los valores de las cartas ordenados de mayor a menor
		 jugada2[i]=carta_a_valor(D[i-1].carta);
	 if(escalera(jugada2)) //se llama a las funciones escalera y color para comprobar si existen estos juegos
		{
		 jugada2[0]=5; // escalera
	 	 if(color(D))
		 	jugada2[0]=9; // escalera real
		}
	 else if(color(D))
			jugada2[0]=6; // color
	 	
	}
 else if(rep1[0]==2 && rep2[0]==1) //si hubo una repeticion
	{
	 jugada2[0]=2; // un par
	 jugada2[1]=carta_a_valor(D[rep1[1]].carta);
	 jugada2[2]=jugada2[1];
	 for(i=0,j=3;i<5;i++) //se llenan el array los valores de las cartas no emparejadas de mayor a menor
		{
		 if(i!=rep1[1] && i!=rep1[2])
			{
			 jugada2[j]=carta_a_valor(D[i].carta);
			 j++;
			}
		}
	}
 else if(rep1[0]==2 && rep2[0]==2) //si hubo 2 repeticiones
	{
	 jugada2[0]=3; // doble par
	 jugada2[1]=carta_a_valor(D[rep1[1]].carta);
	 jugada2[2]=jugada2[1];
	 jugada2[3]=carta_a_valor(D[rep2[1]].carta);
	 jugada2[4]=jugada2[3];
	 for(i=0,j=5;i<5;i++) //se llena el array con la unica carta que no esta emparejada
		{
		 if(i!=rep1[1] && i!=rep1[2] && i!=rep2[1] && i!=rep2[2])
			 jugada2[j]=carta_a_valor(D[i].carta);
		}
	}
 else if(rep1[0]==3 && rep2[0]==1) //si hubo una repeticion triple
	{
	 jugada2[0]=4; // trio
	 jugada2[1]=carta_a_valor(D[rep1[1]].carta);
	 jugada2[2]=jugada2[1];
	 jugada2[3]=jugada2[1];
	 for(i=0,j=4;i<5;i++) //se llena el array con las cartas no emparejadas
		{
		 if(i!=rep1[1] && i!=rep1[2] && i!=rep1[3])
			{
			 jugada2[j]=carta_a_valor(D[i].carta);
			 j++;
			}
		}
	}
 else if((rep1[0]==3 && rep2[0]==2) || (rep1[0]==2 && rep2[0]==3))//si hubo una repeticion triple y otra doble
	{
	 jugada2[0]=7; // full
	 if(rep1[0]>rep2[0])
		{
		 jugada2[1]=carta_a_valor(D[rep1[1]].carta);
		 jugada2[2]=jugada2[1];
		 jugada2[3]=jugada2[1];
		 jugada2[4]=carta_a_valor(D[rep2[1]].carta);
		 jugada2[5]=jugada2[4];
		}	
	 else
		{
		 jugada2[1]=carta_a_valor(D[rep2[1]].carta);
		 jugada2[2]=jugada2[1];
		 jugada2[3]=jugada2[1];
		 jugada2[4]=carta_a_valor(D[rep1[1]].carta);
		 jugada2[5]=jugada2[4];
		}			
	}
 else if(rep1[0]==4)//si hubo una repeticion cuadruple
	{ 
	 jugada2[0]=8; // poker
	 jugada2[1]=carta_a_valor(D[rep1[1]].carta);
	 jugada2[2]=jugada2[1];
	 jugada2[3]=jugada2[1];
	 jugada2[4]=jugada2[1];
	 for(i=0,j=5;i<5;i++) //se llena el array con la carta no emparejada
		{
		 if(i!=rep1[1] && i!=rep1[2] && i!=rep1[3] && i!=rep1[4])
			 jugada2[j]=carta_a_valor(D[i].carta);
		}
	}
}
 
char carta_a_valor(char c)//esta funcion convierte el caracter de la carta al valor numerico correspondiente
{
 if(c >= '2' && c <= '9')
	return c-48;
 if(c=='A')
	return 14;
 if(c=='T')
	return 10;
 if(c=='J')
	return 11;
 if(c=='Q')
	return 12;
 if(c=='K')
	return 13;
 return 0;
}
	 
int escalera(char * jugada2) //esta funcion comprueba si hay escalera
{
 int i;
 if(jugada2[1]==14 && jugada2[2]==5 && jugada2[3]==4 && jugada2[4]==3 && jugada2[5]==2) 
	{
	 jugada2[1]=5;
	 jugada2[2]=4;
	 jugada2[3]=3;
	 jugada2[4]=2;
	 jugada2[5]=1;
	 return 1;
	}	  
 for(i=1;i<5;i++)
	{
	 if(jugada2[i]-jugada2[i+1]!=1)
		return 0;
	}
 return 1;
}

int color(CARTA * D)//esta funcion comprueba si hay color
{
 int i;
 for(i=0;i<4;i++)
	{
	 if(strcmp(D[i].palo,D[i+1].palo)!=0)
		return 0;
	}
 return 1;
}	 
	 
int calcular_jugada4(char * jugada,char *jugada2)/*esta funcion compara dos jugadas y si la 2da es mejor, la copia en la primera*/
{
 int i;
 for(i=0;i<6;i++)//se recorrer el array jugada, en cuanto haya algo distinto, el valor mas grande sera la mejor jugada
	{
	 if(jugada2[i]>jugada[i])
		{
	 	 strcpy(jugada,jugada2);
	 	 return 2; 
		}
	 else if(jugada2[i]<jugada[i])
		 return 1;
	}
 return 3; //si hay empate devuelve 3
}
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
