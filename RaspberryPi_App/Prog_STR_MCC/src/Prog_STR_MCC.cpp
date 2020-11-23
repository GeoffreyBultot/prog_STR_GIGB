
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

using namespace std;
int i_measure;

void my_delay(int del)
{
	struct timespec deadline;
	clock_gettime(CLOCK_MONOTONIC, &deadline);
	deadline.tv_nsec += del*1000;

	if( deadline.tv_nsec >= 1000000000)
	{
		deadline.tv_nsec -= 1000000000;
		deadline.tv_sec++;
	}

	clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
}



void terminerProgramme()
{
	std::cout<< "termine"<<std::endl;

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

int main()
{
	int i = 0;
	int PID;
	sched_param schedparam;
	PID = getpid();
	cout << "PROG STR MCC PID = "<< PID << endl; // prints Signal carre 500µs

	// - - - - - Mise en place du handler de signal - - - - -

    // Création et initialisation de la structure POSIX
    struct sigaction signalAction;
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags = 0;
    // Fonction appelée lors de la signalisation
    signalAction.sa_handler = &generalSignalHandler;
    // Interception de SIGINT uniquement

    if (sigaction(SIGTERM, &signalAction, NULL) == -1) // si erreur
        std::cerr << "Impossible d'intercepter SIGTERM !" << std::endl;


	sched_getparam(PID,&schedparam);
	schedparam.sched_priority = 1;
	sched_setscheduler(PID,SCHED_FIFO,&schedparam);

	initSerial(900, 2, 3);
	initPID_Thread(18);

	while(1)
	{
		i_measure = readSpeed();
		i_measure = 100;
		std::cout<< "mesure = " <<i_measure<<std::endl;

	}
	return 0;
}



