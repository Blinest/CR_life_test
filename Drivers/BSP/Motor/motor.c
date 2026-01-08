/**
* @file motor.c
 * @brief 电机指令处理模块
 *
 * 本模块提供电机指令处理功能：
 * - motor_init()：电机初始化，初始化流程包括电机参数设置、
 * - motor_run()：启动电机，并设置绝对目标位置
 * - motor_position_control_snf()：
 * - motor_emergency_stop_all()：紧急停止所有电机
 *
 * 工作原理：
 *
 *
 * @note 使用TIM4的PWM通道4控制蜂鸣器
 */

#include "motor.h"
#include "Emm_V5.h"
#include "math.h"
#include "Common/utils.h"
#include <string.h>
#include "stm32f1xx_hal.h"
#define MOTOR_NUM 6


void UART2_SendString(char* msg)
{
    while (*msg)
    {
        HAL_UART_Transmit(&huart2, (uint8_t*)msg++, 1, 1000);
    }
}

Motor motor;

void motor_init(void)
{
    motor.steppter_motor.daocheng = 2; // 根据丝杠导程设置，2mm
    motor.steppter_motor.xifen = 128; // 平滑控制
    motor.steppter_motor.speed_max = 50; // 最大速度建议50rpm以下，速度过高，步进电机(FUYU35, 2025年)会出现抖动
    motor.steppter_motor.acc = 0; // 由于位移量较小，为提高响应速度，直接启动，不做加减速处理
    char msg[100];
    // 建立与电机的通信连接
    for(int i = 1; i <= MOTOR_NUM; i++)
    {
        sprintf(msg, "Initializing Motor %d...\r\n", i);
        UART2_SendString(msg);
        // 读取系统信息，如果没有返回值，则进入等待，并持续向该电机发送指令，直到通信建立成功
        Emm_V5_Read_Sys_Params(i, S_VER);
        // 等待指令返回，延时1s
        delay_ms(1000);
        while(rxFrameFlag == false) {
            delay_ms(1000);
            // 等待返回命令
            if(rxFrameFlag == false)
            {
                sprintf(msg, "Motor %d fail, please wait a minute\r\n", i);
                UART2_SendString(msg);
                // 对当前电机进行校准
                // send_calibration_command(i);
                // 等待10s 完成校准
                delay_ms(10000);
            }
            Emm_V5_Read_Sys_Params(i, S_VER);
            delay_ms(1000);
            if(rxFrameFlag == true){
                // 解析返回指令

                Emm_V5_En_Control(i, true, false);
                break;
            }
        }
        sprintf(msg, "Enabling motor...\r\n");
        UART2_SendString(msg);
        Emm_V5_En_Control(i, true, false);
        delay_ms(50);
        sprintf(msg, "Motor %d initialization complete!\r\n", i);
        UART2_SendString(msg);
        // 清空串口1标志与缓冲区
        rxFrameFlag = false;
        rxCount = 0;
        memset((void*)rxCmd, 0, FIFO_SIZE);
    }
    // 完成所有电机初始化
    sprintf(msg, "\r\n===== All motors initialized successfully! =====\r\n");
    UART2_SendString(msg);
}


/**
  * @brief 启动电机，并达到指定位置
  * @param addr: 电机地址
  * @param speed: 速度控制
  * @param target: 目标位置(绝对位置)，
  * @param snf: 同步标志位，true同步
  */
void motor_run(int addr, uint16_t speed, float target, bool snf) {
    const double destination = 200 * target * motor.steppter_motor.xifen / motor.steppter_motor.daocheng;
    const int dir = destination > 0 ? 1:0;
    const uint32_t clk = (uint32_t)fabs(destination);

    Emm_V5_Pos_Control(addr, dir, speed, motor.steppter_motor.acc, clk, true, snf);
}

/**
  * @brief    启动电机
  * @param    cur[]：当前位置
  * @param    target：目标位置
  * @param    size：电机数量
  * @retval
  */
void motor_position_control_snf(float cur[], float target[], int size){
    for(int i = 1; i <= size; i++)
    {
        Emm_V5_En_Control(i, true, true);
        delay_ms(5);
    }
    float maxdistance = 0;
    float distance[size + 1];
    for(int i = 1; i <= size; i++)
    {
        distance[i] = fabs(cur[i] - target[i]);
        maxdistance = fmax(maxdistance, distance[i]);
    }
    if(maxdistance == 0) return;
    uint16_t speed[size + 1];
    // 动态速度调整：线性比例控制
    // 速度分配，仍然是常数
    for(int i = 1; i <= 6; i++)
    {
       speed[i] = (uint16_t)(distance[i] * motor.steppter_motor.speed_max / maxdistance);
    }
    while(rxFrameFlag == false);
    Emm_V5_Synchronous_motion(0);
    rxFrameFlag = false;
    delay_ms(50);
    for(int i = 1; i <= 6; i++)
    {
        motor_run(i, speed[i], target[i], true);
        delay_ms(5);
    }
    while(rxFrameFlag == false);
    Emm_V5_Synchronous_motion(0);
    rxFrameFlag = false;
}

void motor_emergency_stop_all(void)
{
    char message[64];
    // 发送紧急停止消息
    sprintf(message, "stop all motors！\r\n");
    UART2_SendString(message);

    for(int i = 1; i <= 6; i++) {
        Emm_V5_Stop_Now(i, false);
        delay_ms(5); // 小延迟避免总线冲突

        // 更新电机状态
        motor.motor_enabled[i] = false;
        motor.motor_responding[i] = false;
    }

    // 发送停止完成消息
    sprintf(message, "all motors have been stopped\r\n");
    UART2_SendString(message);
}
