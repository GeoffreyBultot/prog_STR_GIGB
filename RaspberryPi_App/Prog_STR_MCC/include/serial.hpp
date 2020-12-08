#ifndef _SERIAL_H_
#define _SERIAL_H_

#define C_N_BITS 			(8)
#define C_N_BYTES_DATA 		(sizeof(int))
#define C_N_BYTES_TOT		(C_N_BYTES_DATA+1)	// (4) de data + 1 byte checksum = 5

#define C_POS_FIRST_DATA 	(0)
#define C_POS_CHECKSUM		(C_N_BYTES_TOT-1)

#define C_START_BYTE		(0xF4)
#define C_STOP_BYTE			(0xAA)

void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin);
/*blocking function*/
int readAngle(int*);

#endif //_SERIAL_H_
