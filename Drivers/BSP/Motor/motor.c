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


void UART2_SendString(char* msg)
{
    while (*msg)
    {
        HAL_UART_Transmit(&huart2, (uint8_t*)msg++, 1, 1000);
    }
}
// 创建全局电机对象
Motor motor[MOTOR_NUM];
/**
  * @brief    电机初始化
  */
void motor_init(void)
{
    // 步进电机相关参数初始化
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        motor[i].id = MOTOR_ID + i;
        motor[i].stepper_motor.daocheng = 2; // 根据丝杠导程设置，2mm
        motor[i].stepper_motor.xifen = 128; // 平滑控制
        motor[i].stepper_motor.step_angle = 1.8; // 步距角
        motor[i].vel_max = 50; // 最大速度建议50rpm以下，速度过高，步进电机(FUYU35, 2025年)会出现抖动
        motor[i].current_acc = 0; // 由于位移量较小，为提高响应速度，直接启动，不做加减速处理 (0-255)
    }
    char msg[100];
    // 电机使能
    motor_enable(motor[0].id, MOTOR_NUM);
    // 电机状态返回值接收

    // 完成所有电机初始化
}
/**
  * @brief    电机信息获取
  * @param    addr：电机地址
  * @param    motor：电机结构体
  * @retval
  */
void motor_system_info(int addr, Motor m)
{
    // 读取目标位置
    SysParams_t cur_vel = S_TPOS;
    Emm_V5_Read_Sys_Params(addr, cur_vel);
    uint8_t rx_len = 0;
    uint32_t timeout = 100;
    // 清空接收缓冲区
    memset(m.cmd, 0, sizeof(m.cmd));
    // 字节接收或超时停止
    while (rx_len < 8 && timeout--)
    {
        if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE))
        {
            m.cmd[rx_len++] = (uint8_t)(huart1.Instance->DR & 0xFF);
        }
        HAL_Delay(1);
    }
    // 串口数据解析[0]：地址，[1]：0x33，[2]：符号，[3-6]：目标位置，[7]：校验字节
    // 分辨率转换：0x00010000 =(65536*360)/65536 =360.0
    uint32_t val_abs = ((uint32_t)m.cmd[6] << 24) | ((uint32_t)m.cmd[5] << 16) |
                     ((uint32_t)m.cmd[4] << 8) | m.cmd[3];
    // 当前旋转角度：deg
    float angle = m.cmd[2] == 1 ? -val_abs * 360.0f / 65535.0f  : val_abs * 360.0f / 65535.0f;
    // 当前位置：mm
    m.stepper_motor.current_pos = angle * m.stepper_motor.daocheng / 360.0f;
}

/*
  * @brief    单电机使能
  * @param    addr：电机地址
  * @param    state：电机状态
  * @retval
  */
void motor_enable(int addr, bool state)
{
    Emm_V5_En_Control(addr, true, false);
}

/**
  * @brief 启动步进电机，并达到指定位置
  * @param addr: 电机地址
  * @param speed: 速度值, rpm
  * @param target: 目标位置(绝对位置), mm
  * @param snf: 同步标志位，true同步
  */
void motor_run(int addr, uint16_t speed, float target, bool snf) {
    int idx = (addr >= MOTOR_ID && addr < MOTOR_ID + MOTOR_NUM) ? (addr - MOTOR_ID) : 0;
    int xifen = motor[idx].stepper_motor.xifen;
    int daocheng = motor[idx].stepper_motor.daocheng;
    double step_angle = motor[idx].stepper_motor.step_angle;
    // 位置计算
    float distance = xifen * 360.0f / step_angle * target / daocheng;

    const int dir = distance > 0 ? 1:0;
    const uint32_t clk = (uint32_t)fabs(distance);
    Emm_V5_Pos_Control(addr, dir, speed, motor[idx].current_acc, clk, true, snf);
}

/**
  * @brief    启动电机
  * @param    cur[]：当前位置
  * @param    target[]：目标位置
  * @param    size：电机数量
  * @retval
  */
void motor_position_control_snf(float cur[], float target[], int size){
    if (size <= 0 || size > MOTOR_NUM) return;
    float max_distance = 0;
    float distance[size];
    uint16_t speed[size];
    // 找到最长路径（cur/target 下标 1..size 对应电机 1..size）
    for (int i = 0; i < size; i++)
    {
        distance[i] = fabs(cur[i] - target[i]);
        max_distance = fmax(max_distance, distance[i]);
    }
    if (max_distance == 0) return;
    // 动态速度调整：线性比例控制
    for (int i = 0; i < size; i++)
        speed[i] = (uint16_t)(distance[i] * motor[i].vel_max / max_distance);
    while (rxFrameFlag1 == false);  /* TODO: 加超时，避免死等 */
    Emm_V5_Synchronous_motion(0);
    rxFrameFlag1 = false;
    delay_ms(50);
    for (int i = 0; i < size; i++)
    {
        motor_run(motor[i].id, speed[i], target[i], true);
        delay_ms(5);
    }
    while (rxFrameFlag1 == false);
    Emm_V5_Synchronous_motion(0);rxFrameFlag1 = false;
}


void motor_emergency_stop_all(void)
{
    char message[64];
    // 发送紧急停止消息
    // sprintf(message, "stop all motors！\r\n");
    // UART2_SendString(message);

    for(int i = 0; i < MOTOR_NUM; i++) {
        Emm_V5_Stop_Now(motor[i].id, false);
        delay_ms(5); // 小延迟避免总线冲突

        // 更新电机状态（motor 数组下标 0~MOTOR_NUM-1）
        motor[i].state = false;
    }

    // 发送停止完成消息
    // sprintf(message, "all motors have been stopped\r\n");
    // UART2_SendString(message);
}
