#ifndef __MOTOR_H
#define __MOTOR_H
#include <stdbool.h>
#include <stdint.h>
#include "../Drivers/fifo.h"
/**********************************************************
***	编写作者：Lin

***	qq：1071378062
**********************************************************/

#define MOTOR_NUM 3 // 定义电机数量
#define MOTOR_ID 4 // 定义电机起始 ID
// ==================== 步进电机参数 ====================

typedef struct StepperMotor
{
    uint8_t daocheng;
    uint8_t xifen;
    double step_angle;
    uint8_t current_pos; // mm
    uint8_t current_vel; // mm/s
    uint8_t target_pos; // mm
    uint8_t target_vel; // mm/s
} StepperMotor;

// ==================== 电机参数 ====================
typedef struct Motor
{
    int id;
    bool state;
    StepperMotor stepper_motor;
    uint8_t last_response_time;
    uint8_t timeout_threshold;
    uint8_t current_pos; // rad
    uint8_t  current_vel; // rpm -> rad/s
    uint8_t  current_acc; // rad/s^2
    uint8_t target_pos;
    uint8_t target_vel;
    uint8_t target_acc;
    float vel_max;
    uint8_t size;
    __IO bool motor_responding; //
    uint32_t cmd[16];
} Motor;

void motor_init(void);
void motor_system_info(int addr, Motor motor);
void motor_enable(int addr, bool state);
void motor_run(int addr, uint16_t speed, float deltaL, bool snf);
void motor_position_control_snf(float cur[], float target[], int size);
void motor_emergency_stop_all(void);
extern Motor motor[MOTOR_NUM];
#endif
