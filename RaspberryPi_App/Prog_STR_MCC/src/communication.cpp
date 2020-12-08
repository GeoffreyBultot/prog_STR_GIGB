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
#include <MCC_PID.hpp>
#include <sys/time.h>

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

int angle;
void* thread_Communication(void*)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    int client_sock;
    int read_size;

    c = sizeof(struct sockaddr_in);
    std::cout<< "[INFO] Waiting for incoming connections..."<<std::endl;


	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	while(client_sock < 0)
	{
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	}

	std::cout<<"[INFO] Client connected"<<std::endl;

	char buffer[6];
	int difftime = 0;
	struct timeval stop, start;
	gettimeofday(&start, NULL);
	while(1)
    {
		//TODO : machine d'état pour les reconnexions ?
		int32_t AngleConsigne;
		char *data = (char*)&AngleConsigne;
		int size = sizeof(AngleConsigne);

		if( (read_size = recv(client_sock , data , size , MSG_DONTWAIT)) > 0 )
		{
			//TODO ajouter des vérifications sur la lecture de C
			//TODO : mutex ?
			C = *((int*)data);//atoi(data);
			std::cout<<"data:"<<data<<std::endl;
		}
		else if(read_size == 0)
		{
			puts("[WARNING] Client disconnected");
			client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			while(client_sock < 0)
			{
				client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			}
			std::cout<<"[INFO] Client reconnected"<<std::endl;

		}
		else if(read_size == -1)
		{
			//perror("recv failed");
		}
		gettimeofday(&stop, NULL);
		//printf("us :  %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
		difftime = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
		//std::cout<<difftime<<std::endl;
		if(difftime > 100000)
		{
			sprintf(buffer, "%d;%d", i_measure, MCC_Status); //TODO  \0 ?
			write(client_sock, buffer , strlen(buffer));
			gettimeofday(&start, NULL);
		}



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



