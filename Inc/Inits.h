#ifndef _INITS_H
#define _INITS_H

#ifdef __cplusplus


extern "C"
{
#endif  //#ifdef __cplusplus
  // инициализация всего.
  // перед while(1) в main
  void BD_init(void);
  
  // это крутится в мэйне
  void controlFuncs(void);
  
  // обработчики прерываний кнопок для каналов 5-9
  void exti_it1(void);
  
  //обработчик прерываний кнопки для 10-го канала (кнопка включения)
  void exti_it2(void);
  
  // обработчик прерывания для TIM7
  // выключает TIM7 если он не используется в globalMkSTimer
  void mks_it(void);
  
  void testTim_it(void);
  
  void keysCheck(void);
  
  void info(void);
  
#ifdef __cplusplus
}
#endif  //#ifdef __cplusplus


#endif	//#ifndef _INITS_H