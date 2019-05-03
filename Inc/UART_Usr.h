#ifndef _UART_USR_H
#define _UART_USR_H

#include "stm32l4xx_hal.h"

#ifdef __cplusplus

#include "UART_Prototype.h"
#include "UARTBuffs.h"


namespace UART
{
  
  
#define MB_TIMEOUT 28
  
  //UART interface class
  //writed with HAL functions
  class cUART : public cUART_interfacePrototype, protected classWithBoobs
  {
    public:
        //you may use same rxBuff, txBuff vith class who use rx/tx metods
        explicit cUART(uint8_t* rxBuff,//plase where kept recived data
                       uint8_t* txBuff,//plase where coppy transmited data for transmit
                       uint16_t rxBuffLen,// len of rxBuff
                       uint16_t txBuffLen,// ken of txBuff
                       UART_HandleTypeDef* uartHandle,// HAL hendler of UART what we use 
                       uint16_t MBTimeout//min timeot in bits betwen packets. if =0 not used 
                         );
        
        //this metod starts recive.
        //If recive olways onn this may clear FIFO and counter of recived simbols.
        //Recived data mast taken with getRxedData metod.
        //false = error
        bool rxStart(void);
        
        //This metod returns number of recived bytes.
        uint16_t getRxedDatalen(void);
        
        //this metod write rxBuffLen bytes of recived data in rxBuff.
        //if data not recived, or recive of the packet not over or, error it return false. For more information call getRxState.
        //if this class without Modbus this metod don't check packet over(bluetooth).
        bool getRxedData(uint8_t* rxBuff, uint16_t rxBuffLen);
        
        //this metod transmit txBuffLen bytes from txBuff
        // if this return false transmit not sarted
        bool txStart(uint8_t* txBuff, uint16_t txBuffLen);
        
        //this metod returns state tagRxErrors of recive proces.
        tagRxErrors getRxState(void);
        
        //this metod reinit uart with new speed from newSpeed.
        //if newSpeed = 0 this metod hardly reinit all UART registers in original state(state that was when start chip).
        //false = error
        bool reInit(uint32_t newSpeed);
        
        void intControl(void);
        
    protected:
      
        //this metod stops recive
        //false = error
        bool rxStop(void);
        
        bool dataRxed;
      
        //min timeot in bits betwen packets. if =0 not used 
        uint16_t MBTimeout;
        
        // HAL hendler of UART what we use 
        UART_HandleTypeDef* uartHandle;
        
  };
};

extern "C"
{
#endif  //#ifdef __cplusplus
  
  void UART_intConrol(UART_HandleTypeDef* uartHandle);
  
#ifdef __cplusplus
}
#endif  //#ifdef __cplusplus
#endif	//#ifndef _UART_USR_H