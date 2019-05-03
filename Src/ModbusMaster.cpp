#include "ModbusMaster.h"
#include "BD_definitions.h"
#include <string.h>
#include "exeptions.h"

#include <stdio.h>

using namespace UART;
using namespace CRC_USR;

#include "ft8xx.h"

//bool gggg = false;

namespace MODBUS
{
#define BD_TIMEOUT      500
#define TIMEOUT_TIM     0
#define FINDE_BD_TIM    1
#define BD_CHECK_TIM    2
  
  uint32_t UART_SPEEDS[10] = {300, 600, 1200,  2400, 4800, 9600, 19200,  38400, 57600, 115200};
  
  cBD::cBD(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,//This buffer ned for resend
                uint16_t rxBuffLen,// len of rxBuff
                uint16_t txBuffLen,// len of txBuff
                UART::cUART_interfacePrototype* UART, //interface for UART
                TIMER::cMS_timer_prototype* tim
                  )
  {
    this->UART = UART;
    this->tim = tim;
    this->CRC_class = &CRC_global;
    buffersInit(rxBuff, txBuff, rxBuffLen, txBuffLen);
    this->BD_adr = 0x01;
    answTxed = false;
    txlen = 0;
    rxlen = 0;
    txTry = 0;
    type_CRC = AT_CRC;
    checkProc = false;
    queue = 0;
    curSpeed = 5;
    pollStart = false;
  }
  
  bool cBD::answProc(void)
  {
    if(txTry >= 3)
    {
      bdInf = false;
      busy = false;
      checkProc = false;
      return false;
    }
    
    if(!busy)
      return false;
    
    switch(UART->getRxState())
    {
    case TX_REDY:
      break;
      
    case TX_IN_PROCESS:
          if(timerErr)
            {
               if(tim->stop(TIMEOUT_TIM) && (tim->start(BD_TIMEOUT, TIMEOUT_TIM) == TIMEOUT_TIM))
                  timerErr = false;
            }
            else
            {
              int itmAnsw = tim->check(TIMEOUT_TIM);
              if(itmAnsw)
              {
                if(itmAnsw < 0)
                {
                  if(tim->stop(TIMEOUT_TIM) && (tim->start(BD_TIMEOUT, TIMEOUT_TIM) == TIMEOUT_TIM))
                  {
                    timerErr = false;
                  }
                  else
                  {
                    timerErr = true;
                  }
                }
                else
                {
                  errorProc(); //timeout
                  //const uint8_t resDose[] = {0x01, 0x05, 0x00, 0x23, 0xff, 0x00};
                  if(txTry >= 3)// && !memcmp(buffs.trmBuff, resDose, 6))
                    BD_id = 0x00;
                  return false;
                }
              }
            }
    return false;
    
    default:
    case TX_ERROR:
      errorProc();
      return false;
    }
    
    rxlen = UART->getRxedDatalen();
    if(rxlen < 3 || rxlen > buffs.constRcvBuffLen || !UART->getRxedData(buffs.rcvBuff, rxlen))
    {
      UART->rxStart();
      checkProc = false;
      busy = false;
      return false;
    }
    if(CRC_class->check(buffs.rcvBuff, rxlen, type_CRC))
      return answTxed = true;
    else
    {
      /*
      if(!CRC_class->check(buffs.rcvBuff, rxlen, changeCRCType()))
      {
        changeCRCType();
        txTry++;
      }
      */
      errorProc();
    }
    return false;
  }
  
  void cBD::toQeue(void)
  {
    queue++;
  }
  
  void cBD::errorProc(void)
  {
    if(BD_id)
    {
      txTry++;
      if(txTry >= 3)
      {
      }
      else
      {
        tim->stop(TIMEOUT_TIM);
        tim->start(100, TIMEOUT_TIM);
        while(!sendTxBuffWithCRC())
        {
          if(!tim->check(TIMEOUT_TIM))
            UART->reInit(0);
          else
            exception(__FILE__,__FUNCTION__,__LINE__,"Uncnown UART error");
        }
      }
    }
    else
    {
          bdInf = false;
          busy = false;
          checkProc = false;
    }
  }
  
  uint8_t cBD::getBD_ID(void)
  {
    return BD_id;
  }
  
  uint16_t cBD::getAnswLen(void)
  {
    if(answProc())
      return rxlen;
    else
      return rxlen = 0;
  }
  
