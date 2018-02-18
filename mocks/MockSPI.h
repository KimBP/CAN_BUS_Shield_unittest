#ifndef _MOCK_SPI_H_
#define _MOCK_SPI_H_

#include <stdint.h>


void MockSPI_Create(unsigned int maxExpectations);
void MockSPI_Destroy(void);

void MockSPI_Expect_Write(uint8_t* buf, uint8_t len);
void MockSPI_Expect_Write_Read(uint8_t* wrbuf, uint8_t* rdbuf, uint8_t len);


void MockSPI_VerifyComplete(void);

void MockSPI_TransferWrRd(uint8_t* tx, uint8_t* rx, uint8_t len);
void MockSPI_TransferWr(uint8_t* tx, uint8_t len);

#endif
