//
// Created by q3634 on 2026/1/7.
//

#include <stdint.h>
#include <stdbool.h>
#include "Types/SensorType.h"
#ifndef CR_LIFE_TEST_PRESSSENSOR_H
#define CR_LIFE_TEST_PRESSSENSOR_H
#define PRESS_SENSOR_NUM  3
#define MOTOR_ID 1 // 定义传感器起始 ID
/** 压力传感器初始化（ADC/SPI/I2C 等，按硬件选型实现） */
void sensors_init(void);

/**
 * 读取指定通道压力值
 * @param id 传感器 ID (sensor_1 / sensor_2 / sensor_3)
 * @param value 输出压力值（单位由标定决定，如 kPa 或 N）
 * @return true 成功，false 失败或未就绪
 */
bool sensor_read_pressure(PressSensorId id, float *value);

/**
 * 获取指定传感器的完整消息（含状态与数值）
 * @param id 传感器 ID
 * @param msg 输出 pressMessage
 * @return true 成功，false 失败
 */
bool sensor_get_message(PressSensorId id, pressMessage *msg);

/**
 * 读取所有压力传感器并填入数组
 * @param values 输出数组，长度至少 PRESS_SENSOR_NUM，顺序对应 sensor_1~sensor_3
 * @return 成功读取的通道数
 */
int sensors_read_all(float *values);

/** 传感器自检/校准接口（可选，后续实现） */
bool sensor_self_test(PressSensorId id);
#endif //CR_LIFE_TEST_PRESSSENSOR_H