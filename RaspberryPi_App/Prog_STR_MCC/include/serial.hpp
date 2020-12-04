#ifndef _SERIAL_H_
#define _SERIAL_H_

#define C_N_BITS 			(8)
#define C_N_BYTES_DATA 		(2)
#define C_N_BYTES_TOT		(C_N_BYTES_DATA+5)	// 1 byte start + (2) de data + 1 byte d'erreur + 1 byte checksum + 1 byte stop = 7

#define C_POS_START_BYTE 	(0)
#define C_POS_FIRST_DATA 	(1)
#define C_POS_CHECKSUM		(C_N_BYTES_TOT-2)
#define C_POS_STOP_BYTE 	(C_N_BYTES_TOT-1)

#define C_START_BYTE		(0xF4)
#define C_STOP_BYTE			(0xAA)

void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin);
/*blocking function*/
int readSpeed(float*);

#endif //_SERIAL_H_
