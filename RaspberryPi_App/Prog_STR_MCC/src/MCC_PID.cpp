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
#include <sys/time.h>
#include "string.h"

/* DEFINES */
#define MAX_PWM 1024	//RANGE max
#define MIN_PWM 0		//PWM MIN
#define CLOCK_PWM 4096	//CLOCK PWM (between 0 TO 4094) //TODO savoir la fréquence exacte

//NICHOLS ZIEGLER //TODO
#define Kcr		0.14
#define Tcr		74.653

#define dt	4.77			//Période ech en mS //5 = 210Hz
#define invdt 1/dt

/*
#define Kp  Kcr*0.6			//Gain proportionnel
#define Ti  Tcr*0.5 		//Gain intégrale
#define Td  Tcr*0.125		//Gain dérivée
*/
#define Kp 0.01
#define Ti 1000000
#define Td 99999999999999

#define Ki	1/Ti
#define Kd	1/Td


/*status bytes (in the same order of telemetries list on the C# application*/

#define C_STATUS_REGULATION_ON		1<<0
#define C_STATUS_COMMUNICATION_ON	1<<1
#define C_STATUS_ANALOG_SUPPLY_ON	1<<2
#define C_STATUS_LOGIC_SUPPLY_ON	1<<3
#define C_STATUS_ALL_ON				0xFF //Clear or set all flags

/* GLOBAL VARIABLES */
int C = 0;
float u = 0;			//Valeur renvoyée au robot
float E = 0;			//Erreur actuelle
float E_before = 0;	//Erreur précedente
float E2_before = 0;	//Erreur avant la précédente
float M = 0;			//Mesure

T_ROTATION_SENS CurrentSensRotation=E_SENS_DEFAULT;

/* PROTOTYPES*/
void* thread_PID(void*);

pthread_attr_t threadPID_attr;
pthread_t threadPID_t;
int ipin_MLI;
int i_measure;
int MCC_Status;

pthread_mutex_t mutex_MCCAngle;
pthread_mutex_t mutex_MCCStatus;
pthread_mutex_t mutex_MCCConsigne;

pthread_mutexattr_t mutex_MCCAngle_attr;
pthread_mutexattr_t mutex_MCCStatus_attr;
pthread_mutexattr_t mutex_MCCConsigne_attr;

//TODO Remove after found Kcr
#ifdef LOG_MEASURE
static float GainProp = 0.0;
char filename[200] = "";
struct timeval stop_kcr, start_kcr;
FILE* fp;
#endif


