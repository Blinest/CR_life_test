#ifndef MOTOR_DATA_TYPE_H
#define MOTOR_DATA_TYPE_H

#include <stdint.h>

typedef struct {
    uint8_t command;
    uint8_t motor_id;
    float speed;
    uint32_t timestamp;
} MotorMessage;
extern MotorMessage MotorMessageArray[];
#endif // MOTOR_DATA_TYPE_H
