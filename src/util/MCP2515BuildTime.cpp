#include "MCP2515BuildTime.h"

MCP2515BuildTime::MCP2515BuildTime()
: dateTime(__DATE__ " " __TIME__)
{
}

MCP2515BuildTime::~MCP2515BuildTime()
{
}

const char* MCP2515BuildTime::GetDateTime()
{
    return dateTime;
}

