/**
* @file MotorDataTask.c
 * @brief 电机数据处理模块
 *
 * 本模块提供电机数据处理功能：
 * - MotorSysData：电机系统数据采集，包括电机使能、电机速度设置等
 * - SendMotorCommand()：发送电机控制指令
 * - ReceiveMotorFeedback()：接收电机返回值，并进行相关处理
 * - GenerateMotorCtrlCommand()：上层实现电机控制，底层调用商家提供的指令集或库函数
 *
 * 任务流程：从MotorQueue获取电机数据 → 处理电机数据 → 通过USART2发送数据给ESP8266
 */
#include "cmsis_os.h"
#include "main.h"
#include "Types/MotorType.h"

void StartMotorDataTask(void *argument)
{

    for(;;)
    {
        // char *msg;
        // TODO: 电机返回数据接收
        //osMessageQueueGet(MotorQueueHandle);
        // TODO: 电机数据处理

        // TODO: 处理数据发送

        osDelay(1);
    }

}
