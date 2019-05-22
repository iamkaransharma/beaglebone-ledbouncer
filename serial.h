// The serial module is a hardware abstraction module of the serial port

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

void Serial_init();
void Serial_setRxIsrCallback(void (*rxIsrCallback)(uint8_t));

#endif
