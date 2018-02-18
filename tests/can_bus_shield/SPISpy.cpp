

#include "SPISpy.h"

extern "C" {
#include "MockSPI.h"
}

SPIClass::SPIClass(SPI_TypeDef* SPIx)
{

}

SPIClass::~SPIClass() {};

void SPIClass::begin()
{

}

void SPIClass::transfer(unsigned char* tx, unsigned char* rx, uint8_t len)
{
	MockSPI_TransferWrRd(tx, rx, len);
}

unsigned char SPIClass::transfer(unsigned char val)
{
	unsigned char retval;
	MockSPI_TransferWrRd(&val, &retval, 1);
	return val;
}
