#ifndef TASK_IHM_HPP
#define TASK_IHM_HPP

#include "thread.hpp"
#include "hd44780.h"
#include "driver/adc.h"
#include "EspGpios.hpp"
#include "SharedStruct.hpp"
#include "TaskEspNow.hpp"
#include "esp_log.h"

using namespace cpp_freertos;

class TaskIHM : public Thread
{
public:
    TaskIHM(std::string name, uint32_t stackDepth, UBaseType_t priority, SharedStruct *sharedData);

    void Run() override;

private:
    void initLcdDisplay();
    struct hd44780 lcd;
    bool lcdbutstate = 0;
    bool lcdbutstateant = 0;
    int LCD_Mode = 0;
    int dutycycle = 0;
    int sendInterval = 0;

    SharedStruct *shared = NULL;

};

#endif