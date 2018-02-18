

#include "MockSPI.h"
#include "CppUTest/TestHarness_c.h"
#include <string.h>
#include <stdio.h>
#include <memory.h>

struct MockExpectation {
	int expectIn;
	int expectOut;			// Out contains tx/wr, In contains rx/rd
	uint8_t inBuf[255];
	uint8_t outBuf[255];
	unsigned int len;
};

static struct MockExpectation* expectations;
static unsigned int setExpectCnt;
static unsigned int getExpectCnt;
static unsigned int expectMax;
static unsigned int failureReported;

void MockSPI_Create(unsigned int maxExpectations)
{
	expectMax= maxExpectations;
	expectations = (struct MockExpectation*)malloc((unsigned int)(maxExpectations*sizeof(struct MockExpectation)));
	setExpectCnt = 0;
	getExpectCnt = 0;
	failureReported = 0;
}

void MockSPI_Destroy(void)
{
	free (expectations);
}

static void fail(const char* message)
{
	if (!failureReported)
	{
		FAIL_TEXT_C(message);
	}
	failureReported++;;
}

static void failWhenNotInitialized()
{
	if (0 == expectMax)
		fail("MockSPI not initialized");
}

static void failWhenNoRoomForExpectation()
{
	failWhenNotInitialized();
	if (setExpectCnt == expectMax)
		fail("No more MockSPI expectations available");
}

static void failWhenNoUnusedExpectations(const char* caller, int len)
{
	if (getExpectCnt >= setExpectCnt) {
		char format[] = "To few expectations (%d): %s(len %d)";
		char message[sizeof(format) + 5 +5 + strlen(caller)]; // Make room for format expansions
		snprintf(message, sizeof(message), format, setExpectCnt, caller, len);
		fail (message);
	}
}

static void failWhenExpectationLenMismatch(int len)
{
	if (expectations[getExpectCnt].len != len) {
		char format[] = "Length mismatch. Expected %d. Got %d (Expect. %d of %d)";
		char message[sizeof(format) + 5 + 5 + 5 + 5]; // MAke room for %d expansions
		snprintf(message, sizeof(message), format,
				expectations[getExpectCnt].len, len, getExpectCnt+1, setExpectCnt);
		fail (message);
	}
}

static void uint8AsHex(uint8_t val, char* str)
{
	sprintf(str, "0x%02x ", val);
}

static void failWhenExpectationTxMismatch(uint8_t* tx)
{
	size_t len = expectations[getExpectCnt].len;
	if (memcmp(tx, expectations[getExpectCnt].outBuf, len)) {
		char exp[len*5];
		char got[len*5];
		exp[0] = '\0';
		got[0] = '\0';
		char byteStr[10];
		for (size_t i = 0; i < len; i++) {
			uint8AsHex(tx[i], byteStr);
			strncat(got, byteStr, sizeof(got));
			uint8AsHex(expectations[getExpectCnt].outBuf[i], byteStr);
			strncat(exp, byteStr, sizeof(exp));
		}
		char format[] = "Mismatch in TX buffer, Expect #%d\n\tExp: %s\n\tGot: %s\n";
		char message[sizeof(format) + 5 + strlen(exp) + strlen(got)]; // Make room for %d expansion
		snprintf(message, sizeof(message), format, getExpectCnt+1, exp, got);
		fail (message);
	}
}

static void failWhenRxExpectation()
{
	if (0 != expectations[getExpectCnt].expectIn) {
		fail ("Expected a wr transaction");
	}
}

static void failWhenNoRxExpectation()
{
	if (0 == expectations[getExpectCnt].expectIn) {
		fail ("Expected a wr-rd transaction");
	}
}

static void recordExpectation(int wr, uint8_t* wrbuf, int rd, uint8_t* rdbuf, uint8_t len)
{
	struct MockExpectation* expectation = & expectations[setExpectCnt];
	setExpectCnt++;

	expectation->expectOut = 0;
	expectation->expectIn = 0;
	expectation->len = len;

	if (wr) {
		expectation->expectOut = 1;
		memcpy(expectation->outBuf, wrbuf, len);
	}
	if (rd) {
		expectation->expectIn = 1;
		memcpy(expectation->inBuf, rdbuf, len);
	}
}

void MockSPI_Expect_Write(uint8_t* buf, uint8_t len)
{
	failWhenNoRoomForExpectation();

	recordExpectation(1, buf, 0, 0, len);
}

void MockSPI_Expect_Write_Read(uint8_t* wrbuf, uint8_t* rdbuf, uint8_t len)
{
	failWhenNoRoomForExpectation();

	recordExpectation(1, wrbuf, 1, rdbuf, len);
}



void MockSPI_VerifyComplete(void)
{
	if (setExpectCnt == getExpectCnt)
		return;

	char format[] = "Expected %d transfers but got %d";
	char message[sizeof(format) + 5 + 5]; // Make room for %d expansions
	snprintf(message, sizeof(message), format, setExpectCnt, getExpectCnt);
	fail(message);
}



/****************************************************************/
// Functions to be called by the SPI class


void MockSPI_TransferWrRd(uint8_t* tx, uint8_t* rx, uint8_t len)
{
	failWhenNoUnusedExpectations("TransferWrRd", len);
	failWhenExpectationLenMismatch(len);
	failWhenExpectationTxMismatch(tx);
	failWhenNoRxExpectation();

	// Setup return value
	memcpy(rx, expectations[getExpectCnt].inBuf, len);
	getExpectCnt++;
}
void MockSPI_TransferWr(uint8_t* tx, uint8_t len)
{
	failWhenNoUnusedExpectations("TransferWr", len);
	failWhenExpectationLenMismatch(len);
	failWhenExpectationTxMismatch(tx);
	failWhenRxExpectation();

	getExpectCnt++;
}
