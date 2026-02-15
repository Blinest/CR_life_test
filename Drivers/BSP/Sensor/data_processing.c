//
// Created by q3634 on 2026/2/15.
//

#include "BSP/Sensor/data_processing.h"

#include <string.h>

/**
 * 初始化传感器校准参数
 * @param cal 校准参数结构
 * @param offset 零点偏移量
 * @param scale 缩放系数
 * @param min_value 最小值
 * @param max_value 最大值
 */
void sensor_init_calibration(SensorCalibration *cal, float offset, float scale, float min_value, float max_value)
{
    if (cal == NULL)
    {
        return;
    }
    
    cal->offset = offset;
    cal->scale = scale;
    cal->min_value = min_value;
    cal->max_value = max_value;
}

/**
 * 校准传感器数据
 * @param cal 校准参数结构
 * @param raw_value 原始数据
 * @return 校准后的数据
 */
float sensor_calibrate_data(SensorCalibration *cal, float raw_value)
{
    if (cal == NULL)
    {
        return raw_value;
    }
    
    // 应用校准公式：校准值 = (原始值 - 偏移) * 缩放
    float calibrated_value = (raw_value - cal->offset) * cal->scale;
    
    // 限制数据范围
    return sensor_limit_value(calibrated_value, cal->min_value, cal->max_value);
}

/**
 * 初始化滑动平均值滤波器
 * @param filter 滤波器结构
 */
void filter_init_moving_average(MovingAverageFilter *filter)
{
    if (filter == NULL)
    {
        return;
    }
    
    memset(filter, 0, sizeof(MovingAverageFilter));
    filter->index = 0;
    filter->count = 0;
    filter->sum = 0.0f;
    
    // 初始化缓冲区
    for (uint8_t i = 0; i < MAX_FILTER_SIZE; i++)
    {
        filter->buffer[i] = 0.0f;
    }
}

/**
 * 添加数据到滑动平均值滤波器
 * @param filter 滤波器结构
 * @param value 新数据
 */
void filter_add_moving_average(MovingAverageFilter *filter, float value)
{
    if (filter == NULL)
    {
        return;
    }
    
    // 从总和中减去旧值
    filter->sum -= filter->buffer[filter->index];
    
    // 添加新值到缓冲区
    filter->buffer[filter->index] = value;
    
    // 将新值添加到总和
    filter->sum += value;
    
    // 更新索引
    filter->index = (filter->index + 1) % MAX_FILTER_SIZE;
    
    // 更新计数
    if (filter->count < MAX_FILTER_SIZE)
    {
        filter->count++;
    }
}

/**
 * 获取滑动平均值
 * @param filter 滤波器结构
 * @return 平均值
 */
float filter_get_moving_average(MovingAverageFilter *filter)
{
    if (filter == NULL || filter->count == 0)
    {
        return 0.0f;
    }
    
    return filter->sum / filter->count;
}

/**
 * 处理传感器原始数据（校准 + 滤波）
 * @param cal 校准参数结构
 * @param filter 滤波器结构
 * @param raw_value 原始数据
 * @return 处理后的数据
 */
float sensor_process_data(SensorCalibration *cal, MovingAverageFilter *filter, float raw_value)
{
    // 首先校准数据
    float calibrated_value = sensor_calibrate_data(cal, raw_value);
    
    // 然后添加到滤波器
    filter_add_moving_average(filter, calibrated_value);
    
    // 返回滤波后的值
    return filter_get_moving_average(filter);
}

/**
 * 限制数据范围
 * @param value 输入数据
 * @param min 最小值
 * @param max 最大值
 * @return 限制后的数据
 */
float sensor_limit_value(float value, float min, float max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}