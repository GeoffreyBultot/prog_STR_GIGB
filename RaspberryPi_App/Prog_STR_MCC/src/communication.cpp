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

	int policy = 0;
	int max_prio_for_policy = 0;

	pthread_attr_init(&threadCOM_attr);
	pthread_attr_getschedpolicy(&threadCOM_attr, &policy);
	max_prio_for_policy = sched_get_priority_max(policy);
	pthread_setschedprio(threadCOM_t, max_prio_for_policy-1);

	int err = pthread_create( &threadCOM_t , &threadCOM_attr,  thread_Communication , NULL);

	if(err)
	{

		//return 1; //TODO ERROR
	}

	//Listen

    return err;
}

int angle;
void* thread_Communication(void*)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    int client_sock;
    int read_size;

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    while (socket_desc == -1)
    {
    	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    	std::cout<<"Socket could not create socket"<<std::endl;
    	usleep(1);//Preemption point (if cancel)
    }

    std::cout<<"[INFO] socket Created"<<std::endl;

    //Prepare sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 2600 );

    //Bind
	while( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
    	std::cout<<"could not bind"<<std::endl;
    	usleep(1);//Preemption point (if cancel)
	}

	std::cout<<"[INFO] bind successfull"<<std::endl;

	listen(socket_desc , 3);

    c = sizeof(struct sockaddr_in);
    std::cout<< "[INFO] Waiting for incoming connections..."<<std::endl;


	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	while(client_sock < 0)
	{
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	}

	std::cout<<"[INFO] Client connected"<<std::endl;

	char buffer[20];
	int difftime = 0;
	struct timeval stop, start;
	gettimeofday(&start, NULL);
	while(1)
    {
		int32_t AngleConsigne;
		char *data = (char*)&AngleConsigne;
		int size = sizeof(AngleConsigne);

		if( (read_size = recv(client_sock , data , size , MSG_DONTWAIT)) > 0 )
		{
			setConsigne(*(int*)data);
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
		/*
		else if(read_size == -1)
		{
			perror("recv failed");
		}*/
		gettimeofday(&stop, NULL);
		difftime = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
		if(difftime > 100000)
		{
			int measure = getMCCAngle();
			int status = getMCCStatus();
			int consigne = getConsigne();
			int command = (10000.0*getPIDCommand());
			sprintf(buffer, "%d;%d;%d;%d", measure, status,consigne, command); //TODO  \0 ?
			write(client_sock, buffer , strlen(buffer));
			gettimeofday(&start, NULL);
		}
		pthread_testcancel();
    }
}



int stopCOM_thread(void)
{
	setsockopt(socket_desc,SOL_SOCKET,SO_REUSEADDR,NULL,sizeof(int));
	close(socket_desc);
	int err = pthread_cancel(threadCOM_t);
	pthread_join(threadCOM_t, NULL);
	return err;
}



