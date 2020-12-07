#include "communication.hpp"
#include<stdio.h>
#include<string.h>    //for strlen
#include<stdlib.h>    //for strlen
#include<sys/socket.h>
#include<arpa/inet.h>
#include <iostream>
#include<unistd.h>
#include<pthread.h>
#include <stdio.h>

void* thread_Communication(void*);


pthread_attr_t threadCOM_attr;
pthread_t threadCOM_t;

int socket_desc = socket(AF_INET , SOCK_STREAM , 0);
int c;
struct sockaddr_in server , client;

int initCOM_Thread(void)
{


    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 2600 );

    //Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
	}
	puts("bind done");

	if( pthread_create( &threadCOM_t , &threadCOM_attr,  thread_Communication , NULL) < 0)
	{
		//return 1; //TODO ERROR
	}

	//Listen
	listen(socket_desc , 3);

    return 0;
}


void* thread_Communication(void*)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    //Get the socket descriptor
    //int socket_desc = *(int*)socket;
    int client_sock;
    int read_size;
    char *message , client_message[20];

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	std::cout << "test"<<std::endl;
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	while(client_sock < 0)
	{
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	}

	std::cout<<"connected"<<std::endl;

	while(1)
    {
		//TODO : machine d'état pour les reconnexions ?
		if( (read_size = recv(client_sock , client_message , 2 , 0)) > 0 )
		{
			std::cout<<client_message<<std::endl;
			//write(client_sock, client_message , strlen(client_message));
			//Todo : recevoir les informations en bytes
			memset(client_message, 0, 3);//Clear
		}
		else if(read_size == 0)
		{
			puts("Client disconnected");
			//fflush(stdout);
			client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			while(client_sock < 0)
			{
				client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			}
		}
		else if(read_size == -1)
		{
			perror("recv failed");
		}

		//TODO : Envoyer l'angle moteur tous les x temps
		//TODO : Set la consigne (mutex ?)
		pthread_testcancel();
    }
}



int stopCOM_thread(void)
{
	pthread_cancel(threadCOM_t);
	pthread_join(threadCOM_t, NULL);
	//TODO : ajouter la valeur de retour du cancel.
	return 0;
}



