/**
* @file PressSignTask.c
 * @brief 压力传感器任务模块
 *
 * 本模块提供压力传感器任务功能：
 *
 * 任务流程：申请Usart1Mutex → 通过USART1发送压力传感器请求 → 接收压力数据 → 释放Usart1Mutex → 将数据放入PressQueue
 *
 * @note 本模块使用USART1通过RS485与压力传感器通信
 */
#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "Types/PressType.h"
#include "Types/PressType.h"

/* External variables --------------------------------------------------------*/
extern osMutexId_t Usart1MutexHandle;
extern osMessageQueueId_t PressQueueHandle;

/* Private variables ---------------------------------------------------------*/
// Placeholder for voltage request command
// TODO:
static uint8_t PressRequestCmd[] = {0x00, 0x00, 0x00, 0x00};

// Placeholder for received buffer
#define PRESS_RX_BUFFER_SIZE 32
static uint8_t PressRxBuffer[PRESS_RX_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void SendPressRequest(void);
static void ReceivePressData(void);

void StartPressSignTask(void *argument)
{

    for(;;)
    {
        // 1.发送请求
        SendPressRequest();

        // 2. 等待并接收压力数据
        ReceivePressData();

        // 3. 数据处理并发送给UART2
        osDelay(100); // 100ms sampling rate (adjust as needed)
    }
}

static void SendPressRequest(void)
{
    // Acquire Mutex to protect USART1
    if (osMutexAcquire(Usart1MutexHandle, osWaitForever) == osOK)
    {
        // TODO: Implement the specific HEX protocol for voltage request
        // For now, we just send the placeholder command
        HAL_UART_Transmit(&huart1, PressRequestCmd, sizeof(PressRequestCmd), 100);

        // Release Mutex
        osMutexRelease(Usart1MutexHandle);
    }
}

static void ReceivePressData(void)
{
    if (osMutexAcquire(Usart1MutexHandle, osWaitForever) == osOK)
    {
        // TODO: 接收压力数据
        HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, PressRxBuffer, PRESS_RX_BUFFER_SIZE, 50);
        
        if (status == HAL_OK)
        {
            // TODO: 解析压力数据指令
            // Example:
            // uint16_t voltage = (VolRxBuffer[1] << 8) | VolRxBuffer[2];
            
            // 模拟空队列进行压力数据发送
            uint16_t dummyPress = 0; // Replace with parsed value
            
            // 将压力队列数据发送给 PressDataTask 处理
            osMessageQueuePut(PressQueueHandle, &dummyPress, 0, 0);
        }

        osMutexRelease(Usart1MutexHandle);
    }
}
