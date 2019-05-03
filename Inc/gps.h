#ifndef _GPS_H
#define _GPS_H

#ifdef __cplusplus

#include "UART_Prototype.h"
#include "UARTBuffs.h"
#include "Mode_prototype.h"

namespace GPS
{
#define GPS_COLD_RES 0
#define GPS_WARM_RES 1
#define GPS_WARM2_RES 2
#define GPS_HOT_RES 3
  
  class cGPS : protected UART::classWithBoobs
  {
  public:
    explicit  cGPS(uint8_t* rxBuff,//plase where kept recived data
                       uint8_t* txBuff,//plase where coppy transmited data for transmit
                       uint16_t rxBuffLen,// len of rxBuff
                       uint16_t txBuffLen,// ken of txBuff
                       UART::cUART_interfacePrototype* UART,
                       void (*power)(bool onOff));
    bool init(bool reInit);
    void control(void);
    void (*power)(bool onOff);
    void restart(uint8_t resType);
    
    bool getState(void);
  private:
    bool isInited;
    
    UART::cUART_interfacePrototype* UART;//UART interface class
  };
}

namespace MODES
{
  class cGPS_mode : public cMode_prototype
  {
  public:
    explicit cGPS_mode(void);
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

#endif	//#ifndef _GPS_H