#include <stdint.h>

#ifndef _ARDUINO_H_
#define _ARDUINO_H_


#define INT8U uint8_t
#define PROGMEM

#define LOW 12
#define HIGH 18

#define INPUT 14
#define OUTPUT 17

#ifdef __cplusplus
extern "C"
{
#endif

void pinMode(uint8_t port, uint8_t direction);
void digitalWrite(uint8_t port, uint8_t val);

#ifdef __cplusplus
}
#endif

#define delay(x) {}
#define delayMicroseconds(x) {}

#endif
