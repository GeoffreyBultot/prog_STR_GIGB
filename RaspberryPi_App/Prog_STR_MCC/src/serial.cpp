

#include "serial.hpp"

#include <iostream>
#include "unistd.h"
#include <wiringPi.h>
#include <math.h>
#include "MODULES_DEFINE.hpp"

int iTxPin;
int iRxPin;

int half_nanoPeriod = 0;
unsigned char receptionArray[C_N_BYTES_TOT];
int getSpeedFromCOM();
void serialNanoSleep(int nanoSec);

void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin)
{
	half_nanoPeriod = (int) (1000000000.0/( (float)baudrate * (float)C_N_BITS * 2.0));
	std::cout << "[DEBUG] Half_nanoPeriod : " << half_nanoPeriod << std::endl;
	iRxPin = rxPin;
	iTxPin = txPin;

	pinMode(iRxPin,INPUT);
	pinMode(iTxPin,OUTPUT);

	digitalWrite(iTxPin, HIGH);
	//usleep(1000000);
}

/*
 * @brief function
 * return register of occured errors
 * params */
int readAngle(float* speed)
{
	unsigned int i;
	unsigned int j;
	int currentByte = 0;
	bool currentbit;

	int ErrorReg = 0;
	//TODO ADD FALLING EDGE DETECTION
	if(digitalRead(iRxPin) != HIGH)
	{
		std::cout<<"[DEBUG] PAS DE NUCLEO DETECTE" <<std::endl;
		ErrorReg = 2; //ERROR : pas de nucleo detecte
	}
	else
	{
		//Low -> StartBit
		digitalWrite(iTxPin, LOW);
		//Waiting com from Nucleo

		int start_timeout = time(NULL);

		while(digitalRead(iRxPin) != LOW){
			if(difftime(time(NULL), start_timeout) >= 1){
				std::cout<<"[DEBUG] NUCLEO DETECTED BUT NOT RESPONDING"<<std::endl;
				ErrorReg = 3;
			}
		}

		if(ErrorReg == 0)
		{
			std::cout << "Réception : \t"; //TODO JUST TO DEBUG
			for(i=0;i<C_N_BYTES_TOT;i++){
				currentByte = 0;
				for(j=0;j<C_N_BITS;j++){
					//HIGH
					digitalWrite(iTxPin, HIGH);
					serialNanoSleep(half_nanoPeriod);
					//LOW
					digitalWrite(iTxPin, LOW);
					serialNanoSleep(half_nanoPeriod);
					currentbit = digitalRead(iRxPin);
					//FALLING => Reading
					currentByte |= currentbit<<j;
				}
				receptionArray[i] = currentByte;
				std::cout << currentByte << "\t"; //TODO JUST TO DEBUG
			}
			int display_speed = 0;
			for(int a = 0; a < C_N_BYTES_DATA; a++){
				display_speed += receptionArray[C_POS_FIRST_DATA+a]*pow(256,a);
			}
			std::cout<< " Position du moteur = " << display_speed << std::endl; //TODO JUST TO DEBUG

			digitalWrite(iTxPin, HIGH);


			if( (receptionArray[C_POS_START_BYTE] == C_START_BYTE) && (receptionArray[C_POS_STOP_BYTE] == C_STOP_BYTE) )
			{
				unsigned char checksum = 0;
				checksum = C_START_BYTE;
				for(i = C_POS_FIRST_DATA ; i< C_POS_CHECKSUM; i++){
					checksum += receptionArray[i];
				}
				checksum += C_STOP_BYTE;

				if(checksum == receptionArray[C_POS_CHECKSUM])
				{
					//TODO: add speed calculation
					int temp_speed = 0;
					for(j = 0 ; j < C_N_BYTES_DATA ; j ++)
					{
						temp_speed += receptionArray[j+C_POS_FIRST_DATA] << 8*j; //Mise des bytes dans un int (à valider si on fait comme ça)
					}
					(*speed) = ((float)temp_speed) * C_CONSTANT_SPEED_CALCULATION;

				}
				else{ErrorReg = 4;}
			}
			else{ErrorReg = 5;}
			//return getSpeedFromCOM();
		}
	}
	return ErrorReg;
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



