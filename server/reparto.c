#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "defs.h"


void reparto (CARTA * jug1,CARTA * jug2,CARTA * mesa) //esta funcion reparte las cartas a ambos jugadores y a la mesa
{
 NODE * mazo=NULL; //declaracion del mazo
 fill_mazo(&mazo); //se carga con todas las cartas
 deal_cards(&mazo,jug1,jug2,mesa); //se mezcla y reparte
 delete_list(&mazo);//se borra el mazo
}

void fill_mazo(NODE ** mazo) //esta funcion carga las cartas al mazo
{
 int i,j;
 NODE * N;
 for(i=0;i<4;i++) //dos for para pasar por todas las cartas de un mazo
	{
	 for(j=0;j<13;j++)
		{
	 	 N = (NODE *)malloc(sizeof(NODE)); //se pide un nodo al sistema operativo
		 switch(i) //se carga un palo y un valor a la carta
			{
			 case 0:
				strcpy(N->D.palo,"CORAZONES");
				break;
			 case 1:
				strcpy(N->D.palo,"DIAMANTES");
				break;
			 case 2:
				strcpy(N->D.palo,"TREBOLES");
				break;
			 case 3:
				strcpy(N->D.palo,"PICAS");
				break;
			}
		 switch(j)
			{
			 case 0:
				N->D.carta='A';
				break;
			 case 1:
				N->D.carta='2';
				break;
			 case 2:
				N->D.carta='3';
				break;
			 case 3:
				N->D.carta='4';
				break;
			 case 4:
				N->D.carta='5';
				break;
			 case 5:
				N->D.carta='6';
				break;
			 case 6:
				N->D.carta='7';
				break;
			 case 7:
				N->D.carta='8';
				break;
			 case 8:
				N->D.carta='9';
				break;
			 case 9:
				N->D.carta='T';
				break;
			 case 10:
				N->D.carta='J';
				break;
			 case 11:
				N->D.carta='Q';
				break;
			 case 12:
				N->D.carta='K';
				break;
			}			 
		insert_node(mazo,N);//se ingresa en el mazo
		}
	}
}

int count_nodes(NODE * L) //esta funcion cuenta las cartas de un mazo
{
 int ndatos=0;
 NODE * aux = L;
 if(aux!=NULL)
	{
	 for(ndatos=1;aux->next!=L;ndatos++)
	 	aux=aux->next;
	}
 return ndatos;
}

void deal_cards(NODE ** mazo,CARTA * jug1,CARTA * jug2,CARTA * mesa)//esta funcion mezcla el mazo y reparte las cartas
{
 CARTA * jug=jug1;
 NODE * N;
 int i,ncarta,cant=count_nodes(*mazo); // se define la cantidad inicial de cartas del mazo
 /*printf("\nMazo ordenado:\n\n");
 print_mazo(*mazo);*/
 *mazo = shuffle_mazo (mazo,cant); //se mezcla el mazo
 /*printf("\nMazo mezclado:\n\n");
 print_mazo(*mazo);*/
 for(i=0,ncarta=0;i<4;i++) //for para repartir 4 cartas
	{
	 N = *mazo;
	 jug[ncarta]=N->D; //se saca la primera carta del mazo y se asigna los valores a la mano del jugador
	 detach_node(mazo,N);
	 free(N); //se borra el nodo de esa carta
	 if(jug==jug1) //se cambia el puntero jug para apuntar al otro jugador, y asi repartir 2 cartas a ambos
		jug=jug2;
	 else
		{
		 jug=jug1;
		 ncarta++;
		}
	} 
 for(i=0;i<5;i++) //de la misma forma se reparten 5 cartas a la mesa
	{
	 N = *mazo;
	 mesa[i]=N->D;
	 detach_node(mazo,N);	 
	 free(N);
	}
}

NODE * shuffle_mazo (NODE ** unshuffled_mazo,int cant)//funcion que mezcla el mazo 
{
 NODE * shuffled_mazo = NULL, * N; //se crea otra lista para el mazo mezclado
 for(;cant>0;cant--) //bucle que termina cuando no quedan mas cartas en el mazo ordenado
	{
	 N = select_node (*unshuffled_mazo,cant); /*se selecciona una carta del mazo aleatoriamente y se coloca como primera del mazo mezclado, desligandola del mazo ordenado*/
	 detach_node(unshuffled_mazo,N); 
	 N->next=NULL;
	 insert_node(&shuffled_mazo,N);
	}
 return shuffled_mazo; //se devuelve el mazo mezclado
}
	
void print_mazo(NODE * L)//esta funcion imprime el mazo 
{
 NODE * aux=L;
 do
	{
	 printf("%c de %s\n",aux->D.carta,aux->D.palo);
	 aux=aux->next;
	}while(aux!=L);
}	 
 

void insert_node(NODE ** L, NODE * N) //esta funcion ingresa un nodo a una lista
{
 NODE * aux;
 if(*L==NULL)
 	 *L=N;
 else
 	{
  	 aux=*L;
  	 while(aux->next!=*L)
		aux=aux->next;
	 aux->next=N;
 	}
 N->next=*L;
}
	 
NODE * select_node(NODE * L,int cant) //esta funcion selecciona un nodo aleatoriamente
{
 int i,num=random_num(cant);
 for(i=0;i<num;i++)
	 L=L->next;
 return L;
}

void detach_node(NODE ** L,NODE * N) //esta funcion separa un nodo de una lista (sin borrarlo)
{
 NODE * aux= *L;
 if((*L)==(*L)->next)
	*L=NULL;
 else if(N==*L)
	{
	 while(aux->next!=*L)
		aux=aux->next;
	 (*L)=(*L)->next;
	 aux->next = *L;
	}
 else
	{
	 while(aux->next!=N)
		aux=aux->next;
	 aux->next=N->next;
	}
}

int random_num (int cant) /*esta funcion devuelve un numero aleatorio , que se utiliza para recorrer el mazo y seleccionar una carta aleatoriamente*/
{
 int rd,n,multiplo=max_multiplo(cant);
 unsigned char rnd;
 rd=open("/dev/urandom",O_RDONLY);
 do 
	{
 	 read(rd,&rnd,sizeof(unsigned char));
	}
 while(rnd>=multiplo);
 n=rnd;
 close(rd);
 return n;
}

int max_multiplo (int n) //esta funcion calcula el multiplo mayor menor a 256 de un numero
{
 int i,resultado;
 for(i=1;n*i<=256;i++);
 resultado = (n * i);
 return resultado;
}


void delete_list(NODE ** L) //funcion que borra una lista
{
 NODE * N;
 while(*L)
	{
	 N = *L;
	 detach_node(L,N);
	 free(N);
	}
}
