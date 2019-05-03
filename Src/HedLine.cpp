#include "HedLine.h"
#include "Mode_prototype.h"
#include <stdio.h>
#include "clock.h"

const char CUR_MONTH[40] = "nuljenfebmaraprmayjunjulaugsepoctnovdec";

uint8_t showDate = false;

tagRTC_time time;
    
uint8_t secLast = 0;

using namespace MODES;

void hedline_control(void)
{
      
        time = getTime();
        
        if(time.Seconds%10 == 0)
        {
          if(time.Seconds%20 == 0)
            showDate = true;
          else
            showDate = false;
        }
        
        if(time.Seconds != secLast)
        {
          secLast = time.Seconds;
          currentMode->show();
        }
}

void hedline_show(void)
{
        time = getTime();
  
        cmd( COLOR_RGB(0xff, 0xff, 0x00));
        uint8_t str[8];
        if(showDate)
        {
            sprintf((char*)str, "%02i   %02i", time.Date, time.Year);
            for(int i = 0; i < 3; i++)
              str[2+i] = CUR_MONTH[time.Month*3 + i];
            cmd_text(2, 0, 27, 0, str);
        }
        else
        {
            sprintf((char*)str, "%02i.%02i.%02i", time.Hours, time.Minutes, time.Seconds);
            cmd_text(2, 0, 27, 0, str);
        }
}