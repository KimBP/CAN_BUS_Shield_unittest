#ifndef D_MCP2515BuildTime_H
#define D_MCP2515BuildTime_H

///////////////////////////////////////////////////////////////////////////////
//
//  MCP2515BuildTime is responsible for recording and reporting when
//  this project library was built
//
///////////////////////////////////////////////////////////////////////////////

class MCP2515BuildTime
  {
  public:
    explicit MCP2515BuildTime();
    virtual ~MCP2515BuildTime();
    
    const char* GetDateTime();

  private:
      
    const char* dateTime;

    MCP2515BuildTime(const MCP2515BuildTime&);
    MCP2515BuildTime& operator=(const MCP2515BuildTime&);

  };

#endif  // D_MCP2515BuildTime_H
