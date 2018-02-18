#include "CppUTest/TestHarness.h"

extern "C"
{
#include "arduinoGpioSpy.h"
}

TEST_GROUP(ArduinoGpioSpy)
{
	static const int testport = 11;
	void setup()
	{
		ArduinoGpioSpy_Create(testport);
	}

	void teardown()
	{
		ArduinoGpioSpy_Destroy();
	}
};

TEST(ArduinoGpioSpy, WrongPort)
{
	digitalWrite(10,HIGH);
	CHECK_EQUAL(true, ArduinoGpioSpy_isFailAccessed());
	CHECK_EQUAL(false, ArduinoGpioSpy_isAccessed());
}

TEST(ArduinoGpioSpy, RightPort)
{
	digitalWrite(testport,LOW);
	CHECK_EQUAL(true, ArduinoGpioSpy_isAccessed());
}

TEST(ArduinoGpioSpy, PortHigh )
{
	digitalWrite(testport, HIGH);
	CHECK_EQUAL(DIG_STATE_CHANGED_HIGH, ArduinoGpioSpy_GetState());
}

TEST(ArduinoGpioSpy, PortLow )
{
	digitalWrite(testport, LOW);
	CHECK_EQUAL(DIG_STATE_CHANGED_LOW, ArduinoGpioSpy_GetState());
}

TEST(ArduinoGpioSpy, PortHighLow )
{
	digitalWrite(testport, HIGH);
	digitalWrite(testport, LOW);
	CHECK_EQUAL(DIG_STATE_CHANGED_LOW, ArduinoGpioSpy_GetState());
}

TEST(ArduinoGpioSpy, PortLowHigh )
{
	digitalWrite(testport, LOW);
	digitalWrite(testport, HIGH);
	CHECK_EQUAL(DIG_STATE_CHANGED_HIGH, ArduinoGpioSpy_GetState());
}

TEST(ArduinoGpioSpy, PortDirectionWrongPin)
{
	pinMode(testport -1, OUTPUT);
	CHECK_EQUAL(true, ArduinoGpioSpy_isFailAccessed());
}

TEST(ArduinoGpioSpy, PortDirectionIn)
{
	pinMode(testport, INPUT);
	CHECK_EQUAL(INPUT, ArduinoGpioSpy_GetDirection());
}

TEST(ArduinoGpioSpy, PortDirectionOut)
{
	pinMode(testport, OUTPUT);
	CHECK_EQUAL(OUTPUT, ArduinoGpioSpy_GetDirection());
}
