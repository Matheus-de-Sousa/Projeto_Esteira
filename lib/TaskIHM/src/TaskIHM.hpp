#ifndef TASK_IHM_HPP
#define TASK_IHM_HPP

#include "thread.hpp"
#include "hd44780.h"
#include "EspGpios.hpp"
#include "SharedStruct.hpp"
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
    SharedStruct *shared = NULL;

};

#endif