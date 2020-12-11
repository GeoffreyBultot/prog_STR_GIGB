#ifndef _MCC_PID_H_
#define _MCC_PID_H_

typedef enum{
	E_SENS_DEFAULT,
	E_SENS_HORAIRE,
	E_SENS_ANTI_HORAIRE
}T_ROTATION_SENS;

/*Extern because variables used in communication.cpp*/
//extern int C;
//extern int i_measure;
//extern int MCC_Status;

//#define LOG_MEASURE

#ifdef LOG_MEASURE
int initPID_Thread(int pin_MLI, float Prop);
#else
int initPID_Thread(int pin_MLI);
#endif

void PowerRelayON();
void PowerRelayOFF();
void LogicRelayON();
void LogicRelayOFF();
int stopPID_Regulation(void);
void setSensRotation(T_ROTATION_SENS sensRotation);
void setConsigne(int consigne);

int getConsigne();
void setConsigne(int consigne);
int getMCCStatus();
void SetMCCStatusFlag(int flag, bool value);
int getMCCAngle();
void setMCCAngle(int angle);

float getPIDCommand();
void setPIDCommand(float command);

#endif
