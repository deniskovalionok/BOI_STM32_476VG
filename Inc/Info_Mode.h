#ifndef _INFO_MODE_H
#define _INFO_MODE_H

#ifdef __cplusplus

#include "Mode_prototype.h"

namespace MODES
{
  class cInfo_mode : public cMode_prototype
  {
  public:
    explicit cInfo_mode(void);
  private:
    void userControl(void);
    
    void onShow(void);
    
    void enterKeyAction(void);
    void rKeyAction(void);
    void lKeyAction(void);
    void onKeyAction(void);
  };
};

#endif  //#ifdef __cplusplus
#endif	//#ifndef _INFO_MODE_H