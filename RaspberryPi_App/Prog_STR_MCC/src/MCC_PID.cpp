/**============================================================================
 * @file MCC_PID.cpp
 * @brief
 * @details
 *
 * - Company			: HE2B - ISIB
 * - Project			: progra STR : MCC asservissement
 * - Authors			: Bultot Geoffrey, Ishimaru Geoffrey
 *   Copyright			: All right reserved
 *   Description		: This file contain
 *   							- PID regulation for the MCC asservissement LAB
 *   							- Functions to set and get values of shared variables
 *   							- Shared variables protections using mutexs
 *=============================================================================*/


/***************************************************************************
* Includes Directives
***************************************************************************/

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
#include "sys/times.h"


/***************************************************************************
* Constants
***************************************************************************/

#define MAX_PWM 1024	//RANGE max
#define MIN_PWM 0		//PWM MIN
#define CLOCK_PWM 4096	//CLOCK PWM (between 0 TO 4094) //TODO savoir la fréquence exacte

#define dt	4.77			//Période ech en mS //5 = 210Hz
#define invdt 1/dt

#define C_NB_ECH_INTEGRAL 5
#define C_NB_ECH_MOTOR_BLOCKED 100

#define Kp 0.0019*0.65			//sol A *0.65	//sol B *0.05		//sol C *1.0
#define Ki 0.000034*5.45		//sol A *5.45	//sol B *6.0		//sol C *5.0//025
#define Td 0.034*4.0			//sol A *4.0		//sol B *0.5		//sol C *1.8


/*status bytes (in the same order of telemetries list on the C# application*/
#define C_STATUS_REGULATION_ON		1<<0
#define C_STATUS_COMMUNICATION_ON	1<<1
#define C_STATUS_ANALOG_SUPPLY_ON	1<<2
#define C_STATUS_LOGIC_SUPPLY_ON	1<<3
#define C_STATUS_IS_MOTOR_BLOCKED	1<<4
#define C_STATUS_ALL_ON				0xFF //Clear or set all flags

/***************************************************************************
* GLOBAL Variables                                                              **
***************************************************************************/
float p = 0;
float i = 0;
float d = 0;
float pid = 0;
float C = 0;
float E_before = 0;	//Erreur précedente
float errPrec[C_NB_ECH_MOTOR_BLOCKED] = {0};

int ipin_MLI;
int i_measure;
int MCC_Status;

T_ROTATION_SENS CurrentSensRotation = E_SENS_DEFAULT;

pthread_attr_t threadPID_attr;
pthread_t threadPID_t;

pthread_mutex_t mutex_MCCAngle;
pthread_mutex_t mutex_MCCStatus;
pthread_mutex_t mutex_MCCConsigne;
pthread_mutex_t mutex_PIDCommand;

pthread_mutexattr_t mutex_MCCAngle_attr;
pthread_mutexattr_t mutex_MCCStatus_attr;
pthread_mutexattr_t mutex_MCCConsigne_attr;
pthread_mutexattr_t mutex_PIDCommand_attr;


/***************************************************************************
** Functions Prototypes
***************************************************************************/

void* thread_PID(void*);


/***************************************************************************
** Functions                                                              **
***************************************************************************/

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
	mutex_PIDCommand = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&mutex_MCCConsigne, &mutex_MCCConsigne_attr);
	pthread_mutex_init(&mutex_MCCStatus, &mutex_MCCStatus_attr);
	pthread_mutex_init(&mutex_MCCAngle, &mutex_MCCAngle_attr);
	pthread_mutex_init(&mutex_PIDCommand, &mutex_PIDCommand_attr);


	int policy = 0;
	int max_prio_for_policy = 0;

	pthread_attr_init(&threadPID_attr);
	pthread_attr_getschedpolicy(&threadPID_attr, &policy);
	max_prio_for_policy = sched_get_priority_max(policy);
	pthread_setschedprio(threadPID_t, max_prio_for_policy);

	err = pthread_create(&threadPID_t, &threadPID_attr, thread_PID, NULL);

	if( err ==0)
	{
		pinMode(C_PIN_RELAY_LOGIC, OUTPUT);	//PIN relay logic part 	: PIN OUTPUT
		pinMode(C_PIN_RELAY_POWER, OUTPUT);	//Pin relay power part 	: PIN OUTPUT
		pinMode(C_PIN_SENS_ROT1, OUTPUT);	//Rotation sense 1 		: PIN OUTPUT
		pinMode(C_PIN_SENS_ROT2, OUTPUT);	//Rotation sense 2		: PIN OUTPUT
		pinMode(ipin_MLI, PWM_OUTPUT);		//Pin MLI 				: PWM OUTPUT

		pwmSetMode(PWM_MODE_BAL);	//PWM Balanced mode
		pwmSetRange(1024);			//Range PWM : 0 to 1023
		pwmSetClock(4094);			//TODO: check the divider
		pwmWrite(ipin_MLI, 0);		//Set pwm to 0
	}
	return err;
}

