#ifndef _SERIAL_H_
#define _SERIAL_H_

#define C_BAUDRATE 			(70)
#define C_N_BITS 			(8)
#define C_N_BYTES 			(2)

#define C_POS_START_BYTE 	(0)
#define C_POS_FIRST_DATA 	(1)
#define C_POS_CHECKSUM		(C_N_BYTES-2)
#define C_POS_STOP_BYTE 	(C_N_BYTES-1)


#define C_START_BYTE		(0xF4)
#define C_STOP_BYTE			(0xAA)

void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin);
/*blocking function*/
int readSpeed();

#endif //_SERIAL_H_
