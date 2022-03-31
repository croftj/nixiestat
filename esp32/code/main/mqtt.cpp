/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "mqtt.h"

#include <cstring>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <thread>

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_pthread.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "PT7C4339.h"

#include "configuration.h"
#include "display.h"
#include "json11.hpp"
#include "SensorData.h"
#include "thermostat.h"

#define TAG __PRETTY_FUNCTION__

#define PROCESS_INCOMMING 1
#define PROCESS_MESSAGES  1

#define CONFIGURATION_TOPIC   "/configure"
#define TIME_TOPIC            "/time"

#define CMD_DISPLAY_ENABLE "display_enb"
#define CMD_DIGIT_ON       "digit_ctl"
#define CMD_SEGMENT_ON       "segment_ctl"

using namespace std;
using namespace json11;

extern Configuration *config;

static pthread_t mqtt_thread = 0;


MQTT::topic_list_t MQTT::m_subTopics;
bool MQTT::m_busDisconnect = false;
bool MQTT::m_notReady = false;
MQTT::actions_t MQTT::m_action = MQTT::NOP;
Wifi* MQTT::m_wifi         = NULL;

esp_mqtt_client_handle_t  MQTT::m_client = NULL;

MQTT* mqtt_bus = new MQTT;

MQTT::MessageEntry* m_msgInQueue[] =
{
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   (MQTT::MessageEntry*)NULL
};

MQTT::MessageEntry* m_msgOutQueue[] =
{
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(), new MQTT::MessageEntry(),
   (MQTT::MessageEntry*)NULL
};

StaticSemaphore_t       MQTT::m_msgMutexBuf;
SemaphoreHandle_t       MQTT::m_msgMutex;

static void *connectionExec(void *obj)
{
   MQTT *mqtt = (MQTT*)obj;
   ESP_LOGI(TAG, "connectionExec(): Enter");
   while (true)
   {
      if (false)
      {
         if (mqtt->m_busDisconnect)
         {
            ESP_LOGE(TAG, "connectionExec(): mqtt disconnect detected!");
            mqtt->m_notReady = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            esp_mqtt_client_stop(mqtt->m_client);
            esp_mqtt_client_destroy(mqtt->m_client);
            mqtt->m_client = NULL;
            mqtt->m_busDisconnect = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
         }
         if (! mqtt->connected())
         {
            ESP_LOGI(TAG, "connectionExec(): calling start");
            mqtt->start();
         }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      esp_task_wdt_reset();
   }
   return(NULL);
}

static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
   static char topic[MQTT::TOPIC_LEN + 1];
   static char message[MQTT::MESG_LEN + 1];
   char *scp;
   char *cp;
   int fs;

    // your_context_t *context = event->context;
//   ESP_LOGI(TAG, "MQTT_EVENT: %d", event->event_id);
   switch (event->event_id)
   {
      case MQTT_EVENT_CONNECTED:
         ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
         MQTT::m_action = MQTT::SUBSCRIBE_TOPICS;
         MQTT::m_notReady = false;
         break;

      case MQTT_EVENT_DISCONNECTED:
         ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
         MQTT::m_notReady = false;
//         MQTT::m_busDisconnect = true;
//         mqtt_bus->m_client = NULL;
         break;

      case MQTT_EVENT_SUBSCRIBED:
         ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
         break;

      case MQTT_EVENT_UNSUBSCRIBED:
         ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
         break;

      case MQTT_EVENT_PUBLISHED:
         ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
         break;

      case MQTT_EVENT_DATA:
#if PROCESS_INCOMMING
         if (true && MQTT::TOPIC_LEN > event->topic_len && MQTT::MESG_LEN > event->data_len)
         {
            /***********************************************/
            /* Start be getting the message and topic name */
            /* into a local buffer                         */
            /***********************************************/
            memset(topic, '\0', MQTT::TOPIC_LEN + 1);
            cp = topic;
            scp = event->topic;
            for (int len = event->topic_len; len > 0; cp++, scp++, len--)
               *cp = *scp;
            *(cp++) = '\0';

            memset(message, '\0', MQTT::MESG_LEN + 1);
            cp = message;
            scp = event->data;
            for (int len = event->data_len; len > 0; cp++, scp++, len--)
               *cp = *scp;
            *(cp++) = '\0';

            /**************************************************/
            /* Find the next avalable slot in the buffer list */
            /* and save the message                           */
            /**************************************************/
            if (xSemaphoreTake(MQTT::m_msgMutex, 50 / portTICK_RATE_MS))
            {
               bool fail = true;
               for (int x = 0; m_msgInQueue[x] != NULL; x++)
               {
//                  ESP_LOGI(TAG, "Testing buf %d", x);
                  if ( ! m_msgInQueue[x]->isValid())
                  {
//                     ESP_LOGI(TAG, "Saving to buf %d", x);
                     m_msgInQueue[x]->set(topic, message);
                     fail = false;
                     break;
                  }
               }
               xSemaphoreGive(MQTT::m_msgMutex);
               if (fail)
               {
                  ESP_LOGE(TAG, "Message dropped! No room to in the queue");
               }
            }
            else
            {
               ESP_LOGE(TAG, "timeout!");
            } 
         }
         else
         {
            ESP_LOGE(TAG, "Message too long, skipped!");
         }
#else
         cp = topic;
         scp = event->topic;
         for (int len = event->topic_len; len > 0; cp++, scp++, len--)
            *cp = *scp;
         *(cp++) = '\0';
         cp = message;
         scp = event->data;
         for (int len = event->data_len; len > 0; cp++, scp++, len--)
            *cp = *scp;
         *(cp++) = '\0';
         ESP_LOGI(TAG, "Message received: topic: %s, msg: %s", topic, message);
#endif
         break;

      case MQTT_EVENT_ERROR:
         ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
         if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
         {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
         }
         ESP_LOGI(TAG, "MQTT_EVENT_ERROR- done");
         break;

      default:
         ESP_LOGI(TAG, "Other event id:%d", event->event_id);
         break;
   }

//   ESP_LOGI(TAG, "Exiting");
   return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb((esp_mqtt_event_handle_t)event_data);
}

