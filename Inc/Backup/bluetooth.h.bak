#ifndef _BLUETOTH_H
#define _BLUETOTH_H

#ifdef __cplusplus

//#include "UART_Prototype.h"
#include "UART_Usr.h"
#include "UARTBuffs.h"

namespace LMX9838
{
  
#ifndef ULONG_PTR
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#endif  //#ifndef ULONG_PTR
  
#ifndef LOBYTE
#define LOBYTE(w)           ((uint8_t)(((ULONG_PTR)(w)) & 0xff))
#endif  //#ifndef LOBYTE
  
  class cBT : protected UART::classWithBoobs
  {
  public:
    explicit cBT(uint8_t* rxBuff, uint8_t* txBuff, UART::cUART_interfacePrototype* UART);
    bool sendCmd(uint8_t pType, uint8_t opCode, uint16_t dataSize, uint8_t* data);
    virtual void switchON(void);
    virtual void switchOFF(void);
    bool getAnsw(void);
  protected:
    bool checksumCheck(void);
    bool checksumRight;
    uint8_t txTry;
    bool waitAnsw;
    uint16_t txlen;
    uint16_t rxlen;
    UART::cUART_interfacePrototype* UART;
  };
};

extern "C"
{
#endif  //#ifdef __cplusplus
  void bluetoothTest(void);
#ifdef __cplusplus
};
#endif  //#ifdef __cplusplus
#endif	//#ifndef _BLUETOTH_H