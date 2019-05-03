#include "UARTBuffs.h"
#include <string.h>

#include "exeptions.h"

namespace UART
{
  void  classWithBoobs::buffersInit(uint8_t* rxBuff, uint8_t* txBuff, uint16_t rxBuffLen, uint16_t txBuffLen)
  {
      if(rxBuff == NULL || txBuff == NULL || rxBuffLen == 0 || txBuffLen == 0)
      {
        exception(__FILE__,__FUNCTION__,__LINE__,"invalid buffer");
        return;
      }
      buffs.rcvBuff = rxBuff;
      buffs.trmBuff = txBuff;
    
      buffs.constRcvBuffLen = rxBuffLen;
      buffs.constTrmBuffLen = txBuffLen;
  }
  
  void classWithBoobs::buffersInit(uint8_t* Buff, uint16_t BuffLen)
  {
    buffersInit(Buff, Buff, BuffLen, BuffLen);
  }
};