#ifndef TASK_MOTORS_HPP
#define TASK_MOTORS_HPP

#include "thread.hpp"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "EspGpios.hpp"
#include "SharedStruct.hpp"
#include "esp_log.h"

using namespace cpp_freertos;

#define LEDC_TIMER              LEDC_TIMER_0 // Timer do LEDC utilizado
#define LEDC_SERVOTIMER         LEDC_TIMER_1 // Timer do LEDC utilizado para o servo
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE // Modo de velocidade do LEDC
#define PWM_A_CHANNEL           LEDC_CHANNEL_0 // Canal do LEDC utilizado
#define PWM_SERVO_CHANNEL       LEDC_CHANNEL_1 // Canal do LEDC utilizado para o servo
#define LEDC_DUTY_RES           LEDC_TIMER_12_BIT // Resolução do PWM
#define LEDC_FREQUENCY          5000 // Frequência em Hertz do sinal PWM
#define LEDC_SERVOFREQUENCY     50 // Frequência em Hertz do sinal PWM para o servo

class TaskMotors : public Thread
{
public:
    TaskMotors(std::string name, uint32_t stackDepth, UBaseType_t priority, SharedStruct *sharedData);

    void Run() override;

private:
    int iloop = 0;
    void InitPWM(gpio_num_t pin, ledc_channel_t channel, gpio_num_t servopin, ledc_channel_t servochannel);
    void InitMotorsGpios();
    void PwmWrite(ledc_channel_t channel, int pwm);
    void ServoWrite(ledc_channel_t servochannel, int ang); // Função apenas para servos de 180°
    SharedStruct *shared = NULL;
};

#endif