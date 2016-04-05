#include <stdio.h>		// Standard I/O
#include <sys/types.h>	// Tipos y estructuras
#include <sys/socket.h>	// Sockets
#include <stdlib.h> 	// Libreria standard de C, para usar exit()
#include <unistd.h>		// Unix standard, para usar close()
#include <netinet/in.h>	// Estructuras para sockets
#include <strings.h>	// Para usar bzero()
#include <string.h>
#include "sockets.h"
#include "defs.h"

int recibir_cartas(char * ip,int port,CARTA * yo, CARTA * rival, CARTA * mesa)//funcion recibe cartas de ambos jugador y mesa
{
 int socketfd;
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 read(socketfd,yo,sizeof(CARTA)*2);
 read(socketfd,mesa,sizeof(CARTA)*5);
 read(socketfd,rival,sizeof(CARTA)*2);
 cerrar(socketfd);
 return 1;
}

int preflop(int * pyo,int * prival,int * ptempyo,int * ptemprival,int * g,int id,int m,int sb,int bb,char * ip,int port)/*funcion para llevar a cabo el preflop*/
{
 char a=0,msj[10];
 int * pjug,* ptemp, * ptempaux; //punteros que apuntaran a los datos de quien hable, alternadamente
 int v=0,i=0; 
 if(m==id) //si el jugador es ciega chica
	{
	 strcpy(msj,"Ud");
	 *ptempyo=sb;
	 *ptemprival=bb;
	 pjug=pyo;
	 ptemp=ptempyo;
	 ptempaux=ptemprival;
	}
 if(m!=id)//si el jugador es ciega grande
	{
	 strcpy(msj,"Rival");
	 *ptempyo=bb;
	 *ptemprival=sb;
	 pjug=prival;
	 ptemp=ptemprival;
	 ptempaux=ptempyo;
	}
 if(*pyo<*ptempyo)//verificaciones por si hay un jugador que queda all in tras colocar las ciegas
	{	
	 *ptempyo=*pyo;
	 *pyo=0;
	}
 else
 	*pyo=*pyo-*ptempyo;
 if(*prival<*ptemprival)
	{	
	 *ptemprival=*prival;
	 *prival=0;
	}
 else	
 	*prival=*prival-*ptemprival;
 if(*pyo==0 && *ptempyo <= *ptemprival)
	{
	 *prival=*prival + (*ptemprival-*ptempyo);
	 *ptemprival=*ptemprival - (*ptemprival-*ptempyo);
	 i=2;
	}
 else if(*prival==0 && *ptemprival <= *ptempyo)
	{
	 *pyo=*pyo + (*ptempyo-*ptemprival);
	 *ptempyo=*ptempyo - (*ptempyo-*ptemprival);
	 i=2;
	}		 	 
 print_puntos(*pyo,*prival,*ptempyo,*ptemprival); //se imprimen los puntos de cada jugador, y las apuestas parciales
 if(i==2) //si hay all in se sale
	return 2;
 if(m==id) //si el jugador es ciega chica, toma el turno
	{
 	 if(!turno(&a,&v,*pjug,*ptemp,sb,ip,port,1))
	 	return 0;
	}
 while(1) //bucle infinito, se sale cuando se recibe la señal del servidor, y la variable 'i' deja de ser 0
	{
	 if(!recibir_datos(&i,&a,&v,ip,port)) //se reciben los datos de la ultima jugada
		return 0;
	 if(a=='f') //si hubo una retirada
		{
		 printf("\n%s foldea\n",msj); //se imprime lo sucedido ('msj' es "Ud" o "Rival")
		 if(m==1) //se define el ganador de la mano 
			*g=2;
		 else if(m==2)
			*g=1;
		 if(m==id)//se ajustan las pilas adecuadamente,devolviendo la apuesta no cubierta
			{
			 *prival=*prival + (*ptemprival-*ptempyo);
			 *ptemprival=*ptempyo;
			}
		 else if(m!=id)
			{
			 *pyo=*pyo + (*ptempyo-*ptemprival);
			 *ptempyo=*ptemprival;
			}
		}
	 if(a=='x') //en caso de check, o de igualada se imprime lo sucedido y se ajustan las pilas de ser necesario
		 printf("\n%s hace check\n",msj);		 
	 if(a=='l')
		{
		 printf("\n%s iguala\n",msj);
		 if(sb>*pjug)
			{
			 *ptemp=*ptemp + *pjug;
			 *pjug=0;
			 if(m==id)
			 	 *prival = *prival + (*ptempaux - *ptemp);
			 else
				 *pyo = *pyo + (*ptempaux - *ptemp);
			 *ptempaux = *ptemp;
			}
		 else
			{
			 *pjug=*pjug-sb;
		 	 *ptemp=*ptemp+sb;
			}		 			
		}
	 if(a=='c')
		{
		 printf("\n%s iguala\n",msj);
		 if((*ptempaux-*ptemp)>*pjug)
			{
			 *ptemp=*ptemp+*pjug;
			 *pjug=0;
			 if(m==id)
			 	 *prival = *prival + (*ptempaux - *ptemp);
			 else
				 *pyo = *pyo + (*ptempaux - *ptemp);
			 *ptempaux = *ptemp;
			}
		 else
			{	 
			 *pjug=*pjug-(*ptempaux-*ptemp);
			 *ptemp=*ptemp+(*ptempaux-*ptemp);
			}
		}
	 if(a=='r') //en caso de subida, se verifica si se jugo todo el resto o no, y se imprime el mensaje correspondiente
		{
		 if(v == (*pjug + *ptemp))
			printf("\n%s va all in\n",msj);
		 else
		 	printf("\n%s sube a %d\n",msj,v);
	 	 *pjug=*pjug-(v-*ptemp);
		 *ptemp=*ptemp+(v-*ptemp);
		}
	 if(a=='c' || a=='l' || a=='r')
 	 	print_puntos(*pyo,*prival,*ptempyo,*ptemprival); /*en caso de igualada o subida se imprime el nuevo estado de la mesa*/
	 if(i==1)
		return 1;
	 if(i==2)
		return 2;
	 if(i==3)
		return 3; //se sale de la funcion en caso de 'i'= 1 (continua al flop), 2 (showdown por allin), o 3 (retirada)
	 switch(m) //se cede el turno al otro jugador
		{
		 case 1:
			m=2;
			break;
		 case 2:
			m=1;;
			break;
		}
	 if(m==id) //cambiando los punteros adecuadamente dependiendo del id
		{
	 	 strcpy(msj,"Ud");
		 pjug=pyo;
		 ptemp=ptempyo;
		 ptempaux=ptemprival;
		}
	 if(m!=id)
		{
	 	 strcpy(msj,"Rival");
		 pjug=prival;
		 ptemp=ptemprival;
		 ptempaux=ptempyo;
		}
	 if(m==id) //el jugador que habla toma el turno
	 	{ 
		 if(!turno(&a,&v,*pjug,*ptemp,sb,ip,port,0))
			return 0;
		}	 
	}
} 