  bool cBD::getAnsw(uint8_t* buffer, uint16_t len)
  {
    if(!len)
      return false;
    if(answTxed || answProc())
    {
      if(buffs.rcvBuff != buffer)
        memcpy(buffer, buffs.rcvBuff, len);
      busy = false;
      return true;
    }
    else
      return false;
  }
  
  bool cBD::isBusy(void)
  {
    return busy;
  }
  
  
   //This metod safe send comand to BD
  bool cBD::sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg)
  {
    if(busy)
      return false;
    buffs.trmBuff[0] = BD_adr;
    buffs.trmBuff[1] = cmd;
    buffs.trmBuff[2] = LO2BYTE(startRegNo);
    buffs.trmBuff[3] = LOBYTE(startRegNo);
    buffs.trmBuff[4] = LO2BYTE(numOfReg);
    buffs.trmBuff[5] = LOBYTE(numOfReg);
    txlen = 8;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithQueue();
  }
  
    //This metod safe send comand to BD
  bool cBD::sendCmdWithMunOfBytes(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg)
  {
    if(busy)
      return false;
    buffs.trmBuff[0] = BD_adr;
    buffs.trmBuff[1] = cmd;
    buffs.trmBuff[2] = 4;
    buffs.trmBuff[3] = LO2BYTE(startRegNo);
    buffs.trmBuff[4] = LOBYTE(startRegNo);
    buffs.trmBuff[5] = LO2BYTE(numOfReg);
    buffs.trmBuff[6] = LOBYTE(numOfReg);
    txlen = 9;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithQueue();
  }
  
  //This metod safe send comand to BD
  bool cBD::sendCmdWithMunOfBytes(uint8_t cmd, uint16_t specNum, uint16_t startRegNo, uint16_t numOfReg)
  {
    if(busy)
      return false;
    buffs.trmBuff[0] = BD_adr;
    buffs.trmBuff[1] = cmd;
    buffs.trmBuff[2] = 6;
    buffs.trmBuff[3] = LO2BYTE(specNum);
    buffs.trmBuff[4] = LOBYTE(specNum);
    buffs.trmBuff[5] = LO2BYTE(startRegNo);
    buffs.trmBuff[6] = LOBYTE(startRegNo);
    buffs.trmBuff[7] = LO2BYTE(numOfReg);
    buffs.trmBuff[8] = LOBYTE(numOfReg);
    txlen = 11;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithQueue();
  }
  
   //This metod safe send comand to BD
  bool cBD::sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg, uint16_t* data)
  {
    if(busy)
      return false;
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
    return sendTxBuffWithQueue();
  }
  
   //This metod safe send comand to BD
  bool cBD::sendCmd(uint8_t cmd)
  {
    if(busy)
      return false;
    buffs.trmBuff[0] = BD_adr;
    buffs.trmBuff[1] = cmd;
    txlen = 4;
    txTry = 0;
    answTxed = false;
    return sendTxBuffWithQueue();
  }
  
  //this metod for unsafe send
  bool cBD::sendBuff(uint8_t* buff, uint16_t len)
  {
    if(busy)
      return false;
    if(len < 3 || len > buffs.constTrmBuffLen || buff == NULL)
      return false;
    memcpy(buffs.trmBuff, buff, len);
    txlen = len;
    txTry = 0;
    answTxed = false;
    buffs.trmBuff[0] = BD_adr;
    
    return sendTxBuffWithQueue();
  }
  
  bool cBD::sendTxBuffWithQueue(void)
  {
    if(sendTxBuffWithCRC())
    {
      if(queue != 0)
        queue--;
      return true;
    }
    else
      return false;
  }
  
  //this metod calculate CRC and send rxBuff with UART
  bool cBD::sendTxBuffWithCRC(void)
  {
    if(txTry >= 3)
      return false;
    CRC_class->calc(buffs.trmBuff, txlen - 2, type_CRC);
    if(!tim->stop(TIMEOUT_TIM) || (tim->start(BD_TIMEOUT, TIMEOUT_TIM) != TIMEOUT_TIM))
      timerErr = true;
    busy = (UART->txStart(buffs.trmBuff, txlen) && UART->rxStart());
    if(busy)
    {
      tim->stop(FINDE_BD_TIM);
      tim->start(1000, FINDE_BD_TIM);
    }
    return busy;
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
  
  void cBD::control(void)
  {
    answProc();
    //oledCntrl();
    
    if(!BD_id)
      findBD();
    
    if(bdInf && !busy)
    {
      uint8_t buf[7] = {0x01, 0x12, 0x04, 0xff, 0xfe, 0x00, 0x02};
      switch(startupCmds)
      {
      case 2:
          queue++;
          bdData.Inf.CurRange = 0;
          bdData.Inf.CurUnit = 0;
          doseMode = false;
          busy = false;
          sendBuff(buf, 9);
          break;
      case 1:
      case 3:
            bdData.Inf.ROMVer = 0xffff;
            doseMode = true;
            busy = false;
            sendCmd(BD_READ_CON_REG, 0, 2);
            break;
      case 0xff:
      default:
        bdInf = false;
        return;
      }
    }
    
    if(answTxed && bdInf)
    {
      
      if(buffs.rcvBuff[1] == 0x03)
      {
        bdData.Inf.CurRange = ((uint16_t)buffs.rcvBuff[3]<<8)|((uint16_t)buffs.rcvBuff[4]);
        bdData.Inf.CurUnit = ((uint16_t)buffs.rcvBuff[5]<<8)|((uint16_t)buffs.rcvBuff[6]);
        busy = false;
        queue++;
        if(startupCmds == 3)
        {
          uint8_t buf[7] = {0x01, 0x12, 0x04, 0xff, 0xfe, 0x00, 0x02};
          busy = false;
          sendBuff(buf, 9);
        }
        else
          sendCmd(BD_DIAGNOSTIC, 2, 0);
      }
      if(buffs.rcvBuff[1] == 0x12)
      {
        bdData.Inf.ROMVer = ((uint16_t)buffs.rcvBuff[3]<<8)|((uint16_t)buffs.rcvBuff[4]);
        busy = false;
        queue++;
        sendCmd(BD_DIAGNOSTIC, 2, 0);
      }
      if(buffs.rcvBuff[1] == BD_DIAGNOSTIC)
      {
        bdData.Inf.diagReg = ((uint16_t)buffs.rcvBuff[4]<<8)|((uint16_t)buffs.rcvBuff[5]);
        busy = false;
        bdInf = false;
      }
    }
    else if(BD_id && !bdInf)
    {
      
      bdCeck();
      if(tim->check(BD_CHECK_TIM) > 0  && !queue)
      {
        if(busy)
          return;
        //const uint8_t resDose[] = {0x01, 0x05, 0x00, 0x23, 0xff, 0x00};
        if(bdData.contRegs.pollIninted || bdData.dataRegs.pollIninted)
        {
            busy = false;
            queue++;
            
            if(bdData.contRegs.pollIninted && bdData.dataRegs.pollIninted)
            {
              if(!pollStart && sendCmd(BD_READ_DATA_REG, bdData.dataRegs.startReg, bdData.dataRegs.numOfRegs))
                pollStart = true;
              else
              {
                  tim->stop(BD_CHECK_TIM);
                  tim->start(pollPeriod, BD_CHECK_TIM);
              }
              checkProc = true;
            }
            else if(bdData.dataRegs.pollIninted && sendCmd(BD_READ_DATA_REG, bdData.dataRegs.startReg, bdData.dataRegs.numOfRegs))
            {
              checkProc = true;
              tim->stop(BD_CHECK_TIM);
              tim->start(pollPeriod, BD_CHECK_TIM);
            }
            else if(bdData.contRegs.pollIninted && sendCmd(BD_READ_CON_REG, bdData.contRegs.startReg, bdData.contRegs.numOfRegs))
            {
              checkProc = true;
              tim->stop(BD_CHECK_TIM);
              tim->start(pollPeriod, BD_CHECK_TIM);
            }
            else
              queue = 0; 
        }
      
      if(tim->check(FINDE_BD_TIM) > 0  && !queue && !checkProc)
      {
        bdInf = true;
        busy = false;
        queue++;
        if(sendCmd(BD_DIAGNOSTIC, 2, 0))
        {
          tim->stop(FINDE_BD_TIM);
          tim->start(1000, FINDE_BD_TIM);
        }
      }
      else if(tim->check(FINDE_BD_TIM) < 0)
      {
          tim->stop(FINDE_BD_TIM);
          tim->start(333, FINDE_BD_TIM);
      }
      }
      else if(tim->check(BD_CHECK_TIM) < 0)
      {
          tim->stop(BD_CHECK_TIM);
          tim->start(333, BD_CHECK_TIM);
      }
    }
  }
  
  bool cBD::bdCeck(void)
  {
    if(checkProc && answTxed && (buffs.rcvBuff[1] == BD_READ_DATA_REG))
    {
      if(buffs.rcvBuff[2] != bdData.dataRegs.numOfRegs * 2)
      {
        checkProc = false;
        busy = false;
        return false;
      }
      memcpy(bdData.dataRegs.regs, &buffs.rcvBuff[2], bdData.dataRegs.numOfRegs * 2);
      
      busy = false;
      queue++;
      if(bdData.contRegs.pollIninted && sendCmd(BD_READ_CON_REG, bdData.contRegs.startReg, bdData.contRegs.numOfRegs))
        ;
      else
      {
        checkProc = false;
        queue = 0;
      }
      return true;
    }
    else if(checkProc && answTxed && (buffs.rcvBuff[1] == BD_READ_CON_REG))
    {
      if(buffs.rcvBuff[2] != bdData.dataRegs.numOfRegs*2)
      {
        checkProc = false;
        busy = false;
        return false;
      }
      memcpy(&bdData.dataRegs.regs, &buffs.rcvBuff[3], bdData.dataRegs.numOfRegs * 2);
      busy = false;
      checkProc = false;
      return true;
    }
    else
      return false;
  }
  
  bool cBD::findBD(void)
  { 
    if(BD_id)
      return true;
    
    pollStart = false;
    
    if(answTxed && (buffs.rcvBuff[1] == BD_READ_ID))
    { 
      BD_id = buffs.rcvBuff[3];
      bdData.Inf.Serial = ((uint16_t)buffs.rcvBuff[5]<<8)|((uint16_t)buffs.rcvBuff[6]);
      bdData.Inf.ManMonth = buffs.rcvBuff[7];
      bdData.Inf.ManYear = buffs.rcvBuff[8];
      if( buffs.rcvBuff[2] > 10)
        reversCpy(&buffs.rcvBuff[8], &bdData.Inf.numOfCanal, 2);
      bdInf = true;
      busy = false;
      return true;
    }
    
    if((tim->check(FINDE_BD_TIM) > 0) && !busy)
    {
      if(curSpeed != 9)
        curSpeed++;
      else
      {
        curSpeed = 6;
        changeCRCType();
      }
      UART->reInit(UART_SPEEDS[curSpeed]);
      if(sendCmd(BD_READ_ID))
      {
          //checkProc = true;
          tim->stop(FINDE_BD_TIM);
          tim->start(1000, FINDE_BD_TIM);
      }
    }
    
    if(tim->check(FINDE_BD_TIM) < 0)
    {
      tim->stop(FINDE_BD_TIM);
      tim->start(2000, FINDE_BD_TIM);
    }
    
    return false;
  }
  
/*
      void cBD::oledCntrl(void)
    {
      
        char buf[100];
        
        uint16_t erval = (uint16_t)(bdData.cps_err+0.5);
	if(erval<1)
		erval = 1;
        
	const char* pFltMsk;
	if(bdData.cps_average<1)
		pFltMsk = "%.3f cps";
	else if(bdData.cps_average<10)
		pFltMsk = "%.2f cps";
	else if(bdData.cps_average<100)
		pFltMsk = "%.1f cps";
	else
		pFltMsk = "%.0f cps";
	
        cmd(CMD_DLSTART);
        
	sprintf(buf, pFltMsk, bdData.cps_average, erval);
	cmd(CLEAR_COLOR_RGB(0,0,0));
        cmd(CLEAR(1,1,1)); 
    
        cmd( COLOR_RGB(153, 204, 0) );
        cmd_text(45, 45, 30, 0, "CPS");
	cmd_text(45, 90, 30, 0, (uint8_t*)buf);
        sprintf(buf, "err %u%%", erval);
        cmd_text(45, 135, 30, 0, (uint8_t*)buf);
        
        erval = (uint16_t)(bdData.doserate_err+0.5);
	if(erval<1)
		erval = 1;
        
	if(bdData.doserate<1)
		pFltMsk = "%.3f uSv/h";
	else if(bdData.doserate<10)
		pFltMsk = "%.2f uSv/h";
	else if(bdData.doserate<100)
		pFltMsk = "%.1f uSv/h";
	else
		pFltMsk = "%.0f uSv/h";
	
        
        
	sprintf(buf, pFltMsk, bdData.doserate, erval);
        
        cmd_text(45, 180, 30, 0, "Doserate");
	cmd_text(45, 225, 30, 0, (uint8_t*)buf);
        sprintf(buf, "err %u%%", erval);
        cmd_text(45, 270, 30, 0, (uint8_t*)buf);
        
        cmd(DISPLAY());
        cmd(CMD_SWAP); 
    }
*/
  
  void cBD::reversCpy(void* valConst, void* val, uint16_t valLen)
  {
    for(int i = 0; i < valLen; i++)
      *((char*) val + i) = *((char*) valConst + (valLen - i));
  }
  
  tagBDInf cBD::getBDInfo(void)
  {
    return bdData.Inf;
  }
  
  void cBD::resetPoll(tagRegsType regType)
  {
    switch(regType)
    {
    case CON_REGS:
        bdData.contRegs.pollIninted = false;
        break;
    case DATA_REGS:
        bdData.dataRegs.pollIninted = false;
        break;
    default:
        exception(__FILE__,__FUNCTION__,__LINE__,"Vrong register type");
        return;
    }
  }
   
  void cBD::setPollRegs(tagRegsType regType,uint16_t startReg, uint16_t len)
  {
    
    tagRegs *regs;
    switch(regType)
    {
    case CON_REGS:
      regs = &bdData.contRegs;
      break;
    case DATA_REGS:
      regs = &bdData.dataRegs;
      break;
    default:
      exception(__FILE__,__FUNCTION__,__LINE__,"Vrong register type");
      return;
    }
    if(regs->pollIninted)
    {
      if(startReg < regs->startReg)
        regs->startReg = startReg;
      int32_t endRegsDiff = regs->startReg + regs->numOfRegs - startReg - len;
      if(endRegsDiff < 0)
        regs->numOfRegs += endRegsDiff * (-1);
    }
    else
    {
      regs->startReg = startReg;
      regs->numOfRegs = len;
    }
    regs->pollIninted = true;
  }
  
  void cBD::getDataFromRegs(tagRegsType regType, uint8_t regPose, int16_t* retVal)
  {
    if(retVal == NULL)
      exception(__FILE__,__FUNCTION__,__LINE__,"Vrong pointer");
    tagRegs *regs;
    switch(regType)
    {
    case CON_REGS:
      regs = &bdData.contRegs;
      break;
    case DATA_REGS:
      regs = &bdData.dataRegs;
      break;
    default:
      exception(__FILE__,__FUNCTION__,__LINE__,"Vrong register type");
      return;
    }
    
    if(regPose > regs->startReg + regs->numOfRegs || regPose < regs->startReg || !regs->pollIninted)
      exception(__FILE__,__FUNCTION__,__LINE__,"This regs isn't poll");
    
    reversCpy(&regs->regs[regPose - regs->startReg], retVal, 2);
  }
  
  void cBD::getDataFromRegs(tagRegsType regType, uint8_t regPose, int32_t* retVal)
  {
    if(retVal == NULL)
      exception(__FILE__,__FUNCTION__,__LINE__,"Vrong pointer");
    tagRegs *regs;
    switch(regType)
    {
    case CON_REGS:
      regs = &bdData.contRegs;
      break;
    case DATA_REGS:
      regs = &bdData.dataRegs;
      break;
    default:
      exception(__FILE__,__FUNCTION__,__LINE__,"Vrong register type");
      return;
    }
    
    if(regPose > regs->startReg + regs->numOfRegs || regPose < regs->startReg || !regs->pollIninted)
      exception(__FILE__,__FUNCTION__,__LINE__,"This regs isn't poll");
    
    reversCpy(&regs->regs[regPose - regs->startReg], retVal, 4);
  }
  
  void cBD::getDataFromRegs(tagRegsType regType, uint8_t regPose, float* retVal)
  {
    if(retVal == NULL)
      exception(__FILE__,__FUNCTION__,__LINE__,"Vrong pointer");
    tagRegs *regs;
    switch(regType)
    {
    case CON_REGS:
      regs = &bdData.contRegs;
      break;
    case DATA_REGS:
      regs = &bdData.dataRegs;
      break;
    default:
      exception(__FILE__,__FUNCTION__,__LINE__,"Vrong register type");
      return;
    }
    
    if(regPose > regs->startReg + regs->numOfRegs || regPose < regs->startReg || !regs->pollIninted)
      exception(__FILE__,__FUNCTION__,__LINE__,"This regs isn't poll");
    
    reversCpy(&regs->regs[regPose - regs->startReg], retVal, 4);
    //reversCpy(&regs->regs[regPose - regs->startReg + 1], retVal+2, 2);
    //memcpy(retVal, &regs->regs[regPose - regs->startReg], 4);
  }
  
  void cBD::setStartupCmds(uint8_t cmds)
  {
    startupCmds = cmds;
    bdInf = true;
  }

  void cBD::setPollPeriod(uint16_t MSTime)
  {
    pollPeriod = MSTime;
  }
  
};

