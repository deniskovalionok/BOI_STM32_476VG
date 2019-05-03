#ifndef _UART_BUFFS_H
#define _UART_BUFFS_H

#include <stdint.h>

#ifdef __cplusplus

namespace UART
{

  struct tagUART
  {
	uint8_t* rcvBuff;
	
	uint8_t* trmBuff;
	
	//длинны буферов обмена
	uint16_t constRcvBuffLen;
	uint16_t constTrmBuffLen;
        
  };
  
  //многие классы используют буффера приема передачи. Это их инициализация. 
  class classWithBoobs
  {
  protected:
    void buffersInit(uint8_t* rxBuff, uint8_t* txBuff, uint16_t rxBuffLen, uint16_t txBuffLen);
    void buffersInit(uint8_t* Buff, uint16_t BuffLen);
    struct tagUART buffs;
  };

};

#endif  //#ifdef __cplusplus
#endif	//#ifndef _UART_BUFFS_H