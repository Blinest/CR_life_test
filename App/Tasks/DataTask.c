/**
* @file DataTask.c
 * @brief 电机数据处理模块
 *
 *
 * 任务流程：从CmdDataQueue获取串口1数据 → 处理数据 → 通过USART2发送数据给上位机
 */
#include "cmsis_os.h"
#include "main.h"
#include "Types/SensorType.h"
#include "BSP/Motor/motor.h"
#include "fifo.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "usart.h"
#include "Common/cmd_parse.h"



void StartDataTask(void *argument)
{
    uint8_t receive;
    for(;;)
    {
        /* 串口测试：
       * Usart_SendString(&huart1, (const char*)test_msg1, sizeof(test_msg1) - 1);
       * osDelay(2000);
       * Usart_SendString(&huart2, (const char*)test_msg2, sizeof(test_msg2) - 1);
       * osDelay(2000);
       */

        /* 取出CmdDataQueue队列中的控制指令
         * - 取到字节数据：进行指令解析
         * - 未取到：继续等待
        */
        if (osMessageQueueGet(CmdDataQueueHandle, &receive, 0, osWaitForever) != osOK) continue;
        //指令解析处理，具体实现在cmd_parse.c中
        cmd_parse_feed_byte(receive);
        
        osDelay(10);
    }
}
