
//============================================================================
// Name        : Prog_STR_MCC.cpp
// Author      : Bultot Geoffrey, Ishumaru Geoffrey
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <MCC_PID.hpp>
#include <serial.hpp>
#include <iostream>
#include "unistd.h"
#include <wiringPi.h>
#include "time.h"
#include <signal.h>
#include "MODULES_DEFINE.hpp"

void terminerProgramme()
{
	stopPID_Regulation();
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

	if (sigaction(SIGTERM, &signalAction, NULL) == -1){ // si erreur
		std::cerr << "Impossible d'intercepter SIGTERM !" << std::endl;
	}
}

int main()
{
	//int i = 0;
	int PID;
	sched_param schedparam;
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

	int err = initPID_Thread(C_PIN_MLI);
	if(err == 0)
		std::cout << "[INFO] THREAD REGULATION initialized"<< std::endl;
	else
		std::cout << "[INFO] THREAD REGULATION not started ERROR VALUE: "<< err << std::endl;

	setSensRotation(E_SENS_HORAIRE);
	setConsigne(30);

	// SIGTERM est jamais intercepté ! donc pour terminer le pgrm proprement en attendant...
	sleep(5);
	terminerProgramme();

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



