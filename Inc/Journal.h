#ifndef _JOURNAL_H
#define _JOURNAL_H

#ifdef __cplusplus

#include "HedLine.h"
#include "Mode_prototype.h"
#include "clock.h"
extern "C"
{
#endif  //#ifdef __cplusplus
#include "filesystem.h"
#ifdef __cplusplus
}

#pragma pack(1)

typedef struct{
tagRTC_time     datetime;
uint32_t        pose;
} tagJournalLines;

#pragma pack()

namespace MODES
{
  class cJournal_mode : public cMode_prototype
  {
  public:
    //explicit cJournal_mode(void);
  private:
    
    tagJournalLines jourLines;
    
    void userControl(void);
    
    void onShow(void);
    
    void enterKeyAction(void);
    void rKeyAction(void);
    void lKeyAction(void);
    void onKeyAction(void);
  };
};

namespace JOURNAL
{

  class cJournal
  {
  public:
    explicit cJournal(void);
    
    bool getLine(uint32_t lineNum, tagRTC_time* datetime, uint8_t* buff);
    
    void createLine(const uint8_t* line,const uint16_t len);
    
    bool getLine(uint8_t* buff, uint16_t buffLen, tagRTC_time* createTime, uint16_t num);
    
    uint32_t getNumOfLines(void);
    
  protected:
    
    HFILE loadFiles(void);
    
    void reWrite(void);
    
    uint32_t curJournalLen;
    
    uint32_t numOfLines;
    
  };
};

#endif  //#ifdef __cplusplus
#endif	//#ifndef _JOURNAL_H