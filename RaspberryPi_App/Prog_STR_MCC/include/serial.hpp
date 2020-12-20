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

//7812 NOP for 9600 bauds
#define SerialDelayNOP() nop_1000()nop_1000()nop_1000()nop_1000()nop_1000()nop_1000()nop_1000()nop_500()nop_200()nop_100()nop_10()asm("nop");asm("nop");
#define nop_1000() nop_500()nop_500()
#define nop_500() nop_200()nop_200()nop_100()
#define nop_200() nop_100()nop_100()
#define nop_100() nop_10()nop_10()nop_10()nop_10()nop_10()nop_10()nop_10()nop_10()nop_10()nop_10()
#define nop_10() asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

#endif //_SERIAL_H_
