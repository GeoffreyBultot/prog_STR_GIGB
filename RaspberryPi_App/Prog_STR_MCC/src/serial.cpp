

#include "serial.hpp"

#include <iostream>
#include "unistd.h"
#include <wiringPi.h>

int iTxPin;
int iRxPin;

int half_nanoPeriod = 0;
unsigned char receptionArray[C_N_BYTES_TOT];
int getSpeedFromCOM();
void serialNanoSleep(int nanoSec);

void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin)
{
	wiringPiSetupGpio();
	//half_period = (baudrate/C_N_BITS);
	half_nanoPeriod = (int) (1000000000.0/( (float)baudrate * (float)C_N_BITS * 2.0));

	std::cout << "[DEBUG] Half_nanoPeriod : " << half_nanoPeriod << std::endl;
	iRxPin = rxPin;
	iTxPin = txPin;

	pinMode(iRxPin,INPUT);
	pinMode(iTxPin,OUTPUT);

	digitalWrite(iTxPin, HIGH);
	usleep(1000000);
}

int readSpeed()
{
	int i;
	int j;
	int currentByte = 0;
	bool currentbit;
	//TODO ADD FALLING EDGE DETECTION
	/*if(digitalRead(iRxPin) != HIGH)
		return -1;
	 */
	//Low -> StartBit
	digitalWrite(iTxPin, LOW);
	//Waiting com from Nucleo

	int start_timeout = time(NULL);


	//TODO TEST TIMEOUT
	while(digitalRead(iRxPin) != LOW)
	{
		if(difftime(time(NULL), start_timeout) >= 1)
		{
			std::cout<<"NUCLEO NOT RESPONDING"<<std::endl;
			return -1;
		}
	}



	std::cout << "Réception : ";
	for(i=0;i<C_N_BYTES_TOT;i++)
	{
		currentByte = 0;
		for(j=0;j<C_N_BITS;j++)
		{
			//HIGH
			digitalWrite(iTxPin, HIGH);
			serialNanoSleep(half_nanoPeriod);

			//LOW
			digitalWrite(iTxPin, LOW);
			currentbit = digitalRead(iRxPin);

			//FALLING => Reading
			currentByte |= currentbit<<j;

			serialNanoSleep(half_nanoPeriod);
		}
		receptionArray[i] = currentByte;
		std::cout << currentByte << "\t";
	}
	std::cout<<std::endl;

	digitalWrite(iTxPin, HIGH);

	return getSpeedFromCOM();
}


//TODO TEST this function
int getSpeedFromCOM()
{
	int i;
	unsigned char checksum = 0;
	checksum = C_START_BYTE;
	for(i = C_POS_FIRST_DATA ; i< C_POS_CHECKSUM; i++)
	{
		checksum += receptionArray[i];
		//ChecksumCalculation
	}

	checksum += C_STOP_BYTE;

	if(checksum == receptionArray[C_POS_CHECKSUM])
	{
		//TODO: add speed calculation
		return receptionArray[C_POS_FIRST_DATA];
	}
	else return -1;

}

void serialNanoSleep(int nanoSec)
{
	struct timespec deadline;
	clock_gettime(CLOCK_MONOTONIC, &deadline);
	deadline.tv_nsec += nanoSec;

	if( deadline.tv_nsec >= 1000000000)
	{
		deadline.tv_nsec -= 1000000000;
		deadline.tv_sec++;
	}

	clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
}