bool MQTT::clientReady()
{
   return(m_client != NULL && ! m_busDisconnect && ! m_notReady);
}

void MQTT::subscribe(std::string &topic)
{
   if (clientReady())
   {
      esp_mqtt_client_subscribe(m_client, topic.c_str(), 0);
   }
}

MQTT::message_t *MQTT::getNext(MessageEntry** queue)
{
   static int next_x = 0;
   message_t *rv = NULL;
   if (xSemaphoreTake(MQTT::m_msgMutex, 50 / portTICK_RATE_MS))
   {
      int x = next_x;
      while (true)
      {

//         ESP_LOGI(TAG, "Checking for valid message @ %d", x);
         if (queue[x] == (message_t*)NULL)
         {
            x = 0;
         }
         else
         {
            if (queue[x]->isValid())
            {
               rv = queue[x];
               next_x = x;
               break;
            }
            x++;
         }
         if (x == next_x)
         {
            break;
         }
      }
      xSemaphoreGive(MQTT::m_msgMutex);
   }
   else
   {
      ESP_LOGE(TAG, "timeout!");
   } 
   return(rv);
}

void MQTT::releaseMessage(message_t *msg)
{
   if (xSemaphoreTake(MQTT::m_msgMutex, 50 / portTICK_RATE_MS))
   {
      msg->isValid();
      xSemaphoreGive(MQTT::m_msgMutex);
   }
   else
   {
      ESP_LOGE(TAG, "timeout!");
   } 
}

