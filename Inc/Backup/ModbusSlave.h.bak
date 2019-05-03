#ifndef _MODBUS_SLAVE_H
#define _MODBUS_SLAVE_H

#ifdef __cplusplus

#include "UART_Prototype.h"
#include "UARTBuffs.h"
#include "AT_CRC.h"
#include "ModbusMaster.h"

namespace MODBUS
{
  
   struct tagBDs{
    cBD *BD;
    uint8_t BDAdr;
    bool inQueue;
    bool useNow;
  };
  
  class cPC : protected UART::classWithBoobs
  {
  public:
    
    explicit cPC(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,//This buffer ned for resend
                uint16_t rxBuffLen,// len of rxBuff
                uint16_t txBuffLen,// len of txBuff
                UART::cUART_interfacePrototype* UART, //interface for UART
                tagBDs *BDs,
                uint16_t lenOfBDsArr,
                uint8_t myAdr
                  );
    
    explicit cPC(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,//This buffer ned for resend
                uint16_t rxBuffLen,// len of rxBuff
                uint16_t txBuffLen,// len of txBuff
                UART::cUART_interfacePrototype* UART, //interface for UART
                uint8_t myAdr
                  );
    
    void control(void);
    
    
    
  protected:
    bool checkRx(void);
    bool errCom(uint8_t errType);
    bool cmdProc(void);
    bool txInQueue;
    uint16_t txBuffLen;
    UART::cUART_interfacePrototype* UART;
    CRC_USR::cCRC* CRC_class;//class for CRC calculate
    tagBDs *BDs;
    uint16_t lenOfBDsArr;
    uint8_t myAdr;
    uint16_t rxLen;
    bool transferInProcess;
  };
};
#endif  //#ifdef __cplusplus
#endif	//#ifndef _MODBUS_SLAVE_H