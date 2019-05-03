//keyboard control
//31/10/2008 Bystrov

//#include <stdio.h>
#include <ctype.h>
//#include <inarm.h>

#include "common.h"
#include "keyboard.h"
#include "types.h"
#include "syncObj.h"
#include "sound.h"
#include "powerControl.h"
#include "modes.h"
#include "interproc.h"
#include "display.h"
#include "geiger.h" // for call to geiger interrupt handler



struct tagKeyboardData volatile keyboard;
struct tagKeyboardData volatile keyboard_safe;




void KeyboardControl_copyKeyboardDataToSafePlace(void)
{
	SyncObj_memcpy_safe((BYTE*)&keyboard_safe, (const BYTE*)&keyboard, sizeof(struct tagKeyboardData));
}


BOOL KeyboardControl_testKeyDown(void)
{
    return (Chip_GPIO_GetPinState(LPC_GPIO, PORT_KEY1, PIN_KEY1) == 0);
}

BOOL KeyboardControl_testKeyLeft(void)
{
  return (Chip_GPIO_GetPinState(LPC_GPIO, PORT_KEY4, PIN_KEY4) == 0);
}

BOOL KeyboardControl_testKeyRight(void)
{
  return (Chip_GPIO_GetPinState(LPC_GPIO, PORT_KEY2, PIN_KEY2) == 0);
}

BOOL KeyboardControl_testKeyUp(void)
{
  return (Chip_GPIO_GetPinState(LPC_GPIO, PORT_KEY3, PIN_KEY3) == 0);
}

void KeyboardControl_init(void)
{
	NVIC_DisableIRQ(GPIO_IRQn);
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, 0xf);
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT2, 0xf);

	KeyboardControl_keyboardData_Init();
}

void TIMER0_IRQHandler()
{
	_INT_Keyboard_Timer0();
}

//interrupt from timer0
void _INT_Keyboard_Timer0(void)
{
  	unsigned int mask = 0;
  	unsigned int irq_status;
  	irq_status = Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT2);
	if(Chip_TIMER_MatchPending(LPC_TIMER0, 0))
	{//long term holding after first click
	
		//check if button still pressed then enable int for holding
		//if no then disable timer0 and change polarity for INT3.
		if(!KeyboardControl_anyKeyPressed_hardware_intcall())
		{//no key is pressed
			KeyboardControl_turnOFF();
			//clear interrupt status
			Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, irq_status);
			NVIC_EnableIRQ(GPIO_IRQn); // necessary?
		}else
		{
			KeyboardControl_refresh_keyboard_status_intcall(1/*reset processed if key is holding*/, KEYBOARD_FIRST_PUSH_TIME);
			Chip_TIMER_MatchDisableInt(LPC_TIMER0, 0);
			Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);
			Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);
			Chip_TIMER_Reset(LPC_TIMER0);
		}
		mask |= _BIT(0);
		
	}else
	if(Chip_TIMER_MatchPending(LPC_TIMER0, 1))
	{//short term for next holding button
	
		if(!KeyboardControl_anyKeyPressed_hardware_intcall())
		{//no key is pressed
			KeyboardControl_turnOFF();
			//clear interrupt status
			Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, irq_status);
			NVIC_EnableIRQ(GPIO_IRQn);
		}else
		{
			KeyboardControl_refresh_keyboard_status_intcall(1/*reset processed if key is holding*/, KEYBOARD_OTHER_PUSH_TIME);
		}
		mask |= _BIT(1);
	}else
	if(Chip_TIMER_MatchPending(LPC_TIMER0, 2))
	{//catch first click
	  	
		if(!KeyboardControl_anyKeyPressed_hardware_intcall())
		{//no key is pressed
			KeyboardControl_turnOFF();
			//clear interrupt status
			Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, irq_status);
			NVIC_EnableIRQ(GPIO_IRQn);
		}else
		{
			KeyboardControl_refresh_keyboard_status_intcall(1/*reset processed if key is holding*/, 0);
			Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);
			Chip_TIMER_ResetOnMatchDisable(LPC_TIMER0, 0);
			Chip_TIMER_StopOnMatchDisable(LPC_TIMER0, 0);
			
			Chip_TIMER_MatchDisableInt(LPC_TIMER0, 1);
			Chip_TIMER_ResetOnMatchDisable(LPC_TIMER0, 1);
			Chip_TIMER_StopOnMatchDisable(LPC_TIMER0, 1);
			
			Chip_TIMER_MatchDisableInt(LPC_TIMER0, 2);
			Chip_TIMER_ResetOnMatchDisable(LPC_TIMER0, 2);
			Chip_TIMER_StopOnMatchDisable(LPC_TIMER0, 2);
			
			Chip_TIMER_Reset(LPC_TIMER0);
			
			soundControl.bPlayClick = TRUE;
		
		}
		mask |= _BIT(2);
	}else
	if(Chip_TIMER_MatchPending(LPC_TIMER0, 3))
	{
		//clear TIMER0 interrupt status
	  	Chip_TIMER_SetMatch(LPC_TIMER0, 3, LPC_TIMER0->TC + 1);
		//50 ms for process catch first click
		if(!KeyboardControl_anyKeyPressed_hardware_intcall())
		{//no key is pressed
			KeyboardControl_turnOFF();
			//clear p2_12 interrupt status
			Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, irq_status);
			NVIC_EnableIRQ(GPIO_IRQn);
		}

		mask |= _BIT(3);
	}
	//clear TIMER0 interrupt status
	LPC_TIMER0->IR = mask;

}




