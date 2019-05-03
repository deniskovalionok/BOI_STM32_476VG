/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CC_CNTRL_Pin GPIO_PIN_3
#define CC_CNTRL_GPIO_Port GPIOE
#define NBCD_Pin GPIO_PIN_5
#define NBCD_GPIO_Port GPIOE
#define NPOWER_EN_Pin GPIO_PIN_6
#define NPOWER_EN_GPIO_Port GPIOE
#define RTC_OSC_I_Pin GPIO_PIN_14
#define RTC_OSC_I_GPIO_Port GPIOC
#define RTC_OSC_O_Pin GPIO_PIN_15
#define RTC_OSC_O_GPIO_Port GPIOC
#define OSC_I_Pin GPIO_PIN_0
#define OSC_I_GPIO_Port GPIOH
#define OSC_O_Pin GPIO_PIN_1
#define OSC_O_GPIO_Port GPIOH
#define T_VCC_BD_Pin GPIO_PIN_0
#define T_VCC_BD_GPIO_Port GPIOC
#define DRIVE_0_Pin GPIO_PIN_2
#define DRIVE_0_GPIO_Port GPIOC
#define NSLEEP_Pin GPIO_PIN_3
#define NSLEEP_GPIO_Port GPIOC
#define RX_GPS_Pin GPIO_PIN_0
#define RX_GPS_GPIO_Port GPIOA
#define TX_GPS_Pin GPIO_PIN_1
#define TX_GPS_GPIO_Port GPIOA
#define RXD_USB_Pin GPIO_PIN_2
#define RXD_USB_GPIO_Port GPIOA
#define TXD_USB_Pin GPIO_PIN_3
#define TXD_USB_GPIO_Port GPIOA
#define NRES_GPS_Pin GPIO_PIN_4
#define NRES_GPS_GPIO_Port GPIOA
#define SCK_EVE_Pin GPIO_PIN_5
#define SCK_EVE_GPIO_Port GPIOA
#define MOSI_EVE_Pin GPIO_PIN_6
#define MOSI_EVE_GPIO_Port GPIOA
#define MISO_EVE_Pin GPIO_PIN_7
#define MISO_EVE_GPIO_Port GPIOA
#define CSN_EVE_Pin GPIO_PIN_4
#define CSN_EVE_GPIO_Port GPIOC
#define INT_N_Pin GPIO_PIN_5
#define INT_N_GPIO_Port GPIOC
#define NCSB_Pin GPIO_PIN_0
#define NCSB_GPIO_Port GPIOB
#define PD_N_Pin GPIO_PIN_7
#define PD_N_GPIO_Port GPIOE
#define NGPS_ON_Pin GPIO_PIN_8
#define NGPS_ON_GPIO_Port GPIOE
#define KEY_ON_P_Pin GPIO_PIN_10
#define KEY_ON_P_GPIO_Port GPIOE
#define NKLL_Pin GPIO_PIN_12
#define NKLL_GPIO_Port GPIOE
#define NINT_PBC_Pin GPIO_PIN_13
#define NINT_PBC_GPIO_Port GPIOE
#define VBD_CNTRL_Pin GPIO_PIN_14
#define VBD_CNTRL_GPIO_Port GPIOE
#define SCL1_Pin GPIO_PIN_10
#define SCL1_GPIO_Port GPIOB
#define SDA1_Pin GPIO_PIN_11
#define SDA1_GPIO_Port GPIOB
#define OLED_ON_Pin GPIO_PIN_12
#define OLED_ON_GPIO_Port GPIOB
#define GM_CNT_Pin GPIO_PIN_14
#define GM_CNT_GPIO_Port GPIOB
#define TXD2_Pin GPIO_PIN_8
#define TXD2_GPIO_Port GPIOD
#define RXD2_Pin GPIO_PIN_9
#define RXD2_GPIO_Port GPIOD
#define VIBR_Pin GPIO_PIN_12
#define VIBR_GPIO_Port GPIOD
#define ALRM_L2_Pin GPIO_PIN_13
#define ALRM_L2_GPIO_Port GPIOD
#define CHRG_LED_Pin GPIO_PIN_14
#define CHRG_LED_GPIO_Port GPIOD
#define KEY_R_Pin GPIO_PIN_6
#define KEY_R_GPIO_Port GPIOC
#define KEY_ENTER_Pin GPIO_PIN_7
#define KEY_ENTER_GPIO_Port GPIOC
#define KEY_L_Pin GPIO_PIN_9
#define KEY_L_GPIO_Port GPIOC
#define OSC_F_Pin GPIO_PIN_8
#define OSC_F_GPIO_Port GPIOA
#define RXD_BT_Pin GPIO_PIN_9
#define RXD_BT_GPIO_Port GPIOA
#define TX_BT_Pin GPIO_PIN_10
#define TX_BT_GPIO_Port GPIOA
#define RES_BT_Pin GPIO_PIN_11
#define RES_BT_GPIO_Port GPIOA
#define NBT_ON_Pin GPIO_PIN_12
#define NBT_ON_GPIO_Port GPIOA
#define SCK_AT45_Pin GPIO_PIN_10
#define SCK_AT45_GPIO_Port GPIOC
#define SI_AT45_Pin GPIO_PIN_11
#define SI_AT45_GPIO_Port GPIOC
#define RXD1_Pin GPIO_PIN_12
#define RXD1_GPIO_Port GPIOC
#define CS_AT45_Pin GPIO_PIN_0
#define CS_AT45_GPIO_Port GPIOD
#define WP_AT45_Pin GPIO_PIN_1
#define WP_AT45_GPIO_Port GPIOD
#define TXD1_Pin GPIO_PIN_2
#define TXD1_GPIO_Port GPIOD
#define PHONE_ON_Pin GPIO_PIN_4
#define PHONE_ON_GPIO_Port GPIOD
#define GPIO_Output_Pin GPIO_PIN_5
#define GPIO_Output_GPIO_Port GPIOD
#define AUDIO_ON_Pin GPIO_PIN_6
#define AUDIO_ON_GPIO_Port GPIOD
#define SO_AT45_Pin GPIO_PIN_5
#define SO_AT45_GPIO_Port GPIOB
#define ALRT1_Pin GPIO_PIN_8
#define ALRT1_GPIO_Port GPIOB
#define ALRM_L1_Pin GPIO_PIN_0
#define ALRM_L1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
