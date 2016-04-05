#include <stdio.h>		
#include <sys/types.h>	
#include <sys/socket.h>	
#include <stdlib.h> 	
#include <unistd.h>		
#include <netinet/in.h>	
#include <strings.h>	
#include "sockets.h"

int iniciar(int port) //funcion que inicia un socket en un puerto determinado
{
 int result,socketfd; 
 struct sockaddr_in server_addr;
 socketfd = socket(AF_INET, SOCK_STREAM, 0);
 if(!socketfd)
 	{
	 perror("Error al abrir socket");
	 return 0;
 	}
 bzero((char*) &server_addr, sizeof(server_addr));
 server_addr.sin_family = AF_INET;
 server_addr.sin_addr.s_addr = INADDR_ANY;
 server_addr.sin_port = htons(port);
 result = bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
 if(result<0) 
 	{
	 perror("Error al bindear socket");
	 return 0;
	}
 result = listen(socketfd, BACKLOG);
 if(result<0)
 	{
	 perror("Error intentando escuchar en puerto");
	 return 0;
	}
 printf("\nServidor conectado en puerto %d\n", port);
 return socketfd;
}

int aceptar(int socketfd) //funcion que acepta la conexion por parte de un cliente en un socket iniciado previamente
{
 int clientfd;
 socklen_t client_length;
 struct sockaddr_in client_addr;
 client_length = sizeof(client_addr);	  
 clientfd = accept(socketfd, (struct sockaddr *) &client_addr, &client_length);
 if(clientfd<0) 
 	{
	 perror("Error recibiendo conexion");
	 return 0;
	}
 return clientfd;
}


void cerrar(int fd) //funcion que cierra un socket
{
 close(fd);
}

