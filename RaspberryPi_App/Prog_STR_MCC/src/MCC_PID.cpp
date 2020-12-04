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



/* DEFINES */
#define MAX_PWM 1024	//RANGE max
#define MIN_PWM 0		//PWM MIN
#define CLOCK_PWM 4096	//CLOCK PWM (between 0 TO 4094) //TODO savoir la fréquence exacte

//NICHOLS ZIEGLER //TODO
#define Kcr		10
#define Tcr		5

#define H	4.77			//Période ech en mS //5 = 210Hz
#define Tp  Kcr*0.6			//Gain proportionnel
#define Ti  Tcr*0.5 		//Gain intégrale
#define Td  Tcr*0.125		//Gain dérivée

//Constantes multiplicatives dépendant de la
//fréquence, du gain d'intégrale Ti et dérivée Td
#define B0 Tp * ( (H/(2*Ti)) + (Td/H) + 1 )
#define B1 Tp * ( (H/(2*Ti)) - (2*Td/H) - 1 )
#define B2 Tp * Td / H

/* GLOBAL VARIABLES */
int C = 0;
double u = 0;			//Valeur renvoyée au robot
double E = 0;			//Erreur actuelle
double E_before = 0;	//Erreur précedente
double E2_before = 0;	//Erreur avant la précédente
double M = 0;			//Mesure

/* PROTOTYPES*/
void* thread_PID(void*);

pthread_attr_t threadPID_attr;
pthread_t threadPID_t;
int ipin_MLI;
int i_measure;

int initPID_Thread(int pin_MLI)
{
	int err = 0;
	ipin_MLI = pin_MLI;
	err = pthread_create(&threadPID_t, &threadPID_attr, thread_PID, NULL);
	if( err ==0)
	{
		pinMode(ipin_MLI, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_BAL);
		pwmSetRange(1024);
		pwmSetClock(4094);
		pwmWrite(ipin_MLI, 0);
	}
	return err;
}


void Calcul()
{
	i_measure = readSpeed();
	//std::cout << osef
	E=i_measure-C;	//Calcul de l'erreur

	u = u + B0*E + B1*E_before + B2*E2_before; //Calcul de la commande

	E2_before = E_before;		//définition de l'erreur avant la précédente
	E_before = E;				//définition de l'erreur précédente
	pwmWrite(ipin_MLI, u);
}

void* thread_PID(void* x)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	while(1)
	{

		Calcul();
		usleep(H*10000);
		pthread_testcancel();
	}
	return 0;
}

int stopPID_Regulation()
{

	pthread_cancel(threadPID_t);
	pthread_join(threadPID_t, NULL);
	pwmWrite(ipin_MLI, 0);
	//TODO : ajouter la valeur de retour du cancel.
	return 0;
}

