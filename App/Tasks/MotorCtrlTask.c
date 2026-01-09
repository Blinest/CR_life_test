/**
* @file MotorCtrlTask.c
 * @brief 电机控制模块
 *
 *
 * 任务流程：获取电机控制指令 → 申请Usart1Mutex → 通过USART1发送电机控制命令 → 释放Usart1Mutex → 等待执行结果
 *
 * @note 使用USART1通过RS485控制三个步进电机循环运动
 */

#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "Types/MotorType.h"
#include "BSP/Motor/Emm_V5.h"
#include "BSP/Motor/motor.h"

/* External variables --------------------------------------------------------*/

extern osMutexId_t Usart1MutexHandle;
extern osMessageQueueId_t MotorQueueHandle;


void StartMotorCtrlTask(void *argument)
{
    osStatus_t status;
    uint8_t motor_addr = 4; // 电机地址，根据实际情况修改，三电机地址为4,5,6
    //Usart_SendString(&huart2, "【StartMotorCtrlTask】start In for",32);
    for(;;)
    {
        // 申请 Usart1Mutex
        //Usart_SendString(&huart2, "【StartMotorCtrlTask】In for",16);
        status = osMutexAcquire(Usart1MutexHandle, osWaitForever);
        if (status == osOK)
        {
            // 发送电机使能指令
            Emm_V5_En_Control(motor_addr, true, false);
            
            // 等待一段时间，确保有足够时间接收返回值
            // 115200波特率下，1字节数据传输约需87us，考虑RS485收发切换和系统处理时间
            // 设置5ms等待时间，确保稳定接收
            osDelay(5);
            
            // 释放 Usart1Mutex
            osMutexRelease(Usart1MutexHandle);
        }
        
        // 等待一段时间后再次发送指令
        osDelay(500);
    }

}