void KeyboardControl_turnOFF(void)
{
  	Chip_TIMER_Disable(LPC_TIMER0);
}


//restore default state, as no key is pressed
void KeyboardControl_restoreKeyboard(void)
{
	//снимем обработку нажатой кнопки, чтобы пропустить первое нажатие после выход из повердауна
	KeyboardControl_turnOFF();
	PowerControl_sleep(100);
	SAFE_DECLARE; __disable_irq(); 
	keyboard.keyDown.bProcessed=1;//set key as processed
	keyboard.keyUp.bProcessed=1;//set key as processed
	keyboard.keyLeft.bProcessed=1;//set key as processed
	keyboard.keyRight.bProcessed=1;//set key as processed
	NVIC_DisableIRQ(GPIO_IRQn);
	__enable_irq();
	PowerControl_sleep(100);
	NVIC_EnableIRQ(GPIO_IRQn);
	LPC_SYSCTL->EXTINT |= _BIT(_EINT2);
}



void GPIO_IRQHandler(void)
{
	if (Chip_GPIOINT_IsIntPending(LPC_GPIOINT, GPIOINT_PORT2))
		_INT_Keyboard();
	else
		_INT_Geiger();
}

//keyboard interrupts, called from supervisor mode!!!
//run under high level! so we need disable int2 after processing
void _INT_Keyboard(void)
{//resuming after idle and powerdown mode.

	unsigned int irq_status;
	irq_status = Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT2);
	if(KeyboardControl_anyKeyPressed_hardware_intcall())
	{
		KeyboardControl_turnON();
		Chip_TIMER_Enable(LPC_TIMER0);
		NVIC_DisableIRQ(GPIO_IRQn);
		powerControl.bAwakedByKeyboard = 1;	//awaken by key pushing
	}
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT2, irq_status);
	//clear INT2 interrupt status
	// LPC_SYSCTL->EXTINT |= _BIT(_EINT2);
}

//test for any key pressed by hardware GPIO
BOOL KeyboardControl_anyKeyPressed_hardware_intcall(void)
{
  BOOL retVal=0;
  retVal |= KeyboardControl_testKeyDown()|KeyboardControl_testKeyLeft()|KeyboardControl_testKeyRight()|KeyboardControl_testKeyUp();
  return retVal;
}

//test for any key pressed (only by keyboard data not by hardware GPIO)
BOOL KeyboardControl_anyKeyPressed_safe(void)
{
  BOOL retVal=0;
  retVal = (keyboard_safe.keyUp.bPressed & !keyboard_safe.keyUp.bProcessed)|
    (keyboard_safe.keyDown.bPressed & !keyboard_safe.keyDown.bProcessed)|
    (keyboard_safe.keyLeft.bPressed & !keyboard_safe.keyLeft.bProcessed)|
    (keyboard_safe.keyRight.bPressed & !keyboard_safe.keyRight.bProcessed);
  return retVal;
}



//init keyboard data
void KeyboardControl_keyboardData_Init(void)
{
  keyboard.keyUp.bPressed = 0;
  keyboard.keyUp.pressedTime = 0;
  keyboard.keyUp.bProcessed = 0;
  keyboard.keyDown.bPressed = 0;
  keyboard.keyDown.pressedTime = 0;
  keyboard.keyDown.bProcessed = 0;
  keyboard.keyLeft.bPressed = 0;
  keyboard.keyLeft.pressedTime = 0;
  keyboard.keyLeft.bProcessed = 0;
  keyboard.keyRight.bPressed = 0;
  keyboard.keyRight.pressedTime = 0;
  keyboard.keyRight.bProcessed = 0;
//keyboard.bStatusUpdated = FALSE;
}




