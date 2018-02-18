#include "CppUTest/TestHarness.h"

#include "can_bus_shield/mcp_can.h"
//#include <iostream>

extern "C"
{
#include "arduinoGpioSpy.h"
#include "MockSPI.h"
}

#include <cstring>

// For constructor 2
#define CHIPSELECT_UNTOUCHED -1
static int chipSelect;
static void CSCallback(uint8_t val)
{
	//std::cout << "CSCallback: " << (int)val << std::endl;
	chipSelect = val;
}

TEST_GROUP(McpCan)
{
	SPIClass* spiSpy;
	uint8_t SPI_CS; // For constructor 1
	MCP_CAN *mcp;
	void setup()
	{
		SPI_CS = 12;
		chipSelect = CHIPSELECT_UNTOUCHED;
		SPI_TypeDef SPIx;
		spiSpy = new SPIClass(&SPIx);
		ArduinoGpioSpy_Create(SPI_CS);
		MockSPI_Create(100);
		mcp = new MCP_CAN(*spiSpy, CSCallback);
	}

	void teardown()
	{
		delete mcp;
		ArduinoGpioSpy_Destroy();
		delete spiSpy;
		MockSPI_Destroy();
	}


	void Expect_SetRegister(uint8_t reg, uint8_t val)
	{
		uint8_t wrBuf[3];
		uint8_t rdBuf[3];
		memset(rdBuf, 0, 3);

		wrBuf[0] = MCP_WRITE;
		wrBuf[1] = reg;
		wrBuf[2] = val;

		MockSPI_Expect_Write_Read(wrBuf, rdBuf, 3);
	}

	void Expect_ReadRegister(uint8_t reg, uint8_t val)
	{
		uint8_t wrBuf[3];
		uint8_t rdBuf[3];
		memset(rdBuf, 0, 3);

		wrBuf[0] = MCP_READ;
		wrBuf[1] = reg;
		wrBuf[2] = 0xFF;
		rdBuf[2] = val;

		MockSPI_Expect_Write_Read(wrBuf, rdBuf, 3);
	}

	void Expect_ModifyRegister(uint8_t reg, uint8_t mask, uint8_t val)
	{
		uint8_t wrBuf[4];
		uint8_t rdBuf[4];
		memset(rdBuf, 0, 4);
		wrBuf[0] = MCP_BITMOD;
		wrBuf[1] = reg;
		wrBuf[2] = mask;
		wrBuf[3] = val;

		MockSPI_Expect_Write_Read(wrBuf, rdBuf, 4);
	}

	void Expect_ReadStatus(uint8_t status)
	{
		uint8_t wrBuf[2];
		uint8_t rdBuf[2];

		wrBuf[0] = MCP_READ_STATUS;
		wrBuf[1] = 0xFF;

		rdBuf[0] = 0x00;
		rdBuf[1] = status;
		MockSPI_Expect_Write_Read(wrBuf, rdBuf, 2);
	}

	void Expect_ReadRegisterS(uint8_t address, uint8_t len, uint8_t* buf)
	{
		uint8_t wrBuf[2+len];
		uint8_t rdBuf[2+len];
		memset(wrBuf, 0xFF, sizeof(wrBuf));
		memset(rdBuf, 0x00, sizeof(rdBuf));

		wrBuf[0] = MCP_READ;
		wrBuf[1] = address;

		for(size_t i = 0; i <len; i++) {
			rdBuf[2+i] = buf[i];
		}

		MockSPI_Expect_Write_Read(wrBuf, rdBuf, (uint8_t)(2+len));
	}

	void Expect_ReadId(uint8_t address)
	{
		uint8_t wrBuf[6];
		memset(wrBuf, 0xFF, 6);
		wrBuf[0] = MCP_READ;
		wrBuf[1] = address;
		uint8_t rdBuf[6];
		rdBuf[0] = 0xFF;
		rdBuf[1] = 0xFF;
		rdBuf[2] = 0xEF;
		rdBuf[3] = 0x90;
		rdBuf[4] = 0xCD;
		rdBuf[5] = 0xAB;

		MockSPI_Expect_Write_Read(wrBuf, rdBuf, 6);
	}

	void Expect_WriteRegisterS(uint8_t address, uint8_t len, uint8_t* buf)
	{
		uint8_t wrBuf[2+len];
		uint8_t rdBuf[2+len];

		wrBuf[0] = MCP_WRITE;
		wrBuf[1] = address;
		memcpy(&wrBuf[2], buf, len);

		memset(rdBuf, 0, 2+len);
		MockSPI_Expect_Write_Read(wrBuf, rdBuf, (uint8_t)(len+2));
	}
};

IGNORE_TEST(McpCan, Construct_1)
{
	//Constructor 1 core dumps due uninitialized CS function
	MCP_CAN mcp(*spiSpy, SPI_CS);
	CHECK_EQUAL(OUTPUT, ArduinoGpioSpy_GetDirection());
	CHECK_EQUAL(true, ArduinoGpioSpy_isAccessed());
	CHECK_EQUAL(DIG_STATE_LOW, ArduinoGpioSpy_GetState());

}

