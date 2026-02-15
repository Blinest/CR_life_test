//
// Created by q3634 on 2026/2/15.
//

#include "BSP/Sensor/trans_modbus.h"

#include <string.h>

/* 若使用 STM32 HAL */
#ifdef USE_HAL_DRIVER
#include "stm32f1xx_hal.h"
#include "usart.h"
#endif

// 计算Modbus CRC校验
uint16_t modbus_calculate_crc(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;
    
    for (i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

// 构建Modbus读取保持寄存器请求
bool modbus_build_read_request(ModbusRequest *req, uint8_t address, uint16_t start_addr, uint16_t count)
{
    if (req == NULL || count == 0 || count > MODBUS_MAX_DATA_LEN)
    {
        return false;
    }
    
    memset(req, 0, sizeof(ModbusRequest));
    req->address = address;
    req->function = MODBUS_FUNC_READ_HOLDING_REGISTERS;
    req->start_address = start_addr;
    req->register_count = count;
    
    // 构建请求数据
    uint8_t data[8];
    data[0] = address;
    data[1] = MODBUS_FUNC_READ_HOLDING_REGISTERS;
    data[2] = (start_addr >> 8) & 0xFF;
    data[3] = start_addr & 0xFF;
    data[4] = (count >> 8) & 0xFF;
    data[5] = count & 0xFF;
    
    // 计算CRC
    req->crc = modbus_calculate_crc(data, 6);
    
    return true;
}

// 构建Modbus写单个寄存器请求
bool modbus_build_write_single_request(ModbusRequest *req, uint8_t address, uint16_t reg_addr, uint16_t value)
{
    if (req == NULL)
    {
        return false;
    }
    
    memset(req, 0, sizeof(ModbusRequest));
    req->address = address;
    req->function = MODBUS_FUNC_WRITE_SINGLE_REGISTER;
    req->start_address = reg_addr;
    req->register_count = 1;
    req->data[0] = value;
    req->data_len = 1;
    
    // 构建请求数据
    uint8_t data[8];
    data[0] = address;
    data[1] = MODBUS_FUNC_WRITE_SINGLE_REGISTER;
    data[2] = (reg_addr >> 8) & 0xFF;
    data[3] = reg_addr & 0xFF;
    data[4] = (value >> 8) & 0xFF;
    data[5] = value & 0xFF;
    
    // 计算CRC
    req->crc = modbus_calculate_crc(data, 6);
    
    return true;
}

// 构建Modbus写多个寄存器请求
bool modbus_build_write_multiple_request(ModbusRequest *req, uint8_t address, uint16_t start_addr, uint16_t count, uint16_t *data)
{
    if (req == NULL || data == NULL || count == 0 || count > MODBUS_MAX_DATA_LEN)
    {
        return false;
    }
    
    memset(req, 0, sizeof(ModbusRequest));
    req->address = address;
    req->function = MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS;
    req->start_address = start_addr;
    req->register_count = count;
    req->data_len = count;
    memcpy(req->data, data, count * sizeof(uint16_t));
    
    // 构建请求数据
    uint8_t req_data[256];
    req_data[0] = address;
    req_data[1] = MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS;
    req_data[2] = (start_addr >> 8) & 0xFF;
    req_data[3] = start_addr & 0xFF;
    req_data[4] = (count >> 8) & 0xFF;
    req_data[5] = count & 0xFF;
    req_data[6] = count * 2; // 数据长度（字节）
    
    // 填充数据
    for (uint16_t i = 0; i < count; i++)
    {
        req_data[7 + i * 2] = (data[i] >> 8) & 0xFF;
        req_data[8 + i * 2] = data[i] & 0xFF;
    }
    
    // 计算CRC
    req->crc = modbus_calculate_crc(req_data, 7 + count * 2);
    
    return true;
}

// 解析Modbus响应
bool modbus_parse_response(ModbusResponse *resp, uint8_t *data, uint16_t len)
{
    if (resp == NULL || data == NULL || len < 5)
    {
        return false;
    }
    
    memset(resp, 0, sizeof(ModbusResponse));
    resp->address = data[0];
    resp->function = data[1];
    
    // 检查是否是错误响应
    if (resp->function & 0x80)
    {
        resp->error = data[2];
        return false;
    }
    
    // 根据功能码解析响应
    switch (resp->function)
    {
        case MODBUS_FUNC_READ_HOLDING_REGISTERS:
            resp->data_len = data[2];
            if (len < 3 + resp->data_len + MODBUS_CRC_LEN)
            {
                return false;
            }
            
            // 解析数据
            for (uint8_t i = 0; i < resp->data_len / 2; i++)
            {
                resp->data[i] = (data[3 + i * 2] << 8) | data[4 + i * 2];
            }
            break;
            
        case MODBUS_FUNC_WRITE_SINGLE_REGISTER:
            resp->data_len = 4;
            if (len < 7)
            {
                return false;
            }
            
            // 解析数据
            resp->data[0] = (data[2] << 8) | data[3];
            resp->data[1] = (data[4] << 8) | data[5];
            break;
            
        case MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS:
            resp->data_len = 4;
            if (len < 7)
            {
                return false;
            }
            
            // 解析数据
            resp->data[0] = (data[2] << 8) | data[3];
            resp->data[1] = (data[4] << 8) | data[5];
            break;
            
        default:
            resp->error = MODBUS_ERR_INVALID_FUNCTION;
            return false;
    }
    
    // 计算CRC
    uint16_t crc = modbus_calculate_crc(data, len - MODBUS_CRC_LEN);
    uint16_t received_crc = (data[len - 2] << 8) | data[len - 1];
    
    if (crc != received_crc)
    {
        resp->error = MODBUS_ERR_CRC;
        return false;
    }
    
    resp->error = MODBUS_ERR_NONE;
    return true;
}

// 发送Modbus请求并接收响应
bool modbus_send_request(ModbusRequest *req, ModbusResponse *resp, uint32_t timeout)
{
    if (req == NULL || resp == NULL)
    {
        return false;
    }
    
    // 构建请求数据
    uint8_t tx_data[256];
    uint16_t tx_len = 0;
    
    tx_data[tx_len++] = req->address;
    tx_data[tx_len++] = req->function;
    
    switch (req->function)
    {
        case MODBUS_FUNC_READ_HOLDING_REGISTERS:
            tx_data[tx_len++] = (req->start_address >> 8) & 0xFF;
            tx_data[tx_len++] = req->start_address & 0xFF;
            tx_data[tx_len++] = (req->register_count >> 8) & 0xFF;
            tx_data[tx_len++] = req->register_count & 0xFF;
            break;
            
        case MODBUS_FUNC_WRITE_SINGLE_REGISTER:
            tx_data[tx_len++] = (req->start_address >> 8) & 0xFF;
            tx_data[tx_len++] = req->start_address & 0xFF;
            tx_data[tx_len++] = (req->data[0] >> 8) & 0xFF;
            tx_data[tx_len++] = req->data[0] & 0xFF;
            break;
            
        case MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS:
            tx_data[tx_len++] = (req->start_address >> 8) & 0xFF;
            tx_data[tx_len++] = req->start_address & 0xFF;
            tx_data[tx_len++] = (req->register_count >> 8) & 0xFF;
            tx_data[tx_len++] = req->register_count & 0xFF;
            tx_data[tx_len++] = req->register_count * 2;
            
            for (uint16_t i = 0; i < req->register_count; i++)
            {
                tx_data[tx_len++] = (req->data[i] >> 8) & 0xFF;
                tx_data[tx_len++] = req->data[i] & 0xFF;
            }
            break;
            
        default:
            return false;
    }
    
    // 添加CRC
    tx_data[tx_len++] = (req->crc >> 8) & 0xFF;
    tx_data[tx_len++] = req->crc & 0xFF;
    
    /* 若使用 STM32 HAL，这里可以使用 HAL_UART_Transmit 和 HAL_UART_Receive */
#ifdef USE_HAL_DRIVER
    // 发送数据
    if (HAL_UART_Transmit(&huart1, tx_data, tx_len, timeout) != HAL_OK)
    {
        return false;
    }
    
    // 接收响应
    uint8_t rx_data[256];
    uint16_t rx_len = 0;
    
    if (HAL_UART_Receive(&huart1, rx_data, sizeof(rx_data), timeout) != HAL_OK)
    {
        return false;
    }
    
    // 解析响应
    return modbus_parse_response(resp, rx_data, rx_len);
#else
    // 这里是占位实现，实际需要根据硬件通信方式实现发送和接收
    return false;
#endif
}

// 初始化Modbus通信
bool modbus_init(void)
{
    /* 若使用 STM32 HAL，这里可以初始化串口等通信设备 */
#ifdef USE_HAL_DRIVER
    // 假设使用USART1作为Modbus通信端口
    // MX_USART1_UART_Init() 应该已经在main.c中调用
    return true;
#else
    return false;
#endif
}

// 读取单个寄存器
bool modbus_read_register(uint8_t address, uint16_t reg_addr, uint16_t *value, uint32_t timeout)
{
    if (value == NULL)
    {
        return false;
    }
    
    ModbusRequest req;
    ModbusResponse resp;
    
    if (!modbus_build_read_request(&req, address, reg_addr, 1))
    {
        return false;
    }
    
    if (!modbus_send_request(&req, &resp, timeout))
    {
        return false;
    }
    
    *value = resp.data[0];
    return true;
}

// 读取多个寄存器
bool modbus_read_registers(uint8_t address, uint16_t start_addr, uint16_t count, uint16_t *values, uint32_t timeout)
{
    if (values == NULL || count == 0)
    {
        return false;
    }
    
    ModbusRequest req;
    ModbusResponse resp;
    
    if (!modbus_build_read_request(&req, address, start_addr, count))
    {
        return false;
    }
    
    if (!modbus_send_request(&req, &resp, timeout))
    {
        return false;
    }
    
    memcpy(values, resp.data, count * sizeof(uint16_t));
    return true;
}

// 写入单个寄存器
bool modbus_write_register(uint8_t address, uint16_t reg_addr, uint16_t value, uint32_t timeout)
{
    ModbusRequest req;
    ModbusResponse resp;
    
    if (!modbus_build_write_single_request(&req, address, reg_addr, value))
    {
        return false;
    }
    
    if (!modbus_send_request(&req, &resp, timeout))
    {
        return false;
    }
    
    return true;
}

// 写入多个寄存器
bool modbus_write_registers(uint8_t address, uint16_t start_addr, uint16_t count, uint16_t *values, uint32_t timeout)
{
    if (values == NULL || count == 0)
    {
        return false;
    }
    
    ModbusRequest req;
    ModbusResponse resp;
    
    if (!modbus_build_write_multiple_request(&req, address, start_addr, count, values))
    {
        return false;
    }
    
    if (!modbus_send_request(&req, &resp, timeout))
    {
        return false;
    }
    
    return true;
}
