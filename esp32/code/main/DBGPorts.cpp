#include "DBGPorts.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define TAG __PRETTY_FUNCTION__

void init_debug()
{
   ESP_LOGW(TAG, "debug gpio mask = 0x%0x", (unsigned int)DBG_PIN_MSK);
   gpio_config_t gc;
   gc.intr_type = GPIO_INTR_DISABLE;
   gc.mode = GPIO_MODE_OUTPUT;
   //bit mask of the pins that you want to set,e.g.GPIO18/19
   gc.pin_bit_mask = DBG_PIN_MSK;
   gc.pull_down_en = (gpio_pulldown_t)0;
   gc.pull_up_en = (gpio_pullup_t)0;
   gpio_config(&gc);
}