TEST(McpCan, Construct_2)
{
	CHECK_EQUAL(CHIPSELECT_UNTOUCHED, chipSelect);
}

IGNORE_TEST(McpCan, TestBeginIllegalBitrate)
{
#define ILLEGAL_BITRATE 0
	uint8_t res = mcp->begin(ILLEGAL_BITRATE, MCP_8MHz);
	CHECK_EQUAL(CAN_FAILINIT, res);
}


IGNORE_TEST(McpCan, TestBeginIllegalFrequence)
{
#define ILLEGAL_FREQ 0
	uint8_t res = mcp->begin(CAN_250KBPS, ILLEGAL_FREQ);
	CHECK_EQUAL(CAN_FAILINIT, res);
}

TEST(McpCan, TestBegin)
{
	uint8_t wrBuf[10];
	uint8_t rdBuf[10];
	memset(rdBuf, 0, 10);
	// Reset
	wrBuf[0] = MCP_RESET;
	MockSPI_Expect_Write_Read(wrBuf, rdBuf, 1);

	Expect_ModifyRegister(MCP_CANCTRL, MODE_MASK, MODE_CONFIG);

	Expect_ReadRegister(MCP_CANCTRL, MODE_CONFIG);

	Expect_SetRegister(MCP_CNF1, MCP_8MHz_250kBPS_CFG1);
	Expect_SetRegister(MCP_CNF2, MCP_8MHz_250kBPS_CFG2);
	Expect_SetRegister(MCP_CNF3, MCP_8MHz_250kBPS_CFG3);

	for (unsigned i = 0; i < 14; i++) {
		Expect_SetRegister(uint8_t(MCP_TXB0CTRL + i), 0);
		Expect_SetRegister(uint8_t(MCP_TXB1CTRL + i), 0);
		Expect_SetRegister(uint8_t(MCP_TXB2CTRL + i), 0);
	}
	Expect_SetRegister(MCP_RXB0CTRL, 0);
	Expect_SetRegister(MCP_RXB1CTRL, 0);

	Expect_ModifyRegister(MCP_CANINTE, MCP_RX0IF, 0xFF);
	Expect_ModifyRegister(MCP_RXB0CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_ANY);
	Expect_ModifyRegister(MCP_CANCTRL, MODE_MASK, MODE_NORMAL);

	Expect_ReadRegister(MCP_CANCTRL, MODE_NORMAL);

	uint8_t res = mcp->begin(CAN_250KBPS, MCP_8MHz);
	CHECK_EQUAL(CAN_OK, res);
}

TEST(McpCan, ReadStatusData)
{
	Expect_ReadStatus(0xAA);
	uint8_t status = mcp->mcp2515_readStatus();

	CHECK_EQUAL(status, 0xAA);
}


TEST(McpCan, ReadMessage3Bytes)
{

	uint8_t expBuf[3];
	expBuf[0] = 0x12;
	expBuf[1] = 0x34;
	expBuf[2] = 0x56;
	uint8_t rdBuf[3];
	uint8_t len = 0;


	Expect_ReadStatus(MCP_STAT_RX0IF); // Buf 0
	Expect_ReadId(MCP_RXBUF_0);
	Expect_ReadRegister(MCP_RXBUF_0-1, 0x08);
	Expect_ReadRegister(MCP_RXBUF_0+4, 3);

	Expect_ReadRegisterS(MCP_RXBUF_0+5, 3, expBuf);
	Expect_ModifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
	mcp->readMsgBuf(&len, rdBuf);

	CHECK_EQUAL(len, 3);
	MEMCMP_EQUAL(rdBuf, expBuf, 3);
}

TEST(McpCan, WriteMessage3BytesDoWait)
{
	uint8_t wrBuf[4];
	uint8_t dataBuf[3];
	dataBuf[0] = 0x23;
	dataBuf[1] = 0x45;
	dataBuf[2] = 0x67;

	Expect_ReadRegister(MCP_TXB0CTRL, ~MCP_TXB_TXREQ_M); // Find free buf in first attempt;

	Expect_WriteRegisterS(MCP_TXB0CTRL+1 +5, 3, dataBuf);
	Expect_SetRegister(MCP_TXB0CTRL+1 +4, 3 | 0);

	uint32_t canid = 0x1234;

	wrBuf[0] = (uint8_t)(canid >> 3);
	wrBuf[1] = (uint8_t)((canid & 0x07) << 5);
	wrBuf[2] = 0;
	wrBuf[3] = 0;
	Expect_WriteRegisterS(MCP_TXB0CTRL+1, 4, wrBuf);

	Expect_ModifyRegister(MCP_TXB0CTRL+1-1, MCP_TXB_TXREQ_M,MCP_TXB_TXREQ_M);

	// Simulate immediately sent
	Expect_ReadRegister(MCP_TXB0CTRL, ~MCP_TXB_TXREQ_M);

	mcp->sendMsgBuf(canid, 0, 3, dataBuf, true);
}
