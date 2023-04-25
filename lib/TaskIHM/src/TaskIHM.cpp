#include "TaskIHM.hpp"

TaskIHM::TaskIHM(std::string name, uint32_t stackDepth, UBaseType_t priority, SharedStruct *sharedData):Thread(name, stackDepth, priority)
{
    esp_log_level_set(name.c_str(), ESP_LOG_INFO);
    ESP_LOGI(GetName().c_str(), "Inicializando a interface com o Display LCD");
    gpio_set_direction((gpio_num_t)bootPin, GPIO_MODE_INPUT);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(PotPin, ADC_ATTEN_DB_11);
    initLcdDisplay();
    hd44780_clear(&lcd);
    hd44780_puts(&lcd,"Teste");
    shared = sharedData;
}

void TaskIHM::Run()
{
    TickType_t xLastTimeWake = xTaskGetTickCount();
    for(;;)
    {
        lcdbutstate = gpio_get_level((gpio_num_t)bootPin);
        if(lcdbutstate)
        {
            lcdbutstateant = true;
        }
        
        SharedStruct sharedTemp;
        if(shared->xSemaphore != NULL )
        {     
            if( xSemaphoreTake( shared->xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
                sharedTemp = (*shared);
                xSemaphoreGive( shared->xSemaphore );
            }
        }
        std::string lcdData;
        if(!lcdbutstate && lcdbutstateant)
        {
            lcdbutstateant = false;
            if(LCD_Mode == 0) LCD_Mode++;
            else LCD_Mode = 0;
            switch(LCD_Mode)
            {
                case 0:
                    lcdData = "Duty:"; 
                    lcdData +=  std::to_string(sharedTemp.dutyCycle) + "%";
                    hd44780_clear(&lcd);
                    hd44780_puts(&lcd,lcdData.c_str());
                    ESP_LOGI(GetName().c_str(), "%s", lcdData.c_str());
                    break;
                case 1:
                    lcdData = "Rejeitadas:" + std::to_string(sharedTemp.CountRejected);
                    hd44780_clear(&lcd);
                    hd44780_puts(&lcd,lcdData.c_str());
                    ESP_LOGI(GetName().c_str(), "%s", lcdData.c_str());
                    break;
            }
        }
        
        sendInterval++;
        if(sendInterval >= 25)
        {
            dutycycle = adc1_get_raw(PotPin);
            sendInterval = 0;
            SharedStruct sharedTempSend;
            if(shared->xSemaphore != NULL )
            {
                
                if( xSemaphoreTake( shared->xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
                {
                    shared->dutyCycle = dutycycle;
                    sharedTempSend = (*shared);
                    xSemaphoreGive( shared->xSemaphore );
                }
            }
            TaskEspNow::getInstance()->Send(sharedTempSend);
        }
        xTaskDelayUntil(&xLastTimeWake, 100 / portTICK_PERIOD_MS);
    }
    
}

void TaskIHM::initLcdDisplay()
{
    lcd.write_cb = NULL;
    lcd.pins.d4 = Lcd_d4;
    lcd.pins.d5 = Lcd_d5;
    lcd.pins.d6 = Lcd_d6;
    lcd.pins.d7 = Lcd_d7;
    lcd.pins.rs = Lcd_rs;
    lcd.pins.e = Lcd_e;
    lcd.pins.bl = HD44780_NOT_USED;
    lcd.font = HD44780_FONT_5X8;
    lcd.lines = 2;
    lcd.backlight = true;
    hd44780_init(&lcd);
}
