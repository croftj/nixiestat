#include "configuration.h"
#include "json11.hpp"
#include "mqtt.h"
#include "esp_log.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "TempSensors.h"

#define TAG __PRETTY_FUNCTION__

static TempSensors ts;

using namespace std;
using namespace json11;

void* TempSensorThread(void* arg)
{
   Configuration *config = static_cast<Configuration*>(arg);
   time_t timestamp;

   ESP_LOGD(TAG, "Initializing sensor port: %d", config->value("sens_port").toInt());
   ts.initialize(config->value("sens_port").toInt());
   for (int x = 0; x < ts.count(); x++)
   {
      ts.readNextSensor();
   }

   ts.dumpSensors();

   time_t last_report_time;
   time(&last_report_time);

   while(true)
   {
      ts.discover();
      config->updateSensors(ts.allDevices());
      for (int x = 0; x < ts.count(); x++)
      {
            ts.readNextSensor();
      }

      time(&timestamp);

      string device = config->value("mqtt_ident").toString();
      std::vector<std::string> addrs = ts.allDevices();
      for (int x = 0; x < ts.count(); x++)
      {
         float val = ts.read(x, TempSensors::CURRENT);
         string addr = addrs[x];

         json11::Json js = Json::object
         {
            { "message_type", "temperature" }, 
            { "sensor",       addr },
            { "temperature",  val },
            { "timestamp",   (int)timestamp },
            { "device",       device }
         };
         string topic = config->value("mqtt_temp_topic").toString();
         ESP_LOGD(TAG, "Sending '%s' to topic '%s'", js.dump().c_str(), topic.c_str());
         mqtt_bus->sendData(topic.c_str(), js.dump().c_str());
      }

      if (timestamp - config->value("rept_int").toInt() > last_report_time)
      {
         time(&last_report_time);
         for (int x = 0; x < ts.count(); x++)
         {
            std::string sensor_data = "{\"message_type\": \"log\", ";
            sensor_data += "\"temp\": ";
            sensor_data += ts.toJson(x, timestamp, device);
            sensor_data += "}";
            std::string topic = config->value("mqtt_log_topic").toString();
            ESP_LOGD(TAG, "Sending '%s' to topic '%s'", sensor_data.c_str(), topic.c_str());
            mqtt_bus->sendData(topic, sensor_data);
         }
         ts.clearTotals();
      }

      if (config->value("samp_int").toInt() > 0)
      {
         this_thread::sleep_for(std::chrono::seconds(config->value("samp_int").toInt()));
      }
      else
      {
         this_thread::sleep_for(std::chrono::seconds(30));
      }
   }
   return(NULL);
}
