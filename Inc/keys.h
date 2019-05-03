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
    
    //���������� true ���� ���� �������. ��� ���� ���������� ���, �.�. ����� ��������� ��������� �������. 
    bool wasClicked(void);
    
    // ���������� ��������� ���������� ��� ������
    void intControl(bool polarity);
    
    void keyLock(bool nY);
  protected:
    // ��������� �� ������� �� ������
    bool click;
    
    bool preClick;
    
    bool locked;
    
    uint16_t place;
    
    bool drebTim;
    //��� ������
    TIMER::cMS_timer_prototype* tim;
  };
}

#endif  //#ifdef __cplusplus
#endif	//#ifndef _KEYS_H