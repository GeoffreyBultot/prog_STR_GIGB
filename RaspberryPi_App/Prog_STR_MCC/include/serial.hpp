#ifndef _SERIAL_H_
#define _SERIAL_H_

#define C_BAUDRATE 700
#define C_N_BITS 8
#define C_N_BYTES 4
/*blocking function*/

void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin);
int readSpeed();

#endif //_SERIAL_H_
