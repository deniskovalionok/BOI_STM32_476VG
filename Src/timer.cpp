#include "timer.h"
#include "stm32l4xx_hal.h"
#include "exeptions.h"

extern uint16_t MKs_timCount;


namespace IMER
{ 
#define MAX_TIMERS 10
  
    cSuperTimer* timClasses[MAX_TIMERS];
  
  //~~~~~~~~~~~~~~~~~~~~~Милисекундный таймер~~~~~~~~~~~~~~~~~~~~~~~~

  cMS_timer::cMS_timer(tagTimer *timStruct//указатель на массив структур таймеров
                       , uint16_t timArrLen//его длинна
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
  
 //включает таймер
  //timNum - номер таймера в массиве (если <0 то будет запущен любой незанятый таймер)
  //timeout - время отщета в Милисикундах
  //возвращает номер запущенного таймера. -1 - ошибка
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
  //проверка таймера
  //timNum - номер таймера в массиве (>0)
  //возвращает 0 если время не прошло, >0 - время прошло, <0 ошибка 
  int cMS_timer::check(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen || !tim[timNum].isOn)
      return TIM_ERR;
    if((HAL_GetTick() - tim[timNum].startTick) >= tim[timNum].timeout)
      return (HAL_GetTick() - tim[timNum].startTick) - tim[timNum].timeout + 1;
    else
      return 0;
  }
  
  //выключает таймер
  //timNum - номер таймера в массиве (>0)
  //после этого метода таймер может быть использован другими функциями
  //ПЕРЕД тем как использовать таймер его необходимо выключить. иНАЧЕ БУДЕТ ОШИБКА.
  //если возвращает false - ошибка
  bool cMS_timer::stop(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen)
      return false;
    tim[timNum].isOn = false;
    return true;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~Микросекундный таймер~~~~~~~~~~~~~~~~~~~~~~~~
  
  //проверка таймера для микросикундого таймера
  //timNum - номер таймера в массиве (>0)
  //возвращает 0 если время не прошло, >0 - время прошло, <0 ошибка 
  int cMKs_timer::check(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen || !tim[timNum].isOn)
      return TIM_ERR;
    if((MKs_timCount - tim[timNum].startTick) >= tim[timNum].timeout)
      return (MKs_timCount - tim[timNum].startTick) - tim[timNum].timeout + 1;
    else
      return 0;
  }

  //включает микросекундный таймер
  //timNum - номер таймера в массиве (если <0 то будет запущен любой незанятый таймер)
  //timeout - время отщета в Милисикундах
  //возвращает номер запущенного таймера. -1 - ошибка
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
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~Супер таймер~~~~~~~~~~~~~~~~~~~~~~~~~
  
  cSuperTimer::cSuperTimer(tagSTimer *timStruct//указатель на массив структур таймеров
                       , uint16_t timArrLen//его длинна
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
  
  //включает таймер
  //timNum - номер таймера в массиве (если <0 то будет запущен любой незанятый таймер)
  //timeout - время отщета в Милисикундах
  //возвращает номер запущенного таймера. -1 - ошибка
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
  
  //проверка таймера
  //timNum - номер таймера в массиве (>0)
  //возвращает 0 если время не прошло, >0 - время прошло, <0 ошибка 
  int cSuperTimer::check(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen || !tim[timNum].isOn)
      return TIM_ERR;
    if(tim[timNum].isFinish)
      return 1;
    else
      return 0;
  }
  
  //выключает таймер
  //timNum - номер таймера в массиве (>0)
  //после этого метода таймер может быть использован другими функциями
  //ПЕРЕД тем как использовать таймер его необходимо выключить. иНАЧЕ БУДЕТ ОШИБКА.
  //если возвращает false - ошибка
  bool cSuperTimer::stop(int timNum)
  {
    if(timNum < 0 || timNum >= timArrLen)
      return false;
    tim[timNum].isOn = false;
    return true;
  }
  
  //запуск таймера
  //timNum - номер таймера в массиве (если <0 то будет запущен любой незанятый таймер)
  //timeout - время отщета в Милисикундах
  //func - указатель на функцию запускаемую по истечению времени
  //если true таймер перезапускается после окончания отсчета
  //возвращает номер запущенного таймера. -1 - ошибка
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
  
  
  //обработчик прерывания
  //его надо запихнуть в прерывание иначе ничего работать не будет
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