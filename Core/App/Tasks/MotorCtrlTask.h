#ifndef MOTOR_CTRL_TASK_H
#define MOTOR_CTRL_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

void StartMotorCtrlTask(void *argument);

/**
 * @brief  Sets the target motor speed.
 * @param  speed: New target speed.
 */
void Motor_SetTargetSpeed(float speed);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_CTRL_TASK_H
