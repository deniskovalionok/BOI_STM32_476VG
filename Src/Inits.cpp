#include "Inits.h"

#include "usart.h"
#include "timer.h"
#include "UART_Usr.h"
#include "ModbusSlave.h"
#include "Mode_prototype.h"
#include "test.h"
#include "keys.h"
#include "rtc.h"
#include "clock.h"
#include "Info_Mode.h"
#include "BD_mode.h"
#include "Journal.h"
#include "geiger.h"
#include "gpio.h"

#include "gps.h"

using namespace MODBUS;
using namespace TIMER;
using namespace UART;
using namespace MODES;
using namespace KEYBORD;
using namespace GPS;

// буфера для UARTов
// для UARTов и их обработчиков можно использовать общие буфера
// правдо для работы в режиме slave лучше использовать разные для возможности приема собщений во время обработки старых
uint8_t buffer1[1024];
uint8_t buffer2[1024];
uint8_t buffer3[1024];
uint8_t buffer4[1024];
uint8_t buffer5[1024];
uint8_t buffer6[1024];
uint8_t buffer7[1024];
uint8_t buffer8[1024];

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart5;
extern TIM_HandleTypeDef htim7;

extern cSuperTimer testTim;

//инициализация классов
cUART iUSB(buffer1, buffer2, 1024, 1024, &huart2, MB_TIMEOUT);
cUART iBD(buffer3, buffer4, 1024, 1024, &huart5, MB_TIMEOUT);
tagSTimer timBDStr[3];
cSuperTimer timBD(timBDStr, 3);
cBD BD(buffer3, buffer4, 1024, 1024, &iBD, &timBD);
tagBDs bdStr;
cPC USB(buffer5, buffer6, 1024, 1024, &iUSB, &bdStr, 1, 0x02);
cTest_mode test((char*)"Test1");
cTest_mode test2((char*)"Test2");
cInfo_mode infoMode;
cJournal_mode journal_mode;

cGM_mode gey;

tagSTimer confTimStr;
cSuperTimer timDiag(&confTimStr, 1);

tagSTimer specTimStr;
cSuperTimer timSpec(&confTimStr, 1);

cBD_Spec_mode specMode(&BD, &timSpec);

cBD_config conf(&BD, &timDiag, &specMode);

cBD_mode bd_mode(&BD, &conf);

cGPS_mode GPS_mode;

// классы кнопок
extern cKey* rKey;
extern cKey* lKey;
extern cKey* enterKey;
extern cKey* onKey;

bool allInited = false;
  
tagSTimer keyTimStr[4];
cSuperTimer timKeys(keyTimStr, 4);

cKey keyR(&timKeys, 0);
cKey keyL(&timKeys, 1);
cKey keyE(&timKeys, 2);
cKey keyO(&timKeys, 3);

uint16_t timCount = 0;
  
void tTest(void)
{
   timCount++;
   /*
   if(timCount%2 == 0)
     HAL_GPIO_WritePin(NRES_GPS_GPIO_Port, NRES_GPS_Pin, GPIO_PIN_RESET);
   else
     HAL_GPIO_WritePin(NRES_GPS_GPIO_Port, NRES_GPS_Pin, GPIO_PIN_SET);
    */
}

cUART iGPS(buffer7, buffer8, 1024, 1024, &huart4, MB_TIMEOUT);

void GPS_power(bool onOff)
{
  HAL_GPIO_WritePin(NGPS_ON_GPIO_Port, NGPS_ON_Pin, onOff?GPIO_PIN_RESET:GPIO_PIN_SET);
  HAL_GPIO_WritePin(NRES_GPS_GPIO_Port, NRES_GPS_Pin, GPIO_PIN_SET);
}

cGPS gpsClass(buffer7, buffer8, 1024, 1024, &iGPS, GPS_power);

//~~~~~~~~~~~~~~~~~~~~~~~RTC functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

RTC_TimeTypeDef rTime;
RTC_DateTypeDef rDate;

extern RTC_HandleTypeDef hrtc;

