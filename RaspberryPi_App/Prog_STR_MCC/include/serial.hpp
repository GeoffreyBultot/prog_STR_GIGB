#ifndef _SERIAL_H_
#define _SERIAL_H_

#define C_BAUDRATE 70
#define C_N_BITS 8
#define C_N_BYTES 2

#define C_TX_PIN 17
#define C_RX_PIN 22

/*blocking function*/
void initSerial(int baudrate, unsigned char rxPin, unsigned char txPin);
int readSpeed();

#endif //_SERIAL_H_
