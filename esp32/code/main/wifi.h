#ifndef WIFI_H
#define WIFI_H

#include "driver/gpio.h"
#include "esp_event.h"

#include "configuration.h"

void wifi_initialize(void);

class Wifi
{
public:
   Wifi(gpio_num_t led_iop);

   bool connected()
   {
      return(m_isConnected);
   }

   static void* connectWifi(void*);

private:
   /*
   static void event_handler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data);
   void static wifi_initialize(void);
   */
   static bool       m_isConnected;
//   static gpio_num_t m_ledPin;
};

#endif
