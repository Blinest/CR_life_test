/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "Types/SensorType.h"
#include "Types/CmdCtrlType.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Definitions for Usart1Mutex */
// 创建Usart1锁
osMutexId_t Usart1MutexHandle;
// 定义Usart1锁的相关属性
const osMutexAttr_t Usart1Mutex_attributes = {
  .name = "Usart1Mutex"
};
// 创建Usart2锁
osMutexId_t Usart2MutexHandle;
// 定义Usart2锁的相关属性
const osMutexAttr_t Usart2Mutex_attributes = {
  .name = "Usart2Mutex"
};
/* USER CODE END Variables */
/* Definitions for DataTask */
osThreadId_t DataTaskHandle;
const osThreadAttr_t DataTask_attributes = {
  .name = "DataTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CmdCtrlTask */
osThreadId_t CmdCtrlTaskHandle;
const osThreadAttr_t CmdCtrlTask_attributes = {
  .name = "CmdCtrlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for CmdDataQueue */
osMessageQueueId_t CmdDataQueueHandle;
const osMessageQueueAttr_t CmdDataQueue_attributes = {
  .name = "CmdDataQueue"
};
/* Definitions for SensorMessageQueue */
osMessageQueueId_t SensorMessageQueueHandle;
const osMessageQueueAttr_t SensorMessageQueue_attributes = {
  .name = "SensorMessageQueue"
};
/* Definitions for CmdCtrlQueue */
osMessageQueueId_t CmdCtrlQueueHandle;
const osMessageQueueAttr_t CmdCtrlQueue_attributes = {
  .name = "CmdCtrlQueue"
};
/* Definitions for CmdQueue */
osMessageQueueId_t CmdQueueHandle;
const osMessageQueueAttr_t CmdQueue_attributes = {
  .name = "CmdQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDataTask(void *argument);
extern void StartCmdCtrlTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* creation of Usart1Mutex */
  Usart1MutexHandle = osMutexNew(&Usart1Mutex_attributes);
  /* creation of Usart2Mutex */
  Usart2MutexHandle = osMutexNew(&Usart2Mutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of CmdDataQueue */
  CmdDataQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &CmdDataQueue_attributes);

  /* creation of SensorMessageQueue */
  SensorMessageQueueHandle = osMessageQueueNew (16, sizeof(SensorMessage*), &SensorMessageQueue_attributes);

  /* creation of CmdCtrlQueue */
  CmdCtrlQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &CmdCtrlQueue_attributes);

  /* creation of CmdQueue */
  CmdQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &CmdQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DataTask */
  DataTaskHandle = osThreadNew(StartDataTask, NULL, &DataTask_attributes);

  /* creation of CmdCtrlTask */
  CmdCtrlTaskHandle = osThreadNew(StartCmdCtrlTask, NULL, &CmdCtrlTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDataTask */
/**
  * @brief  Function implementing the DataTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDataTask */
__weak void StartDataTask(void *argument)
{
  /* USER CODE BEGIN StartDataTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDataTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

