#ifndef SHARED_STRUCT_HPP
#define SHARED_STRUCT_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

typedef struct
{
    bool StatusPecas = false;
    bool EspCamRead = false;
    bool EspCamFinished = false;
    int CountAccepted = 0;
    int CountRejected = 0;
    int LCDMode = 0;
    int dutyCycle = 0;
    SemaphoreHandle_t xSemaphore = NULL;
    
}SharedStruct;

#endif