#ifndef _INITS_H
#define _INITS_H

#ifdef __cplusplus


extern "C"
{
#endif  //#ifdef __cplusplus
  // ������������� �����.
  // ����� while(1) � main
  void BD_init(void);
  
  // ��� �������� � �����
  void controlFuncs(void);
  
  // ����������� ���������� ������ ��� ������� 5-9
  void exti_it1(void);
  
  //���������� ���������� ������ ��� 10-�� ������ (������ ���������)
  void exti_it2(void);
  
  // ���������� ���������� ��� TIM7
  // ��������� TIM7 ���� �� �� ������������ � globalMkSTimer
  void mks_it(void);
  
  void testTim_it(void);
  
  void keysCheck(void);
  
  void info(void);
  
#ifdef __cplusplus
}
#endif  //#ifdef __cplusplus


#endif	//#ifndef _INITS_H