#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "sockets.h"

int main()
{
 char opc, nombre_jug1[MAX_LONG_NOMBRE], nombre_jug2[MAX_LONG_NOMBRE];
 int socketfd1,socketfd2,nv,sb,bb,nmano,pjug1,pjug2,ptemp1,ptemp2,pot,r,g,p,m;
 CARTA jug1[2],jug2[2],mesa[5];
 printf("\n::::::::::::::::::POKER::TEXAS:HOLDEM::NL::HU::::::::::::::::by:Damian:Cohen::\n");
 printf("\nSERVIDOR\n");
 do{
	 do{
		 printf("\nElija una opción del menú para comenzar, ingresando el numero correspondiente:\n");
		 printf("\n1)Nueva partida\n2)Calculadora de probabilidades\n3)Calculadora de ICM\n4)Salir\n"); //menu
		 printf("\nOpción: ");
		 scanf("%d",(int *)&opc);
		 while(opc<1 || opc>4) // repite si la entrada no es válida
			{
		 	 printf("\nEntrada inválida, ingrese nuevamente: ");
			 scanf("%d",(int *)&opc);
			}
		 getchar(); //libera el buffer de entrada 
		 if(opc == 2)
			 calculadora(); //ingresa a la calculadora de probabilidades
		 else if(opc == 3)
			 ICM(); //ingresa a la calculadora de ICM
		 else if(opc == 4)
			{
			 printf("\nGracias por utlizar este programa\n\n"); //sale del programa
			 return 0;
			}
	 }while(opc!=1); //repite mientras no se entre a una nueva partida
	 printf("\nPresione <Enter> para iniciar el servidor...");
	 getchar(); 
	 socketfd1=iniciar(PORT1); //se inician los sockets en los dos puertos, en caso de error se sale del programa
	 if(!socketfd1)
		return 0;
	 socketfd2=iniciar(PORT2);
	 if(!socketfd2)
		return 0;
	 printf("\nListo para recibir conexiones...esperando jugadores...\n"); //se espera a que ambos jugadores se conecten
	 if(!recibir_nombre(socketfd1,nombre_jug1))
		return 0;
	 if(!recibir_nombre(socketfd2,nombre_jug2))
		return 0;
	 printf("\nJugador 1 - %s y Jugador 2 - %s conectados!\n",nombre_jug1,nombre_jug2); 
	 printf("\nEsperando confirmación...\n"); //se espera la confirmacion de ambos jugadores antes de empezar
	 if(!recibir_conf(socketfd1))
		return 0;
	 if(!recibir_conf(socketfd2))
		return 0;
	 if(!enviar_conf(socketfd1)) //una vez recibida se envia a ambos jugadores que esta todo listo 
		return 0;
	 if(!enviar_conf(socketfd2))
		return 0; 
	 printf("\nSe inicia la partida!\n"); 
	 sb=10; //seteo inicial de variables
	 bb=sb*2;
	 nv=1;
	 m=1;
	 pjug1=pjug2=P_INICIALES;
	 printf("\nCiegas: SB: %d BB: %d\n",sb,bb);
	 for(nmano=0;pjug1 && pjug2;nmano++) //bucle de manos que termina cuando algun jugador se queda sin fichas
		{
		 printf("\nMano N° %d\n",nmano+1);
		 p=1; // 'p' define la instancia en la que termina la mano (preflop,flop,turn, river) para un posterior uso
		 if(nmano % MANOS_POR_NIVEL == 0 && nmano>0)//cada X manos (siendo X un numero definido en el header) suben las ciegas
			{
			 nv++;
			 switch(nv)
				{
				 case 2:
					sb=15;
					break;
				 case 3:
					sb=25;
					break;
				 case 4:
					sb=50;
					break;
				 case 5:
					sb=75;
					break;
				 case 6:
					sb=100;
					break;
				 case 7:
					sb=125;
					break;
				 case 8:
					sb=150;
					break;
				 case 9:
					sb=200;
					break;
				 default:
					 sb=sb+100;
					 break;
				}
			 bb=(2*sb);
			 printf("\nSuben las ciegas: SB: %d BB: %d\n",sb,bb);
			}
		 reparto(jug1,jug2,mesa); //llama a la funcion para repartir las manos y las cartas en la mesa
		 if(!enviar_cartas(socketfd1,socketfd2,jug1,jug2,mesa))//y se envian las manos a ambos jugadores
			return 0;
		 printf("\nCartas repartidas a ambos jugadores\n");
		 pot=0;
		 printf("\nPreflop\n");
		 if((r=preflop(&pjug1,&pjug2,&ptemp1,&ptemp2,&g,m,sb,bb,socketfd1,socketfd2))==0) /*se llama la funcion que lleva a cabo el preflop, esta devuelve un valor dependiendo de si la mano debe continuar al flop o alguien se ha retirado*/
			return 0;
		 if(r==1)
			{
			 pot=pot+ptemp1+ptemp2; //si se continua al flop, se meten al pozo las apuestas parciales de cada jugador
			 ptemp1=0;
		 	 ptemp2=0;
			 p++; 
			 printf("\nFlop:\n\n");
			 print_cards(mesa,3); //impresion del flop
		 	 if((r=postflop(&pjug1,&pjug2,&ptemp1,&ptemp2,&g,m,sb,bb,socketfd1,socketfd2))==0) /*se llama a la funcion que lleva a cabo el flop,devolviendo el valor r*/
			 	return 0;
			 else if(r==1)
				{
				 pot=pot+ptemp1+ptemp2; //se continua de la misma forma si se prosigue al turn
		 		 ptemp1=0;
		 		 ptemp2=0;
				 p++;
				 printf("\nTurn:\n\n");
				 print_cards(mesa+3,1); //impresion del turn
		 	 	 if((r=postflop(&pjug1,&pjug2,&ptemp1,&ptemp2,&g,m,sb,bb,socketfd1,socketfd2))==0)/*funcion para el turn*/				return 0;
				 else if(r==1)
					{
					 pot=pot+ptemp1+ptemp2;
		 			 ptemp1=0;
		 			 ptemp2=0;
					 p++;
					 printf("\nRiver:\n\n");
					 print_cards(mesa+4,1); //impresion del river
		 	 	 	 if((r=postflop(&pjug1,&pjug2,&ptemp1,&ptemp2,&g,m,sb,bb,socketfd1,socketfd2))==0)/*funcion para el river, nuevamente se devuelve r con el valor que dictara si debe haber showdown o no*/
					 	return 0;
					 if(r==1)
					 	r=2;
					 }
				}
			}
		 pot=pot+ptemp1+ptemp2; //luego del river, o de que alguien se retire, se arma el pozo final
		 ptemp1=0;
		 ptemp2=0;
		 if(r==2) // dependiendo si hay showdown o no, se llama a la funcion correspondiente
			{	
	 	 	 if(!showdown(jug1,jug2,mesa,&pjug1,&pjug2,pot,p,socketfd1,socketfd2))
				return 0;
			}
		 else if(r==3)
			no_showdown(&pjug1,&pjug2,pot,g);				
		 if(m==1) //se invierte el jugador que sera mano
			m=2;
		 else
			m=1;
		}
	 if(pjug1>0) //cuando un jugador se queda sin fichas, se verifica el ganador
		printf("\nGana Jugador 1\n\n");
	 else if(pjug2>0)
		printf("\nGana Jugador 2\n\n");	 
	 cerrar(socketfd1); //finalmente, se cierran los sockets 
	 cerrar(socketfd2);
 }while(1); //se vuelve al menu inicial
 return 0;
}








