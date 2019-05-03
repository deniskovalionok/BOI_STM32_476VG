#include "timer.h"
#include "stm32l4xx_hal.h"
#include "exeptions.h"

extern uint16_t MKs_timCount;


namespace IMER
{ 
#define MAX_TIMERS 10
  
    cSuperTimer* timClasses[MAX_TIMERS];
  
  //~~~~~~~~~~~~~~~~~~~~~������������� ������~~~~~~~~~~~~~~~~~~~~~~~~

  cMS_timer::cMS_timer(tagTimer *timStruct//��������� �� ������ �������� ��������
                       , uint16_t timArrLen//��� ������
                         )    
  {
    if(timStruct == NULL || timArrLen == 0)
    {
      exception(__FILE__,__FUNCTION__,__LINE__,"invalid pointer");
      return;
    }
    tim = timStruct;
    this->timArrLen = timArrLen;
    for(uint16_t i = 0; i < timArrLen; i++)
      tim[i].isOn = false;
  }
  
 //�������� ������
  //timNum - ����� ������� � ������� (���� <0 �� ����� ������� ����� ��������� ������)
  //timeout - ����� ������ � ������������
  //���������� ����� ����������� �������. -1 - ������
  int cMS_timer::start(uint16_t timeout, int timNum)
  {
    if(!timeout || timNum >= timArrLen)
      return TIM_ERR;
    if(timNum < 0)
    {
        for(int i = 0; i < timArrLen; i++)
        {
          if(!tim[i].isOn)
          {
            timNum = i;
            break;
          }
        }
    }
    if(timNum >= 0)
    {
      if(tim[timNum].isOn)
        return TIM_ERR;
      tim[timNum].startTick = HAL_GetTick();
      tim[timNum].timeout = timeout;
      tim[timNum].isOn = true;
      return timNum;
    }
    else
        return TIM_ERR;
  }
  //�������� �������
  //timNum - ����� ������� � ������� (>0)
  //���������� 0 ���� ����� �� ������, >0 - ����� ������, <0 ������ 
  int cMS_timer::check(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen || !tim[timNum].isOn)
      return TIM_ERR;
    if((HAL_GetTick() - tim[timNum].startTick) >= tim[timNum].timeout)
      return (HAL_GetTick() - tim[timNum].startTick) - tim[timNum].timeout + 1;
    else
      return 0;
  }
  
  //��������� ������
  //timNum - ����� ������� � ������� (>0)
  //����� ����� ������ ������ ����� ���� ����������� ������� ���������
  //����� ��� ��� ������������ ������ ��� ���������� ���������. ����� ����� ������.
  //���� ���������� false - ������
  bool cMS_timer::stop(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen)
      return false;
    tim[timNum].isOn = false;
    return true;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~�������������� ������~~~~~~~~~~~~~~~~~~~~~~~~
  
  //�������� ������� ��� �������������� �������
  //timNum - ����� ������� � ������� (>0)
  //���������� 0 ���� ����� �� ������, >0 - ����� ������, <0 ������ 
  int cMKs_timer::check(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen || !tim[timNum].isOn)
      return TIM_ERR;
    if((MKs_timCount - tim[timNum].startTick) >= tim[timNum].timeout)
      return (MKs_timCount - tim[timNum].startTick) - tim[timNum].timeout + 1;
    else
      return 0;
  }

  //�������� �������������� ������
  //timNum - ����� ������� � ������� (���� <0 �� ����� ������� ����� ��������� ������)
  //timeout - ����� ������ � ������������
  //���������� ����� ����������� �������. -1 - ������
  int cMKs_timer::start(uint16_t timeout, int timNum)
  {
    if(!timeout || timNum >= timArrLen)
      return TIM_ERR;
    if(timNum < 0)
    {
        for(int i = 0; i < timArrLen; i++)
        {
          if(!tim[i].isOn)
          {
            timNum = i;
            break;
          }
        }
    }
    if(timNum >= 0)
    {
      if(tim[timNum].isOn)
        return TIM_ERR;
      tim[timNum].startTick = MKs_timCount;
      tim[timNum].timeout = timeout;
      tim[timNum].isOn = true;
      return timNum;
    }
    else
        return TIM_ERR;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~����� ������~~~~~~~~~~~~~~~~~~~~~~~~~
  
  cSuperTimer::cSuperTimer(tagSTimer *timStruct//��������� �� ������ �������� ��������
                       , uint16_t timArrLen//��� ������
                         )
  {
    if(timStruct == NULL || timArrLen == 0)
    {
      exception(__FILE__,__FUNCTION__,__LINE__,"invalid pointer");
      return;
    }
    tim = timStruct;
    this->timArrLen = timArrLen;
    for(uint16_t i = 0; i < timArrLen; i++)
      tim[i].isOn = false;
    bool findPlace = false;
    for(int i = 0; i < MAX_TIMERS; i++)
    {
      if(timClasses[i] == NULL)
      {
        timClasses[i] = this;
        findPlace = true;
        break;
      }
    }
    if(!findPlace)
      exception(__FILE__,__FUNCTION__,__LINE__,"To many timers!");
  }
  
  //�������� ������
  //timNum - ����� ������� � ������� (���� <0 �� ����� ������� ����� ��������� ������)
  //timeout - ����� ������ � ������������
  //���������� ����� ����������� �������. -1 - ������
  int cSuperTimer::start(uint16_t timeout, int timNum)
  {
    if(!timeout || timNum >= timArrLen)
      return TIM_ERR;
    if(timNum < 0)
    {
        for(int i = 0; i < timArrLen; i++)
        {
          if(!tim[i].isOn)
          {
            timNum = i;
            break;
          }
        }
    }
    if(timNum >= 0)
    {
      if(tim[timNum].isOn)
        return TIM_ERR;
      tim[timNum].counter = 0;
      tim[timNum].timeout = timeout;
      tim[timNum].isOn = true;
      tim[timNum].func = NULL;
      tim[timNum].isFinish = false;
      tim[timNum].refresh = false;
      return timNum;
    }
    else
        return TIM_ERR;
  }
  
  //�������� �������
  //timNum - ����� ������� � ������� (>0)
  //���������� 0 ���� ����� �� ������, >0 - ����� ������, <0 ������ 
  int cSuperTimer::check(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen || !tim[timNum].isOn)
      return TIM_ERR;
    if(tim[timNum].isFinish)
      return 1;
    else
      return 0;
  }
  
  //��������� ������
  //timNum - ����� ������� � ������� (>0)
  //����� ����� ������ ������ ����� ���� ����������� ������� ���������
  //����� ��� ��� ������������ ������ ��� ���������� ���������. ����� ����� ������.
  //���� ���������� false - ������
  bool cSuperTimer::stop(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen)
      return false;
    tim[timNum].isOn = false;
    return true;
  }
  
  //������ �������
  //timNum - ����� ������� � ������� (���� <0 �� ����� ������� ����� ��������� ������)
  //timeout - ����� ������ � ������������
  //func - ��������� �� ������� ����������� �� ��������� �������
  //���� true ������ ��������������� ����� ��������� �������
  //���������� ����� ����������� �������. -1 - ������
  int cSuperTimer::start(uint16_t timeout, int timNum, void (*func)(void), bool refresh)
  {
    int usTimNum = start(timeout, timNum);
    if(usTimNum >= 0)
    {
      tim[usTimNum].func = func;
      tim[usTimNum].refresh = refresh;
    }
    return usTimNum;
  }
  
  
  //���������� ����������
  //��� ���� ��������� � ���������� ����� ������ �������� �� �����
  void cSuperTimer::it(void)
  {
    for(uint16_t i = 0; i < timArrLen; i++)
    {
      if(tim[i].isOn && !tim[i].isFinish)
      {
        tim[i].counter++;
        if(tim[i].counter >= tim[i].timeout)
        {
          if(tim[i].func != NULL)
            tim[i].func();
          if(tim[i].refresh)
            tim[i].counter = 0;
          else
            tim[i].isFinish = true;
        }
      }
    }
  }
  
};
using namespace TIMER;

void mainTimTI(void)
{
  for(uint16_t i = 0; i < MAX_TIMERS; i++)
  {
    if(timClasses[i] != NULL)
      timClasses[i]->it();
  }
}