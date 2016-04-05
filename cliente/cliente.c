#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "sockets.h"


int main(int n,char** argv, char** argc)/*se toman el IP y el puerto como argumentos del main, un jugador debe usar el puerto 1 y el otro el puerto 2, y el IP debe ser el de la maquina que corre el servidor*/
{
 char nombre[MAX_LONG_NOMBRE];
 int id,nv=1,sb=10,bb=(2*sb),nmano,pjug1=P_INICIALES,pjug2=P_INICIALES,ptemp1,ptemp2,pot,r,g,p=1,m=1,*ptempyo,*ptemprival,*pyo,*prival;
 CARTA jug1[2],jug2[2],mesa[5]; //se declaran punteros para apuntar al puntaje dependiendo del puerto donde se ingresa
 CARTA * yo,*rival;
 printf("\n::::::::::::::::::POKER::TEXAS:HOLDEM::NL::HU::::::::::::::::by:Damian:Cohen::\n"); //presentacion e instrucciones
 printf("\nCOMANDOS VALIDOS(se deben ingresar en Mayusculas):\n");
 printf("FOLD_______________________________________Retirarse\n");
 printf("CHECK______________________________________Pasar\n");
 printf("CALL_______________________________________Igualar\n");
 printf("RAISE <valor de apuesta>___________________Apostar/Subir\n");
 printf("ALLIN______________________________________Ir_all_in\n");
 printf("\nPresione <Enter> para empezar...");
 getchar();
 if(atoi(argv[2]) == PORT1)/*si se ingreso del puerto 1, el jugador toma como propias las variables del jugador 1, y como las del rival las del jugador 2, y el id se pone a 1*/
	{	
	 id=1;
	 yo=jug1;
	 rival=jug2;
	 pyo=&pjug1;
	 prival=&pjug2;
	 ptempyo=&ptemp1;
	 ptemprival=&ptemp2;
	}
 if(atoi(argv[2]) == PORT2)//si se ingreso del puerto 2, al reves
	{
	 id=2;
	 yo=jug2;
	 rival=jug1;
	 pyo=&pjug2;
	 prival=&pjug1;
	 ptempyo=&ptemp2;
	 ptemprival=&ptemp1;
	}
 printf("\nIngrese su nombre (max. %d caracteres): ",MAX_LONG_NOMBRE); //ingreso del nombre
 scanf("%s",nombre);
 getchar();
 if(!enviar_nombre(argv[1],atoi(argv[2]),nombre))
	return 0;
 printf("\nNombre enviado al servidor, ud es el jugador %d\n",id);
 printf("\nPresione <Enter> para empezar la partida..."); 
 getchar();
 if(!enviar_conf(argv[1],atoi(argv[2]))) //se envia la confirmacion para empezar la partida
	return 0; 
 printf("\nEsperando confirmación del rival...\n"); 
 if(!recibir_conf(argv[1],atoi(argv[2])))
	return 0;
 printf("\nSe inicia la partida!\n"); 
 printf("\nCiegas: SB: %d BB: %d\n",sb,bb);
 for(nmano=0;pjug1 && pjug2;nmano++) /*bucle de manos termina cuando un jugador se queda sin fichas, funciona igual que el server para decidir cuando continuar y cuando ir al showdown, o al 'no showdown'*/
	{
	 printf("\nMano N° %d\n",nmano+1);
	 p=1;
	 if(nmano % MANOS_POR_NIVEL == 0 && nmano>0)
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
	 if(!recibir_cartas(argv[1],atoi(argv[2]),yo,rival,mesa))
		return 0;
	 printf("\nSus cartas:\n\n");
	 print_cards(yo,2);
	 pot=0;
	 printf("\n___________________Pozo:_%d____________________\n",pot); 
	 if((r=preflop(pyo,prival,ptempyo,ptemprival,&g,id,m,sb,bb,argv[1],atoi(argv[2])))==0)
		return 0;
	 if(r==1)
		{
		 pot=pot+ptemp1+ptemp2;
		 ptemp1=0;
	 	 ptemp2=0;
		 p++; 
		 printf("\nFlop:\n\n");
		 print_cards(mesa,3);
		 printf("\n___________________Pozo:_%d____________________\n",pot);
	 	 if((r=postflop(pyo,prival,ptempyo,ptemprival,&g,id,m,sb,bb,argv[1],atoi(argv[2])))==0)
		 	return 0;
		 else if(r==1)
			{
			 pot=pot+ptemp1+ptemp2;
	 		 ptemp1=0;
	 		 ptemp2=0;
			 p++;
			 printf("\nFlop:\n\n");
			 print_cards(mesa,3);	
			 printf("\nTurn:\n\n");
			 print_cards(mesa+3,1);
			 printf("\n___________________Pozo:_%d____________________\n",pot); 
	 	 	 if((r=postflop(pyo,prival,ptempyo,ptemprival,&g,id,m,sb,bb,argv[1],atoi(argv[2])))==0)
				return 0;
			 else if(r==1)
				{
				 pot=pot+ptemp1+ptemp2;
	 			 ptemp1=0;
	 			 ptemp2=0;
				 p++;
				 printf("\nFlop:\n\n");
				 print_cards(mesa,3);	
				 printf("\nTurn:\n\n");
				 print_cards(mesa+3,1);
				 printf("\nRiver:\n\n");
				 print_cards(mesa+4,1);
				 printf("\n___________________Pozo:_%d____________________\n",pot); 
	 	 	 	 if((r=postflop(pyo,prival,ptempyo,ptemprival,&g,id,m,sb,bb,argv[1],atoi(argv[2])))==0)
					return 0;
				 if(r==1)
				 	r=2;
				 }
			}
		}
	 pot=pot+ptemp1+ptemp2;
	 ptemp1=0;
	 ptemp2=0;
	 printf("\n___________________Pozo:_%d____________________\n",pot); 
	 if(r==2)
		{
		 if(!showdown(jug1,jug2,mesa,&pjug1,&pjug2,pot,id,p,argv[1],atoi(argv[2])))
			return 0;
		}
	 else if(r==3)
		no_showdown(&pjug1,&pjug2,pot,g,id);				
	 if(m==1)
		m=2;
	 else
		m=1;
	 if(pjug1 && pjug2) //pausa entre manos
		{
	 	 printf("\nPresione <Enter> para continuar...");
	 	 getchar();
		}	
	}
 if(*pyo > 0) //cuando se termina, se informa el ganador dependiendo quien quedo con fichas
	printf("\nFelicitaciones! Ud gana!\n\n");
 else
	printf("\nLo siento, ha perdido\n\n");	 
 return 0;
}

