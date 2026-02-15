/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "fifo.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
extern __IO bool rxFrameFlag1;
extern __IO uint8_t rxCmd1[FIFO_SIZE];
extern __IO uint8_t rxCount1;
extern __IO bool rxFrameFlag2;     // USART2 帧接收完成标志
extern __IO uint8_t rxCmd2[FIFO_SIZE]; // USART2 接收缓冲区
extern __IO uint8_t rxCount2;           // USART2 接收数据长度

void Usart_SendString(UART_HandleTypeDef* huart, unsigned char *str, unsigned short len);
void UsartPrintf(UART_HandleTypeDef* huart, const char *fmt,...);
void UART1_Receive_Start();
void UART2_Receive_Start();
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
