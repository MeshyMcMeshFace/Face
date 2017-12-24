#include "esp32-hal.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void delay(uint32_t ms)
{
   vTaskDelay( ms / portTICK_PERIOD_MS);
}

void yield()
{
   vPortYield();
}
