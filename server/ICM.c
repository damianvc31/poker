#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "defs.h"

void ICM (void)//esta funcion muestra el menu de la calculadora de ICM
{
 char opc;
 printf("\nCALCULADORA DE ICM\n");
 do 
	{
	 printf("\nElija una opción del menú\n");
	 printf("\n1)Calcular Eq$ de los jugadores de un SNG de 9 jugadores\n2)Volver al menu principal\n"); 
	 printf("\nOpción: ");
	 scanf("%d",(int *)&opc);
	 while(opc<1 || opc>2)
		{
	 	 printf("\nEntrada inválida, ingrese nuevamente: ");
		 scanf("%d",(int *)&opc);
		}
	 getchar();
	 if(opc == 1)
		 ICM_1();
	}while(opc!=2);
}

void ICM_1(void)/*esta funcion simula un SNG de 9 jugadores a partir de los datos provistos y muestra el Eq$ de cada jugador*/
{
 int * stacks;
 double * Eq$;
 int buyin,prize,njug,i;
 printf("\nIngrese el buyin del SNG (sin incluir la comisión):\n$"); 
 scanf("%d",&buyin);
 prize=9*buyin;
 printf("\nIngrese la cantidad de jugadores en juego (2-9):\n");
 scanf("%d",&njug);
 while(njug<2 || njug>9)
	{
	 printf("\nEntrada inválida, ingrese nuevamente: ");
	 scanf("%d",&njug);
	}
 getchar();
 stacks = (int *) malloc (njug * sizeof(int)); //pide memoria para los array de stacks y de Eq$ dependiendo del numero de jug
 Eq$ = (double *) malloc (njug * sizeof(double));
 printf("\nIngrese los stacks de los jugadores:\n\n");  //carga stacks de los jugadores
 for(i=0;i<njug;i++)
	{
 	 printf("Jugador %d: ",i+1);
	 scanf("%d",stacks+i);	  	 
	}
 for(i=0;i<njug;i++)
	 Eq$[i]=calculo_de_Eq$(stacks,prize,njug,i); //llama a la funcion para calcular la Eq$ de cada jugador
 printf("\nEl Eq$ de cada jugador es:\n\n"); 
 for(i=0;i<njug;i++)
	 printf("Jugador %d: $%lg \n",i+1,Eq$[i]); //muestra los resultados
 free(stacks);
 free(Eq$);
}	

double calculo_de_Eq$(int * stacks,int prize,int njug,int i)
{
 double prob_1=0,prob_2=0,prob_3=0,Eq$=0;
 int j,k;
 for(j=0;j<njug;j++)
	{
	 if(j==i)
	 	prob_1 = prob_1 + (stacks[i]/suma(stacks,njug,-1,-1));
	 else
		{
		 for(k=0;k<njug;k++)
			{
			 if(k==i)
				 prob_2 = prob_2 + (stacks[j]/suma(stacks,njug,-1,-1)) * (stacks[i]/suma(stacks,njug,j,-1));
			 else if(k!=j)
				 {
				  prob_3 = prob_3 + (stacks[j]/suma(stacks,njug,-1,-1)) * (stacks[k]/suma(stacks,njug,j,-1)) * (stacks[i]/suma(stacks,njug,j,k));
				}
			}
		}
	}
 Eq$= prob_1 * (0.5 * prize) + prob_2 * (0.3 * prize) + prob_3 * (0.2 * prize);
 return Eq$;
}
			 

double suma (int * stacks,int njug,int j,int k)
{
 int i;
 double r=0;
 for(i=0;i<njug;i++)
	{
	 if(i!=j && i!=k)
	 	r=r+stacks[i];
	}
 return r;
}
	
