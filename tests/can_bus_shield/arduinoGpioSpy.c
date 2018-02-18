#include "arduinoGpioSpy.h"
#include "assert.h"


static int failAccess;

static struct ArduinoGpioSpy_Port {
	int port;
	int count;
	int state;
	int accessed;
	int direction;
} portAccess;


int ArduinoGpioSpy_Create(int port)
{
	failAccess = 0;
	portAccess.port = port;
	portAccess.count = 0;
	portAccess.state = DIG_STATE_UNKNOWN;
	portAccess.accessed = 0;
	return 0;
}


int ArduinoGpioSpy_Destroy(void)
{
	return 0;
}

int ArduinoGpioSpy_GetState(void)
{
	return portAccess.state;
}

int ArduinoGpioSpy_GetCount(void)
{
	return portAccess.count;
}

int ArduinoGpioSpy_isFailAccessed(void)
{
	return failAccess;
}

int ArduinoGpioSpy_isAccessed(void)
{
	int isAccessed = portAccess.accessed;

	portAccess.accessed = 0;
	if (portAccess.state == DIG_STATE_CHANGED_LOW)
		portAccess.state = DIG_STATE_LOW;
	if (portAccess.state == DIG_STATE_CHANGED_HIGH)
		portAccess.state = DIG_STATE_HIGH;
	return isAccessed;
}

int ArduinoGpioSpy_GetDirection(void)
{
	return portAccess.direction;
}

/*****************************************************************/
// ArduinoGpio functions


void pinMode(uint8_t port, uint8_t direction)
{
	if (port != portAccess.port) {
		failAccess = 1;
		return;
	}
	portAccess.direction = direction;
}


void digitalWrite(uint8_t port, uint8_t val)
{
	if (port != portAccess.port)
	{
		failAccess = 1;
		return;
	}

	portAccess.count++;
	portAccess.accessed = 1;
	switch (portAccess.state) {
	case DIG_STATE_CHANGED_LOW:
	case DIG_STATE_LOW:
		if (val == HIGH) {
			portAccess.state = DIG_STATE_CHANGED_HIGH;
		}
		break;
	case DIG_STATE_CHANGED_HIGH:
	case DIG_STATE_HIGH:
		if (val == LOW) {
			portAccess.state = DIG_STATE_CHANGED_LOW;
		}
		break;
	case DIG_STATE_UNKNOWN:
		if (val == HIGH) {
			portAccess.state = DIG_STATE_CHANGED_HIGH;
		} else if (val == LOW){
			portAccess.state = DIG_STATE_CHANGED_LOW;
		} else {
			assert(0);
		}
		break;
	default: assert(0); break;
	}
}
