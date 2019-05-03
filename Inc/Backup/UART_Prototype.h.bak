#ifndef _UART_PROTOTYPE_H
#define _UART_PROTOTYPE_H

#include <stdint.h>

namespace UART
{
  
  enum tagRxErrors{
    TX_REDY,
    TX_IN_PROCESS,
    TX_ERROR
  };
  
  //this is portotype of UARTclass wat need for modbus & bluetooth
  class cUART_interfacePrototype
  {
  public:
        //metods wat using in modbus
        
        //this metod starts recive.
        //If recive olways onn this may clear FIFO and counter of recived simbols.
        //Recived data mast taken with getRxedData metod.
        //false = error
        virtual bool rxStart(void) = 0;
        
        //This metod returns number of recived bytes.
        virtual uint16_t getRxedDatalen(void) = 0;
        
        //this metod write rxBuffLen bytes of recived data in rxBuff.
        //if data not recived, or recive of the packet not over or, error it return false. For more information call getRxState.
        //if this class without Modbus this metod don't check packet over(bluetooth).
        virtual bool getRxedData(uint8_t* rxBuff, uint16_t rxBuffLen) = 0;
        
        //this metod transmit txBuffLen bytes from txBuff
        // if this return false transmit not sarted
        virtual bool txStart(uint8_t* txBuff, uint16_t txBuffLen) = 0;
        
        //this metod returns state tagRxErrors of recive proces.
        virtual tagRxErrors getRxState(void) = 0;
        
        //this metod reinit uart with new speed from newSpeed.
        //if newSpeed = 0 this metod hardly reinit all UART registers in original state(state that was when start chip).
        //false = error
        virtual bool reInit(uint32_t newSpeed) = 0;
  };
  
};

#endif	//#ifndef _UART_PROTOTYPE_H