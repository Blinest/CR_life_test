//
// Created by q3634 on 2026/1/18.
//

#ifndef CR_LIFE_TEST_PROTOCOL_CONFIG_H
#define CR_LIFE_TEST_PROTOCOL_CONFIG_H
// ==================== 协议配置 ====================
#define MAX_FRAME_SIZE          2048    // 最大帧大小
#define MAX_PAYLOAD_SIZE        1024    // 最大数据负载
#define HEARTBEAT_INTERVAL      1000    // 心跳间隔(ms)
#define COMM_TIMEOUT_MS         5000    // 通信超时(ms)
#define MAX_RETRY_COUNT         3       // 最大重试次数

// ==================== 电机参数配置 ====================
#define DEFAULT_MAX_SPEED       3000    // 默认最大转速(rpm)
#define DEFAULT_MAX_TORQUE      500     // 默认最大扭矩(0.1Nm)
#define DEFAULT_MAX_CURRENT     5000    // 默认最大电流(mA)

// ==================== 运动参数配置 ====================
#define DEFAULT_ACCELERATION    1000.0f // 默认加速度(度/秒²)
#define DEFAULT_JERK           5000.0f // 默认加加速度(度/秒³)
#define SYNC_TOLERANCE         0.1f    // 同步容差(度)
#define POSITION_TOLERANCE     0.01f   // 位置容差(度)

// ==================== 缓冲区配置 ====================
#define RX_BUFFER_SIZE         4096    // 接收缓冲区大小
#define TX_BUFFER_SIZE         4096    // 发送缓冲区大小
#define TRAJECTORY_BUFFER_SIZE 1024    // 轨迹缓冲区大小

// ==================== 性能优化配置 ====================
#define USE_DMA_TRANSFER       0       // 使用DMA传输
#define USE_HARDWARE_CRC       0       // 使用硬件CRC
#define USE_RING_BUFFER        0       // 使用环形缓冲区
#define ENABLE_COMPRESSION     0       // 启用数据压缩
#define ENABLE_ENCRYPTION      0       // 启用数据加密
#endif //CR_LIFE_TEST_PROTOCOL_CONFIG_H