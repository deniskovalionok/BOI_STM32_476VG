#include "keys.h"
#include <string.h>

using namespace TIMER;

//обработка нажатий с антидребезгом

namespace KEYBORD
{
  cKey::cKey(TIMER::cMS_timer_prototype* tim, uint16_t place)
  {
    this->tim = tim;
    click = false;
    preClick = false;
    drebTim = false;
    this->place = place;
    locked = true;
  }
  
  //возвращает true если было нажатие. При этом сбрасывает его, т.е. будет ожидаться следующее нажатие. 
  bool cKey::wasClicked(void)
  {
    bool tempVar = click;
    click = false;
    return tempVar;
  }
  
  
  // обработчик внешнего прерывания для кнопок
  void cKey::intControl(bool polarity)
  {
    if(!polarity)
    {
        if(tim->check(place) != 0)
        {
          if(!preClick)
          {
              tim->stop(place);
              tim->start(100, place);
              preClick = true;
              drebTim = true;
          }
          else
          {
            click = true;
            if(locked && !drebTim)
              click = false;
            tim->stop(place);
            tim->start(100, place);
            drebTim = false;
          }
        }
    }
    else
    {
      if(!drebTim)
      {
          tim->stop(place);
          tim->start(20, place);
          preClick = false;
          drebTim = true;
      }
    }
  }
  
  void cKey::keyLock(bool yN)
  {
    locked = yN;
  }
  
}