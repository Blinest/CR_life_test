/**
* @file MotorCtrlTask.c
 * @brief 电机控制模块
 *
 * 本模块提供电机控制功能：
 * - MotorInit()：电机初始化，包括电机使能、电机速度设置等
 * - SendMotorCommand()：发送电机控制指令
 * - GenerateMotorCtrlCommand()：上层实现电机控制，底层调用商家提供的指令集或库函数
 *
 * 任务流程：获取电机控制指令 → 申请Usart1Mutex → 通过USART1发送电机控制命令 → 释放Usart1Mutex → 等待执行结果
 *
 * @note 使用USART1通过RS485控制三个步进电机循环运动
 */

#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "Types/MotorType.h"

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
    float newTargetSpeed __attribute__((unused));
    osStatus_t status __attribute__((unused));

    for(;;)
    {
        // TODO:发送指令


        // TODO:接收指令


    }

}




