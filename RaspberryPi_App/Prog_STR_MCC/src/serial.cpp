

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
	half_period = (baudrate/C_N_BITS);

	pinMode(C_RX_PIN,INPUT);
	//pullUpDnControl(C_RX_PIN, PUD_UP);
	//pullUpDnControl(C_TX_PIN, PUD_UP);

	pinMode(C_TX_PIN,OUTPUT);
	iRxPin = rxPin;
	iTxPin = txPin;
	half_period = (baudrate/C_N_BITS);

	std::cout<<"RX: "<<iRxPin << "   TX : " << iTxPin << std::endl;
	digitalWrite(C_TX_PIN, HIGH);
}

int readSpeed()
{
	int i;
	int j;
	int currentByte = 0;
	bool currentbit;
	//Low -> StartBit
	digitalWrite(C_TX_PIN, LOW);
	//Waiting com from Nucle
	usleep(1200);
	while(digitalRead(C_RX_PIN) != LOW);
	for(i=0;i<C_N_BYTES;i++)
	{
		currentByte = 0;
		for(j=0;j<C_N_BITS;j++)
		{
			//HIGH
			digitalWrite(C_TX_PIN, HIGH);
			usleep(25000);
			//usleep(half_period * 1000);
			//LOW
			digitalWrite(C_TX_PIN, LOW);
			currentbit = digitalRead(C_RX_PIN);
			//FALLING => Reading
			currentByte |= currentbit<<j;
			usleep(25000);
			//usleep(half_period * 1000);

		}
		receptionArray[i] = currentByte;
		std::cout<<"byte numéro " << i << ": " << currentByte<<std::endl;
	}

	digitalWrite(C_TX_PIN, HIGH);

	return(receptionArray[0]);
}
