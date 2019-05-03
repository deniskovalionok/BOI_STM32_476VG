#ifndef _BD_H
#define _BD_H

#ifdef __cplusplus

#include "UART_Prototype.h"
#include "AT_CRC.h"
#include "UARTBuffs.h"

namespace BD
{
  class cBD : protected UART::classWithBoobs
  {
  public:
    explicit cBD(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,//This buffer ned for resend
                uint16_t rxBuffLen,// len of rxBuff
                uint16_t txBuffLen,// len of txBuff
                UART::cUART_interfacePrototype* UART //interface for UART
                  );
    
    //This metod safe send comand to BD
    bool sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg);
    bool sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg, uint16_t* data);
    bool sendCmd(uint8_t cmd);
    
    void control(void);
    //this metod for unsafe send
    bool sendBuff(uint8_t* buff, uint16_t len);
  protected:
    //this metod calculate CRC and send rxBuff with UART
    bool sendTxBuffWithCRC(void);
    
    //this metod change CRC tupe for another
    CRC_USR::tagCRC_type changeCRCType(void);
    
    bool getAnsw(void);
    
    CRC_USR::cCRC* CRC_class;//class for CRC calculate
    
    uint16_t txlen;//len of transmited data
    
    uint16_t rxlen;//len of recived data
    
    UART::cUART_interfacePrototype* UART;//UART interface class
    
    uint8_t BD_adr;
    
    uint8_t txTry;//number of wrong trys to send data
    
    bool answTxed;
    UART::tagRxErrors rxErrors : 3; 
    CRC_USR::tagCRC_type type_CRC : 1;//type of CRC16 using in this BD may be AT or MB 
  };
  
};

#endif  //#ifdef __cplusplus
#endif	//#ifndef _BD_H