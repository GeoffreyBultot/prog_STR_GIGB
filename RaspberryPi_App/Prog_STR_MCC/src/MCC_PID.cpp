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
#include "serial.hpp"


#define MAX_SPEED 1024	//Vitesse max
#define H	 5		//Période ech en mS //5 = 200Hz

#define Tp  5		//Gain proportionnel
#define Ti  1 	//Gain intégrale
#define Td  0.2		//Gain dérivée

//Constantes multiplicatives dépendant de la
//fréquence, du gain d'intégrale Ti et dérivée Td
//Voir rapport pour l'explication
#define B0 Tp * ( (H/(2*Ti)) + (Td/H) + 1 )
#define B1 Tp * ( (H/(2*Ti)) - (2*Td/H) - 1 )
#define B2 Tp * Td / H

int C = 0;
double u=0;			//Valeur renvoyée au robot
double E=0;			//Erreur actuelle
double E_before = 0;	//Erreur précedente
double E2_before = 0;	//Erreur avant la précédente
double M = 0;			//Mesure
void* thread_PID(void*);

pthread_attr_t threadPID_attr;
pthread_t threadPID_t;
int ipin_MLI;
int i_measure;

int initPID_Thread(int pin_MLI)
{
	pinMode(pin_MLI, PWM_OUTPUT );
	digitalWrite(pin_MLI, LOW);
	ipin_MLI = pin_MLI;

	if( pthread_create(&threadPID_t, &threadPID_attr, thread_PID, NULL) ==0)
		std::cout << "[INFO] Thread REGULATION created" << std::endl;
	else
		std::cout << "[WARNING] Thread REGULATION not created " << std::endl;

	pwmSetMode(PWM_MODE_BAL );
	pwmSetRange(1024);
	pwmSetClock(4094);
	pwmWrite(18, 0);

	usleep(1000000);
	return 0;
}


void Calcul()
{
	i_measure = readSpeed();
	E=i_measure-C;	//Calcul de l'erreur

	u = u + B0*E + B1*E_before + B2*E2_before; //Calcul de la commande

	E2_before = E_before;		//définition de l'erreur avant la précédente
	E_before = E;				//définition de l'erreur précédente
	//TODO: uncomment to start PWM
	//pwmWrite(ipin_MLI, u);
}

void* thread_PID(void* x)
{

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	while(1)
	{

		Calcul();
		usleep(H*1000);
		pthread_testcancel();
	}
	return 0;
}
