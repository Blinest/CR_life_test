#include "protocol_defines.h"
#include "BSP/Motor/motor.h"

// ==================== 帧构建器 ====================
typedef struct {
    uint8_t tx_buffer[2048];
    uint16_t tx_length;
    uint16_t frame_id_counter;
} FrameBuilder_t;

// 构建帧头
static void build_frame_header(FrameBuilder_t *builder, 
                              uint16_t cmd_type, 
                              uint8_t dest_addr,
                              uint16_t data_len,
                              uint8_t priority) {
    FrameHeader_t header;

    header.frame_id = builder->frame_id_counter++;
    header.src_addr = DEVICE_ADDRESS;
    header.dest_addr = dest_addr;
    header.cmd_type = cmd_type;
    header.data_len = data_len;

    
    memcpy(builder->tx_buffer, &header, sizeof(FrameHeader_t));
    builder->tx_length = sizeof(FrameHeader_t);
}

// 构建帧尾
static void build_frame_tail(FrameBuilder_t *builder) {
    uint16_t crc = calculate_crc16(builder->tx_buffer, builder->tx_length);
    FrameTail_t tail;
    
    tail.checksum = crc;
    tail.frame_end = 0x0D;
    
    memcpy(builder->tx_buffer + builder->tx_length, &tail, sizeof(FrameTail_t));
    builder->tx_length += sizeof(FrameTail_t);
}

// ==================== 命令构建函数 ====================

// 1. 单电机控制命令
ErrorCode_t build_motor_enable_cmd(FrameBuilder_t *builder, 
                                   uint8_t motor_id, 
                                   bool enable) {
    uint8_t data[2] = {motor_id, enable ? 1 : 0};
    
    build_frame_header(builder, CMD_MOTOR_ENABLE, motor_id, 2, 5);
    memcpy(builder->tx_buffer + sizeof(FrameHeader_t), data, 2);
    builder->tx_length += 2;
    build_frame_tail(builder);
    
    return ERR_SUCCESS;
}

// 2. 多电机协同运动命令
ErrorCode_t build_group_motion_cmd(FrameBuilder_t *builder,
                                   const GroupMotion_t *motion) {
    uint16_t data_len = sizeof(GroupMotion_t);
    
    build_frame_header(builder, CMD_GROUP_START, 0xFF, data_len, 10);
    memcpy(builder->tx_buffer + sizeof(FrameHeader_t), motion, data_len);
    builder->tx_length += data_len;
    build_frame_tail(builder);
    
    return ERR_SUCCESS;
}

// 3. PVT轨迹命令
ErrorCode_t build_pvt_trajectory_cmd(FrameBuilder_t *builder,
                                     const PVTTrajectory_t *traj) {
    uint16_t data_len = sizeof(PVTTrajectory_t);
    
    build_frame_header(builder, CMD_TRAJ_PVT, 0xFF, data_len, 15);
    memcpy(builder->tx_buffer + sizeof(FrameHeader_t), traj, data_len);
    builder->tx_length += data_len;
    build_frame_tail(builder);
    
    return ERR_SUCCESS;
}

// 4. 插补运动命令
ErrorCode_t build_interpolation_cmd(FrameBuilder_t *builder,
                                    const InterpolationParams_t *params) {
    uint16_t data_len = sizeof(InterpolationParams_t);
    
    build_frame_header(builder, CMD_MODE_INTERPOLATION, 0xFF, data_len, 20);
    memcpy(builder->tx_buffer + sizeof(FrameHeader_t), params, data_len);
    builder->tx_length += data_len;
    build_frame_tail(builder);
    
    return ERR_SUCCESS;
}

// ==================== 响应构建 ====================
typedef struct {
    ErrorCode_t error_code;
    uint8_t data[256];
    uint16_t data_len;
} ResponseData_t;

ErrorCode_t build_response_frame(FrameBuilder_t *builder,
                                 uint16_t original_frame_id,
                                 const ResponseData_t *response) {
    FrameHeader_t header;
    
    header.magic = PROTOCOL_MAGIC_NUMBER;
    header.frame_id = original_frame_id;
    header.src_addr = DEVICE_ADDRESS;
    header.dest_addr = response->error_code == ERR_SUCCESS ? 0x01 : 0x00;
    header.cmd_type = CMD_STATUS_REPORT;
    header.data_len = sizeof(ErrorCode_t) + response->data_len;
    header.priority = 10;
    header.flags = 0;
    header.timestamp = osKernelGetTickCount();
    
    // 拷贝帧头
    memcpy(builder->tx_buffer, &header, sizeof(FrameHeader_t));
    builder->tx_length = sizeof(FrameHeader_t);
    
    // 拷贝错误码和数据
    memcpy(builder->tx_buffer + builder->tx_length, 
           &response->error_code, sizeof(ErrorCode_t));
    builder->tx_length += sizeof(ErrorCode_t);
    
    memcpy(builder->tx_buffer + builder->tx_length, 
           response->data, response->data_len);
    builder->tx_length += response->data_len;
    
    build_frame_tail(builder);
    
    return ERR_SUCCESS;
}