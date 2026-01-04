#include "MotorCtrlTask.h"
#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "Types/MotorCtrlType.h"
#include "Types/MotorDataType.h"

/* External variables --------------------------------------------------------*/
extern osMutexId_t Usart1MutexHandle;
extern osMessageQueueId_t MotorQueueHandle;

/* Private variables ---------------------------------------------------------*/
static float currentTargetSpeed = 0.0f;
static float lastSentSpeed = -1.0f; // Initialized to a value unlikely to be 0 to ensure first send

// Placeholder for received motor feedback
#define MOTOR_RX_BUFFER_SIZE 32
static uint8_t MotorRxBuffer[MOTOR_RX_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static uint32_t GenerateMotorCtrlCmd(uint8_t *buffer, float targetSpeed);
static void SendMotorCommand(void);
static void ReceiveMotorFeedback(void);

// Note: Function name capitalized to match naming convention. 
// Please ensure freertos.c is updated to StartMotorCtrlTask if it was startMotorCtrlTask.
void StartMotorCtrlTask(void *argument)
{
    float newTargetSpeed;
    osStatus_t status;

    /* USER CODE BEGIN StartMotorCtrlTask */
    /* Infinite loop */
    for(;;)
    {
        // 1. Wait for new command from Queue
        // osWaitForever means the task sleeps until data arrives
        // To also handle periodic feedback, we can use a timeout (e.g. 50ms)
        // If timeout, it means no new command, but we might still want to check feedback
        status = osMessageQueueGet(MotorCmdQueueHandle, &newTargetSpeed, NULL, 50);

        if (status == osOK)
        {
             // New command received! Send it immediately.
             SendMotorCommand(newTargetSpeed);
        }

        // 2. Receive Feedback (Periodically check)
        // Note: Ideally feedback should be interrupt driven and notify this task,
        // but polling here every 50ms (due to queue timeout) is acceptable for simple systems.
        ReceiveMotorFeedback();
    }
    /* USER CODE END StartMotorCtrlTask */
}

static void SendMotorCommand(float targetSpeed)
{
    // Generate command packet
    uint32_t cmdLen = GenerateMotorCtrlCmd(MotorTxBuffer, targetSpeed);

    // Acquire Mutex to protect USART1
    if (osMutexAcquire(Usart1MutexHandle, osWaitForever) == osOK)
    {
        if (cmdLen > 0)
        {
             HAL_UART_Transmit(&huart1, MotorTxBuffer, cmdLen, 100);
        }
        osMutexRelease(Usart1MutexHandle);
    }
}

/**
 * @brief  Sets the target motor speed by sending to queue.
 * @param  speed: New target speed.
 */
void Motor_SetTargetSpeed(float speed)
{
    // Put the new speed into the queue
    // Timeout is 0 (non-blocking), if queue is full, command is dropped (or handle error)
    osMessageQueuePut(MotorCmdQueueHandle, &speed, 0, 0);
}

/**
 * @brief  生成电机控制命令包
 * @param  buffer: 指向命令缓冲区的指针
 * @param  targetSpeed: 目标速度值，包含在命令中
 * @retval 生成的命令包长度
 */
static uint32_t GenerateMotorCtrlCmd(uint8_t *buffer, float targetSpeed)
{
    if (buffer == NULL)
    {
        return 0;
    }

    // TODO: 实现电机控制命令协议打包
    // 协议格式: [HEAD] [CMD] [DATA_H] [DATA_L] [CHECKSUM]
    // Header: 0xAA
    // Command: 0x01 (Set Speed)
    // Data: Speed (scaled by 10, e.g. 10.5 -> 105)
    
    uint16_t speedInt = (uint16_t)(targetSpeed * 10); // Simple scaling
    
    buffer[0] = 0xAA;           // Header
    buffer[1] = 0x01;           // Command ID
    buffer[2] = (speedInt >> 8) & 0xFF; // Data High Byte
    buffer[3] = speedInt & 0xFF;        // Data Low Byte
    buffer[4] = 0x00;           // Checksum (Placeholder)
    
    // Calculate Checksum (Simple Sum)
    for(int i = 0; i < 4; i++)
    {
        buffer[4] += buffer[i];
    }
    
    return 5; // Return total packet length
}

static void ReceiveMotorFeedback(void)
{
    // Note: Similar to VolSignTask, this blocking receive is a simple implementation.
    // In a robust system, use DMA/Interrupts to handle incoming data streams.
    
    // Check if we can receive feedback (non-blocking)
    if (osMutexAcquire(Usart1MutexHandle, 10) == osOK) // Short timeout for check
    {
        // Try to receive feedback
        HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, MotorRxBuffer, MOTOR_RX_BUFFER_SIZE, 10);
        
        if (status == HAL_OK)
        {
            // TODO: Parse the received HEX data in MotorRxBuffer
            // Example: Check headers, checksum, extract position/speed
            
            // Allocate memory or use a static buffer to pass message
            // MotorMessage *msg = ...;
            // osMessageQueuePut(MotorQueueHandle, &msg, 0, 0);
        }

        osMutexRelease(Usart1MutexHandle);
    }
}
