#include "BD.h"
#include "BD_definitions.h"
#include <string.h>

using namespace UART;
using namespace CRC_USR;

namespace BD
{
#define RX_TIMEOUT 1000
#define TX_TIMEOUT 10
  
  cBD::cBD(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,//This buffer ned for resend
                uint16_t rxBuffLen,// len of rxBuff
                uint16_t txBuffLen,// len of txBuff
                UART::cUART_interfacePrototype* UART //interface for UART
                  )
  {
    this->UART = UART;
    this->CRC_class = &CRC_global;
    buffersInit(rxBuff, txBuff, rxBuffLen, txBuffLen);
    this->BD_adr = 0x01;
    answTxed = false;
    txlen = 0;
    rxlen = 0;
    txTry = 0;
    type_CRC = AT_CRC;
  }
  
  bool cBD::getAnsw(void)
  {
    if(txTry >= 3)
      return false;
    switch(UART->getRxState())
    {
    case ERROR:
    case TIMEOUT:
      txTry++;
      sendTxBuffWithCRC();
      return false;
    case DATA_RXED:
      break;
    case IN_PROCESS:
    case REDY:
    default:
      return false;
    }
    rxlen = UART->getRxedDatalen();
    if(rxlen < 3 || rxlen > buffs.constRcvBuffLen || !UART->getRxedData(buffs.rcvBuff, rxlen))
      return false;
    if(CRC_class->check(buffs.rcvBuff, rxlen, type_CRC))
      return true;
    else
    {
      if(!CRC_class->check(buffs.rcvBuff, rxlen, changeCRCType()))
      {
        changeCRCType();
        txTry++;
      }
      sendTxBuffWithCRC();
      return false;
    }
  }
  
  void cBD::control(void)
  {
    if(getAnsw())
      answTxed = true;
  }
  
  
   //This metod safe send comand to BD
  bool cBD::sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg)
  {
    buffs.trmBuff[0] = BD_adr;
    buffs.trmBuff[1] = cmd;
    buffs.trmBuff[2] = LOBYTE(startRegNo);
    buffs.trmBuff[3] = LO2BYTE(startRegNo);
    buffs.trmBuff[4] = LOBYTE(numOfReg);
    buffs.trmBuff[5] = LO2BYTE(numOfReg);
    txlen = 8;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithCRC();
  }
  
   //This metod safe send comand to BD
  bool cBD::sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg, uint16_t* data)
  {
    buffs.trmBuff[0] = BD_adr;
    buffs.trmBuff[1] = cmd;
    buffs.trmBuff[2] = LOBYTE(startRegNo);
    buffs.trmBuff[3] = LO2BYTE(startRegNo);
    buffs.trmBuff[4] = LOBYTE(numOfReg);
    buffs.trmBuff[5] = LO2BYTE(numOfReg);
    memcpy(&buffs.trmBuff[6], data, numOfReg*2);
    txlen = 8 + numOfReg*2;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithCRC();
  }
  
   //This metod safe send comand to BD
  bool cBD::sendCmd(uint8_t cmd)
  {
    buffs.trmBuff[0] = BD_adr;
    buffs.trmBuff[1] = cmd;
    txlen = 4;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithCRC();
  }
  
  //this metod for unsafe send
  bool cBD::sendBuff(uint8_t* buff, uint16_t len)
  {
    if(len < 3 || len > buffs.constTrmBuffLen || buff == NULL)
      return false;
    memcpy(buffs.trmBuff, buff, len);
    txlen = len;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithCRC();
  }
  
  //this metod calculate CRC and send rxBuff with UART
  bool cBD::sendTxBuffWithCRC(void)
  {
    if(txTry >= 3)
      return false;
    CRC_class->calc(buffs.trmBuff, txlen - 2, type_CRC);
    return (UART->txStart(buffs.trmBuff, txlen) && UART->rxStart(1000));
  }
  
  //this metod change CRC tupe for another
   tagCRC_type cBD::changeCRCType(void)
   {
     if(type_CRC == AT_CRC)
       type_CRC = MB_CRC;
     else
       type_CRC = AT_CRC;
     return type_CRC;
   }
};

