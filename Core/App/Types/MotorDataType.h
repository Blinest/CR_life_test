#ifndef MOTOR_DATA_TYPE_H
#define MOTOR_DATA_TYPE_H

#include <stdint.h>

typedef struct {
    int32_t position;
    float speed;
    float torque;
    uint32_t errorParams;
} MotorDataTypeDef;

#endif // MOTOR_DATA_TYPE_H
