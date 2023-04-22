#include "TaskMotors.hpp"

TaskMotors::TaskMotors(std::string name, uint32_t stackDepth, UBaseType_t priority, SharedStruct *sharedData):Thread(name, stackDepth, priority)
{
    esp_log_level_set(name.c_str(), ESP_LOG_INFO);
    ESP_LOGI(GetName().c_str(), "Inicializando Driver dos motores");
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(PotPin, ADC_ATTEN_DB_11);

    // Inicializa PWM e motores (Servo motor e motor DC)
    InitPWM((gpio_num_t)PwmPin, PWM_A_CHANNEL, (gpio_num_t)ServoPwmPin, PWM_SERVO_CHANNEL);
    InitMotorsGpios();
    ServoWrite(PWM_SERVO_CHANNEL, 135);
    shared = sharedData;
}

void TaskMotors::Run()
{
    TickType_t xLastTimeWake = xTaskGetTickCount();
    int PotRead = 0;
    for(;;)
    {
        // Lê sensor infravermelho e aciona servo motor
        if(gpio_get_level((gpio_num_t)IRSensor))
        {
            gpio_set_level((gpio_num_t)A1,0);
            gpio_set_level((gpio_num_t)A2,0);
            ServoWrite(PWM_SERVO_CHANNEL, 135);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        else
        {
            ServoWrite(PWM_SERVO_CHANNEL, 45);
        }

        // Controla velocidade e direção do motor DC por potenciômetro
        PotRead = adc1_get_raw(PotPin);
        PotRead -= 2048;
        if(PotRead <= 0)
        {
            gpio_set_level((gpio_num_t)A1,0);
            gpio_set_level((gpio_num_t)A2,1);
            PotRead = -PotRead;
        }
        else
        {
            gpio_set_level((gpio_num_t)A1,1);
            gpio_set_level((gpio_num_t)A2,0);
        }
        PwmWrite(PWM_A_CHANNEL, PotRead * 2);
        iloop++;
        if(iloop>=15)
        {
            iloop = 0;
            ESP_LOGI(GetName().c_str(), "Duty Cycle Motor:%d",PotRead * 2);
            ESP_LOGI(GetName().c_str(), "Sensor IR:%d",gpio_get_level((gpio_num_t)IRSensor));
        }
        xTaskDelayUntil(&xLastTimeWake, 100 / portTICK_PERIOD_MS);
    }
    
}

void TaskMotors::PwmWrite(ledc_channel_t channel, int pwm){
    ledc_set_duty_and_update(LEDC_MODE,channel,pwm,0); // Atribui um novo duty para o PWM
}

void TaskMotors::ServoWrite(ledc_channel_t servochannel, int ang)
{
    float Thigh = (float)ang/180.0 + 1; // T high em ms 
    float duty = Thigh/20; // calcula o duty com base na frequência do pwm de 50hz 
    PwmWrite(servochannel,(int)(4095.0*duty)); // Posiciona o servo de acordo com o angulo desejado
}

void TaskMotors::InitPWM(gpio_num_t pin, ledc_channel_t channel, gpio_num_t servopin, ledc_channel_t servochannel){
    // Prepara e aplica a configuração do timer do LEDC para o motor DC
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode      = LEDC_MODE;
    ledc_timer.duty_resolution = LEDC_DUTY_RES;
    ledc_timer.timer_num       = LEDC_TIMER;
    ledc_timer.freq_hz         = LEDC_FREQUENCY; // Frequência de 5Khz
    ledc_timer.clk_cfg         = LEDC_AUTO_CLK; // Configuração da fonte de clock
    ledc_timer_config(&ledc_timer);

    // Prepara e aplica a configuração do timer do LEDC para o servo
    ledc_timer_config_t ledc_servotimer;
    ledc_servotimer.speed_mode      = LEDC_MODE;
    ledc_servotimer.duty_resolution = LEDC_DUTY_RES;
    ledc_servotimer.timer_num       = LEDC_SERVOTIMER;
    ledc_servotimer.freq_hz         = LEDC_SERVOFREQUENCY; // Frequência de 50hz
    ledc_servotimer.clk_cfg         = LEDC_AUTO_CLK; // Configuração da fonte de clock
    ledc_timer_config(&ledc_servotimer);

    // Prepara e aplica a configuração do canal do LEDC para o motor DC
    ledc_channel_config_t ledc_channel;
    ledc_channel.gpio_num       = pin;
    ledc_channel.speed_mode     = LEDC_MODE;
    ledc_channel.channel        = channel;
    ledc_channel.intr_type      = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel      = LEDC_TIMER;
    ledc_channel.duty           = 0; 
    ledc_channel.hpoint         = 0; // Ponto de início do duty cycle
    ledc_channel_config(&ledc_channel);

    // Prepara e aplica a configuração do canal do LEDC para o servo
    ledc_channel_config_t ledc_servochannel;
    ledc_servochannel.gpio_num       = servopin;
    ledc_servochannel.speed_mode     = LEDC_MODE;
    ledc_servochannel.channel        = servochannel;
    ledc_servochannel.intr_type      = LEDC_INTR_DISABLE;
    ledc_servochannel.timer_sel      = LEDC_SERVOTIMER;
    ledc_servochannel.duty           = 0; 
    ledc_servochannel.hpoint         = 0; // Ponto de início do duty cycle
    ledc_channel_config(&ledc_servochannel);

    ledc_fade_func_install(0);
}

void TaskMotors::InitMotorsGpios()
{
    gpio_set_direction((gpio_num_t)IRSensor, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)A1, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)A2, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)A1,0);
    gpio_set_level((gpio_num_t)A2,0);
}