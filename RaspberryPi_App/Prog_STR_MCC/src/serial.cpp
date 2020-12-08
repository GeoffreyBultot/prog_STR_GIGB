

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
int readAngle(int* angle)
{
	unsigned int i;
	unsigned int j;
	int currentByte = 0;
	bool currentbit;

	int ErrorReg = 0;
	//TODO ADD FALLING EDGE DETECTION
	if(digitalRead(iRxPin) != HIGH)
	{
		//std::cout<<"[DEBUG] PAS DE NUCLEO DETECTE" <<std::endl;
		ErrorReg = 2; //ERROR : pas de nucleo detecte
	}
	else
	{
		//Low -> StartBit
		digitalWrite(iTxPin, LOW);
		//Waiting com from Nucleo

		int start_timeout = time(NULL);
		//TODO DIMINUER CE TEMPS
		while(digitalRead(iRxPin) != LOW){
			if(difftime(time(NULL), start_timeout) >= 1){
				//std::cout<<"[DEBUG] NUCLEO DETECTED BUT NOT RESPONDING"<<std::endl;
				ErrorReg = 3;
			}
		}

		if(ErrorReg == 0)
		{
			//std::cout << "Réception : \t"; //TODO JUST TO DEBUG
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
				//std::cout << currentByte << "\t"; //TODO JUST TO DEBUG
			}

			digitalWrite(iTxPin, HIGH);


				unsigned char checksum = 0;
				for(i = C_POS_FIRST_DATA ; i< C_POS_CHECKSUM; i++){
					checksum += receptionArray[i];
				}

				if(checksum == receptionArray[C_POS_CHECKSUM])
				{
					signed int temp_speed = *(   (int*)(&(receptionArray[C_POS_FIRST_DATA])));
					//std::cout<< " Position du moteur = " << temp_speed << std::endl; //TODO JUST TO DEBUG
					(*angle) = temp_speed;
					//ErrorReg = 0;
				}
				else{ErrorReg = 4;}
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



