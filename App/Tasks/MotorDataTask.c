/**
* @file MotorDataTask.c
 * @brief 电机数据处理模块
 *
 *
 * 任务流程：从MotorQueue获取电机数据 → 处理电机数据 → 通过USART2发送数据给ESP8266
 */
#include "cmsis_os.h"
#include "main.h"
#include "Types/MotorType.h"
#include "BSP/Motor/motor.h"
#include "fifo.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "usart.h"

extern osMutexId_t Usart2MutexHandle;

void StartMotorDataTask(void *argument)
{
    char msg[50];
    
    // 定义成功响应的字节序列
    uint8_t success_response[] = {0x01, 0xF3, 0x02, 0x6B};
    uint8_t motor_addr = 4; // 提前声明电机地址变量
    success_response[0] = motor_addr;
    osStatus_t status;
    for(;;)
    {
        // 获取指令

        status = osMutexAcquire(Usart2MutexHandle, osWaitForever);
        if (status == osOK)
        {
            // 检查是否接收到完整的电机返回数据
            if (rxFrameFlag)
            {

                // 检查是否是我们期望的成功响应
                if (memcmp(rxCmd, success_response, sizeof(success_response)) == 0)
                {
                    // 将对应电机的motor_enabled设置为true
                    motor.motor_enabled[motor_addr] = true;


                    // 向串口2发送成功消息
                    sprintf(msg, "motor %d enable success\r\n", motor_addr);
                    Usart_SendString(&huart2, (unsigned char*)msg, strlen(msg));
                }
                else
                {
                    // 其他情况，发送失败消息
                    sprintf(msg, "motor %d enable failed\r\n", motor_addr);
                    Usart_SendString(&huart2, (unsigned char*)msg, strlen(msg));
                }

                // 清空接收缓冲区和标志
                rxFrameFlag = false;
                rxCount = 0;
                memset((void*)rxCmd, 0, FIFO_SIZE);
                
                // 重新启动ReceiveToIdle_DMA接收
                HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxCmd, sizeof(rxCmd));
                __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
            }
            // 释放 Usart2Mutex
            osMutexRelease(Usart2MutexHandle);
        }
        
        osDelay(100);
    }

}