/**
 * @brief This function is used to power ON the logic part of the circuit
 * @return void
 */
void LogicRelayON(){
	digitalWrite(C_PIN_RELAY_LOGIC, HIGH);
	SetMCCStatusFlag(C_STATUS_LOGIC_SUPPLY_ON, true);
}
/**
 * @brief This function is used to power OFF the logic part of the circuit
 * @return void
 */
void LogicRelayOFF(){
	digitalWrite(C_PIN_RELAY_LOGIC, LOW);
	SetMCCStatusFlag(C_STATUS_LOGIC_SUPPLY_ON, false);
}
/**
 * @brief This function is used to power ON the power part of the circuit
 * @return void
 */
void PowerRelayON(){
	digitalWrite(C_PIN_RELAY_POWER, HIGH);
	SetMCCStatusFlag(C_STATUS_ANALOG_SUPPLY_ON, true);
}
/**
 * @brief This function is used to power OFF the power part of the circuit
 * @return void
 */
void PowerRelayOFF(){
	digitalWrite(C_PIN_RELAY_POWER, LOW);
	SetMCCStatusFlag(C_STATUS_ANALOG_SUPPLY_ON, false);
}

/**
 * @brief This function is used to set the rotation sense of the motor
 * @param[in] sensRotation
 * @return void
 */
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

/**
 * @brief This function is used to get the current set point (with mutex protection)
 * @return void
 */
int getConsigne()
{
	int consigne;
	pthread_mutex_lock(&mutex_MCCConsigne);
	consigne = C;
	pthread_mutex_unlock(&mutex_MCCConsigne);
	return consigne;
}

/**
 * @brief This function is used to set the current set point (with mutex protection)
 * @param[in] consigne
 * @return void
 */
void setConsigne(int consigne)
{
	pthread_mutex_lock(&mutex_MCCConsigne);
	C = consigne;
	pthread_mutex_unlock(&mutex_MCCConsigne);
}

/**
 * @brief This function is used to get the MCCStatus register (with mutex protection)
 * @return void
 */
int getMCCStatus()
{
	int status;
	pthread_mutex_lock(&mutex_MCCStatus);
	status = MCC_Status;
	pthread_mutex_unlock(&mutex_MCCStatus);
	return status;
}

/**
 * @brief This function is used to set the MCCStatus register (with mutex protection)
 * @param[in] flag
 * @param[in] value
 * @return void
 */
void SetMCCStatusFlag(int flag, bool value)
{
	pthread_mutex_lock(&mutex_MCCStatus);
	if(value)
		MCC_Status |=  flag;
	else
		MCC_Status &=~ flag;
	pthread_mutex_unlock(&mutex_MCCStatus);
}

/**
 * @brief This function is used to get the current angle (with mutex protection)
 * @return void
 */
int getMCCAngle()
{
	int measure;
	pthread_mutex_lock(&mutex_MCCAngle);
	measure = i_measure;
	pthread_mutex_unlock(&mutex_MCCAngle);
	return measure;
}

/**
 * @brief This function is used to set the current angle (with mutex protection)
 * @param[in] angle
 * @return void
 */
void setMCCAngle(int angle)
{
	pthread_mutex_lock(&mutex_MCCAngle);
	i_measure = angle;
	pthread_mutex_unlock(&mutex_MCCAngle);

}

/**
 * @brief This function is used to get the current pid command (with mutex protection)
 * @return void
 */
float getPIDCommand()
{
	float command;
	pthread_mutex_lock(&mutex_PIDCommand);
	command = pid;
	pthread_mutex_unlock(&mutex_PIDCommand);
	return command;
}

/**
 * @brief This function is used to set the current pid command (with mutex protection)
 * @param[in] command
 * @return void
 */
