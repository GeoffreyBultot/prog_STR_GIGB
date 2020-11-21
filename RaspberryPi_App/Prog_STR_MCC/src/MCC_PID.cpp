/*
 * MCC_PID.c
 *
 *  Created on: 21 nov. 2020
 *      Author: bultot
 */


#include "MCC_PID.hpp"

#include <iostream>
#include "unistd.h"
#include <wiringPi.h>
#include <semaphore.h>
#include "pthread.h"

#include "softPwm.h"

void* thread_PID(void*);

pthread_attr_t threadPID_attr;
pthread_t threadPID_t;
int ipin_MLI;

int initPID_Thread(int pin_MLI)
{

	pinMode(pin_MLI, OUTPUT);
	digitalWrite(pin_MLI, LOW);
	ipin_MLI = pin_MLI;
	if( pthread_create(&threadPID_t, &threadPID_attr, thread_PID, NULL) ==0)
		std::cout << "[INFO] Thread REGULATION created" << std::endl;
	else
		std::cout << "[WARNING] Thread REGULATION not created " << std::endl;

	return 0;
}


void* thread_PID(void* x)
{
	softPwmCreate (ipin_MLI, 0, 1000) ;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	while(1)
	{
		std::cout<<"erreur= " << i_measure <<std::endl;
		digitalWrite(ipin_MLI, LOW);
		softPwmWrite (ipin_MLI, i_measure) ;

		pthread_testcancel();
	}
	return 0;
}
