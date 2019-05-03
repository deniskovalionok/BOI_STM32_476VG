#include "bluetooth.h"
#include "LMX9838_definitions.h"

uint32_t BT_On = 0;

using namespace UART;

namespace LMX9838
{
  cBT::cBT(uint8_t* rxBuff, uint8_t* txBuff, UART::cUART_interfacePrototype* UART)
  {
    buffersInit(rxBuff, txBuff, sizeof(rxBuff), sizeof(txBuff));
    this->UART = UART;
    
  }
  
  bool cBT::getAnsw(void)
  {
    if(!BT_On)
      return false;
    /*
    switch(UART->getRxState())
    {
    case REDY:
      return false;
    case IN_PROCESS:
      if(!rxlen)
      {
        uint16_t len = UART->getRxedDatalen();
        if(len < 6)
            return false;
        if(!UART->getRxedData(buffs.rcvBuff, len))
            return false;
        if(buffs.rcvBuff[0] != LMX_START_DEL || !checksumCheck())
          
        rxlen = len;
      }
      else
        rxlen = UART->getRxedDatalen();
      if(rxlen >= (uint16_t)(buffs.rcvBuff[3] + 
                             (((uint16_t)buffs.rcvBuff[3] << 8) & 0xff00)))
      {
        if(buffs.rcvBuff[rxlen] != LMX_STOP_DELL)
          return false;
        if(!checksumCheck())
        {
          if(waitAnsw && txTry < 3)
          {
            if(UART->txStart(buffs.trmBuff, txlen))
            {
              txTry++;
              rxlen = 0;
            }
            return false;
          }
        }
      }
    //case ERROR:
    case DATA_RXED:
    default:
      UART->reInit(0);
      return false;
    }
    */
      return false;
  }
  
  bool cBT::checksumCheck(void)
  {
    if(checksumRight)
      return true;
    if(rxlen < 6)
      return false;
    return checksumRight = (LOBYTE(buffs.rcvBuff[1] + buffs.rcvBuff[2] + buffs.rcvBuff[3] + buffs.rcvBuff[4])
            == buffs.rcvBuff[5]);
  }
  
  void cBT::switchON(void)
  {
    BT_On = 1;
    UART->reInit(0);
  }
  
  void cBT::switchOFF(void)
  {
    BT_On = 0;
    UART->reInit(0);
  }
}

#include "UART_Usr.h"
#include "usart.h"

uint8_t buff1[1024];
uint8_t buff2[1024];

cUART BT_UART(buff1, buff2, 1024, 1024, &huart1, false);

void bluetoothTest(void)
{
  BT_On = 1;
  BT_UART.reInit(0);
  
  uint8_t buff[] = {0x02, 0x52, 0x04, 0x09, 0x00, 0x5f, 0x08, 0x54, 0x65, 0x73, 0x74, 0x20, 0x20, 0x23, 0x31, 0x03};
  
  BT_UART.rxStart();
  BT_UART.txStart(buff, 16);
}