int postflop(int * pyo,int * prival,int * ptempyo,int * ptemprival,int * g,int id,int m,int sb,int bb,char * ip,int port)/*funcion que lleva a cabo el flop, turn o river, funciona de manera similar al preflop, sin ciegas de por medio*/
{
 char a=0,msj[10];
 int * pjug,* ptemp, * ptempaux;
 int v=0,i=0;
 switch(m)
	{
	 case 1:
		m=2;
		break;
	 case 2:
		m=1;
		break;
	}
 if(m==id)
	{
	 strcpy(msj,"Ud");
	 pjug=pyo;
	 ptemp=ptempyo;
	 ptempaux=ptemprival;
	}
 if(m!=id)
	{
	 strcpy(msj,"Rival");
	 pjug=prival;
	 ptemp=ptemprival;
	 ptempaux=ptempyo;
	}
 print_puntos(*pyo,*prival,*ptempyo,*ptemprival);
 if(m==id)
	{
 	 if(!turno(&a,&v,*pjug,*ptemp,sb,ip,port,2))
	 	return 0;
	}
 while(1)
	{
	 if(!recibir_datos(&i,&a,&v,ip,port))
		return 0;
	 if(a=='f')
		{
		 printf("\n%s foldea\n",msj);
		 if(m==1)
			*g=2;
		 else if(m==2)
			*g=1;
		 if(m==id)
			{
			 *prival=*prival + (*ptemprival-*ptempyo);
			 *ptemprival=*ptempyo;
			}
		 else if(m!=id)
			{
			 *pyo=*pyo + (*ptempyo-*ptemprival);
			 *ptempyo=*ptemprival;
			}
		}
	 if(a=='x')
		 printf("\n%s hace check\n",msj);		 
	 if(a=='c')
		{
		 printf("\n%s iguala\n",msj);
		 if((*ptempaux-*ptemp)>*pjug)
			{
			 *ptemp=*ptemp+*pjug;
			 *pjug=0;
			 if(m==id)
			 	 *prival = *prival + (*ptempaux - *ptemp);
			 else
				 *pyo = *pyo + (*ptempaux - *ptemp);
			 *ptempaux = *ptemp;
			}
		 else
			{	 
			 *pjug=*pjug-(*ptempaux-*ptemp);
			 *ptemp=*ptemp+(*ptempaux-*ptemp);
			}
		}
	  if(a=='r')
		{
		 if(v == (*pjug + *ptemp))
			printf("\n%s va all in\n",msj);
		 else
		 	printf("\n%s sube a %d\n",msj,v);
	 	 *pjug=*pjug-(v-*ptemp);
		 *ptemp=*ptemp+(v-*ptemp);
		}
	 if(a=='c' || a=='r')
 	 	print_puntos(*pyo,*prival,*ptempyo,*ptemprival);
	 if(i==1)
		return 1;
	 if(i==2)
		return 2;
	 if(i==3)
		return 3;
	 switch(m)
		{
		 case 1:
			m=2;
			break;
		 case 2:
			m=1;;
			break;
		}
	 if(m==id)
		{
	 	 strcpy(msj,"Ud");
		 pjug=pyo;
		 ptemp=ptempyo;
		 ptempaux=ptemprival;
		}
	 if(m!=id)
		{
	 	 strcpy(msj,"Rival");
		 pjug=prival;
		 ptemp=ptemprival;
		 ptempaux=ptempyo;
		}
	 if(m==id)
	 	{ 
		 if(!turno(&a,&v,*pjug,*ptemp,sb,ip,port,2))
			return 0;
		}	 
	}
} 