//first init keyboard control
void KeyboardControl_turnON(void)
{
  	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER0); // Timer 0 power ON
	
	Chip_TIMER_Disable(LPC_TIMER0);
	
	//adjust predivider
	Chip_TIMER_PrescaleSet(LPC_TIMER0, HW_FREQ/1000*KEYBOARD_TIMER_VAL-1);
	//при частоте шины железа 9216000 √ц это значение предделител€ даст нам 50мс циклы инкремента таймера
	Chip_TIMER_Reset(LPC_TIMER0);

	LPC_TIMER0->PC = 0; // function?
	
	//adjust comparators 0 and 1
	Chip_TIMER_SetMatch(LPC_TIMER0, 0, KEYBOARD_FIRST_PUSH_TIME/KEYBOARD_TIMER_VAL-1);
	Chip_TIMER_SetMatch(LPC_TIMER0, 1, KEYBOARD_OTHER_PUSH_TIME/KEYBOARD_TIMER_VAL-1);
	Chip_TIMER_SetMatch(LPC_TIMER0, 2, KEYBOARD_OTHER_PUSH_TIME/KEYBOARD_TIMER_VAL-1);

	//adjust action on comparators 0 and 1
	LPC_TIMER0->MCR = 0;
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 2);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 3);
	Chip_TIMER_Reset(LPC_TIMER0);
	Chip_TIMER_SetMatch(LPC_TIMER0, 3, LPC_TIMER0->TC+1);
	//100 ms for process catch first click
}


//refresh all keys status
//if milisec_addon==0 then reset timers to 0 else it is addon
void KeyboardControl_refresh_keyboard_status_intcall(BOOL resetProcessedAnyWay, DWORD milisec_addon)
{
  BOOL val;

  val = KeyboardControl_testKeyUp();
  if(val && (!keyboard.keyUp.bPressed || resetProcessedAnyWay))
    keyboard.keyUp.bProcessed = 0;  //reset processed flag if it is first click
  keyboard.keyUp.bPressed = val;
  if(val && milisec_addon)
     keyboard.keyUp.pressedTime+=milisec_addon;
  else
     keyboard.keyUp.pressedTime=milisec_addon;

  val = KeyboardControl_testKeyRight();
  if(val && (!keyboard.keyRight.bPressed || resetProcessedAnyWay))
    keyboard.keyRight.bProcessed = 0;  //reset processed flag if it is first click
  keyboard.keyRight.bPressed = val;
  if(val && milisec_addon)
     keyboard.keyRight.pressedTime+=milisec_addon;
  else
     keyboard.keyRight.pressedTime=milisec_addon;

  val = KeyboardControl_testKeyLeft();
  if(val && (!keyboard.keyLeft.bPressed || resetProcessedAnyWay))
    keyboard.keyLeft.bProcessed = 0;  //reset processed flag if it is first click
  keyboard.keyLeft.bPressed = val;
  if(val && milisec_addon)
     keyboard.keyLeft.pressedTime+=milisec_addon;
  else
     keyboard.keyLeft.pressedTime=milisec_addon;

  val = KeyboardControl_testKeyDown();
  if(val && (!keyboard.keyDown.bPressed || resetProcessedAnyWay))
    keyboard.keyDown.bProcessed = 0;  //reset processed flag if it is first click
  keyboard.keyDown.bPressed = val;
  if(val && milisec_addon)
     keyboard.keyDown.pressedTime+=milisec_addon;
  else
     keyboard.keyDown.pressedTime=milisec_addon;

 // keyboard.bStatusUpdated = TRUE;

}


//test key pressed and not processed
BOOL KeyboardControl_keyUpPressed_safe(void)
{
  return (SAFE_GET_DWORD(&keyboard_safe.keyUp.bPressed) & !SAFE_GET_DWORD(&keyboard_safe.keyUp.bProcessed));
}
BOOL KeyboardControl_keyDownPressed_safe(void)
{
  return (SAFE_GET_DWORD(&keyboard_safe.keyDown.bPressed) & !SAFE_GET_DWORD(&keyboard_safe.keyDown.bProcessed));
}
BOOL KeyboardControl_keyLeftPressed_safe(void)
{
  return (SAFE_GET_DWORD(&keyboard_safe.keyLeft.bPressed) & !SAFE_GET_DWORD(&keyboard_safe.keyLeft.bProcessed));
}
BOOL KeyboardControl_keyRightPressed_safe(void)
{
  return (SAFE_GET_DWORD(&keyboard_safe.keyRight.bPressed) & !SAFE_GET_DWORD(&keyboard_safe.keyRight.bProcessed));
}
BOOL KeyboardControl_keyUpPressedOnly_safe(void)
{
	return (KeyboardControl_keyUpPressed_safe() && !KeyboardControl_keyDownPressed_safe() && !KeyboardControl_keyLeftPressed_safe() && !KeyboardControl_keyRightPressed_safe());
}
BOOL KeyboardControl_keyDownPressedOnly_safe(void)
{
	return (!KeyboardControl_keyUpPressed_safe() && KeyboardControl_keyDownPressed_safe() && !KeyboardControl_keyLeftPressed_safe() && !KeyboardControl_keyRightPressed_safe());
}
BOOL KeyboardControl_keyLeftPressedOnly_safe(void)
{
	return (!KeyboardControl_keyUpPressed_safe() && !KeyboardControl_keyDownPressed_safe() && KeyboardControl_keyLeftPressed_safe() && !KeyboardControl_keyRightPressed_safe());
}
BOOL KeyboardControl_keyRightPressedOnly_safe(void)
{
	return (!KeyboardControl_keyUpPressed_safe() && !KeyboardControl_keyDownPressed_safe() && !KeyboardControl_keyLeftPressed_safe() && KeyboardControl_keyRightPressed_safe());
}


