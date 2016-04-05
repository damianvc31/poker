#include <stdio.h>		// Standard I/O
#include <sys/types.h>	// Tipos y estructuras
#include <sys/socket.h>	// Sockets
#include <stdlib.h> 	// Libreria standard de C, para usar exit()
#include <unistd.h>		// Unix standard, para usar close()
#include <netinet/in.h>	// Estructuras para sockets
#include <strings.h>	// Para usar bzero()
#include "sockets.h"
#include "defs.h"

int enviar_cartas(int socketfd1,int socketfd2,CARTA * jug1,CARTA * jug2,CARTA * mesa) /*funcion que evnia las manos a ambos jugadores*/
{
 int clientfd1,clientfd2;
 clientfd1=aceptar(socketfd1);
 if(!clientfd1)
	return 0;
 write(clientfd1,jug1,sizeof(CARTA)*2);
 write(clientfd1,mesa,sizeof(CARTA)*5);
 write(clientfd1,jug2,sizeof(CARTA)*2);
 cerrar(clientfd1);
 clientfd2=aceptar(socketfd2);
 if(!clientfd2)
	return 0;
 write(clientfd2,jug2,sizeof(CARTA)*2);
 write(clientfd2,mesa,sizeof(CARTA)*5);
 write(clientfd2,jug1,sizeof(CARTA)*2);
 cerrar(clientfd2);
 return 1;
}

int preflop(int * pjug1, int * pjug2, int * ptemp1,int * ptemp2,int * g,int m,int sb,int bb,int socketfd1,int socketfd2)/*esta funcion lleva a cabo el preflop*/
{
 char a1=0,a2=0, *a; //variables que definen quien habla, el puntero apunta a una u otra alternativamente
 int * pjug,* ptemp, * ptempaux, * socketfd, * v; /*punteros que apuntan a los puntajes y sockets de los jugadores en forma alternada*/
 int v1=0,v2=0,i=0;/*v define el valor de la apuesta de cada jugador, i marcara si la funcion sale con un fold, si quedan all in, o si se debe seguir al flop*/
 if(m==1)
	{ //si el jugador 1 es ciega chica, empieza hablando, los punteros apuntan a sus variables
	 socketfd=&socketfd1; 
	 *ptemp1=sb;
	 *ptemp2=bb;
	 pjug=pjug1;
	 ptemp=ptemp1;
	 a=&a1;
	 v=&v1;
	 ptempaux=ptemp2;
	}
 else if(m==2)
	{//caso contrario, lo mismo para el jugador 2
	 socketfd=&socketfd2;
	 *ptemp2=sb;
	 *ptemp1=bb;
	 pjug=pjug2;
	 ptemp=ptemp2;
	 a=&a2;
	 v=&v2;
	 ptempaux=ptemp1;
	}
 if(*pjug1<*ptemp1) //se verifica si el jugador 1 no cubre la ciega
	{	
	 *ptemp1=*pjug1;
	 *pjug1=0;
	}
 else
 	*pjug1=*pjug1-*ptemp1; 
 if(*pjug2<*ptemp2) //idem para el jugador 2
	{	
	 *ptemp2=*pjug2;
	 *pjug2=0;
	}
 else
 	*pjug2=*pjug2-*ptemp2;
 if(*pjug1==0 && *ptemp1 <= *ptemp2) //se verifica si algun jugador quedo all in y se debe continuar directamente al showdown
	{
	 *pjug2=*pjug2 + (*ptemp2-*ptemp1);
	 *ptemp2=*ptemp2 - (*ptemp2-*ptemp1);
	 i=2;
	}
 else if(*pjug2==0 && *ptemp2 <= *ptemp1)
	{
	 *pjug1=*pjug1 + (*ptemp1-*ptemp2);
	 *ptemp1=*ptemp1 - (*ptemp1-*ptemp2);
	 i=2;
	}		 	 
 if(i==2)
	return 2;
 if(!turno(a,v,*socketfd)) //se le toma al turno a la ciega chica
	return 0;
 if(*a=='l' || *a=='r') //si la ciega chica completa o sube se prosigue verificando nuevamente si algun jugador queda all in
	{
	 if(sb>=*pjug) //si se queda all in, se hacen los ajustes pertinentes
		{
		 *ptemp=*ptemp + *pjug;
		 *pjug=0;
		 if(ptempaux==ptemp1)
		 	 *pjug1 = *pjug1 + (*ptempaux - *ptemp);
		 if(ptempaux==ptemp2)
			 *pjug2 = *pjug2 + (*ptempaux - *ptemp);
		 *ptempaux= *ptemp;
		 i=2;	//si alguno quedo all in se sale con un 2 para avisar que hay showdown
		 if(!enviar_datos(i,a,v,socketfd1,socketfd2))
		 	return 0;	 
		 return 2;
		}
	 else if(*a=='l')//si no, se ajusta el limp de forma normal
		{
		 *pjug=*pjug-sb;
	 	 *ptemp=*ptemp+sb;
		}
	 else if(*a=='r') //o la subida
		{
		 *pjug=*pjug-(*v-*ptemp);
		 *ptemp=*ptemp+(*v-*ptemp);
		}		
	}
 while(*a!='f') //mientras ningun jugador se retire, bucle de turnos
	{
	 if((*a=='x') || (*a=='c')) /*si un jugador pasa o paga una apuesta sin subir, se verifica si hay all in o se debe continuar al flop*/
		{
		 if((*pjug1)==0 || (*pjug2)==0)
			{
			 i=2;		 	 
			 if(!enviar_datos(i,a,v,socketfd1,socketfd2))
			 	return 0;
			 return 2;
			}	
 	 	 else
			{
			 i=1;		 	 
			 if(!enviar_datos(i,a,v,socketfd1,socketfd2))
			 	return 0;
			 return 1;
			}	
		}
	 if(!enviar_datos(i,a,v,socketfd1,socketfd2)) //se envia los datos de la ultima jugada a cada jugador
		return 0;
	 switch(m) //se pasa el turno al otro jugador
		{
		 case 1:
			m=2;
			socketfd=&socketfd2;
			pjug=pjug2;
			ptemp=ptemp2;
	 		a=&a2;
			v=&v2;
			ptempaux=ptemp1;
			break;
		 case 2:
			m=1;
			socketfd=&socketfd1;
			pjug=pjug1;
			ptemp=ptemp1;
			a=&a1;
			v=&v1;
			ptempaux=ptemp2;
			break;
		}
	 if(!turno(a,v,*socketfd)) // se le toma el turno a cada jugador
		return 0;
	 if(*a=='c') //se toman los cambios en las pilas de los jugadores si pagan una apuesta o suben
		{
		 if((*ptempaux-*ptemp)>*pjug)
			{
			 *ptemp=*ptemp + *pjug;
			 *pjug=0;
			 if(ptempaux==ptemp1)
				 *pjug1 = *pjug1 + (*ptempaux - *ptemp);
			 if(ptempaux==ptemp2)
				 *pjug2 = *pjug2 + (*ptempaux - *ptemp);
			 *ptempaux= *ptemp;		
			}
		 else
			{	 
			  *pjug=*pjug-(*ptempaux-*ptemp);
			  *ptemp=*ptemp+(*ptempaux-*ptemp);
			}
		}
	 if(*a=='r')
		{
	 	 *pjug=*pjug-(*v-*ptemp);
		 *ptemp=*ptemp+(*v-*ptemp);
		}		 
	}
 i=3;	 //cuando alguien se retira el parametro i se pone en 3 para avisar al programa principal
 if(!enviar_datos(i,a,v,socketfd1,socketfd2))//se envia esto a los clientes
	return 0;
 if(m==1) //dependiendo de quien se retiro, el ganador sera el otro jugador
	{
	 *g=2;
	 *pjug2=*pjug2 + (*ptemp2-*ptemp1);
	 *ptemp2=*ptemp1;
	}
 else if(m==2)
	{
	 *g=1;
	 *pjug1=*pjug1 + (*ptemp1-*ptemp2);
	 *ptemp1=*ptemp2;
	}
 return 3;
} 

