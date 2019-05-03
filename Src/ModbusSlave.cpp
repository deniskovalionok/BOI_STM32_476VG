#include "ModbusSlave.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

using namespace UART;
using namespace CRC_USR;

namespace MODBUS
{
  cPC::cPC(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,//This buffer ned for resend
                uint16_t rxBuffLen,// len of rxBuff
                uint16_t txBuffLen,// len of txBuff
                UART::cUART_interfacePrototype* UART, //interface for UART
                tagBDs *BDs,
                uint16_t lenOfBDsArr,
                uint8_t myAdr
                  )
  {
    buffersInit(rxBuff, txBuff, rxBuffLen, txBuffLen);
    this->UART = UART;
    this->CRC_class = &CRC_global;
    this->BDs = BDs;
    this->lenOfBDsArr = lenOfBDsArr;
    this->myAdr = myAdr;
    txInQueue = false;
    transferInProcess = false;
    rxLen = 0;
    for(uint16_t i = 0; i < lenOfBDsArr; i++)
      BDs->useNow = false;
  }
  
  cPC::cPC(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,// This buffer ned for resend
                uint16_t rxBuffLen, // len of rxBuff
                uint16_t txBuffLen, // len of txBuff
                UART::cUART_interfacePrototype* UART, //interface for UART
                uint8_t myAdr
                  )
    {
      buffersInit(rxBuff, txBuff, rxBuffLen, txBuffLen);
      this->UART = UART;
      this->CRC_class = &CRC_global;
      lenOfBDsArr = 0;
      this->myAdr = myAdr;
      txInQueue = false;
      transferInProcess = false;
      rxLen = 0;
    }
    
    void cPC::control(void)
    {
      switch(UART->getRxState())
      {
      case TX_REDY:
        if(!transferInProcess)
            checkRx();
        UART->rxStart();
        break;
      case TX_IN_PROCESS:
        break;
      case TX_ERROR:
      default:
        UART->reInit(0);
        break;
      }
      if(transferInProcess)
      {
        bool bdFinded = false;
        for(uint16_t i = 0; i < lenOfBDsArr; i++)
        if(buffs.rcvBuff[0] == BDs[i].BDAdr)
        {
          if(!BDs[i].BD->getBD_ID())
          {
            bdFinded = true;
            BDs[i].useNow = false;
            BDs[i].inQueue = false;
            errCom(1);
          }
          if(BDs[i].BD->sendBuff(buffs.rcvBuff, rxLen))
          {
            bdFinded = true;
            BDs[i].useNow = true;
            BDs[i].inQueue = false;
          }
          else
            transferInProcess = true;
        }
        if(bdFinded)
            transferInProcess = false;
      }
      
      if(!txInQueue || UART->txStart(buffs.trmBuff, txBuffLen))
      {
          txInQueue = false;
          for(uint16_t i = 0; i < lenOfBDsArr; i++)
          {
            if(BDs[i].useNow)
            {
              if(!BDs[i].BD->isBusy())
              {
                BDs[i].useNow = false;
                errCom(4);
                break;
              }
              
              txBuffLen = BDs[i].BD->getAnswLen();
              if(txBuffLen > buffs.constTrmBuffLen)
              {
                uint8_t buff; 
                if(BDs[i].BD->getAnsw(&buff, 1))
                  errCom(4);
              }
              else
              {
                if(BDs[i].BD->getAnsw(buffs.trmBuff, txBuffLen))
                {
                  if(!UART->txStart(buffs.trmBuff, txBuffLen))
                    txInQueue = true;
                  BDs[i].useNow = false;
                  break;
                }
              }
            }
          }
      }
    }
  
  bool cPC::checkRx(void)
  {
    rxLen = UART->getRxedDatalen();
    if(rxLen > buffs.constRcvBuffLen)
    {
      errCom(1);
      UART->rxStart();
      return false;
    } 
    if(!UART->getRxedData(buffs.rcvBuff, rxLen))
      return false;
    if(!CRC_class->check(buffs.rcvBuff, rxLen, AT_CRC))
    {
      errCom(1);
      return false;
    }
    if(buffs.rcvBuff[0] == myAdr)
    {
      if(cmdProc())
        return true;
    }
    else
    {
      //bool bdFinded = false;
      for(uint16_t i = 0; i < lenOfBDsArr; i++)
        if(buffs.rcvBuff[0] == BDs[i].BDAdr)
        {
          if(!BDs[i].BD->getBD_ID())
          {
            errCom(1);
            return false;
          }
          BDs[i].BD->toQeue();
          if(BDs[i].BD->sendBuff(buffs.rcvBuff, rxLen))
          {
            //bdFinded = true;
            BDs[i].useNow = true;
          }
          else
          {
            BDs[i].inQueue = true;
            transferInProcess = true;
          }
          return true;
        }
      //if(bdFinded)
        //return true;
    }
    errCom(1);
    return false;
  }
  
  bool cPC::errCom(uint8_t errType)
  {
    uint8_t buff[5];
    buff[0] = myAdr;
    buff[1] = 0x84;
    buff[2] = errType;
    CRC_class->calc(buff, 3, AT_CRC);
    return UART->txStart(buff, 5);
  }
  
#ifdef FILESYSTEM
extern "C"
{
#include "filesystem.h"
}
  
#endif //FILESYSTEM
  bool cPC::cmdProc(void)
  {
    uint16_t len = 0;
    switch(buffs.rcvBuff[1])
    {
#ifdef FILESYSTEM
      case 0x51:	//read directory
	len = USBRS_readDir(&filesystem, (char*)buffs.trmBuff, buffs.constTrmBuffLen, (char*)buffs.rcvBuff, rxLen);
	break;
      case 0x52:	//read file
	len = USBRS_readFile(&filesystem, (char*)buffs.trmBuff, buffs.constTrmBuffLen, (char*)buffs.rcvBuff, rxLen);
	break;
      case 0x53:	//write file
	len = USBRS_writeFile(&filesystem, (char*)buffs.trmBuff, buffs.constTrmBuffLen, (char*)buffs.rcvBuff, rxLen);
	break;
      case 0x54:	//create file
	len = USBRS_createFile(&filesystem, (char*)buffs.trmBuff, buffs.constTrmBuffLen, (char*)buffs.rcvBuff, rxLen);
	break;
      case 0x55:	//delete file
	len = USBRS_deleteFile(&filesystem, (char*)buffs.trmBuff, buffs.constTrmBuffLen, (char*)buffs.rcvBuff, rxLen);
	break;
      case 0x56:	//get file num
	len = USBRS_getFileNum(&filesystem, (char*)buffs.trmBuff, buffs.constTrmBuffLen, (char*)buffs.rcvBuff, rxLen);
	break;
      /*case 0x57:	//execute file "execute.sys"
	len = USBRS_executeFile(&filesystem, (char*)&buffs.trmBuff, buffs.constTrmBuffLen, (char*)&buffs.rcvBuff, rxLen);
	break;
       */
#endif //FILESYSTEM
    default:
        return false;
    }
    CRC_class->calc(buffs.trmBuff, len, AT_CRC);
    UART->txStart(buffs.trmBuff, len+2);
    return true;
  }
  
}


//Á12+ Ì28+ Ç30+ À30