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
#include "MODULES_DEFINE.hpp"


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

T_ROTATION_SENS CurrentSensRotation = E_SENS_HORAIRE;

/* PROTOTYPES*/
void* thread_PID(void*);

pthread_attr_t threadPID_attr;
pthread_t threadPID_t;
int ipin_MLI;
float i_measure;

int initPID_Thread(int pin_MLI)
{
	int err = 0;
	ipin_MLI = pin_MLI;
	err = pthread_create(&threadPID_t, &threadPID_attr, thread_PID, NULL);
	if( err ==0)
	{
		pinMode(C_PIN_RELAY_DIGITAL, OUTPUT);
		pinMode(C_PIN_RELAY_ANALOG, OUTPUT);
		pinMode(C_PIN_SENS_ROT1, OUTPUT);
		pinMode(C_PIN_SENS_ROT2, OUTPUT);
		pinMode(ipin_MLI, PWM_OUTPUT);

		pwmSetMode(PWM_MODE_BAL);
		pwmSetRange(1024);
		pwmSetClock(4094);
		pwmWrite(ipin_MLI, 0);
	}
	return err;
}

void setSensRotation(T_ROTATION_SENS sensRotation)
{
	if(sensRotation != CurrentSensRotation)
	{
		/*TODO: (voir si c'est la bonne méthode) l'idee ici c est de s assurer que le moteur est bien
		 * arrete avant de changer de sens de rotation pour ne pas endommager l electronique*/
		int temp_c = C;
		C = 0; //Mise de la consigne à 0
		while(i_measure != 0); //Attendre l'arrête du moteur
		CurrentSensRotation = sensRotation;//Sauvegarde du nouveau sens de rotation

		//Activation des pins
		if(CurrentSensRotation == E_SENS_HORAIRE)
		{
			digitalWrite(C_PIN_SENS_ROT1, HIGH);
			digitalWrite(C_PIN_SENS_ROT2, LOW);
		}
		else if(CurrentSensRotation == E_SENS_ANTI_HORAIRE)
		{
			digitalWrite(C_PIN_SENS_ROT1, LOW);
			digitalWrite(C_PIN_SENS_ROT2, HIGH);
		}

		C = temp_c; //Faire tourner à la même vitesse qu'avant dans l autre sens
	}
}

void setConsigne(int consigne)
{
	//todo mutex
	C = consigne;
	//mutex
}

void Calcul()
{
	 int errors = readSpeed(&i_measure);
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