int postflop(int * pjug1,int * pjug2,int * ptemp1,int * ptemp2,int * g,int m,int sb,int bb,int socketfd1,int socketfd2)/*esta funcion desarrolla el flop, turn o river de forma similar al preflop, aunque sin ciegas iniciales*/
{
 char a1=0,a2=0, *a;
 int * pjug,* ptemp, * ptempaux, * v,* socketfd;
 int v1=0,v2=0,i=0,aux=0;
 if(m==1) //dependiendo de quien fue la ciega chica esta mano, su oponente comienza hablando postflop
	{
	 m=2;
	 socketfd=&socketfd2;
	 pjug=pjug2;
	 ptemp=ptemp2;
	 a=&a2;
	 v=&v2;
	 ptempaux=ptemp1;
	}
 else if(m==2)
	{
	 m=1;
	 socketfd=&socketfd1;
	 pjug=pjug1;
	 ptemp=ptemp1;
	 a=&a1;
	 v=&v1;
	 ptempaux=ptemp2;
	}
 if(!turno(a,v,*socketfd))
 	return 0;
 if(*a=='r')
	{
	 *pjug=*pjug-(*v-*ptemp);
	 *ptemp=*ptemp+(*v-*ptemp);
	}
 while(*a!='f')
	{
	 if((*a=='x' && aux) || (*a=='c'))
		{
		 if((*pjug1)==0 || (*pjug2)==0)
			{
			 i=2;		 	 
			 if(!enviar_datos(i,a,v,socketfd1,socketfd2))
			 	return 0;
			 return 2;
			}	
 	 	 else
			{
			 i=1;		 	 
			 if(!enviar_datos(i,a,v,socketfd1,socketfd2))
			 	return 0;
			 return 1;
			}	
		}
 	 if(!enviar_datos(i,a,v,socketfd1,socketfd2))
		return 0;
	 aux=1;
	 switch(m)
		{
		 case 2:
			m=1;
			socketfd=&socketfd1;
			pjug=pjug1;
			ptemp=ptemp1;
	 		a=&a1;
			v=&v1;
			ptempaux=ptemp2;
			break;
		 case 1:
			m=2;
			socketfd=&socketfd2;
			pjug=pjug2;
			ptemp=ptemp2;
			a=&a2;
			v=&v2;
			ptempaux=ptemp1;
			break;
		}
	 if(!turno(a,v,*socketfd))
		return 0;
	 if(*a=='c')
		{
		 if((*ptempaux-*ptemp)>*pjug)
			{
			 *ptemp=*ptemp+*pjug;
			 *pjug=0;
			 if(ptempaux==ptemp1)
			 	 *pjug1 = *pjug1 + (*ptempaux - *ptemp);
			 if(ptempaux==ptemp2)
				 *pjug2 = *pjug2 + (*ptempaux - *ptemp);
			 *ptempaux= *ptemp;
			}
		 else
			{	 
			  *pjug=*pjug-(*ptempaux-*ptemp);
			  *ptemp=*ptemp+(*ptempaux-*ptemp);
			}
		}
	 if(*a=='r')
		{
	 	 *pjug=*pjug-(*v-*ptemp);
		 *ptemp=*ptemp+(*v-*ptemp);
		}		 
	}
 i=3;		 	 
 if(!enviar_datos(i,a,v,socketfd1,socketfd2))
	return 0;
 if(m==1)
	{
	 *g=2;
	 *pjug2=*pjug2 + (*ptemp2-*ptemp1);
	 *ptemp2=*ptemp1;
	}
 else if(m==2)
	{
	 *g=1;
	 *pjug1=*pjug1 + (*ptemp1-*ptemp2);
	 *ptemp1=*ptemp2;
	}
 return 3;
}	

