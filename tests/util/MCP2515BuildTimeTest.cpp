#include "CppUTest/TestHarness.h"
#include "MCP2515BuildTime.h"

TEST_GROUP(MCP2515BuildTime)
{
  MCP2515BuildTime* projectBuildTime;

  void setup()
  {
    projectBuildTime = new MCP2515BuildTime();
  }
  void teardown()
  {
    delete projectBuildTime;
  }
};

TEST(MCP2515BuildTime, Create)
{
  CHECK(0 != projectBuildTime->GetDateTime());
}

