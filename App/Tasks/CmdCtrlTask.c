/**
 *  @file CmdCtrlTask.c
 * @brief 指令控制任务：从队列取串口数据，交给 cmd_parse 解析并执行
 * @author blin
 *
 * 串口2 接收字节经 CmdCtrlQueueHandle 送入本任务，每字节调用 cmd_parse_feed_byte()，
 * 指令解析与电机/传感器控制逻辑在 Common/cmd_parse 中实现。
 */

#include "cmsis_os2.h"
#include "main.h"
#include "usart.h"
#include "BSP/Motor/motor.h"
#include "Common/cmd_parse.h"



void StartCmdCtrlTask(void *argument)
{
    UART2_Receive_Start();
    motor_init();
    uint8_t receive;
    cmd_parse_reset();

    uint8_t test_msg1[] = "1111111111111111111111111\r\n";
    uint8_t test_msg2[] = "instruction has been received successfully\r\n";

    for (;;)
    {
        /* 串口测试：
         * Usart_SendString(&huart1, (const char*)test_msg1, sizeof(test_msg1) - 1);
         * osDelay(2000);
         * Usart_SendString(&huart2, (const char*)test_msg2, sizeof(test_msg2) - 1);
         * osDelay(2000);
         */

        /* 取出CmdCtrlQueue队列中的控制指令
         * - 取到字节数据：进行指令解析
         * - 未取到：继续等待
        */
        if (osMessageQueueGet(CmdCtrlQueueHandle, &receive, 0, osWaitForever) != osOK) continue;
        //指令解析处理，具体实现在cmd_parse.c中
        cmd_parse_feed_byte(receive);
    }
}
