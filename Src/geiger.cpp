#include "Mode_prototype.h"
#include "geiger.h"
#include "timer.h"
#include "tim.h"
#include <string.h>
#include <stdio.h>
#include "TestResetDlg.h"

#include "exeptions.h"

static uint32_t gmMomCps = 0;
static uint32_t gmCps = 0;
static uint32_t gmMeshTime = 0;
static uint32_t gmCpsBG = 0;
static float gmD = 0;
static float gmK = 0;
static float gmDose = 0;
static float gmErr = 0;
using namespace TIMER;

tagSTimer timGMStr;
cSuperTimer timGM(&timGMStr, 1);

olAverageReset geigReset;

extern TIM_HandleTypeDef htim15;

uint32_t gmTest = 0;


namespace MODES
{
  
  tagMenu gmMenu[4];
  
  cChangeValue changeCpsBG((char*)"Cange N(bg)", &gmCpsBG, 4, 9999, 0);
  cChangeValue changeGMD((char*)"Change d", &gmD, 1, 3, 0.999, 0);
  cChangeValue changeGMK((char*)"Change K", &gmK, 3, 3, 999.999, 0);
  
  void changeNBG(void)
  {
    currentMode->showMessage(&changeCpsBG);
  }
  
  void changeD(void)
  {
    currentMode->showMessage(&changeGMD);
  }
  
  void changeK(void)
  {
    currentMode->showMessage(&changeGMK);
  }
  
  cGM_mode::cGM_mode(void)
  {
    InterfaceColor.red = 222;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 222;
    FontColor.blue = 0;
    strcpy((char*)onButtonText, "menu");
    strcpy((char*)modeName, "Geiger");
    
    strcpy((char*)gmMenu[0].name, "Change K");
    strcpy((char*)gmMenu[1].name, "Change d");
    sprintf((char*)gmMenu[2].name, "Change N(bg)");
    strcpy((char*)gmMenu[3].name, "Restart");
    for(int i =0; i < 4; i++)
    {
      gmMenu[i].isShow = true;
    }
    gmMenu[0].func = changeK;
    gmMenu[1].func = changeD;
    gmMenu[2].func = changeNBG;
    gmMenu[3].func = geigerInit;
    
    rKeyState = true;
    lKeyState = true;
    eKeyState = true;
    oKeyState = true;
    
    strcpy((char*)rightButtonText, "GPS");
    strcpy((char*)leftButtonText, "BD");
  }
  
  void cGM_mode::onShow(void)
  {
    if(!gmMeshTime)
      return;
    char buf[40];
    sprintf(buf, "Dose: %.3f", gmDose);
    cmd_text(5, 80, 28, 0, (uint8_t*)buf);
    //float tempVal = gmCps/gmMeshTime;
    sprintf(buf, "CPS: %.3f", (float)gmCps/gmMeshTime);
    cmd_text(5, 100, 28, 0, (uint8_t*)buf);
    
    sprintf(buf, "Error: %.0f%%", gmErr);
    cmd_text(5, 120, 28, 0, (uint8_t*)buf);
  }
  
  void cGM_mode::userControl(void)
  {
    
  }
  
  void cGM_mode::enterKeyAction(void)
  {
    
  }
  
  void cGM_mode::rKeyAction(void)
  {
    change(modeKarusel[2]);
  }
  
  void cGM_mode::lKeyAction(void)
  {
    change(modeKarusel[0]);
  }
  
  void cGM_mode::onKeyAction(void)
  {
     openMenu(gmMenu, 4);
  }
}

void read(void)
{
  gmMomCps =  __HAL_TIM_GET_COUNTER(&htim15);
  htim15.Instance->CNT = 0;
  gmMomCps += gmTest;
  gmTest = 0;
  gmCps += gmMomCps;
  //if(!gmMomCps)
    //return;
  gmMeshTime++;
  gmDose = ((gmCps/gmMeshTime - gmCpsBG) * gmK)/(1 - gmD * (gmCps/gmMeshTime));
  gmErr = 200/sqrt((float)gmCps);
  BOOL voidBool;
      if(geigReset.update(gmMomCps, &voidBool))
        geigerInit();
}

float geigerGetCPS(void)
{
  return (float)gmCps/gmMeshTime;
}

float geigerGetErr(void)
{
  return gmErr;
}

float geigerGetDose(void)
{
  return gmDose;
}


void geigerInit(void)
{
    gmMomCps = 0;
    gmCps = 0;
    gmMeshTime = 0;
    gmDose = 0;
    gmErr = 0;
    geigReset.reset();
    timGM.start(1000, 0, &read, true);
    HAL_TIM_Base_Start(&htim15);
}