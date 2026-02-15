//
// Created by q3634 on 2026/2/15.
//

#ifndef CR_LIFE_TEST_TRANS_MODBUS_H
#define CR_LIFE_TEST_TRANS_MODBUS_H

#include <stdint.h>
#include <stdbool.h>

#define MODBUS_MAX_DATA_LEN  64
#define MODBUS_CRC_LEN       2

// Modbus功能码
typedef enum {
    MODBUS_FUNC_READ_HOLDING_REGISTERS = 0x03,  // 读取保持寄存器
    MODBUS_FUNC_WRITE_SINGLE_REGISTER  = 0x06,  // 写单个寄存器
    MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS = 0x10, // 写多个寄存器
} ModbusFunctionCode;

// Modbus错误码
typedef enum {
    MODBUS_ERR_NONE = 0,          // 无错误
    MODBUS_ERR_INVALID_ADDRESS,    // 无效地址
    MODBUS_ERR_INVALID_FUNCTION,   // 无效功能码
    MODBUS_ERR_INVALID_DATA,       // 无效数据
    MODBUS_ERR_COMMUNICATION,      // 通信错误
    MODBUS_ERR_CRC,                // CRC校验错误
} ModbusErrorCode;

// Modbus请求结构
typedef struct {
    uint8_t address;               // 从机地址
    uint8_t function;              // 功能码
    uint16_t start_address;         // 起始地址
    uint16_t register_count;        // 寄存器数量
    uint16_t data_len;              // 数据长度
    uint16_t data[MODBUS_MAX_DATA_LEN]; // 数据
    uint16_t crc;                  // CRC校验
} ModbusRequest;

// Modbus响应结构
typedef struct {
    uint8_t address;               // 从机地址
    uint8_t function;              // 功能码
    uint8_t data_len;              // 数据长度
    uint16_t data[MODBUS_MAX_DATA_LEN]; // 数据
    uint16_t crc;                  // CRC校验
    ModbusErrorCode error;         // 错误码
} ModbusResponse;

/**
 * 计算Modbus CRC校验
 * @param data 数据指针
 * @param len 数据长度
 * @return CRC校验值
 */
uint16_t modbus_calculate_crc(uint8_t *data, uint16_t len);

/**
 * 构建Modbus读取保持寄存器请求
 * @param req 请求结构
 * @param address 从机地址
 * @param start_addr 起始寄存器地址
 * @param count 寄存器数量
 * @return 构建是否成功
 */
bool modbus_build_read_request(ModbusRequest *req, uint8_t address, uint16_t start_addr, uint16_t count);

/**
 * 构建Modbus写单个寄存器请求
 * @param req 请求结构
 * @param address 从机地址
 * @param reg_addr 寄存器地址
 * @param value 写入值
 * @return 构建是否成功
 */
bool modbus_build_write_single_request(ModbusRequest *req, uint8_t address, uint16_t reg_addr, uint16_t value);

/**
 * 构建Modbus写多个寄存器请求
 * @param req 请求结构
 * @param address 从机地址
 * @param start_addr 起始寄存器地址
 * @param count 寄存器数量
 * @param data 数据指针
 * @return 构建是否成功
 */
bool modbus_build_write_multiple_request(ModbusRequest *req, uint8_t address, uint16_t start_addr, uint16_t count, uint16_t *data);

/**
 * 解析Modbus响应
 * @param resp 响应结构
 * @param data 数据指针
 * @param len 数据长度
 * @return 解析是否成功
 */
bool modbus_parse_response(ModbusResponse *resp, uint8_t *data, uint16_t len);

/**
 * 发送Modbus请求并接收响应
 * @param req 请求结构
 * @param resp 响应结构
 * @param timeout 超时时间（毫秒）
 * @return 通信是否成功
 */
bool modbus_send_request(ModbusRequest *req, ModbusResponse *resp, uint32_t timeout);

/**
 * 初始化Modbus通信
 * @return 初始化是否成功
 */
bool modbus_init(void);

/**
 * 读取单个寄存器
 * @param address 从机地址
 * @param reg_addr 寄存器地址
 * @param value 读取值
 * @param timeout 超时时间（毫秒）
 * @return 读取是否成功
 */
bool modbus_read_register(uint8_t address, uint16_t reg_addr, uint16_t *value, uint32_t timeout);

/**
 * 读取多个寄存器
 * @param address 从机地址
 * @param start_addr 起始寄存器地址
 * @param count 寄存器数量
 * @param values 读取值数组
 * @param timeout 超时时间（毫秒）
 * @return 读取是否成功
 */
bool modbus_read_registers(uint8_t address, uint16_t start_addr, uint16_t count, uint16_t *values, uint32_t timeout);

/**
 * 写入单个寄存器
 * @param address 从机地址
 * @param reg_addr 寄存器地址
 * @param value 写入值
 * @param timeout 超时时间（毫秒）
 * @return 写入是否成功
 */
bool modbus_write_register(uint8_t address, uint16_t reg_addr, uint16_t value, uint32_t timeout);

/**
 * 写入多个寄存器
 * @param address 从机地址
 * @param start_addr 起始寄存器地址
 * @param count 寄存器数量
 * @param values 写入值数组
 * @param timeout 超时时间（毫秒）
 * @return 写入是否成功
 */
bool modbus_write_registers(uint8_t address, uint16_t start_addr, uint16_t count, uint16_t *values, uint32_t timeout);

#endif //CR_LIFE_TEST_TRANS_MODBUS_H