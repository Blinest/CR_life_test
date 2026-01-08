/**
* @file PressDataTask.c
 * @brief 压力传感器数据处理模块
 *
 * 本模块提供压力传感器数据处理功能：
 *
 * 任务流程：从PressQueue获取压力数据 → 处理压力数据 → 通过USART2发送数据给ESP8266
 *
 * @note 使用USART2通过RS485与ESP8266通信
 */

#include "cmsis_os.h"
#include "main.h"
#include "Types/PressType.h"

void StartPressDataTask(void *argument)
{

    for(;;)
    {
        osDelay(1);
    }

}