int recibir_datos(int * i,char * a,int * v,char * ip,int port) //funcion que recibe los datos de la ultima accion del servidor
{
 int socketfd;
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 read(socketfd,i,sizeof(int));
 read(socketfd,a,1);
 read(socketfd,v,sizeof(int));
 cerrar(socketfd);
 return 1;
}

int turno(char * a,int * v,int pjug,int ptemp,int sb,char * ip,int port,char p)//funcion que lleva a cabo el turno del jugador
{
 char s[20],acc[10],aant=*a; //aant y vant corresponden a la accion anterior, necesaria para saber que acciones son validas
 int socketfd,vant=*v,bb=(sb*2);
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 printf("\nIngrese su jugada: ");
 fgets(s,20,stdin);//se toma una cadena del teclado 
 sscanf(s,"%s %d",acc,v); //se separa en accion y valor de la apuesta (si la hubiera)
 while(strcmp(acc,"FOLD")!=0 && strcmp(acc,"CHECK")!=0 && strcmp(acc,"CALL")!=0 && strcmp(acc,"RAISE")!=0 && strcmp(acc,"ALLIN")!=0)
	{
	 printf("\nEntrada inválida, ingrese nuevamente: ");
 	 fgets(s,20,stdin);
 	 sscanf(s,"%s %d",acc,v); //si la accion no es una palabra valida, se repite el proceso
	} 
 if(*v > (pjug + ptemp)) //si la apuesta excede a la canitdad que el jugador posee, se define como toda su pila
	*v = (pjug + ptemp);
 if(strcmp("FOLD",acc)==0) //si la accion es retirarse
	{
	 if(p==1 || aant=='r') /*se comprueba si el jugador esta en una situacion de retirarse o si podria ver las siguientes cartas gratis*/
		{
	 	 *a = 'f'; 
	 	 *v = 0;
		}
	 else
		{
		 printf("\nEsta seguro? Su rival no ha subido, desea retirarse de todos modos? Responda con 'FOLD' o 'CHECK': "); //si fuera esto ultimo, se repregunta
		 fgets(s,20,stdin);
		 sscanf(s,"%s %d",acc,v);
		 while(strcmp(acc,"FOLD")!=0 && strcmp(acc,"CHECK")!=0)
			{
			 printf("\nEntrada inválida, ingrese nuevamente: ");
		 	 fgets(s,20,stdin);
		 	 sscanf(s,"%s %d",acc,v);
			}
 		 if(strcmp("FOLD",acc)==0) 
			{
	 		 *a = 'f'; //setea el caracter en fold y el valor de la apuesta en 0
	 		 *v = 0;
			}
		}		
	}
 if(strcmp("CHECK",acc)==0) //si la jugada fue check, se hacen comprobaciones similares para verificar si la jugada es valida
	{
	 if((p==0 && aant=='l') || (p==2 && aant==0) || (p==2 && aant=='x'))
	 	{	
 	 	 *a = 'x';
		 *v = 0;
		}
	 else
		{
		 printf("\nNo puede hacer check, ingrese nuevamente: ");
		 fgets(s,20,stdin);
		 sscanf(s,"%s %d",acc,v);	
		 while(strcmp(acc,"FOLD")!=0 && strcmp(acc,"CALL")!=0 && strcmp(acc,"RAISE")!=0 && strcmp(acc,"ALLIN")!=0)
			{
			 if(strcmp(acc,"CHECK")==0)
			 	printf("\nNo puede hacer check, ingrese nuevamente: ");
			 else
			 	printf("\nEntrada inválida, ingrese nuevamente: ");
		 	 fgets(s,20,stdin);
		 	 sscanf(s,"%s %d",acc,v);
			}
 		 if(strcmp("FOLD",acc)==0) 
			{
	 		 *a = 'f';
	 		 *v = 0;
			}
		}
	}
 if(strcmp("CALL",acc)==0)//si la jugada fue call
	{
	 if(p==1)//se setea en 'l' si fue un limp (completar la ciega grande)
		{
		 *a = 'l';
		 *v = 0;
		}
	 else if(aant=='r')//se seta en 'c' si fue luego de una subida	 
		{		
		 *a = 'c';
		 *v = 0;
		}
	 else //si no, se repregunta puesto que no hay ninguna apuesta que igualar
		{
		 printf("\nNo puede hacer call, ingrese nuevamente: ");
		 fgets(s,20,stdin);
		 sscanf(s,"%s %d",acc,v);	
		 while(strcmp(acc,"FOLD")!=0 && strcmp(acc,"CHECK")!=0 && strcmp(acc,"RAISE")!=0 && strcmp(acc,"ALLIN")!=0)
			{
			 if(strcmp(acc,"CALL")==0)
			 	printf("\nNo puede hacer call, ingrese nuevamente: ");
			 else
			 	printf("\nEntrada inválida, ingrese nuevamente: ");
		 	 fgets(s,20,stdin);
		 	 sscanf(s,"%s %d",acc,v);
			}
		 if(strcmp("FOLD",acc)==0) //si se elige fold, puesto que podria ver carta gratis se repregunta nuevamente
			{
			 printf("\nEsta seguro? Su rival no ha subido, desea retirarse de todos modos? Responda con 'FOLD' o 'CHECK': ");
			 fgets(s,20,stdin);
			 sscanf(s,"%s %d",acc,v);
			 while(strcmp(acc,"FOLD")!=0 && strcmp(acc,"CHECK")!=0)
				{
				 printf("\nEntrada inválida, ingrese nuevamente: ");
			 	 fgets(s,20,stdin);
			 	 sscanf(s,"%s %d",acc,v);
				}
	 		 if(strcmp("FOLD",acc)==0) 
				{
		 		 *a = 'f';
		 		 *v = 0;
				}
			}
 		 if(strcmp("CHECK",acc)==0) 
			{
	 		 *a = 'x';
	 		 *v = 0;
			}
		}
		
	}
 if(strcmp("RAISE",acc)==0)//si la jugada fue un raise, se setea a en 'r' 
	 {
	  *a = 'r';
	  if(p!=2) //si se esta en el preflop, la apuesta no puede ser menor a 2 BB
		{	
		 if(*v < (2*bb))
			*v = (2*bb);
		}
	  else //si se esta en el postflop, la apuesta no puede ser menor a 1 BB
		{
		 if(*v < bb)
			*v = bb;
		}
 	  if(aant=='r') /*si la jugada anterior fue otra subida, la subida no puede ser menor que lo que subio el jugador anterior (ejemplo A apuesta 20, B sube a 50 (subio 30), C puede subir a 50 + 30 = 80 como minimo)*/
		{
		 if(*v < (vant + (vant - ptemp)))
			*v = (vant + (vant - ptemp));
		}
	  if(*v > (pjug + ptemp)) /* si al final, lo que el jugador esta apostando es mayor a lo que tiene, la apuesta es todo lo que tiene*/
		*v = (pjug + ptemp);
	}
 if(strcmp("ALLIN",acc)==0) //si el jugador va allin se setea el a en 'r' y v en toda la pila del jugador
	{	
	 *a = 'r';
	 *v = (pjug + ptemp); 
	}
 write(socketfd,a,1);
 write(socketfd,v,sizeof(int));
 cerrar(socketfd); 
 return 1;
}

