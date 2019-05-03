#include "UART_Usr.h"
#include <string.h>
#include "usart.h"

#include "exeptions.h"



namespace UART
{
  #define MAX_UARTS  6
  
  struct tagUARTs{
    cUART* uartClass;
    UART_HandleTypeDef* uartHandle;
  };
  
  tagUARTs UARTs[MAX_UARTS];
  
  //you may use same rxBuff, txBuff vith class who use rx/tx metods
  cUART::cUART(uint8_t* rxBuff,//plase where kept recived data
                       uint8_t* txBuff,//plase where coppy transmited data for transmit
                       uint16_t rxBuffLen,// len of rxBuff
                       uint16_t txBuffLen,// ken of txBuff
                       UART_HandleTypeDef* uartHandle,// HAL hendler of UART what we use 
                       uint16_t MBTimeout//min timeot in bits betwen packets. if =0 not used 
                         )
  {
    
    buffersInit(rxBuff, txBuff, rxBuffLen, txBuffLen);
    
    this->MBTimeout = MBTimeout;
    
    this->uartHandle = uartHandle;
    
    dataRxed = false;
    if(MBTimeout)
    {
        for(uint16_t i = 0; i < MAX_UARTS; i++)
        {
          if(UARTs[i].uartClass == NULL)
          {
            UARTs[i].uartClass = this;
            UARTs[i].uartHandle = uartHandle;
            break;
          }
          else
            if(i >= MAX_UARTS - 1)
              exception(__FILE__,__FUNCTION__,__LINE__,"To many UARTs!");
        }
    }
  }
  
  //this metod starts recive.
  //If recive olways onn this may clear FIFO and counter of recived simbols.
  //Recived data mast taken with getRxedData metod.
  //false = error
  bool cUART::rxStart(void)
  {
    if(!rxStop())
      return false;
    if(MBTimeout)
    {
        WRITE_REG(uartHandle->Instance->RTOR, MBTimeout);
        SET_BIT(uartHandle->Instance->CR1, USART_CR1_RTOIE);
        SET_BIT(uartHandle->Instance->CR2, USART_CR2_RTOEN);
    }
    bool isOK;
    if(uartHandle->hdmarx != NULL)
      isOK = (HAL_UART_Receive_DMA(uartHandle, buffs.rcvBuff, buffs.constRcvBuffLen) == HAL_OK);
    else
      isOK = (HAL_UART_Receive_IT(uartHandle, buffs.rcvBuff, buffs.constRcvBuffLen) == HAL_OK);
    if(isOK)
      dataRxed = false;
    return isOK;
  }
  
  //This metod returns number of recived bytes.
  bool cUART::getRxedData(uint8_t* rxBuff, uint16_t rxBuffLen)
  {
    if(MBTimeout && !dataRxed)
      return false;
    if((rxBuff == NULL) || (rxBuffLen == 0))
      return false;
    if(buffs.rcvBuff != rxBuff)
        memcpy(rxBuff, buffs.rcvBuff, rxBuffLen);
    /*
    if(MBTimeout)
        rxStop();
    */
    return true;
  }
  
  //this metod write rxBuffLen bytes of recived data in rxBuff.
  //if data not recived, or recive of the packet not over or, error it return false. For more information call getRxState.
  //if this class without Modbus this metod don't check packet over(bluetooth).
  uint16_t cUART::getRxedDatalen(void)
  {
    if(uartHandle->hdmarx != NULL)
      return buffs.constRcvBuffLen - uartHandle->hdmarx->Instance->CNDTR;
    else
      return buffs.constRcvBuffLen - uartHandle->RxXferCount;
  }
  
  
  //this metod transmit txBuffLen bytes from txBuff
  // if this return false transmit not sarted
  bool cUART::txStart(uint8_t* txBuff, uint16_t txBuffLen)
  {
      if((txBuff == NULL) || (txBuffLen == 0) || (txBuffLen > buffs.constTrmBuffLen))
        return false;
      if(buffs.trmBuff != txBuff)
        memcpy(buffs.trmBuff, txBuff, txBuffLen);
      if(uartHandle->hdmatx != NULL)
        return (HAL_UART_Transmit_DMA(uartHandle, buffs.trmBuff, txBuffLen) == HAL_OK);
      else
        return (HAL_UART_Transmit_IT(uartHandle, buffs.trmBuff, txBuffLen) == HAL_OK);
  }
  
  
  //this metod returns state tagRxErrors of recive proces.
  tagRxErrors cUART::getRxState(void)
  {
    switch(HAL_UART_GetState(uartHandle))
    {
    case HAL_UART_STATE_READY:
      return TX_REDY;
    case HAL_UART_STATE_BUSY:
    case HAL_UART_STATE_BUSY_TX:
    case HAL_UART_STATE_BUSY_RX:
    case HAL_UART_STATE_BUSY_TX_RX:
        return TX_IN_PROCESS;
    case HAL_UART_STATE_ERROR:
    default:
      return TX_ERROR;
    }
  }

  //this metod reinit uart with new speed from newSpeed.
  //if newSpeed = 0 this metod hardly reinit all UART registers in original state(state that was when start chip).
  //false = error
  bool cUART::reInit(uint32_t newSpeed)
  {
    if(newSpeed)
    {
      uartHandle->Init.BaudRate = newSpeed;
      return (HAL_UART_DeInit(uartHandle) == HAL_OK) && (HAL_UART_Init(uartHandle) == HAL_OK);
    }
    else
    {
      if(HAL_UART_DeInit(uartHandle) != HAL_OK)
        return false;
      if(uartHandle->Instance == USART1)
        MX_USART1_UART_Init();
      else if(uartHandle->Instance == USART2)
        MX_USART2_UART_Init();
      else if(uartHandle->Instance == USART3)
        MX_USART3_UART_Init();
      else if(uartHandle->Instance == UART4)
        MX_UART4_Init();
      else if(uartHandle->Instance == UART5)
        MX_UART5_Init();
      else
        return false;
      return true;
    }
    
  }
  
  void cUART::intControl(void)
  {
    rxStop();
    dataRxed = true;
  }
  
  ////////////////////////Private metods///////////////////////////////////////
    
  //this metod stops recive
  //false = error
  bool cUART::rxStop(void)
  {
    SET_BIT(uartHandle->Instance->ICR, USART_ICR_RTOCF);
    return (HAL_UART_AbortReceive(uartHandle) == HAL_OK);
  }
  
};

using namespace UART;

void UART_intConrol(UART_HandleTypeDef* uartHandle)
{
  if(READ_BIT(uartHandle->Instance->ISR, USART_ISR_RTOF))
  {
      for(uint16_t i = 0; i < MAX_UARTS; i++)
      {
        if(uartHandle == UARTs[i].uartHandle)
          UARTs[i].uartClass->intControl();
      }
  }
}