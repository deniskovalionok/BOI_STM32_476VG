#ifndef _KEYS_H
#define _KEYS_H

#include "timer.h"

#ifdef __cplusplus

namespace KEYBORD
{
  class cKey
  {
  public:
    explicit cKey(TIMER::cMS_timer_prototype* tim, uint16_t place);
    
    //возвращает true если было нажатие. При этом сбрасывает его, т.е. будет ожидаться следующее нажатие. 
    bool wasClicked(void);
    
    // обработчик внешнегот прерывания для кнопок
    void intControl(bool polarity);
    
    void keyLock(bool nY);
  protected:
    // произошло ли нажатие на кнопку
    bool click;
    
    bool preClick;
    
    bool locked;
    
    uint16_t place;
    
    bool drebTim;
    //тут таймер
    TIMER::cMS_timer_prototype* tim;
  };
}

#endif  //#ifdef __cplusplus
#endif	//#ifndef _KEYS_H