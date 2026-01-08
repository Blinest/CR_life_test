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

void StartMotorDataTask(void *argument)
{
    extern __IO uint8_t rxCmd[];
    extern __IO bool rxFrameFlag;
    extern __IO uint8_t rxCount;
    char msg[50];
    
    // 定义成功响应的字节序列
    uint8_t success_response[] = {0x01, 0xF3, 0xE2, 0x6B};
    
    for(;;)
    {
        // 检查是否接收到完整的电机返回数据
        if (rxFrameFlag && rxCount > 0)
        {
            // 检查是否是我们期望的成功响应
            if (rxCount >= sizeof(success_response) && 
                memcmp(rxCmd, success_response, sizeof(success_response)) == 0)
            {
                // 从响应中获取电机地址
                uint8_t motor_addr = rxCmd[0];
                
                // 将对应电机的motor_enabled设置为true
                motor.motor_enabled[motor_addr] = true;
                
                // 向串口2发送成功消息
                sprintf(msg, "motor %d enable success\r\n", motor_addr);
                Usart_SendString(&huart2, msg, sizeof(msg));
            }
            
            // 清空接收缓冲区和标志
            rxFrameFlag = false;
            rxCount = 0;
            memset((void*)rxCmd, 0, FIFO_SIZE);
        }
        
        osDelay(1);
    }

}
