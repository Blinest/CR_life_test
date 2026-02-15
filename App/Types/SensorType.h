#ifndef SENSOR_TYPE_H
#define SENSOR_TYPE_H

#include <stdint.h>
#include <stdbool.h>

#include "CmdCtrlType.h"

// 外设地址
typedef enum {
    motor_1 = 4,
    motor_2 = 5,
    motor_3 = 6,
} MotorId;

typedef enum {
    sensor_1 = 1,
    sensor_2 = 2,
    sensor_3 = 3,
} PressSensorId;

// 状态
typedef enum {
    MotorState_Off = 0,
    MotorState_On = 1,
} MotorState;


typedef enum
{
    PressState_Off = 0,
    PressState_On = 1,
} PressState;


// 电机相关数据
typedef struct{
    MotorId motor_id;
    MotorState motor_state;
    float speed;
    uint16_t xifen;
    float acc;
    uint32_t target_position;
    uint32_t current_position;
    uint32_t cycleNum;
} MotorMessage;

typedef struct {
    PressSensorId press_sensor_id;
    PressState press_state;
    float pressData;
} pressMessage;
// 数据结构体
typedef struct {
    MotorMessage motor_message;
    pressMessage press_message;
    uint32_t timestamp; // 运行时间
    bool state; // 外设状态
} SensorMessage;

#endif // SENSOR_TYPE_H