int enviar_nombre(char * ip, int port, char * nombre)//funcion que envia el nombre al servidor
{
 int socketfd,i;
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 for(i=0;i<=strlen(nombre);i++)
 	write(socketfd,nombre+i,1);
 cerrar(socketfd);
 return 1;
}

int enviar_conf(char * ip, int port)//funcion que envia la confirmacion para que empiece el juego
{
 char conf=1;
 int socketfd;
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 write(socketfd,&conf,1);
 cerrar(socketfd);
 return 1;
}

int recibir_conf(char * ip, int port)//funcion que recibe la confirmacion para que empiece el juego
{
 char conf;
 int socketfd;
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 read(socketfd,&conf,1);
 cerrar(socketfd);
 return 1;
}

int recibir_result(char * ip, int port, char * jugada1, char * jugada2)/*esta funcion recibe las jugadas obtenidas por ambos jugadores del servidor*/
{
 int socketfd;
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 read(socketfd,jugada1,7);
 read(socketfd,jugada2,7);
 cerrar(socketfd);
 return 1;
}

int recibir_ganador(char * ip, int port)//esta funcion recibe el ganador de la mano (en caso de showdown)
{
 int r;
 int socketfd;
 socketfd=conectar(ip,port);
 if(!socketfd)
	return 0;
 read(socketfd,&r,sizeof(int));
 cerrar(socketfd);
 return r;
}
 
