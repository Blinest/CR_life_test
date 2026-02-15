//
// Created by q3634 on 2026/2/15.
//

#ifndef CR_LIFE_TEST_DATA_PROCESSING_H
#define CR_LIFE_TEST_DATA_PROCESSING_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_FILTER_SIZE  10

/**
 * 传感器校准参数结构
 */
typedef struct {
    float offset;         // 零点偏移
    float scale;          // 缩放系数
    float min_value;      // 最小值
    float max_value;      // 最大值
} SensorCalibration;

/**
 * 滑动平均值滤波结构
 */
typedef struct {
    float buffer[MAX_FILTER_SIZE];  // 数据缓冲区
    uint8_t index;                  // 当前索引
    uint8_t count;                  // 有效数据个数
    float sum;                      // 数据总和
} MovingAverageFilter;

/**
 * 初始化传感器校准参数
 * @param cal 校准参数结构
 * @param offset 零点偏移
 * @param scale 缩放系数
 * @param min_value 最小值
 * @param max_value 最大值
 */
void sensor_init_calibration(SensorCalibration *cal, float offset, float scale, float min_value, float max_value);

/**
 * 校准传感器数据
 * @param cal 校准参数结构
 * @param raw_value 原始数据
 * @return 校准后的数据
 */
float sensor_calibrate_data(SensorCalibration *cal, float raw_value);

/**
 * 初始化滑动平均值滤波器
 * @param filter 滤波器结构
 */
void filter_init_moving_average(MovingAverageFilter *filter);

/**
 * 添加数据到滑动平均值滤波器
 * @param filter 滤波器结构
 * @param value 新数据
 */
void filter_add_moving_average(MovingAverageFilter *filter, float value);

/**
 * 获取滑动平均值
 * @param filter 滤波器结构
 * @return 平均值
 */
float filter_get_moving_average(MovingAverageFilter *filter);

/**
 * 处理传感器原始数据（校准 + 滤波）
 * @param cal 校准参数结构
 * @param filter 滤波器结构
 * @param raw_value 原始数据
 * @return 处理后的数据
 */
float sensor_process_data(SensorCalibration *cal, MovingAverageFilter *filter, float raw_value);

/**
 * 限制数据范围
 * @param value 输入数据
 * @param min 最小值
 * @param max 最大值
 * @return 限制后的数据
 */
float sensor_limit_value(float value, float min, float max);

#endif //CR_LIFE_TEST_DATA_PROCESSING_H