#ifndef __MOTOR_H
#define __MOTOR_H
#include <stdbool.h>
#include <stdint.h>
#include "../Drivers/fifo.h"
/**********************************************************
***	编写作者：Lin

***	qq：1071378062
**********************************************************/

#define MOTOR_NUM 6 //定义电机数量
typedef struct Steppter_Motor
{
    uint8_t daocheng;
    uint16_t speed_max;
    uint8_t xifen;
    uint8_t SM_MAX;
    uint8_t SM_MIN;
    uint8_t current_positon;
    uint8_t last_position;
    uint8_t acc;

} SM;

typedef struct Motor
{
    SM steppter_motor;
    bool motor_status[MOTOR_NUM+1];
    uint32_t last_response_time[MOTOR_NUM+1];
    uint32_t timeout_threshold;
    bool motor_enabled[MOTOR_NUM+1]; // 接收电机是否使能
    __IO bool motor_responding[MOTOR_NUM+1]; //
} Motor;

void motor_init(void);
void motor_run(int addr, uint16_t speed, float deltaL, bool snf);
void motor_position_control_snf(float cur[], float target[], int size);
void motor_emergency_stop_all(void);
extern Motor motor;
#endif