int enviar_datos(int i,char * a,int * v,int socketfd1,int socketfd2)/*esta funcion envia datos de la ultima jugada a cada jugador*/
{
 int clientfd1,clientfd2;
 clientfd1=aceptar(socketfd1);
 if(!clientfd1)
	return 0;
 write(clientfd1,&i,sizeof(int)); 
 write(clientfd1,a,1);
 write(clientfd1,v,sizeof(int));
 cerrar(clientfd1);
 clientfd2=aceptar(socketfd2); 
 if(!clientfd2)
	return 0;
 write(clientfd2,&i,sizeof(int));
 write(clientfd2,a,1);
 write(clientfd2,v,sizeof(int));
 cerrar(clientfd2);
 return 1;
}

int turno(char * a,int * v,int socketfd)//esta funcion recibe los datos del jugador que toma el turno
{
 int clientfd;
 clientfd=aceptar(socketfd);
 if(!clientfd)
	return 0;
 read(clientfd,a,1);
 read(clientfd,v,sizeof(int));
 cerrar(clientfd);
 return 1;
}


int recibir_nombre(int socketfd, char * nombre)//esta funcion recibe los nombres de los usuarios
{
 int i=0,clientfd;
 clientfd=aceptar(socketfd);
 if(!clientfd)
	return 0;
 do
	{
 	 read(clientfd,nombre+i,1);
	 i++;
	}while(nombre[i-1]!='\0');
 cerrar(clientfd);
 return 1;
}

int recibir_conf(int socketfd)//esta funcion recibe la confirmacion de los usuarios antes de comenzar la partida
{
 char conf;
 int clientfd;
 clientfd=aceptar(socketfd);
 if(!clientfd)
	return 0;
 read(clientfd,&conf,1);
 cerrar(clientfd);
 return 1;
}

int enviar_conf(int socketfd)//esta funcion envia la confirmacion a los usuarios para que se comience la partida
{
 char conf;
 int clientfd;
 clientfd=aceptar(socketfd);
 if(!clientfd)
	return 0;
 write(clientfd,&conf,1);
 cerrar(clientfd);
 return 1;
}

int enviar_result(int socketfd1,int socketfd2,char * jugada1,char * jugada2)/*esta funcion envia las jugadas obtenidas por ambos jugadores*/
{
 int clientfd1,clientfd2;
 clientfd1=aceptar(socketfd1);
 if(!clientfd1)
	return 0;
 write(clientfd1,jugada1,7);
 write(clientfd1,jugada2,7);
 cerrar(clientfd1);
 clientfd2=aceptar(socketfd2);
 if(!clientfd2)
	return 0;
 write(clientfd2,jugada1,7);
 write(clientfd2,jugada2,7);
 cerrar(clientfd2);
 return 1;
}
 
int enviar_ganador(int socketfd1,int socketfd2,int r)/*esta funcion envia el ganador de la mano a los jugadores(en caso de showdown)*/
{
 int clientfd1,clientfd2;
 clientfd1=aceptar(socketfd1);
 if(!clientfd1)
	return 0;
 write(clientfd1,&r,sizeof(int));
 cerrar(clientfd1);
 clientfd2=aceptar(socketfd2);
 if(!clientfd2)
	return 0;
 write(clientfd2,&r,sizeof(int));
 cerrar(clientfd2);
 return 1;
}


	 
