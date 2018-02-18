extern "C"
{
#include "MockSPI.h"
}

#include "CppUTest/TestHarness.h"


TEST_GROUP(MockSPI)
{

	void setup()
	{
		MockSPI_Create(20);
	}

	void teardown()
	{
		MockSPI_Destroy();
	}
};

TEST(MockSPI, SunnyDayWr)
{
	uint8_t buf[] = {'a','b','c'};
	MockSPI_Expect_Write(buf, sizeof(buf));
	MockSPI_TransferWr(buf, sizeof(buf));
	MockSPI_VerifyComplete();
}

TEST(MockSPI, SunnyDayWrRd)
{
	uint8_t wrBuf[] = {'a','b','c'};
	uint8_t expRdBuf[] = {'1','2','3'};
	uint8_t rdBuf[3];
	MockSPI_Expect_Write_Read(wrBuf, expRdBuf, sizeof(wrBuf));
	MockSPI_TransferWrRd(wrBuf, rdBuf, sizeof(wrBuf));
	MockSPI_VerifyComplete();
	MEMCMP_EQUAL(rdBuf, expRdBuf, sizeof(rdBuf));
}

TEST(MockSPI, MoreExpectations)
{
	uint8_t wrBuf[] = {'a','b','c','d'};
	uint8_t expRdBuf[] = {'1', '2','3','4'};
	uint8_t rdBuf[4];

	MockSPI_Expect_Write_Read(wrBuf, expRdBuf, 4);
	MockSPI_Expect_Write_Read(wrBuf, expRdBuf, 2);

	MockSPI_TransferWrRd(wrBuf, rdBuf, 4);
	MEMCMP_EQUAL(rdBuf, expRdBuf, 4);
	MockSPI_TransferWrRd(wrBuf, rdBuf, 2);
	MEMCMP_EQUAL(rdBuf, expRdBuf, 2);
}
