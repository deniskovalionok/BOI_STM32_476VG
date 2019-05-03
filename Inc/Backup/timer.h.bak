#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include "timer_prototype.h"

#ifdef __cplusplus

namespace TIMER
{
  //структура используемая для таймеров
  struct tagTimer{
    //время отщета
    uint16_t timeout;
    //значение при запуске
    uint16_t startTick;
    //используется ли таймер
    bool isOn;
  };
  
  
  
  class cMS_timer : public cMS_timer_prototype
  {
  public:
    explicit cMS_timer(tagTimer *timStruct//указатель на массив структур таймеров
                       , uint16_t timArrLen//его длинна
                         );
    //запуск таймера
    //timNum - номер таймера в массиве (если <0 то будет запущен любой незанятый таймер)
    //timeout - время отщета в Милисикундах
    //возвращает номер запущенного таймера. -1 - ошибка
    int start(uint16_t timeout, int timNum);
    
    //опрос состояния таймера
    //timNum - номер таймера в массиве (>0)
    //возвращает 0 если время не прошло, >0 - время прошло, <0 ошибка 
    int check(int timNum);
    
    //выключение таймера
    //timNum - номер таймера в массиве (>0)
    //после этого метода таймер может быть использован другими функциями
    //ПЕРЕД тем как использовать таймер его необходимо выключить. иНАЧЕ БУДЕТ ОШИБКА.
    //если возвращает false - ошибка
    bool stop(int timNum);
  protected:
    
    //указатель на массив структур таймеров
    tagTimer *tim;
    
    //его длинна
    uint16_t timArrLen;
  };
  
  class cMKs_timer : public cMS_timer
  {
  public:
    explicit cMKs_timer(tagTimer *timStruct,//указатель на массив структур таймеров
                        uint16_t timArrLen//его длинна
                          ) : cMS_timer(timStruct, timArrLen) {};
    //опрос состояния таймера
    //timNum - номер таймера в массиве (>0)
    //возвращает 0 если время не прошло, >0 - время прошло, <0 ошибка 
    int check(int timNum);
    
    //выключение таймера
    //timNum - номер таймера в массиве (>0)
    //после этого метода таймер может быть использован другими функциями
    //ПЕРЕД тем как использовать таймер его необходимо выключить. иНАЧЕ БУДЕТ ОШИБКА.
    //если возвращает false - ошибка
    int start(uint16_t timeout, int timNum);
  };
  
  //структура используемая для супер таймеров
  struct tagSTimer{
    //время отщета
    uint16_t timeout;
    //счетчик тиков
    uint16_t counter;
    //используется ли таймер
    bool isOn;
    //указатель на функцию запускаемую по истечению времени
    void (*func)(void);
    //таймер закончил отсчет
    bool isFinish;
    //если true таймер перезапускается
    bool refresh;
  };
  
  class cSuperTimer : public cMS_timer_prototype
  {
    public:
      explicit cSuperTimer(tagSTimer *timStruct//указатель на массив структур таймеров
                       , uint16_t timArrLen//его длинна
                         );
      
    //запуск таймера
    //timNum - номер таймера в массиве (если <0 то будет запущен любой незанятый таймер)
    //timeout - время отщета в Милисикундах
    //возвращает номер запущенного таймера. -1 - ошибка
    int start(uint16_t timeout, int timNum);
    
    //запуск таймера
  //timNum - номер таймера в массиве (если <0 то будет запущен любой незанятый таймер)
  //timeout - время отщета в Милисикундах
  //func - указатель на функцию запускаемую по истечению времени
  //если true таймер перезапускается после окончания отсчета
  //возвращает номер запущенного таймера. -1 - ошибка
    int start(uint16_t timeout, int timNum, void (*func)(void), bool refresh);
    
    //опрос состояния таймера
    //timNum - номер таймера в массиве (>0)
    //возвращает 0 если время не прошло, >0 - время прошло, <0 ошибка 
    int check(int timNum);
    
    //выключение таймера
    //timNum - номер таймера в массиве (>0)
    //после этого метода таймер может быть использован другими функциями
    //ПЕРЕД тем как использовать таймер его необходимо выключить. иНАЧЕ БУДЕТ ОШИБКА.
    //если возвращает false - ошибка
    bool stop(int timNum);
    
    //обработчик прерывания
    //его надо запихнуть в прерывание иначе ничего работать не будет
    void it(void);
      
    protected:
      //указатель на массив структур таймеров
      tagSTimer *tim;
      
      //его длинна
      uint16_t timArrLen;
  };
};

extern "C"
{
#endif  //#ifdef __cplusplus
  
    void mainTimTI(void);
    
#ifdef __cplusplus
}
#endif  //#ifdef __cplusplus
#endif	//#ifndef _TIMER_H