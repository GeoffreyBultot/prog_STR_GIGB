
//============================================================================
// Name        : Prog_STR_MCC.cpp
// Author      : Bultot Geoffrey, Ishimaru Geoffrey
// Version     :
// Copyright   : Your copyright notice
// Description : Main of asservissement MCC lab
//============================================================================

#include "MODULES_DEFINE.hpp"
#include <MCC_PID.hpp>
#include <serial.hpp>
#include <communication.hpp>
#include <iostream>
#include "unistd.h"
#include <wiringPi.h>
#include "time.h"
#include <signal.h>

void terminerProgramme()
{
	stopPID_Regulation();
	stopCOM_thread();
	std::cout<< "termine"<<std::endl;
	exit(0);
}

// Pourrait intercepter et gérer tous les signaux, selon la configuration
void generalSignalHandler(int signal)
{
	std::cout<<"signal: " <<signal<<std::endl;
    if (1)//signal == SIGTERM)
    {
        std::cout << std::endl << "Exécution de terminerProgramme() en cours..." << std::endl;
        terminerProgramme();
        std::cout << "Fin." << std::endl;
        exit(0);
    }
    else
        std::cerr << "Signal non-géré" << std::endl;
}

void initSignalHandler()
{
	// - - - - - Mise en place du handler de signal - - - - -

	// Création et initialisation de la structure POSIX
	struct sigaction signalAction;
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = 0;
	// Fonction appelée lors de la signalisation
	signalAction.sa_handler = &generalSignalHandler;
	// Interception de SIGTEM uniquement

	if (sigaction(SIGINT, &signalAction, NULL) == -1){ // si erreur
		std::cerr << "Impossible d'intercepter SIGINT!" << std::endl;
	}
}

int main(int argc,char *argv[] )
{
	//int i = 0;
	int PID;
	sched_param schedparam;

#ifdef LOG_MEASURE
	float kpt;
	if(argc > 1)
		kpt = atof(argv[1]);  // alternative strtod
	else
		kpt = 0.02;
#endif
	PID = getpid();

	/*HIGH PRIORITY AND SCHED FIFO*/
	sched_getparam(PID,&schedparam);
	schedparam.sched_priority = 1;
	sched_setscheduler(PID,SCHED_FIFO,&schedparam);

	std::cout << "PROG STR MCC PID = "<< PID << std::endl;
	wiringPiSetupGpio(); //Init wiringPi
	std::cout << "[INFO] WiringPi initialized"<< std::endl;
	initSignalHandler();
	std::cout << "[INFO] SIG HANDLER initialized"<< std::endl;
	initSerial(C_BAUDRATE, C_PIN_RPI_RX, C_PIN_RPI_TX);
	std::cout << "[INFO] SERIAL initialized"<< std::endl;


#ifdef LOG_MEASURE
	int err = initPID_Thread(C_PIN_MLI,kpt);
#else
	int err = initPID_Thread(C_PIN_MLI);
#endif
	if(err)
	{
		std::cout << "[INFO] THREAD REGULATION not started ERROR VALUE: "<< err << std::endl;
		terminerProgramme();
	}
	else
	{
		std::cout << "[INFO] THREAD REGULATION initialized"<< std::endl;
		//TODO: erreur thread com
		err = initCOM_Thread();
		if(err)
		{
			std::cout<<"[ERROR] COM THREAD NOT STARTED error : " << err <<std::endl;
			terminerProgramme();
		}
		else
		{
			setSensRotation(E_SENS_DEFAULT);
			setConsigne(180);
		}
	}
	// SIGTERM est jamais intercepté ! donc pour terminer le pgrm proprement en attendant...
	//GB : Par contre SIGINT est intercepté lors d'une exécution en console ;)
	//sleep(5);
	//terminerProgramme();

	while(1)//loop forever
	{
		/*
		int i_measure = readSpeed();
		std::cout << "[DEBUG] i_mesure : " << i_measure << std::endl;
		usleep(2000000);
		*/
	}
	return 0;
}