void setPIDCommand(float command)
{
	pthread_mutex_lock(&mutex_PIDCommand);
	pid = command;
	pthread_mutex_unlock(&mutex_PIDCommand);
	if(command < 0)
	{
		if(pid<-1){command=-1.0;}
		setSensRotation(E_SENS_HORAIRE);
		pwmWrite(ipin_MLI, -(command*MAX_PWM));
	}
	else
	{
		if(command>1){command=1.0;}
		setSensRotation(E_SENS_ANTI_HORAIRE);
		pwmWrite(ipin_MLI, command*MAX_PWM);
	}

}

/**
 * @brief This function is used to read angle in NUCLEO and calcul & set a new pid value
 * @return void
 */
void Calcul()
{
	/*Variables locales utilisées pour le calcul. L'utilisation de variables locales est important d'une par pour éviter le TOCTOU
	 * (time of check time of use) et d'autre part pour stocker les variables globales utilisées dans plusieurs threads.
	 * => Moins de contraintes temporelles parce qu'on utilise qu'une seule fois le mutex de chaque variable globale. */
	float epsilon;
	int local_angle_measure = 0;
	float localpid;
	float consigne;

	/* La consigne étant une variable partagée entre plusieurs threads, il faut la récupérer grâce à getConsigne. Cette fonction
	 * contenant un mutex, on la stocke dans une variable locale AVANT de lire la mesure sur la nucleo*/
	consigne = (float)getConsigne();
	/*thread pas cancellable pendant la lecture sur nucleo*/
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	int errors = readAngle(&local_angle_measure);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);


	if(errors){
		/*Protection en cas d'erreur sur la communication:	- Flag de communication à false
		 * 													- Flag de régulation à 0 (on ne régule pas)
		 * 													- Arrêt de la régulation (commande à 0)
		 */
		SetMCCStatusFlag(C_STATUS_COMMUNICATION_ON, false);
		std::cout << "erreur de comm" << std::endl;	//GI
		SetMCCStatusFlag(C_STATUS_REGULATION_ON, false);
		setPIDCommand(0);
	}
	else
	{
		/* Si la communication s'est bien passée, on a récupéré l'angle et on régule le moteur.
		 * 				- Flag communication à true
		 * 				- Flag régulation à true
		 * */
		SetMCCStatusFlag(C_STATUS_REGULATION_ON, true);
		SetMCCStatusFlag(C_STATUS_COMMUNICATION_ON,true);
		epsilon = consigne - (float)local_angle_measure;	//Calcul de l'erreur

		for(int idx = C_NB_ECH_MOTOR_BLOCKED-2; idx >= 0; idx--){
					//i += errPrec[idx]; //Retirer cette ligne = gain de temps. On soustrait la dernière à i et on ajoute l'actuelle
					errPrec[idx+1] = errPrec[idx];
		}

		errPrec[0]= epsilon*dt;
		//Terme proportionnel
		p = Kp*epsilon;
		//Terme intégral
		i -= errPrec[C_NB_ECH_INTEGRAL-1];	//Soustraction de la dernière
		i += errPrec[0];					//Addition de l'actuelle
		//Terme dérivée
		d = epsilon-E_before;
		E_before = epsilon;
		//Localpid = valeur de pid dans cette fonction
		localpid  = p + i*Ki + d*Td*invdt;

		/* Mise à jour de la valeur de la PWM et de la variable globale de la commande*/
		setPIDCommand(localpid);
		/*Mise à jour de l'angle dans la variable globale. On met à jour après avoir calculé le PID pour ne pas ralentir
		* le calcul avec le mutex de setMMCAnle comme expliqué au début de la fonction.*/
		setMCCAngle(local_angle_measure);

		/*Détection de blocage de rotor. On vérifie le tableau des derniers échantillons relevés sur la MCC
		 * Si ils sont tous les mêmes et qu'on a une commande supérieure à 4% en valeur absolue, il y a blocage rotor
		 * Si il y a blocage rotor, on arrête la régulation.*/
		bool toutesLesMemesErreurs = true;
		for(int k = 0; k < C_NB_ECH_MOTOR_BLOCKED-1; k++){
			if(errPrec[k] != errPrec[k+1]){
				toutesLesMemesErreurs = false;
			}
		}
		if( ((localpid > 0.05)||(localpid < -0.05)) && (toutesLesMemesErreurs==true)){
//		if((toutesLesMemesErreurs==true)){
			SetMCCStatusFlag(C_STATUS_IS_MOTOR_BLOCKED, true);
			/* Arrêt d'urgence fonctionnel mais totalement blocant juste pour démo */
			//SetMCCStatusFlag(C_STATUS_REGULATION_ON, false);
			//while(1){setPIDCommand(0);}
		}
		else{
			SetMCCStatusFlag(C_STATUS_IS_MOTOR_BLOCKED, false);
			SetMCCStatusFlag(C_STATUS_REGULATION_ON, true);
		}

		//std::cout<<"[DEBUG] p = " << p << "\t i = " << i*Ki << "\t d = " << d*Td*invdt <<"\t commande = " << localpid <<std::endl;

#ifdef LOG_MEASURE //Log les mesure sur la MCC
		gettimeofday(&stop_kcr, NULL);
		int diff = (stop_kcr.tv_sec - start_kcr.tv_sec) * 1000000 + stop_kcr.tv_usec - start_kcr.tv_usec;
		char buff[20] = {0};
		sprintf(buff,"%d;%d\n",diff,local_angle_measure);
		FILE* fp;
		fp = fopen(filename, "a");
		fprintf(fp, buff);
		fclose(fp);
#endif
	}
}

