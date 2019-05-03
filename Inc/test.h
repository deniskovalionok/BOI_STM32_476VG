#ifndef _TEST_H
#define _TEST_H
#ifdef __cplusplus

#include "Mode_prototype.h"

namespace MODES
{
  class cTest_mode : public cMode_prototype
  {
  public:
    explicit cTest_mode(char * name);
    void menuTest(void);
  private:
    void userControl(void);
    
    void onShow(void);
    
    void enterKeyAction(void);
    void rKeyAction(void);
    void lKeyAction(void);
    void onKeyAction(void);
  };
  
  extern cMode_prototype* mode2;
};

#endif  //#ifdef __cplusplus
#endif	//#ifndef _TEST_H