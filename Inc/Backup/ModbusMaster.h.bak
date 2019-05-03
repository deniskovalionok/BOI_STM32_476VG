#ifndef _MODBUS_MASTER_H
#define _MODBUS_MASTER_H

#ifdef __cplusplus

#include "UART_Prototype.h"
#include "AT_CRC.h"
#include "UARTBuffs.h"
#include "timer.h"

namespace MODBUS
{
  /*
  struct tagBDData{
    uint32_t    cps_mom;
    float       cps_average;
    float       cps_err;
    float       doserate;
    float       doserate_err;
    float       dose;
    uint16_t    cps_dev;
    uint32_t    mesh_time;
    float       dose_full;
  };
  */
  struct tagBDInf{
    uint16_t    Serial;
    uint8_t     ManMonth;
    uint8_t     ManYear;
    uint16_t    ROMVer;
    uint16_t    CurRange;
    uint16_t    CurUnit;
    uint16_t    diagReg;
    uint16_t    numOfCanal;
  };
  
  struct tagRegs{
    uint16_t    regs[120];
    uint16_t    startReg;
    uint16_t    numOfRegs;
    bool pollIninted;
  };
  
  struct tagBDData{
    tagBDInf    Inf;
    uint8_t     binSign;
    tagRegs     contRegs;
    tagRegs     dataRegs;
    
    bool isReadSpec;
    bool supCompSpec;
  };
  
  enum tagRegsType{
    NULL_REG,
    CON_REGS,
    DATA_REGS
  };
  
  class cBD : protected UART::classWithBoobs
  {
  public:
    explicit cBD(uint8_t* rxBuff,// This is safe rx Buffer
                uint8_t* txBuff,//This buffer ned for resend
                uint16_t rxBuffLen,// len of rxBuff
                uint16_t txBuffLen,// len of txBuff
                UART::cUART_interfacePrototype* UART, //interface for UART
                TIMER::cMS_timer_prototype* tim
                  );
    
    //This metod safe send comand to BD
    bool sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg);
    bool sendCmd(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg, uint16_t* data);
    bool sendCmd(uint8_t cmd);
    
    bool sendCmdWithMunOfBytes(uint8_t cmd, uint16_t startRegNo, uint16_t numOfReg);
    bool sendCmdWithMunOfBytes(uint8_t cmd, uint16_t specNum, uint16_t startRegNo, uint16_t numOfReg);
    
    bool isBusy(void);
    
    uint16_t getAnswLen(void);
    
    bool getAnsw(uint8_t* buffer, uint16_t len);
    
    //this metod for unsafe send
    bool sendBuff(uint8_t* buff, uint16_t len);
    
    void toQeue(void);
    
    void control(void);
    
    uint8_t getBD_ID(void);
    
    bool findBD(void);
    
    //void oledCntrl(void);
    
    tagBDInf getBDInfo(void);
    
    void resetPoll(tagRegsType regType);
    
    void setPollRegs(tagRegsType regType, uint16_t startReg, uint16_t len);
    
    void readSpec(bool isCompSpec);
    
    void getDataFromRegs(tagRegsType regType, uint8_t regPose, int16_t* retVal);
    
    void getDataFromRegs(tagRegsType regType, uint8_t regPose, int32_t* retVal);
    
    void getDataFromRegs(tagRegsType regType, uint8_t regPose, float* retVal);
    
    void setStartupCmds(uint8_t cmds);
    
    void setPollPeriod(uint16_t MSTime);
    
    void reversCpy(void* valConst, void* val, uint16_t valLen);
    
  protected:
    //this metod calculate CRC and send rxBuff with UART
    bool sendTxBuffWithCRC(void);
    
    bool sendTxBuffWithQueue(void);
    
    bool bdCeck(void);
    
    void errorProc(void);
    
    bool checkProc;
    
    //this metod change CRC tupe for another
    CRC_USR::tagCRC_type changeCRCType(void);
    
    bool answProc(void);
    
    uint16_t queue;
    
    bool doseMode;
    
    CRC_USR::cCRC* CRC_class;//class for CRC calculate
    
    uint16_t txlen;//len of transmited data
    
    uint16_t rxlen;//len of recived data
    
    UART::cUART_interfacePrototype* UART;//UART interface class
    
    uint8_t BD_adr;
    
    uint8_t txTry;//number of wrong trys to send data
    
    bool answTxed;
    
    bool timerErr;
    
    bool bdInf;
    
    uint8_t BD_id;
    
    uint8_t startupCmds;
    
    bool busy;
    
    bool pollStart;
    
    uint8_t curSpeed;
    
    uint16_t pollPeriod;
    
    //tagBDInf BDInf;
    
    tagBDData bdData;
    
    TIMER::cMS_timer_prototype* tim;
    UART::tagRxErrors rxErrors : 2; 
    CRC_USR::tagCRC_type type_CRC : 1;//type of CRC16 using in this BD may be AT or MB 
  };
  
};

#endif  //#ifdef __cplusplus
#endif	//#ifndef _MODBUS_MASTER_H