/**
 * @brief This function is a thread to handle the motor regulation
 * @param[in] void*
 * @return void*
 */
void* thread_PID(void* x)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    //TODO ajout de sem de synchro pour attendre la fin de l'initialisation

    //Allumage des relais pour alimenter la carte
    sleep(2);
    LogicRelayON();
	std::cout << "[INFO] LogicRelay is ON"<< std::endl;
	sleep(2);
	PowerRelayON();
	std::cout << "[INFO] PowerRelay is ON"<< std::endl;
	/*TODO PIN DE DEBUG*/
	pinMode(C_PIN_LED_GREEN_2, OUTPUT);

	struct timeval t_second, t_first;
	int diff;
	while(1)
	{
		//digitalWrite(C_PIN_LED_GREEN_2, HIGH);
		gettimeofday(&t_first,NULL);
		Calcul();
		gettimeofday(&t_second,NULL);
		diff = (t_second.tv_sec - t_first.tv_sec) * 1000000 + t_second.tv_usec - t_first.tv_usec;
		std::cout<< "Calcul() took " << diff << "us" << std::endl;
		//digitalWrite(C_PIN_LED_GREEN_2, LOW);

		usleep((dt)*1000-diff);
		pthread_testcancel();
	}
	return 0;
}

/**
 * @brief This function is used to stop the pid regulation, stop the motor
 * @return errors in cancel thread
 */
int stopPID_Regulation()
{
	pthread_mutex_destroy(&mutex_MCCConsigne);
	pthread_mutex_destroy(&mutex_MCCAngle);
	pthread_mutex_destroy(&mutex_MCCStatus);
	pthread_mutex_destroy(&mutex_PIDCommand);
	int err = pthread_cancel(threadPID_t);
	pthread_join(threadPID_t, NULL);
	//Stop MLI AFTER the thread finish. => Si on le fait avant on rique d'avoir mis la pwm à 0 puis que le thread calcule une nouvelle pwm avant de passer sur le cancel thread. (déjà arrivé)
	pwmWrite(ipin_MLI, 0);
	setSensRotation(E_SENS_DEFAULT);
	PowerRelayOFF();
	LogicRelayOFF();
	digitalWrite(ipin_MLI, LOW);

	int angleBefore = i_measure + 1; //set angleBefore to a value different to i_measure
	while(i_measure != angleBefore)
	{
		angleBefore = i_measure; //Store angle before
		usleep(100000); //wait 100ms
		int errors = readAngle(&i_measure);//Get new angle
		std::cout<<"[INFO] waiting for the motor stop"<<std::endl;
		if(errors != 0)
		{
			SetMCCStatusFlag(C_STATUS_COMMUNICATION_ON, false);
			return err; //If error in COM, angle cannot be read. Quit the function. TODO: right way ?
		}
	}
	std::cout<<"[INFO] Motor Stopped"<<std::endl;
	SetMCCStatusFlag(C_STATUS_REGULATION_ON, false);
	return err;
}

