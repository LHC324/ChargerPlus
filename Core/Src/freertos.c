/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "charger.h"
#include "spi.h"
#include "Mcp4822.h"
#include "adc.h"
#include "usart.h"
#include "shell.h"
#include "shell_port.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
GPIO_PinState g_Status =  GPIO_PIN_SET;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId LedHandle;
osThreadId DebugHandle;
osThreadId ChargingHandle;
osThreadId SamplingHandle;
osTimerId ReportHandle;
osMutexId shellMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Run_Task(void const * argument);
void Debug_Task(void const * argument);
void ChargingHandle_Task(void const * argument);
void Sampling_Task(void const * argument);
void Report_Callback(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Create the recursive mutex(es) */
  /* definition and creation of shellMutex */
  osMutexDef(shellMutex);
  shellMutexHandle = osRecursiveMutexCreate(osMutex(shellMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of Report */
  osTimerDef(Report, Report_Callback);
  ReportHandle = osTimerCreate(osTimer(Report), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Led */
  osThreadDef(Led, Run_Task, osPriorityIdle, 0, 64);
  LedHandle = osThreadCreate(osThread(Led), NULL);

  /* definition and creation of Debug */
  osThreadDef(Debug, Debug_Task, osPriorityLow, 0, 256);
  DebugHandle = osThreadCreate(osThread(Debug), NULL);

  /* definition and creation of Charging */
  osThreadDef(Charging, ChargingHandle_Task, osPriorityHigh, 0, 512);
  ChargingHandle = osThreadCreate(osThread(Charging), NULL);

  /* definition and creation of Sampling */
  osThreadDef(Sampling, Sampling_Task, osPriorityRealtime, 0, 512);
  SamplingHandle = osThreadCreate(osThread(Sampling), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  osTimerStart(ReportHandle, 1000);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Run_Task */
/**
  * @brief  Function implementing the LED_TASK thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Run_Task */
void Run_Task(void const * argument)
{
  /* USER CODE BEGIN Run_Task */

  /* Infinite loop */
  for(;;)
  { /*Turn on the software timer*/
    g_Status ^=  GPIO_PIN_SET;
    HAL_GPIO_WritePin(RUN_LED_GPIO_Port, RUN_LED_Pin, g_Status);
    osDelay(1000);
  }
  /* USER CODE END Run_Task */
}

/* USER CODE BEGIN Header_Debug_Task */
/**
* @brief Function implementing the DEBUG_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Debug_Task */
void Debug_Task(void const * argument)
{
  /* USER CODE BEGIN Debug_Task */
  /* Infinite loop */
  for(;;)
  {
#if (DEBUGGING == 1U)
    /*Call shell task*/
    shellTask(&shell);
#else
  osDelay(100);
#endif
  }
  /* USER CODE END Debug_Task */
}

/* USER CODE BEGIN Header_ChargingHandle_Task */
/**
* @brief Function implementing the CHARGING_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ChargingHandle_Task */
void ChargingHandle_Task(void const * argument)
{
  /* USER CODE BEGIN ChargingHandle_Task */
  /* Infinite loop */
  for(;;)
  {
    for (uint16_t i = 0; i < CHANNEL_MAX; i++)
    {
      Charging_Poll(&g_Charger[i]);
    }
    osDelay(500);
  }
  /* USER CODE END ChargingHandle_Task */
}

/* USER CODE BEGIN Header_Sampling_Task */
/**
* @brief Function implementing the Sampling thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Sampling_Task */
void Sampling_Task(void const * argument)
{
  /* USER CODE BEGIN Sampling_Task */
  /* Infinite loop */
  for(;;)
  {
    for (uint16_t i = 0; i < CHANNEL_MAX; i++)
    {
      Charging_Sampling(&g_Charger[i]);
    }
    osDelay(50);
  }
  /* USER CODE END Sampling_Task */
}

/* Report_Callback function */
void Report_Callback(void const * argument)
{
  /* USER CODE BEGIN Report_Callback */
  // g_Status ^=  GPIO_PIN_SET;
  // HAL_GPIO_WritePin(RUN_LED_GPIO_Port, RUN_LED_Pin, g_Status);
  for (uint16_t i = 0; i < CHANNEL_MAX; i++)
  {
   Report_ChargerInfo(&g_Charger[i]);
  }
  /* USER CODE END Report_Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
