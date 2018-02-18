#ifndef _ARDUINO_GPIO_SPY_H
#define _ARDUINO_GPIO_SPY_H



#include "Arduino.h"

enum {
	DIG_STATE_UNKNOWN,
	DIG_STATE_LOW,
	DIG_STATE_HIGH,
	DIG_STATE_CHANGED_HIGH,
	DIG_STATE_CHANGED_LOW
};

int ArduinoGpioSpy_Create(int port);
int ArduinoGpioSpy_Destroy(void);
int ArduinoGpioSpy_GetState(void);
int ArduinoGpioSpy_GetCount(void);
int ArduinoGpioSpy_isFailAccessed(void);
int ArduinoGpioSpy_isAccessed(void);
int ArduinoGpioSpy_GetDirection(void);
#endif
