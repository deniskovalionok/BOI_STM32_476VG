#ifndef _TIMER_PROTOTYPE_H
#define _TIMER_PROTOTYPE_H

#ifdef __cplusplus

namespace TIMER
{
  
#ifndef TIM_ERR
#define TIM_ERR (-1)
#endif  //#ifndef TIM_ERR
  
  class cMS_timer_prototype
  {
  public:
    virtual int start(uint16_t timeout, int timNum) = 0;
    virtual int check(int timNum) = 0;
    virtual bool stop(int timNum) = 0;
  };
};

#endif  //#ifdef __cplusplus
#endif	//#ifndef _TIMER_PROTOTYPE_H