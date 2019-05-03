#include "Journal.h"
#include "exeptions.h"
#include <string.h>
#include <stdio.h>

using namespace JOURNAL;

cJournal globalJournal;

uint32_t mem = 0;

extern struct tagFileSystem filesystem;

namespace MODES
{
  void cJournal_mode::onShow(void)
  {
    mem = filesystem_calc_free_memory(&filesystem)/1024;
    uint32_t numOfLines = globalJournal.getNumOfLines();
    for(int i = 0; i < min(12,numOfLines); i++)
    {
      uint8_t buf[40];
      sprintf((char*)buf, "Device ID: %d", i);
      cmd_text(2, 80 + i*10, 17, 0, buf);
    }
  }
  
  void cJournal_mode::userControl(void)
  {
    
  }
  
  void cJournal_mode::enterKeyAction(void)
  {
    
  }
  
  void cJournal_mode::rKeyAction(void)
  {
    
  }
  
  void cJournal_mode::lKeyAction(void)
  {
    
  }
  
  void cJournal_mode::onKeyAction(void)
  {
    
  }
};

namespace JOURNAL
{
  cJournal::cJournal(void)
  {
    numOfLines = 0;
    curJournalLen = 0;
  }
  
  HFILE cJournal::loadFiles(void)
  {
    HFILE file;
    uint32_t len;
    if(curJournalLen == 0)
    {
        file = filesystem_open_file(&filesystem, "journal", "ini");
        if(file == NULL)
          reWrite();
        len = filesystem_get_length(&filesystem, file);
        if(len == 0)
          reWrite();
        curJournalLen = len;
    }
    
    file = filesystem_open_file(&filesystem, "journalHid", "ini");
    if(file == NULL)
    {
      reWrite();
      file = filesystem_open_file(&filesystem, "journalHid", "ini");
    }
    len = filesystem_get_length(&filesystem, file);
    if(len%10 != 0)
    {
      reWrite();
      file = filesystem_open_file(&filesystem, "journalHid", "ini");
    }
    numOfLines = len/10;
    return file;
  }
  
  void cJournal::reWrite(void)
  {
      HFILE file = filesystem_create_file(&filesystem, "journalHid", "ini", 1);
      numOfLines = 1;
      tagJournalLines lineProp;
      lineProp.datetime = getTime();
      lineProp.pose = 0;
      int position = 0;
      if(filesystem_file_put(&filesystem, file, &position,(uint8_t*) &lineProp, 10) != 10)
        exception(__FILE__,__FUNCTION__,__LINE__,"Can't write into journal!");
      file = filesystem_create_file(&filesystem, "journal", "ini", 1);
      curJournalLen = filesystem_file_put(&filesystem, file, 0, "Journal criated.", 16);
      if(curJournalLen != 16)
        exception(__FILE__,__FUNCTION__,__LINE__,"Can't create journal!");
  }
  
  void cJournal::createLine(const uint8_t* line,const uint16_t len)
  {
    HFILE file = loadFiles();
    tagJournalLines lineProp;
    lineProp.datetime = getTime();
    lineProp.pose = curJournalLen;
    int position = numOfLines*10;
    if(line == NULL)
      exception(__FILE__,__FUNCTION__,__LINE__,"Wrong argument!");
    if(filesystem_file_put(&filesystem, file, &position,(uint8_t*) &lineProp, 10) != 10)
        exception(__FILE__,__FUNCTION__,__LINE__,"Can't write into journal!");
    file = filesystem_open_file(&filesystem, "journal", "ini");
    position = curJournalLen;
    filesystem_file_put(&filesystem, file, &position, line, len);
    if(position == len + curJournalLen)
    {
      curJournalLen += position;
      numOfLines++;
    }
    else
      exception(__FILE__,__FUNCTION__,__LINE__,"Can't write into journal!");
  }
  
  bool cJournal::getLine(uint8_t* buff, uint16_t buffLen, tagRTC_time* createTime, uint16_t num)
  {
    HFILE file = loadFiles();
    tagJournalLines lineProp;
    if(num > numOfLines)
      return false;
    if(buff == NULL || createTime == NULL)
      exception(__FILE__,__FUNCTION__,__LINE__,"Wrong argument!");
    int position = num*10;
    if(filesystem_file_get(&filesystem, file, &position, (uint8_t*) &lineProp, 10) != num * 10 + 10)
      exception(__FILE__,__FUNCTION__,__LINE__,"Can't read journal!");
    memcpy(createTime, &lineProp.datetime, 6);
    file = filesystem_open_file(&filesystem, "journal", "ini");
    position = lineProp.pose;
    if(filesystem_get_string(&filesystem, file, &position, (char*)buff, buffLen) == E_FAIL)
      exception(__FILE__,__FUNCTION__,__LINE__,"Can't read journal!");
    return true;
  }
  
  uint32_t cJournal::getNumOfLines(void)
  {
    loadFiles();
    return numOfLines;
  }
  
};