void MQTT::sendData(const char *topic, const char *json)
{
   if (xSemaphoreTake(MQTT::m_msgMutex, 50 / portTICK_RATE_MS))
   {
      bool fail = true;
      for (int x = 0; m_msgOutQueue[x] != NULL; x++)
      {
//                  ESP_LOGI(TAG, "Testing buf %d", x);
         if ( ! m_msgOutQueue[x]->isValid())
         {
            ESP_LOGI(TAG, "Saving to buf %d", x);
            m_msgOutQueue[x]->set(topic, json);
            fail = false;
            break;
         }
      }
      xSemaphoreGive(MQTT::m_msgMutex);
      if (fail)
      {
         ESP_LOGE(TAG, "Message dropped! No room to in the queue");
      }
   }
   else
   {
      ESP_LOGE(TAG, "timeout!");
   } 
}

void MQTT::sendData(std::string &topic, std::string &json)
{
   sendData(topic.c_str(), json.c_str());
}

bool MQTT::start(void)
{
   bool rv = false;
   ESP_LOGI(TAG, "start(): enter");
   std::string broker_str = m_broker;
   esp_mqtt_client_config_t mqtt_cfg;
   memset(&mqtt_cfg, '\0', sizeof(mqtt_cfg));
   mqtt_cfg.uri = broker_str.c_str();
   ESP_LOGI(TAG, "broker: %s", mqtt_cfg.uri);
   if ((m_client = esp_mqtt_client_init(&mqtt_cfg)) != NULL)
   {
      esp_mqtt_client_register_event(m_client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, m_client);
      esp_mqtt_client_start(m_client);
      rv = true;
   }
   return(rv);
}

void MQTT::setBroker(std::string &broker)
{
   m_broker = broker;
}

