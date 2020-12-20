/**============================================================================
 * @file MCC_PID.hpp
 * @brief
 * @details
 *
 * - Company			: HE2B - ISIB
 * - Project			: progra STR : MCC asservissement
 * - Authors			: Bultot Geoffrey, Ishimaru Geoffrey
 *   Copyright			: All right reserved
 *   Description		: Header of MCC_PID.cpp
 *=============================================================================*/

#ifndef _MCC_PID_H_
#define _MCC_PID_H_

/***************************************************************************
* Includes Directives
***************************************************************************/

/***************************************************************************
* Constant declarations
***************************************************************************/


//#define LOG_MEASURE

/***************************************************************************
* Type definitions
***************************************************************************/

typedef enum{
	E_SENS_DEFAULT,
	E_SENS_HORAIRE,
	E_SENS_ANTI_HORAIRE
}T_ROTATION_SENS;

/***************************************************************************
* Variables declarations
***************************************************************************/

/***************************************************************************
* Functions declarations
***************************************************************************/

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


#endif //_MCC_PID_H_
