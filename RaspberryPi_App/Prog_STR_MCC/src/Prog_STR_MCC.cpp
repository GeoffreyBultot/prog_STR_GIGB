//============================================================================
// Name        : Prog_STR_MCC.cpp
// Author      : Bultot Geoffrey, Ishumaru Geoffrey
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "unistd.h"
#include <wiringPi.h>
#include "time.h"
using namespace std;

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

	//digitalWrite(2, HIGH);
	clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
	//digitalWrite(2, LOW);
	//clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
}


int main()
{
	int i = 0;
	int PID;
	sched_param schedparam;
	cout << "Signal carre 500µs" << endl; // prints Signal carre 500µs
	wiringPiSetupGpio();
	PID = getpid();
	sched_getparam(PID,&schedparam);
	schedparam.sched_priority = 1;
	sched_setscheduler(PID,SCHED_FIFO,&schedparam);

	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);

	while(1)//i<100000)
	{
		i=0;
		digitalWrite(2, HIGH);
		//my_delay(240);
		/*while(i<120)
			i++;
		i=0;
		*/
		usleep(250);
		digitalWrite(2, LOW);
		usleep(250);
		//my_delay(240);
		while(i<120)
			i++;
	}


	return 0;
}


