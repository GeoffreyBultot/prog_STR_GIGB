

#include "serial.hpp"

#include <iostream>
#include "unistd.h"
#include <wiringPi.h>

int iTxPin;
int iRxPin;
int half_period = 0;
unsigned char receptionArray[C_N_BYTES];

void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin)
{
	wiringPiSetupGpio();
	pinMode(iRxPin,OUTPUT);
	pinMode(iTxPin,INPUT);

	iRxPin = rxPin;
	iTxPin = txPin;
	half_period = (baudrate/C_N_BITS);
}

int readSpeed()
{
	int i;
	int j;
	int currentByte = 0;
	bool currentbit;
	//Low -> StartBit
	digitalWrite(iTxPin, LOW);
	//Waiting com from Nucleo
	while(digitalRead(iRxPin) == 1);
	for(i=0;i<C_N_BYTES;i++)
	{
		currentByte = 0;
		for(j=0;i<C_N_BITS;i++)
		{
			digitalWrite(iTxPin, HIGH);
			usleep(half_period * 1000);
			digitalWrite(iTxPin, LOW);
			currentbit = digitalRead(iRxPin);
			currentByte |= currentbit<<j;
			usleep(half_period * 1000);
		}
		receptionArray[i] = currentByte;
	}
	digitalWrite(iTxPin, HIGH);

	return(receptionArray[0]);
}