tagRTC_time getTime(void)
{
  HAL_RTC_GetTime(&hrtc, &rTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &rDate, RTC_FORMAT_BIN);
  tagRTC_time curTime;
  curTime.Year = rDate.Year;
  curTime.Date = rDate.Date;
  curTime.Month = rDate.Month;
  curTime.Hours = rTime.Hours;
  curTime.Minutes = rTime.Minutes;
  curTime.Seconds = rTime.Seconds;
  return curTime;
}

//этот файл связывает с и с++

void info(void)
{
  rKey = &keyR;
  lKey = &keyL;
  enterKey = &keyE;
  onKey = &keyO;
  
  modeKarusel[0] = &bd_mode;
  modeKarusel[1] = &gey;
  modeKarusel[2] = &GPS_mode;
  modeKarusel[3] = &specMode;
  
  currentMode = &infoMode;
  currentMode->show();
}

// инициализация всего.
// перед while(1) в main
void BD_init(void)
{
    bdStr.BD = &BD;
    bdStr.BDAdr = 0x01;
    currentMode = modeKarusel[0];
    mode2 = &specMode;
    currentMode->show();
    testTim.start(1000, 0, &tTest, true);
    allInited = true;
    geigerInit();
    gpsClass.init(true);
    //conf.createFileTestBDKG04();
    //conf.createFileTestBDKG11();
    //conf.createFileTestBDKG11txt();
    //conf.createFileTestBDPA();
    //conf.createFileTestBDPB();
    //uint8_t ansBuff;
    /*
    BD.sendCmd(0x05, 0x24, 0x0000); 
    do
      BD.control();
    while(!BD.getAnsw(&ansBuff, 1));
    */
}


// это крутится в мэйне
void controlFuncs(void)
{
  BD.control();
  USB.control();
  currentMode->control();
  gpsClass.control();
  conf.control();
  //TestMode.control();
}

void keysCheck(void)
{
  if(allInited)
  {
      rKey->intControl(HAL_GPIO_ReadPin(GPIOC, KEY_R_Pin) == GPIO_PIN_SET);
      lKey->intControl(HAL_GPIO_ReadPin(GPIOC, KEY_L_Pin) == GPIO_PIN_SET);
      enterKey->intControl(HAL_GPIO_ReadPin(GPIOC, KEY_ENTER_Pin) == GPIO_PIN_SET);
      onKey->intControl(HAL_GPIO_ReadPin(KEY_ON_P_GPIO_Port, KEY_ON_P_Pin) == GPIO_PIN_SET);
  }
}

/*
// обработчики прерываний кнопок для каналов 5-9
void exti_it1(void)
{
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != RESET && rKey != NULL)
      rKey->intControl();
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_7) != RESET && enterKey != NULL)
      enterKey->intControl();
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET && lKey != NULL)
      lKey->intControl();
}

//обработчик прерываний кнопки для 10-го канала (кнопка включения)
void exti_it2(void)
{
  if(onKey != NULL)
    onKey->intControl();
}

// обработчик прерывания для TIM7
// выключает TIM7 если он не используется в globalMkSTimer
void mks_it(void)
{
  bool needTim = false;
  for(int i = 0; i < 4; i++)
  {
    if(globalMkSTimer.check(i) == 0)
      needTim = true;
    else
    {
      switch(i)
      {
      case 0:
        rKey->setClick(HAL_GPIO_ReadPin(GPIOC, KEY_R_Pin));
        break;
      case 1:
        lKey->setClick(HAL_GPIO_ReadPin(GPIOC, KEY_L_Pin));
        break;
      case 2:
        enterKey->setClick(HAL_GPIO_ReadPin(GPIOC, KEY_ENTER_Pin));
        break;
      case 3:
        onKey->setClick(HAL_GPIO_ReadPin(KEY_ON_P_GPIO_Port, KEY_ON_P_Pin));
        break;
      default:
        break;
      }
    }
  }
  
  if(!needTim)
    HAL_TIM_Base_Stop_IT(&htim7);
  
}
*/