//ret 1 if some key was processed else 0
BOOL KeyboardControl_keyboardControl(void)
{
/*	
	//added 10/02/08 for better control keyboard
	if(!keyboard.bStatusUpdated)
	{//status is not changed
		PowerControl_gotoIdleMode();//we can go idle if no keys pressed
		return FALSE;
	}
	keyboard.bStatusUpdated = FALSE;
	//
	*/
	
	KeyboardControl_copyKeyboardDataToSafePlace();

	BOOL ret = 0;	//no key was processed by default

	if(!KeyboardControl_anyKeyPressed_safe())
	{//nothing pressed, no timer is reached and we can go to idle mode
	//!!!!!!!!!!! подводный камень!!!!!!!!!!!!
	//возможна ситуаци€ когда в основном цикле до перехода в idle но после проверки нажати€ кнопки происходит прерывание нажати€ кнопки,
	//и мы его не обрабатываем или обрабатываем но все равно переходим в idle при этом не обработав само нажатие
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		//!!!!!!!!!!!!
		//before idle mode we must check do we need send some cmd to proc2? if yes then NO IDLE
		if(!modeControl.dwTimerReach && //no softtimers reached
		   !interProcControl.bTimeOutReached)	//have to answer on request
			PowerControl_gotoIdleMode();//we can go idle if no keys pressed
		
	}else
	{//pressed any key, must be handled

		if(!display.bLCDON)
		{//first click when LCD OFF, pass it
			Display_turnON();
			//added from 10/02/2011
			modeControl.bMenuON = 0;	//turn off menu by default when exit LCD off mode, else void menu will be showed
			//
			Modes_updateMode();
			KeyboardControl_restoreKeyboard();
			return TRUE;
		}
		
		if(soundControl.bPlayClick)
		{
			soundControl.bPlayClick = FALSE;
			if(soundControl.bSound==SNDST_SOUND)
				SoundControl_Beep(40, SOUND_FREQ_1_SI*8);
		}
		
		//process power off and power down, also once click on button
		if(KeyboardControl_keyDownPressed_safe())
		{
			DWORD ptm = SAFE_GET_DWORD(&keyboard.keyDown.pressedTime);
			if(ptm>TIME_TO_ENTER_POWERDOWN_BY_KEY)
			{
#ifdef BNC	
				//turn off mode
					PowerControl_turnOFF_device("RELEASE THE KEY\0""RELEASE THE KEY\0""RELEASE THE KEY\0""ќ“ѕ”—“»“≈  Ќќѕ ”");
#else //#ifdef BNC	
				//!!!!!!!!!				playSample(SI_TURNOFF, 1);
				if(!SAFE_GET_DWORD(&keyboard.keyLeft.bPressed))
				{//power down mode
					PowerControl_gotoPowerDownMode();
				}else
				{//turn off mode
					PowerControl_turnOFF_device("RELEASE THE KEY\0""RELEASE THE KEY\0""RELEASE THE KEY\0""ќ“ѕ”—“»“≈  Ќќѕ ”");
				}
#endif	//#ifdef BNC	
			}else if(ptm==0)
			{//process once click here
				SAFE_SET(keyboard.keyDown.bProcessed, 1);//set key as processed
				Modes_OnDown();

			}
		}else
		{//next keys will be processed only when down key is unpressed
		
			if(KeyboardControl_keyUpPressedOnly_safe())
			{
				SAFE_SET(keyboard.keyUp.bProcessed, 1); //set key as processed
				Modes_OnUp();
			}
			if(KeyboardControl_keyLeftPressedOnly_safe())
			{
				SAFE_SET(keyboard.keyLeft.bProcessed, 1);//set key as processed
				Modes_OnLeft();
			}
			if(KeyboardControl_keyRightPressedOnly_safe())
			{
				SAFE_SET(keyboard.keyRight.bProcessed, 1);//set key as processed
				Modes_OnRight();
			}
		}
		ret = 1;	//key was processed
	}
	return ret;
}
