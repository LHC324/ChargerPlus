/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "string.h"
#include "math.h"
#include "stdio.h"
#include <stdarg.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define DEBUGGING  1
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CHECK0_Pin GPIO_PIN_13
#define CHECK0_GPIO_Port GPIOC
#define CHECK0_EXTI_IRQn EXTI15_10_IRQn
#define CHECK1_Pin GPIO_PIN_14
#define CHECK1_GPIO_Port GPIOC
#define CHECK1_EXTI_IRQn EXTI15_10_IRQn
#define CHECK2_Pin GPIO_PIN_15
#define CHECK2_GPIO_Port GPIOC
#define CHECK2_EXTI_IRQn EXTI15_10_IRQn
#define VBUS0_Pin GPIO_PIN_0
#define VBUS0_GPIO_Port GPIOA
#define VBAT0_Pin GPIO_PIN_1
#define VBAT0_GPIO_Port GPIOA
#define IBAT0_Pin GPIO_PIN_2
#define IBAT0_GPIO_Port GPIOA
#define VBUS1_Pin GPIO_PIN_3
#define VBUS1_GPIO_Port GPIOA
#define VBAT1_Pin GPIO_PIN_4
#define VBAT1_GPIO_Port GPIOA
#define IBAT1_Pin GPIO_PIN_5
#define IBAT1_GPIO_Port GPIOA
#define VBUS2_Pin GPIO_PIN_6
#define VBUS2_GPIO_Port GPIOA
#define VBAT2_Pin GPIO_PIN_7
#define VBAT2_GPIO_Port GPIOA
#define IBAT2_Pin GPIO_PIN_0
#define IBAT2_GPIO_Port GPIOB
#define HWDT_Pin GPIO_PIN_1
#define HWDT_GPIO_Port GPIOB
#define TS_RX_Pin GPIO_PIN_10
#define TS_RX_GPIO_Port GPIOB
#define TS_TX_Pin GPIO_PIN_11
#define TS_TX_GPIO_Port GPIOB
#define CS2_Pin GPIO_PIN_12
#define CS2_GPIO_Port GPIOB
#define SCK2_Pin GPIO_PIN_13
#define SCK2_GPIO_Port GPIOB
#define RUN_LED_Pin GPIO_PIN_14
#define RUN_LED_GPIO_Port GPIOB
#define MOSI2_Pin GPIO_PIN_15
#define MOSI2_GPIO_Port GPIOB
#define WIFI_RESET_Pin GPIO_PIN_8
#define WIFI_RESET_GPIO_Port GPIOA
#define WIFI_RX_Pin GPIO_PIN_9
#define WIFI_RX_GPIO_Port GPIOA
#define WIFI_TX_Pin GPIO_PIN_10
#define WIFI_TX_GPIO_Port GPIOA
#define WIFI_RELOAD_Pin GPIO_PIN_11
#define WIFI_RELOAD_GPIO_Port GPIOA
#define CHECK2_EN_Pin GPIO_PIN_12
#define CHECK2_EN_GPIO_Port GPIOA
#define CS1_Pin GPIO_PIN_15
#define CS1_GPIO_Port GPIOA
#define SCK1_Pin GPIO_PIN_3
#define SCK1_GPIO_Port GPIOB
#define CHECK0_EN_Pin GPIO_PIN_4
#define CHECK0_EN_GPIO_Port GPIOB
#define MOSI1_Pin GPIO_PIN_5
#define MOSI1_GPIO_Port GPIOB
#define START0_Pin GPIO_PIN_6
#define START0_GPIO_Port GPIOB
#define START1_Pin GPIO_PIN_7
#define START1_GPIO_Port GPIOB
#define START2_Pin GPIO_PIN_8
#define START2_GPIO_Port GPIOB
#define CHECK1_EN_Pin GPIO_PIN_9
#define CHECK1_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define START_OUT_HIGH  10000
#define START_OUT_LOW   0
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
