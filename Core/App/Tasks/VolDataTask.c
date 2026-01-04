#include "VolDataTask.h"
#include "cmsis_os.h"
#include "main.h"
#include "Types/VolDataType.h"

void StartVolDataTask(void *argument)
{
    /* USER CODE BEGIN StartVolDataTask */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartVolDataTask */
}
