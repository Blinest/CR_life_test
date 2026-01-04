#include "MotorDataTask.h"
#include "cmsis_os.h"
#include "main.h"
#include "Types/MotorDataType.h"

void StartMotorDataTask(void *argument)
{
    /* USER CODE BEGIN StartMotorDataTask */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartMotorDataTask */
}
