
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


int main()
{
	int i = 0;
	int PID;
	sched_param schedparam;
	PID = getpid();
	cout << "PROG STR MCC PID = "<< PID << endl; // prints Signal carre 500µs


	sched_getparam(PID,&schedparam);
	schedparam.sched_priority = 1;
	sched_setscheduler(PID,SCHED_FIFO,&schedparam);

	initSerial(900, 2, 3);
	initPID_Thread(4);
	while(1)
	{
		i_measure = readSpeed();
		std::cout<< "mesure = " <<i_measure<<std::endl;

	}
	return 0;
}