void* MQTT::procMessages(void *)
{
   this_thread::sleep_for(std::chrono::seconds(2));
   while (true)
   {
      if (m_wifi == NULL)
      {
         ESP_LOGW(TAG, "No wifi");
         if (m_client != NULL)
         {
            ESP_LOGW(TAG, "disconnecting mqtt bus");
            esp_mqtt_client_disconnect(m_client);
         }
         m_client = NULL;
         this_thread::sleep_for(std::chrono::seconds(1));
         continue;
      }
      else if ( ! m_wifi->connected() )
      {
         ESP_LOGW(TAG, "wifi not connected");
         if (m_client != NULL)
         {
            ESP_LOGW(TAG, "disconnecting mqtt bus");
            esp_mqtt_client_disconnect(m_client);
            m_client = NULL;
         }
         this_thread::sleep_for(std::chrono::seconds(1));
         continue;
      }
      else if (m_client == NULL)
      {
         ESP_LOGW(TAG, "wifi connected");
         ESP_LOGW(TAG, "connecting mqtt bus");
         mqtt_bus->start();
         this_thread::sleep_for(std::chrono::seconds(1));
         continue;
      }

      if (m_action == MQTT::SUBSCRIBE_TOPICS)
      {
         topic_list_t::iterator pos;
         for (pos =  m_subTopics.begin(); pos != m_subTopics.end(); ++pos)
         {
            mqtt_bus->subscribe(*pos);
         }
         m_action = MQTT::NOP;
      }
         
//      ESP_LOGI(TAG, "Processing incomming messages");
      string board = config->value("mqtt_ident").toString();
      MQTT::message_t *msg;
#if PROCESS_MESSAGES
      while ((msg = mqtt_bus->getNext(m_msgInQueue)) != NULL)
      {
         string topic = msg->topic();
         string msg_data = msg->message();
//         ESP_LOGI(TAG, "Invalidating message");
         ESP_LOGI(TAG, "recv msg: topic = %s, message = %s", topic.c_str(), msg_data.c_str());
         msg->invalidate();
         ESP_LOGD(TAG, "Here!");
         if (topic == board + CONFIGURATION_TOPIC)
         {
            string err;
            ESP_LOGI(TAG, "Have config topic");
#if 1
            json11::Json js = json11::Json::parse(msg_data, err);
            if (err.length() > 0)
            {
               ESP_LOGE(TAG, "Json Err: %s", err.c_str());
            }
            string topic = js["topic"].string_value();
            string cmd = js["cmd"].string_value();
            if (cmd.length() > 0)
            {
               std::string resp;
               resp.resize(2048);
               if (cmd == Configuration::GET_KEYS_CMD)
               {
                  resp = config->get_all_keys();
               }
               else if (cmd == CMD_DISPLAY_ENABLE)
               {
                  string state = js["state"].string_value();
                  ESP_LOGI(TAG, "state = %s", state.c_str());
                  if (state == "true")
                  {
                     ESP_LOGI(TAG, "Enabling HV");
                     display->touch();
                  }
                  else
                  {
                     ESP_LOGI(TAG, "Disabling HV");
                     display->disable();
                  }
               }
               else if (cmd == CMD_SEGMENT_ON)
               {
                  int segment = js["segment"].int_value();
                  ESP_LOGI(TAG, "segment = %d", segment);
                  if (segment  < IOPorts::NUM_SEGMENTS)
                  {
                     display->segmentOn(segment);
                  }
                  else
                  {
                     display->segmentsOff();
                  }
               }
               else if (cmd == CMD_DIGIT_ON)
               {
                  int digit = js["digit"].int_value();
                  ESP_LOGI(TAG, "digit = %d", digit);
                  if (digit  < IOPorts::NUM_DIGITS)
                  {
                     display->digitOn(digit);
                  }
                  else
                  {
                     display->digitsOff();
                  }
               }
               else if (cmd == Configuration::GET_KNOWN_SENSORS_CMD)
               {
                  resp = config->get_known_sensors();
               }
               else
               {
                  resp = config->procKeyRequest(cmd, true);
               }
               ESP_LOGI(TAG, "menu resp len: %d, str: %s", strlen(resp.c_str()), resp.c_str());
               if (topic.length() > 0)
               {
                  mqtt_bus->sendData(topic, resp);
               }
            }
#endif
         }
         if (topic == board + TIME_TOPIC)
         {
            string err;
            ESP_LOGI(TAG, "Have time topic");
            json11::Json js = json11::Json::parse(msg_data, err);
            if (err.length() > 0)
            {
               ESP_LOGE(TAG, "Json Err: %s", err.c_str());
            }
            time_t time = js["system_time"].int_value();
            int tz_minutes = js["tz_minutes"].int_value();
            struct timeval tv;
            struct timezone tz;
            tv.tv_sec = time;
            tv.tv_usec = 0;
            tz.tz_minuteswest = 0;
            tz.tz_dsttime = 0;
            ESP_LOGI(TAG, "setting system time: %d", (int)time);
            settimeofday(&tv, &tz);
            config->setTzMinutes(tz_minutes);
            time += config->value("tz_minutes").toInt() * 60; 
            ESP_LOGI(TAG, "setting RTC time: %d", (int)time);
            rtc_4339->updateRTCTime(time);
         }
         if (topic == config->value("mqtt_temp_topic").toString())
         {
            string err;
            ESP_LOGI(TAG, "Have temp topic");
            json11::Json js = json11::Json::parse(msg_data, err);
            if (err.length() > 0)
            {
               ESP_LOGE(TAG, "Json Err: %s", err.c_str());
            }
            float temp        = js["temperature"].number_value();
            string sensor     = js["sensor"].string_value();
            int    timestamp  = js["timestamp"].int_value();
            string device     = js["device"].string_value();
            SensorData sd(sensor, device, temp, timestamp);
            ESP_LOGD(TAG, "Adding or updating sensor");
            SensorData::addSensor(sd);
         }
      }

      while (clientReady() && (msg = mqtt_bus->getNext(m_msgOutQueue)) != NULL)
      {
         string topic = msg->topic();
         string msg_data = msg->message();
//         ESP_LOGI(TAG, "Invalidating message");
         ESP_LOGI(TAG, "Sending msg: topic = %s, message = %s", topic.c_str(), msg_data.c_str());
         msg->invalidate();
         esp_mqtt_client_publish(m_client, topic.c_str(), msg_data.c_str(), 0, 0, 0);
      }
#endif
//      ESP_LOGI(TAG, "Processing complete");
      this_thread::sleep_for(std::chrono::seconds(1));
   }
   return(NULL);
}

