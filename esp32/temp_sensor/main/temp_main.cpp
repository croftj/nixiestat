/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <thread>

#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_pthread.h"
#include "esp_task_wdt.h"
#include "esp_timer_cxx.hpp"
#include "esp_exception.hpp"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "configuration.h"
#include "json11.hpp"
#include "mqtt_client.h"
#include "mqtt.h"
/*

*/

//static const char *TAG = "MQTT_EXAMPLE";
#define TAG __PRETTY_FUNCTION__

#define MQTT_EXAMPLE_CODE     0
#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1
#define WIFI_MAXIMUM_RETRY    10

#define CONFIGURATION_TOPIC   "/configure"
#define TIME_TOPIC            "/time"

#define RED_LED_PIN  ((gpio_num_t)26)
#define GRN_LED_PIN  ((gpio_num_t)21)
#define GPIO_LED_MASK (/*(1ULL<<SNS_LED_PIN) | (1ULL<<RED_LED_PIN) | */(1ULL<<GRN_LED_PIN))

using namespace std;
using namespace json11;

#if 0
extern MQTT*      mqtt_bus;
extern Display*   display;
extern *
extern *
extern *
extern *
#endif

void* TempSensorThread(void* arg);

Configuration *config;
gpio_config_t led_conf;

static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base,
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
      if (s_retry_num <= WIFI_MAXIMUM_RETRY)
      {
         esp_wifi_connect();
         s_retry_num++;
         ESP_LOGI(TAG, "retry to connect to the AP");
      }
      else
      {
         xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
      }
      ESP_LOGI(TAG,"retry wifi connect");
      esp_wifi_connect();
      ESP_LOGI(TAG,"connect to the AP fail");
      gpio_set_level(GRN_LED_PIN, 0);
      this_thread::sleep_for(std::chrono::milliseconds(50));
      gpio_set_level(GRN_LED_PIN, 1);
   }
   else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
   {
      ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
      ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
      s_retry_num = 0;
      xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
      ESP_LOGI(TAG, "Turning on green LED");
      gpio_set_level(GRN_LED_PIN, 0);
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
//   strcpy((char*)wifi_config.sta.ssid, "athena");
   memcpy(wifi_config.sta.ssid, config->value("ssid").toString().c_str(), sizeof(wifi_config.sta.ssid));
//   ESP_LOGI(TAG, "len: %d, config_ssid = %s, ssid = %s", sizeof(wifi_config.sta.ssid), config->value("ssid").toString().c_str(), (char*)wifi_config.sta.ssid);
   memset(wifi_config.sta.password, '\0', sizeof(wifi_config.sta.password));
   memcpy(wifi_config.sta.password, config->value("wpa_key").toString().c_str(), sizeof(wifi_config.sta.password));
//   ESP_LOGI(TAG, "len: %d, config_key = %s, key = %s", sizeof(wifi_config.sta.password), config->value("wpa_key").toString().c_str(), (char*)wifi_config.sta.password);
   wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
   wifi_config.sta.pmf_cfg.capable = true;
   wifi_config.sta.pmf_cfg.required = false;
   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
   ESP_ERROR_CHECK(esp_wifi_start());
   ESP_LOGI(TAG, "Wifi Started!");
}

#define DEFAULT_SCAN_LIST_SIZE 15
extern "C" void wifi_scan()
{
   ESP_ERROR_CHECK(esp_netif_init());
   ESP_ERROR_CHECK(esp_event_loop_create_default());
   esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
   assert(sta_netif);

   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK(esp_wifi_init(&cfg));

   uint16_t number = DEFAULT_SCAN_LIST_SIZE;
   wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
   uint16_t ap_count = 0;
   memset(ap_info, 0, sizeof(ap_info));

   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   ESP_ERROR_CHECK(esp_wifi_start());

   ESP_LOGI(TAG, "Starting scan....");
   esp_wifi_scan_start(NULL, true);
   ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
   ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
   ESP_LOGI(TAG, "Total APs scanned = %d", ap_count);
   for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
   {
      ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
      ESP_LOGI(TAG, "RSSI \t\t%d", (int)ap_info[i].rssi);
      ESP_LOGI(TAG, "Channel \t\t%d", (int)ap_info[i].primary);
   }
}

static void* ping_home(void *)
{
   bool led_state = true;
   this_thread::sleep_for(std::chrono::seconds(15));
   while (true)
   {
      time_t timestamp;
      time(&timestamp);
#if 1
      if (true)
      {
         string config_topic = config->value("mqtt_ident").toString() + "/configure";
         string time_topic = config->value("mqtt_ident").toString() + "/time";
         
         vector<string> devices;
         Configuration::SensorDefList_t device_list = config->sensorList();

         Configuration::SensorDefList_t::iterator dev;
         for (dev = device_list.begin(); dev != device_list.end(); ++dev)
         {
            string sn = config->sensorName(*dev);
            string sa = config->sensorAddress(*dev);
            if (sn.length() > 0)
            {
               devices.push_back(sn);
            }
            else if (sa.length() > 0)
            {
               devices.push_back(sa);
            }
         }
         json11::Json js = Json::object {
            { "message_type", "ping" }, 
            { "conf_topic", config_topic }, 
            { "device", config->value("mqtt_ident").toString() }, 
            { "temp_topic", config->value("mqtt_temp_topic").toString() }, 
            { "log_topic", config->value("mqtt_log_topic").toString( )}, 
            { "sensors", devices }, 
            { "time_topic", time_topic },
            { "timestamp",  (int)timestamp}
         };
         mqtt_bus->sendData("ping", js.dump().c_str());
      }
#endif
      this_thread::sleep_for(std::chrono::seconds(30));
      esp_task_wdt_reset();
   }
   return(NULL);
}