#ifdef LOG_MEASURE
int initPID_Thread(int pin_MLI, float Prop)
#else
int initPID_Thread(int pin_MLI)
#endif
{
	int err = 0;
	ipin_MLI = pin_MLI;
	SetMCCStatusFlag(C_STATUS_ALL_ON, false);

#ifdef LOG_MEASURE
	GainProp = Prop;
	char buffer[1000] = {0};
	sprintf(buffer, "FOUND_Kcr_KP=%f.csv", Prop);
	strcpy(filename, buffer);
	fp = fopen(filename, "w");
	fprintf(fp, "time;Angle;\n");
	fclose(fp);
	gettimeofday(&start_kcr, NULL);
#endif

	mutex_MCCConsigne = PTHREAD_MUTEX_INITIALIZER;
	mutex_MCCStatus = PTHREAD_MUTEX_INITIALIZER;
	mutex_MCCAngle = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&mutex_MCCConsigne, &mutex_MCCConsigne_attr);
	pthread_mutex_init(&mutex_MCCStatus, &mutex_MCCStatus_attr);
	pthread_mutex_init(&mutex_MCCAngle, &mutex_MCCAngle_attr);


	err = pthread_create(&threadPID_t, &threadPID_attr, thread_PID, NULL);

	if( err ==0)
	{
		pinMode(C_PIN_RELAY_LOGIC, OUTPUT);
		pinMode(C_PIN_RELAY_POWER, OUTPUT);
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

void LogicRelayON(){
	digitalWrite(C_PIN_RELAY_LOGIC, LOW);
	SetMCCStatusFlag(C_STATUS_LOGIC_SUPPLY_ON, true);
}

void LogicRelayOFF(){
	digitalWrite(C_PIN_RELAY_LOGIC, HIGH);
	SetMCCStatusFlag(C_STATUS_LOGIC_SUPPLY_ON, false);
}

void PowerRelayON(){
	digitalWrite(C_PIN_RELAY_POWER, LOW);
	SetMCCStatusFlag(C_STATUS_ANALOG_SUPPLY_ON, true);
}

void PowerRelayOFF(){
	digitalWrite(C_PIN_RELAY_POWER, HIGH);
	SetMCCStatusFlag(C_STATUS_ANALOG_SUPPLY_ON, false);
}

void setSensRotation(T_ROTATION_SENS sensRotation)
{
	//Activation des pins
	switch(sensRotation)
	{
	case E_SENS_DEFAULT:
		digitalWrite(C_PIN_SENS_ROT1, LOW);
		digitalWrite(C_PIN_SENS_ROT2, LOW);
		break;
	case E_SENS_HORAIRE:
		digitalWrite(C_PIN_SENS_ROT1, LOW);
		digitalWrite(C_PIN_SENS_ROT2, HIGH);
		break;
	case E_SENS_ANTI_HORAIRE:
		digitalWrite(C_PIN_SENS_ROT1, HIGH);
		digitalWrite(C_PIN_SENS_ROT2, LOW);
		break;
	}
	CurrentSensRotation =  sensRotation;
}

int getConsigne()
{
	int consigne;
	pthread_mutex_lock(&mutex_MCCConsigne);
	consigne = C;
	pthread_mutex_unlock(&mutex_MCCConsigne);
	return consigne;
}
void setConsigne(int consigne)
{
	pthread_mutex_lock(&mutex_MCCConsigne);
	C = consigne;
	pthread_mutex_unlock(&mutex_MCCConsigne);
}

int getMCCStatus()
{
	int status;
	pthread_mutex_lock(&mutex_MCCStatus);
	//todo mutex
	status = MCC_Status;
	pthread_mutex_unlock(&mutex_MCCStatus);
	//mutex
	return status;
}

void SetMCCStatusFlag(int flag, bool value)
{
	pthread_mutex_lock(&mutex_MCCStatus);
	//todo mutex
	if(value)
		MCC_Status |=  flag;
	else
		MCC_Status &=~ flag;
	pthread_mutex_unlock(&mutex_MCCStatus);
	//TODO mutex
}

int getMCCAngle()
{
	int measure;
	pthread_mutex_lock(&mutex_MCCAngle);
	measure = i_measure;
	pthread_mutex_unlock(&mutex_MCCAngle);
	return measure;
}

void setMCCAngle(int angle)
{
	pthread_mutex_lock(&mutex_MCCAngle);
	i_measure = angle;
	pthread_mutex_unlock(&mutex_MCCAngle);

}

float p = 0;
float i = 0;
float d = 0;
float pid = 0;

void Calcul()
{
	//kp = 0.02 : stabilitsation
	float epsilon;
	int errors = readAngle(&i_measure);

	if(errors)
	{
		//TODO : Handle errors. On arrête la régulation pour une erreur de transmit ?
		SetMCCStatusFlag(C_STATUS_COMMUNICATION_ON, false);
		//Si arrêt de la régu : MCC_Status &=~ C_STATUS_REGULATION_ON;
		std::cout<< "[DEBUG] ERROR IN COM : " << errors << std::endl;
	}
	else
	{
#ifdef LOG_MEASURE
		gettimeofday(&stop_kcr, NULL);
		int diff = (stop_kcr.tv_sec - start_kcr.tv_sec) * 1000000 + stop_kcr.tv_usec - start_kcr.tv_usec;
		char buff[20] = {0};
		sprintf(buff,"%d;%d\n",diff,i_measure);
		FILE* fp;
		fp = fopen(filename, "a");
		fprintf(fp, buff);
		fclose(fp);
#endif
		float consigne = (float)getConsigne();
		float measure = (float)getMCCAngle();
		epsilon = consigne - (float)measure;	//Calcul de l'erreur
		p = Kp*epsilon;
		i = (i + epsilon);
		d = epsilon-E_before;

		pid = p + i*Ki + d*Kd;

		E_before = epsilon;
		u = pid;

		std::cout<<"[DEBUG] consigne = " << consigne << "\t i_measure = " << measure << "\t commande = " << pid <<std::endl;

		if(pid < 0)
		{
			if(pid<-1){pid=-1.0;}
			setSensRotation(E_SENS_ANTI_HORAIRE);
			pwmWrite(ipin_MLI, -(pid*MAX_PWM));
		}
		else
		{
			if(pid>1){pid=1.0;}
			setSensRotation(E_SENS_HORAIRE);
			pwmWrite(ipin_MLI, pid*MAX_PWM);
		}
		SetMCCStatusFlag(C_STATUS_COMMUNICATION_ON,true);
	}
}

void* thread_PID(void* x)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    //TODO ajout de sem de synchro pour attendre la fin de l'initialisation

    LogicRelayON();
	std::cout << "[INFO] LogicRelay is ON"<< std::endl;
	//sleep(2);
	PowerRelayON();
	std::cout << "[INFO] PowerRelay is ON"<< std::endl;
	//sleep(2);

	SetMCCStatusFlag(C_STATUS_REGULATION_ON, true);
	while(1)
	{
		Calcul();
		usleep(dt*1000);
		pthread_testcancel();
	}
	return 0;
}

int stopPID_Regulation()
{
	pwmWrite(ipin_MLI, 0);
	digitalWrite(C_PIN_SENS_ROT1, LOW);
	digitalWrite(C_PIN_SENS_ROT2, LOW);
	PowerRelayOFF();
	LogicRelayOFF();
	pthread_mutex_destroy(&mutex_MCCConsigne);
	pthread_mutex_destroy(&mutex_MCCAngle);
	pthread_mutex_destroy(&mutex_MCCStatus);
	pthread_cancel(threadPID_t);
	pthread_join(threadPID_t, NULL);
	//TODO : ajouter la valeur de retour du cancel.
	return 0;
}

