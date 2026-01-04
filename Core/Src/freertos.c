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
#include "Types/MotorCtrlType.h"
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
/* USER CODE END Variables */
/* Definitions for VolSignTask */
osThreadId_t VolSignTaskHandle;
const osThreadAttr_t VolSignTask_attributes = {
  .name = "VolSignTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MotorCtrlTask */
osThreadId_t MotorCtrlTaskHandle;
const osThreadAttr_t MotorCtrlTask_attributes = {
  .name = "MotorCtrlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for MotorDataTask */
osThreadId_t MotorDataTaskHandle;
const osThreadAttr_t MotorDataTask_attributes = {
  .name = "MotorDataTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for VolDataTask */
osThreadId_t VolDataTaskHandle;
const osThreadAttr_t VolDataTask_attributes = {
  .name = "VolDataTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for VolQueue */
osMessageQueueId_t VolQueueHandle;
const osMessageQueueAttr_t VolQueue_attributes = {
  .name = "VolQueue"
};
/* Definitions for MotorCmdQueue */
osMessageQueueId_t MotorCmdQueueHandle;
const osMessageQueueAttr_t MotorCmdQueue_attributes = {
  .name = "MotorCmdQueue"
};

/* Definitions for MotorQueue */
osMessageQueueId_t MotorQueueHandle;
const osMessageQueueAttr_t MotorQueue_attributes = {
  .name = "MotorQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartVolSignTask(void *argument);
extern void  StartMotorCtrlTask(void *argument);
extern void StartMotorDataTask(void *argument);
extern void StartVolDataTask(void *argument);

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
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of VolQueue */
  VolQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &VolQueue_attributes);

  /* creation of MotorCmdQueue */
  // Create a queue that can hold 10 items, each item is a float (speed)
  MotorCmdQueueHandle = osMessageQueueNew (10, sizeof(float), &MotorCmdQueue_attributes);

  /* creation of MotorQueue */
  MotorQueueHandle = osMessageQueueNew (16, sizeof(MotorMessage*), &MotorQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of VolSignTask */
  VolSignTaskHandle = osThreadNew(StartVolSignTask, NULL, &VolSignTask_attributes);

  /* creation of MotorCtrlTask */
  MotorCtrlTaskHandle = osThreadNew(StartMotorCtrlTask, NULL, &MotorCtrlTask_attributes);

  /* creation of MotorDataTask */
  MotorDataTaskHandle = osThreadNew(StartMotorDataTask, NULL, &MotorDataTask_attributes);

  /* creation of VolDataTask */
  VolDataTaskHandle = osThreadNew(StartVolDataTask, NULL, &VolDataTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartVolSignTask */
/**
  * @brief  Function implementing the VolSignTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartVolSignTask */
__weak void StartVolSignTask(void *argument)
{
  /* USER CODE BEGIN StartVolSignTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartVolSignTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

