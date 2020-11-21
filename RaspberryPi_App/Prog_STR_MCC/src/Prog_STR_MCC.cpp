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

using namespace std;

#define C_PIN_TX 3
#define C_PIN_RX 4

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


int main()
{
	int i = 0;
	int PID;
	sched_param schedparam;
	cout << "Signal carre 500µs" << endl; // prints Signal carre 500µs
	wiringPiSetupGpio();
	PID = getpid();
	std::cout << "PID = " << PID << std::endl; // !!! breakpoint avec commande !!!

	sched_getparam(PID,&schedparam);
	schedparam.sched_priority = 1;
	sched_setscheduler(PID,SCHED_FIFO,&schedparam);

	pinMode(2,OUTPUT);
	pinMode(C_PIN_TX,OUTPUT);
	pinMode(C_PIN_RX,INPUT);



	digitalWrite(C_PIN_TX, HIGH);
	i=0;
	while(1)
	{
		std::cout<<readSpeed()<<std::endl;

	}
	return 0;
}

