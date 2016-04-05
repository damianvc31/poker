#include <stdio.h>	
#include <sys/types.h>	
#include <sys/socket.h>	
#include <stdlib.h> 
#include <unistd.h>	
#include <netinet/in.h>	
#include <string.h>	
#include <strings.h>	
#include <netdb.h>		
#include "sockets.h"


int conectar (char * ip,int port)//funcion que conecta con un socket abierto en determinado IP y puerto
{
 int result;
 int socketfd;
 struct hostent *server;
 struct sockaddr_in server_addr;	
 socketfd = socket(AF_INET, SOCK_STREAM, 0);
 server = gethostbyname(ip);
 if(server == NULL) 
 	{
	 perror("Error obteniendo hostname del servidor");
	 return 0;
	}
 bzero((char *) &server_addr, sizeof(server_addr));
 server_addr.sin_family = AF_INET;
 bcopy((char *)server->h_addr,(char *)&server_addr.sin_addr.s_addr,server->h_length);
 server_addr.sin_port = htons(port);
 result = connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
 if(result<0)
	{
	 perror("Error conectandose al servidor");
	 return 0;
	}
 return socketfd;
}

void cerrar(int fd)//funcion que cierra un socket
{
 close(fd);
}
