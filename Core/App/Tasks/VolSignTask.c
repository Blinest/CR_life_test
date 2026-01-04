#include "VolSignTask.h"
#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "Types/VolSignType.h"
#include "Types/VolDataType.h"

/* External variables --------------------------------------------------------*/
extern osMutexId_t Usart1MutexHandle;
extern osMessageQueueId_t VolQueueHandle;

/* Private variables ---------------------------------------------------------*/
// Placeholder for voltage request command
// TODO: Fill in the actual HEX command for voltage request
static uint8_t VolRequestCmd[] = {0x00, 0x00, 0x00, 0x00}; 

// Placeholder for received buffer
#define VOL_RX_BUFFER_SIZE 32
static uint8_t VolRxBuffer[VOL_RX_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void SendVoltageRequest(void);
static void ReceiveVoltageData(void);

void StartVolSignTask(void *argument)
{
    /* USER CODE BEGIN StartVolSignTask */
    /* Infinite loop */
    for(;;)
    {
        // 1. Send Voltage Request
        SendVoltageRequest();

        // 2. Wait and Receive Voltage Data
        // Note: In a real application, reception might be handled via interrupt 
        // and notification/queue, but here we implement a basic structure.
        ReceiveVoltageData();

        // 3. Sampling interval
        osDelay(100); // 100ms sampling rate (adjust as needed)
    }
    /* USER CODE END StartVolSignTask */
}

static void SendVoltageRequest(void)
{
    // Acquire Mutex to protect USART1
    if (osMutexAcquire(Usart1MutexHandle, osWaitForever) == osOK)
    {
        // TODO: Implement the specific HEX protocol for voltage request
        // For now, we just send the placeholder command
        HAL_UART_Transmit(&huart1, VolRequestCmd, sizeof(VolRequestCmd), 100);

        // Release Mutex
        osMutexRelease(Usart1MutexHandle);
    }
}

static void ReceiveVoltageData(void)
{
    // Note: This is a blocking receive for demonstration. 
    // Ideally, use interrupt-based reception (HAL_UART_RxCpltCallback) 
    // to avoid blocking the task and to handle asynchronous data.
    
    if (osMutexAcquire(Usart1MutexHandle, osWaitForever) == osOK)
    {
        // Try to receive data
        // TODO: Adjust timeout and buffer size according to the protocol
        HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, VolRxBuffer, VOL_RX_BUFFER_SIZE, 50);
        
        if (status == HAL_OK)
        {
            // TODO: Parse the received HEX data in VolRxBuffer
            // Example:
            // uint16_t voltage = (VolRxBuffer[1] << 8) | VolRxBuffer[2];
            
            // For now, we simulate a dummy value and send to queue
            uint16_t dummyVoltage = 0; // Replace with parsed value
            
            // Send to VolQueue for VolDataTask to process
            osMessageQueuePut(VolQueueHandle, &dummyVoltage, 0, 0);
        }

        osMutexRelease(Usart1MutexHandle);
    }
}
