#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "SharedStruct.hpp"
#include "TaskMotors.hpp"
#include "TaskEspNow.hpp"
#include "TaskIHM.hpp"

#include "esp_log.h"

TaskMotors *Motors;
TaskEspNow *EspNowProtocol;
//TaskIHM *LCDInterface; 
SharedStruct sharedData;
extern "C"
{
  void app_main(void);
}

void app_main() 
{
    sharedData.xSemaphore = xSemaphoreCreateMutex();
    esp_log_level_set("*", ESP_LOG_ERROR);
    esp_log_level_set("main", ESP_LOG_ERROR);
    Motors = new TaskMotors("TaskMotors", 4096, 5, &sharedData);
    EspNowProtocol = TaskEspNow::getInstance("TaskEspNow",4096,4, &sharedData);
    Motors->Start();
    for(;;)
    {
        ESP_LOGI("main", "TaskMotors: %d", eTaskGetState(Motors->GetHandle()));
        ESP_LOGI("main", "TaskEspNow: %d", eTaskGetState(EspNowProtocol->GetHandle()));
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}