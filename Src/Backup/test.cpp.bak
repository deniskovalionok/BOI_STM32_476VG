#include "test.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include "Journal.h"

#include "exeptions.h"
//это класс для тестировани€ работы с модами

using namespace TIMER;

tagSTimer testTimStr;

cSuperTimer testTim(&testTimStr, 1);

extern uint16_t timCount;

namespace MODES
{
  //cMode_prototype* mode2;
  
  int clickCount = 0;
  
  tagMenu testMenu[21];
  
  cTest_mode* modeNow;
  
  
  void mTest(void)
  {
    modeNow->menuTest();
  }
  
  cTest_mode::cTest_mode(char * name)
  {
    InterfaceColor.red = 222;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 222;
    FontColor.blue = 0;
    strcpy((char*)leftButtonText,"keyT");
    strcpy((char*)rightButtonText, "modeT");
    strcpy((char*)enterButtonText, "menu");
    strcpy((char*)onButtonText, "on");
    strcpy((char*)modeName, name);
    for(int i = 0; i < 20; i++)
    {
      sprintf((char*)testMenu[i].name, "%i", i);
      testMenu[i].isShow = true;
    }
    strcpy((char*)testMenu[20].name, "Hide Show");
    testMenu[20].func = &mTest;
    testMenu[20].isShow = true;
  }
  
  void cTest_mode::menuTest(void)
  {
     for(int i = 0; i < 20; i++)
       menuShowHideLine(i, !menu[i].isShow);
  }
  
  void cTest_mode::userControl(void)
  {
    modeNow = this;
  }
  
  void cTest_mode::onShow(void)
  {
    cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
    uint8_t str[3];
    sprintf((char*)str, "%i", clickCount);
    cmd_text(120, 60, 30, 512, str);
    sprintf((char*)str, "%i", timCount);
    cmd_text(120, 90, 30, 512, str);
    sprintf((char*)str, "%i", sizeof(tagJournalLines));
    cmd_text(120, 120, 30, 512, str);
  }
  
  void cTest_mode::enterKeyAction(void)
  {
    openMenu(testMenu, 21);
  }
  
  void cTest_mode::rKeyAction(void)
  {
    change(mode2);
    mode2 = this;
  }
  
  void cTest_mode::lKeyAction(void)
  {
    clickCount++;
    show();
  }
  
  void cTest_mode::onKeyAction(void)
  {
    
  }
  
};