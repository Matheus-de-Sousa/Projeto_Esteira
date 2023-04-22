#include "TaskIHM.hpp"

TaskIHM::TaskIHM(std::string name, uint32_t stackDepth, UBaseType_t priority, SharedStruct *sharedData):Thread(name, stackDepth, priority)
{
    esp_log_level_set(name.c_str(), ESP_LOG_INFO);
    ESP_LOGI(GetName().c_str(), "Inicializando a interface com o Display LCD");
    initLcdDisplay();
    hd44780_clear(&lcd);
    shared = sharedData;
}

void TaskIHM::Run()
{
    TickType_t xLastTimeWake = xTaskGetTickCount();
    for(;;)
    {
        hd44780_clear(&lcd);
        std::string lcdData;
        int LCD_Mode = 0;
        SharedStruct sharedTemp;
        if(shared->xSemaphore != NULL )
        {
            
            if( xSemaphoreTake( shared->xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
                LCD_Mode = shared->LCDMode;
                sharedTemp = (*shared);
                xSemaphoreGive( shared->xSemaphore );
            }
        }
        switch(LCD_Mode)
        {
            case 0:
                lcdData = "Duty Cycle do motor:"; 
                lcdData +=  std::to_string(sharedTemp.dutyCycle) + "%";
                break;
        }
        hd44780_puts(&lcd,lcdData.c_str());
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
