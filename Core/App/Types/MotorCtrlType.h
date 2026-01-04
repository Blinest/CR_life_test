//
// Created by blin on 2026/1/3.
//

#ifndef CR_LIFE_TEST_MOTORCTRLTYPE_H
#define CR_LIFE_TEST_MOTORCTRLTYPE_H

typedef struct
{

    float targetSpeed;
    float currentSpeed;
    float maxSpeed;
    float minSpeed;
    float speedStep;
} MotorCtrlTypeDef;
#endif //CR_LIFE_TEST_MOTORCTRLTYPE_H