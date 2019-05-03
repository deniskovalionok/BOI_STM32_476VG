#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include "timer_prototype.h"

#ifdef __cplusplus

namespace TIMER
{
  //��������� ������������ ��� ��������
  struct tagTimer{
    //����� ������
    uint16_t timeout;
    //�������� ��� �������
    uint16_t startTick;
    //������������ �� ������
    bool isOn;
  };
  
  
  
  class cMS_timer : public cMS_timer_prototype
  {
  public:
    explicit cMS_timer(tagTimer *timStruct//��������� �� ������ �������� ��������
                       , uint16_t timArrLen//��� ������
                         );
    //������ �������
    //timNum - ����� ������� � ������� (���� <0 �� ����� ������� ����� ��������� ������)
    //timeout - ����� ������ � ������������
    //���������� ����� ����������� �������. -1 - ������
    int start(uint16_t timeout, int timNum);
    
    //����� ��������� �������
    //timNum - ����� ������� � ������� (>0)
    //���������� 0 ���� ����� �� ������, >0 - ����� ������, <0 ������ 
    int check(int timNum);
    
    //���������� �������
    //timNum - ����� ������� � ������� (>0)
    //����� ����� ������ ������ ����� ���� ����������� ������� ���������
    //����� ��� ��� ������������ ������ ��� ���������� ���������. ����� ����� ������.
    //���� ���������� false - ������
    bool stop(int timNum);
  protected:
    
    //��������� �� ������ �������� ��������
    tagTimer *tim;
    
    //��� ������
    uint16_t timArrLen;
  };
  
  class cMKs_timer : public cMS_timer
  {
  public:
    explicit cMKs_timer(tagTimer *timStruct,//��������� �� ������ �������� ��������
                        uint16_t timArrLen//��� ������
                          ) : cMS_timer(timStruct, timArrLen) {};
    //����� ��������� �������
    //timNum - ����� ������� � ������� (>0)
    //���������� 0 ���� ����� �� ������, >0 - ����� ������, <0 ������ 
    int check(int timNum);
    
    //���������� �������
    //timNum - ����� ������� � ������� (>0)
    //����� ����� ������ ������ ����� ���� ����������� ������� ���������
    //����� ��� ��� ������������ ������ ��� ���������� ���������. ����� ����� ������.
    //���� ���������� false - ������
    int start(uint16_t timeout, int timNum);
  };
  
  //��������� ������������ ��� ����� ��������
  struct tagSTimer{
    //����� ������
    uint16_t timeout;
    //������� �����
    uint16_t counter;
    //������������ �� ������
    bool isOn;
    //��������� �� ������� ����������� �� ��������� �������
    void (*func)(void);
    //������ �������� ������
    bool isFinish;
    //���� true ������ ���������������
    bool refresh;
  };
  
  class cSuperTimer : public cMS_timer_prototype
  {
    public:
      explicit cSuperTimer(tagSTimer *timStruct//��������� �� ������ �������� ��������
                       , uint16_t timArrLen//��� ������
                         );
      
    //������ �������
    //timNum - ����� ������� � ������� (���� <0 �� ����� ������� ����� ��������� ������)
    //timeout - ����� ������ � ������������
    //���������� ����� ����������� �������. -1 - ������
    int start(uint16_t timeout, int timNum);
    
    //������ �������
  //timNum - ����� ������� � ������� (���� <0 �� ����� ������� ����� ��������� ������)
  //timeout - ����� ������ � ������������
  //func - ��������� �� ������� ����������� �� ��������� �������
  //���� true ������ ��������������� ����� ��������� �������
  //���������� ����� ����������� �������. -1 - ������
    int start(uint16_t timeout, int timNum, void (*func)(void), bool refresh);
    
    //����� ��������� �������
    //timNum - ����� ������� � ������� (>0)
    //���������� 0 ���� ����� �� ������, >0 - ����� ������, <0 ������ 
    int check(int timNum);
    
    //���������� �������
    //timNum - ����� ������� � ������� (>0)
    //����� ����� ������ ������ ����� ���� ����������� ������� ���������
    //����� ��� ��� ������������ ������ ��� ���������� ���������. ����� ����� ������.
    //���� ���������� false - ������
    bool stop(int timNum);
    
    //���������� ����������
    //��� ���� ��������� � ���������� ����� ������ �������� �� �����
    void it(void);
      
    protected:
      //��������� �� ������ �������� ��������
      tagSTimer *tim;
      
      //��� ������
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