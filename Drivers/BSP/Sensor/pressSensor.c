//
// Created by q3634 on 2026/1/7.
//

#include "BSP/Sensor/pressSensor.h"
#include "BSP/Sensor/trans_modbus.h"
#include "BSP/Sensor/data_processing.h"

#include <string.h>

/* 若使用 STM32 HAL */
#ifdef USE_HAL_DRIVER
#include "stm32f1xx_hal.h"
#endif

static float s_press_raw[PRESS_SENSOR_NUM];
static float s_press_processed[PRESS_SENSOR_NUM];
static bool  s_sensors_initialized = false;

// 传感器校准参数
static SensorCalibration s_calibration[PRESS_SENSOR_NUM];

// 传感器滤波器
static MovingAverageFilter s_filter[PRESS_SENSOR_NUM];

// 传感器Modbus地址映射
static uint8_t s_sensor_addresses[PRESS_SENSOR_NUM] = {
    1,  // sensor_1
    2,  // sensor_2
    3   // sensor_3
};

void sensors_init(void)
{
    /* 初始化传感器校准参数 */
    for (int i = 0; i < PRESS_SENSOR_NUM; i++)
    {
        // 默认校准参数，实际需根据传感器特性调整
        sensor_init_calibration(&s_calibration[i], 0.0f, 1.0f, -1000.0f, 1000.0f);
    }
    
    /* 初始化传感器滤波器 */
    for (int i = 0; i < PRESS_SENSOR_NUM; i++)
    {
        filter_init_moving_average(&s_filter[i]);
    }
    
    memset(s_press_raw, 0, sizeof(s_press_raw));
    memset(s_press_processed, 0, sizeof(s_press_processed));
    s_sensors_initialized = true;
}

/**
 * 从Modbus传感器读取32位有符号数据
 * @param address 传感器地址
 * @param value 输出值
 * @return 是否成功
 */
static bool read_modbus_32bit_data(uint8_t address, float *value)
{
    if (value == NULL)
    {
        return false;
    }
    
    uint16_t registers[2];
    
    // 读取寄存器0和1（低位在前，高位在后）
    if (!modbus_read_registers(address, 0, 2, registers, 1000))
    {
        return false;
    }
    
    // 组合成32位有符号整数
    int32_t int_value = ((int32_t)registers[1] << 16) | (uint16_t)registers[0];
    
    // 转换为浮点数
    *value = (float)int_value;
    
    return true;
}

bool sensor_read_pressure(PressSensorId id, float *value)
{
    if (value == NULL || !s_sensors_initialized)
        return false;
    if (id < sensor_1 || id > sensor_3)
        return false;

    uint8_t index = (int)id - 1;
    uint8_t address = s_sensor_addresses[index];
    
    // 从Modbus传感器读取原始值
    if (!read_modbus_32bit_data(address, &s_press_raw[index]))
    {
        return false;
    }
    
    // 处理传感器数据（校准 + 滤波）
    s_press_processed[index] = sensor_process_data(&s_calibration[index], &s_filter[index], s_press_raw[index]);
    
    *value = s_press_processed[index];
    return true;
}

bool sensor_get_message(PressSensorId id, pressMessage *msg)
{
    if (msg == NULL || !s_sensors_initialized)
        return false;
    if (id < sensor_1 || id > sensor_3)
        return false;

    uint8_t index = (int)id - 1;
    
    msg->press_sensor_id = id;
    msg->press_state     = s_sensors_initialized ? PressState_On : PressState_Off;
    msg->pressData       = s_press_processed[index];
    return true;
}

int sensors_read_all(float *values)
{
    if (values == NULL || !s_sensors_initialized)
        return 0;

    int success_count = 0;
    
    /* 依次读取所有传感器 */
    for (int i = 0; i < PRESS_SENSOR_NUM; i++)
    {
        PressSensorId id = (PressSensorId)(sensor_1 + i);
        if (sensor_read_pressure(id, &values[i]))
        {
            success_count++;
        }
    }
    
    return success_count;
}

bool sensor_self_test(PressSensorId id)
{
    if (!s_sensors_initialized || id < sensor_1 || id > sensor_3)
        return false;
    
    uint8_t index = (int)id - 1;
    uint8_t address = s_sensor_addresses[index];
    
    // 读取传感器数据，检查是否在合理范围
    float value;
    if (!read_modbus_32bit_data(address, &value))
    {
        return false;
    }
    
    // 检查数据是否在合理范围内
    if (value < -5000.0f || value > 5000.0f)
    {
        return false;
    }
    
    return true;
}