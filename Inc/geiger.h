#ifndef _GEIGER_H
#define _GEIGER_H

#ifdef __cplusplus

namespace MODES
{
  class cGM_mode : public cMode_prototype
  {
  public:
    explicit  cGM_mode(void);
  private:
    void userControl(void);
    
    void onShow(void);
    
    void enterKeyAction(void);
    void rKeyAction(void);
    void lKeyAction(void);
    void onKeyAction(void);
  };
  
};

void geigerInit(void);

float geigerGetCPS(void);
float geigerGetErr(void);
float geigerGetDose(void);

#endif  //#ifdef __cplusplus

#endif	//#ifndef _GEIGER_H