extern "C" void app_main(void)
{
   size_t fs = heap_caps_get_free_size(MALLOC_CAP_DEFAULT|MALLOC_CAP_32BIT|MALLOC_CAP_8BIT);
   ESP_LOGI(TAG, "[APP] Startup..");
   ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
   
   esp_log_level_set("*", ESP_LOG_VERBOSE);
   
   esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
   cfg.stack_size = (4 * 1024);
   cfg.inherit_cfg = true;
   esp_pthread_set_cfg(&cfg);

   this_thread::sleep_for(std::chrono::milliseconds(1000));
   ESP_ERROR_CHECK(nvs_flash_init());

   config = new Configuration(0);
   pthread_t console_thread;
   pthread_create(&console_thread, NULL, ConfigurationThread::exec, config);

   this_thread::sleep_for(std::chrono::milliseconds(1000));
   ESP_ERROR_CHECK(nvs_flash_init());

   cfg.stack_size = (6 * 1024);
   cfg.inherit_cfg = true;
   esp_pthread_set_cfg(&cfg);
   ESP_LOGI(TAG, "Starting mqtt message thread");
   pthread_t mqtt_thread;
   pthread_create(&mqtt_thread, NULL, MQTT::procMessages, config);
   ESP_LOGI(TAG, "Free space %d", fs);
   
   ESP_LOGI(TAG, "Starting TempSensor thread");
   pthread_t temp_sensor_thread;
   pthread_create(&temp_sensor_thread, NULL, TempSensorThread, config);
   ESP_LOGI(TAG, "Free space %d", fs);

   ESP_LOGI(TAG, "Starting ping thread");
   pthread_t ping_thread;
   pthread_create(&ping_thread, NULL, ping_home, config);
   ESP_LOGI(TAG, "Free space %d", fs);

   led_conf.intr_type = GPIO_INTR_DISABLE;
   led_conf.mode = GPIO_MODE_OUTPUT;
   //bit mask of the pins that you want to set,e.g.GPIO18/19
   led_conf.pin_bit_mask = GPIO_LED_MASK;
   led_conf.pull_down_en = (gpio_pulldown_t)0;
   led_conf.pull_up_en = (gpio_pullup_t)0;
   gpio_config(&led_conf);
   gpio_set_level(RED_LED_PIN, 0);
   gpio_set_level(GRN_LED_PIN, 0);


   /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
   * Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
   wifi_initialize();

   ESP_LOGI(TAG, "wifi initialised, waiting to connect.");
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
//         ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
//                  config->value("ssid").toString().c_str(), config->value("wpa_key").toString().c_str());
   } else if (bits & WIFI_FAIL_BIT) {
      ESP_LOGI(TAG, "Failed to connect to AP, rebooting");
      assert(0);
//         ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
//                  config->value("m_ssid").c_str(), config->m_wapkey.c_str());
   } else {
      ESP_LOGE(TAG, "UNEXPECTED EVENT");
      assert(0);
   }
   
#if 1
   /***************************************************/
   /* Connect to the MQTT broker and subscribe to the */
   /* required topics                                 */
   /***************************************************/
   if (true)
   {
      cfg.stack_size = (4 * 1024);
      cfg.inherit_cfg = true;
      esp_pthread_set_cfg(&cfg);

      ESP_LOGI(TAG, "Connecting to MQTT buss");
      string board = config->value("mqtt_ident").toString();
      string topic;
      string broker = config->value("mqtt_broker").toString();
      mqtt_bus->setBroker(broker);
#if 1 || PROCESS_MESSAGES
      this_thread::sleep_for(std::chrono::milliseconds(2000));
      ESP_LOGI(TAG, "Subscribing topics");
      ESP_LOGI(TAG, "Subscribing to topic: %s", (board + CONFIGURATION_TOPIC).c_str());
      topic = board + CONFIGURATION_TOPIC;
      mqtt_bus->subscribe(topic);
      ESP_LOGI(TAG, "Subscribing to topic: %s", (board + TIME_TOPIC).c_str());
      topic = board + TIME_TOPIC;
      mqtt_bus->subscribe(topic);
#endif
   }

#endif

   fs = heap_caps_get_free_size(MALLOC_CAP_DEFAULT|MALLOC_CAP_32BIT|MALLOC_CAP_8BIT);
   ESP_LOGI(TAG, "Free space %d", fs);
   ESP_LOGI(TAG, "Returning from app_main....");
}
