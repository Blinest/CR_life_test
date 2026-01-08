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


// 申请 Usart1Mutex
static float currentTargetSpeed __attribute__((unused)) = 0.0f;
static float lastSentSpeed __attribute__((unused)) = -1.0f; // Initialized to a value unlikely to be 0 to ensure first send

// Placeholder for received motor feedback
#define MOTOR_RX_BUFFER_SIZE 32
static uint8_t MotorRxBuffer[MOTOR_RX_BUFFER_SIZE] __attribute__((unused));



// Note: Function name capitalized to match naming convention. 
// Please ensure freertos.c is updated to StartMotorCtrlTask if it was startMotorCtrlTask.
void StartMotorCtrlTask(void *argument)
{
    osStatus_t status;
    uint8_t motor_addr = 1; // 电机地址，根据实际情况修改

    for(;;)
    {
        // 申请 Usart1Mutex
        status = osMutexAcquire(Usart1MutexHandle, osWaitForever);
        if (status == osOK)
        {
            // 发送电机使能指令
            Emm_V5_En_Control(motor_addr, true, false);
            
            // 释放 Usart1Mutex
            osMutexRelease(Usart1MutexHandle);
        }
        
        // 等待一段时间后再次发送指令
        osDelay(1000);
    }

}




