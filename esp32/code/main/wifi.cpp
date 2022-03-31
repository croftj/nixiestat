#include "wifi.h"

#include "esp_pthread.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <thread>

#include "configuration.h"

#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1
#define WIFI_MAXIMUM_RETRY    0

extern Configuration *config;

static EventGroupHandle_t s_wifi_event_group;

gpio_num_t  m_ledPin = (gpio_num_t)0; 
bool        Wifi::m_isConnected = false; 

void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
   static int s_retry_num = 0;
   ESP_LOGI(TAG, "Enter: event_id = %d, retry_num %d", event_id, s_retry_num);
   if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
   {
      ESP_LOGI(TAG, "starting connect");
      esp_wifi_connect();
   }
   else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
   {
      ESP_LOGI(TAG,"retry wifi connect");
      esp_wifi_connect();
      ESP_LOGI(TAG,"connect to the AP failed");
      gpio_set_level(m_ledPin, 1);
   }
   else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
   {
      ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
      ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
      s_retry_num = 0;
      xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
      ESP_LOGI(TAG, "Turning on green LED");
      gpio_set_level(m_ledPin, 0);
   }
}

/***************************************************************/
/* This function does the initialization of the wifi subsystem */
/* and starts it.                                              */
/*                                                             */
/* It does not connect to an Access Point!                     */
/***************************************************************/
void wifi_initialize(void)
{
   ESP_LOGI(TAG, "Enter...");
   s_wifi_event_group = xEventGroupCreate();
   ESP_ERROR_CHECK(esp_netif_init());
   ESP_ERROR_CHECK(esp_event_loop_create_default());
   esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
   assert(sta_netif);
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK(esp_wifi_init(&cfg));

   esp_event_handler_instance_t instance_any_id;
   esp_event_handler_instance_t instance_got_ip;
   ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_any_id));
   ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_got_ip));

   wifi_config_t wifi_config;
   memset(wifi_config.sta.ssid, '\0', sizeof(wifi_config.sta.ssid));
   memset(wifi_config.sta.password, '\0', sizeof(wifi_config.sta.password));
   strcpy((char*)wifi_config.sta.ssid, config->value("ssid").toString().c_str());
   strcpy((char*)wifi_config.sta.password, config->value("wpa_key").toString().c_str());
   ESP_LOGI(TAG, "len: %d, ssid = %s", sizeof(wifi_config.sta.ssid), (char*)wifi_config.sta.ssid);
   ESP_LOGI(TAG, "len: %d, password = %s", sizeof(wifi_config.sta.password), (char*)wifi_config.sta.password);
   wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
   wifi_config.sta.pmf_cfg.capable = true;
   wifi_config.sta.pmf_cfg.required = false;
   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

   ESP_LOGI(TAG, "Starting wifi");
   ESP_ERROR_CHECK(esp_wifi_start());
   ESP_LOGI(TAG, "Wifi Started!");
}

Wifi::Wifi(gpio_num_t led_iop)
{
   m_ledPin = led_iop;
   gpio_set_level(m_ledPin, 1);
   /*
   ESP_LOGI(TAG, "wifi initialised, waiting to connect.");
   EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

   ESP_LOGI(TAG, "Done waiting!");
   */
}


void *Wifi::connectWifi(void*)
{
   /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
   * Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */

   std::this_thread::sleep_for(std::chrono::seconds(5));
   ESP_LOGI(TAG, "wifi initialised, waiting to connect.");
   while (true)
   {
      EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
               WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
               pdFALSE,
               pdFALSE,
               portMAX_DELAY);

      ESP_LOGI(TAG, "Done waiting!");
      
      /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
      * happened. */
      if (bits & WIFI_CONNECTED_BIT) {
         ESP_LOGI(TAG, "Connected to AP");
         m_isConnected = true;
      } else if (bits & WIFI_FAIL_BIT) {
         ESP_LOGI(TAG, "Failed to connect to AP");
         m_isConnected = false;
      } else {
         ESP_LOGE(TAG, "UNEXPECTED EVENT");
         m_isConnected = false;
      }
      ESP_LOGI(TAG, "Clearing bits");
      xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);
   }
   return(NULL);
}
