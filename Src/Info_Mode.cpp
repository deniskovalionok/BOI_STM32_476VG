#include "Info_Mode.h"
#include "filesystem.h"
#include <string.h>
#include <stdio.h>

namespace MODES
{
  
  cInfo_mode::cInfo_mode(void)
  {
    InterfaceColor.red = 255;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 255;
    FontColor.blue = 0;
    strcpy((char*)modeName, "BOI3");
  }
  
  void cInfo_mode::onShow(void)
  {
    cmd( COLOR_RGB(0, 255, 0));
    char buf[40];
    sprintf(buf, "Device ID: %d", infoRegs.Serial);
    cmd_text(5, 80, 28, 0, (uint8_t*)buf);
    sprintf(buf, "Issuse date: %02d.%04d", infoRegs.ManufacturedMonth, infoRegs.ManufacturedYear);
    cmd_text(5, 100, 28, 0, (uint8_t*)buf);
    sprintf(buf, "Software version: %.3f", infoRegs.softwareVer);
    cmd_text(5, 120, 28, 0, (uint8_t*)buf);
    sprintf(buf, "Hardware version: %.3f", infoRegs.hardwareVer);
    cmd_text(5, 140, 28, 0, (uint8_t*)buf);
    sprintf(buf, "Diagnostic reg: h%04x", infoRegs.errors);
    cmd_text(5, 160, 28, 0, (uint8_t*)buf);
  }
  
  void cInfo_mode::userControl(void)
  {
    
  }
  
  void cInfo_mode::enterKeyAction(void)
  {
    
  }
  
  void cInfo_mode::rKeyAction(void)
  {
    
  }
  
  void cInfo_mode::lKeyAction(void)
  {
    
  }
  
  void cInfo_mode::onKeyAction(void)
  {
    
  }
  
};

