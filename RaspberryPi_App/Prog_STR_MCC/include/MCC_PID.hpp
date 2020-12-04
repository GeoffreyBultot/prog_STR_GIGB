#ifndef _MCC_PID_H_
#define _MCC_PID_H_

#define C_PERIOD 200

typedef enum{
	E_SENS_HORAIRE,
	E_SENS_ANTI_HORAIRE,
}T_ROTATION_SENS;

int initPID_Thread(int pin_MLI);
int stopPID_Regulation(void);
void setSensRotation(T_ROTATION_SENS sensRotation);
void setConsigne(int consigne);
#endif
