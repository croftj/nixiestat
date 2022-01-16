#include "SensorData.h"
#include "esp_log.h"

#include <regex>

SensorData::SensorMap_t SensorData::m_known_sensors;
StaticSemaphore_t SensorData::m_sdMutexBuf;
SemaphoreHandle_t SensorData::m_sdMutex;
bool SensorData::m_haveMutex = false;

static std::regex device_name_re("^([-a-z_0-9]+)/");

#define TAG __PRETTY_FUNCTION__

/***************************************************************/
/* This function compares to strings without regard to case    */
/*                                                             */
/* It returns true if string first is less than string secode. */
/***************************************************************/
bool nc_compare (const std::string& first, const std::string& second)
{
   unsigned int i = 0;
   while ( (i < first.length()) && (i < second.length()) )
   {
      if      (tolower(first[i]) < tolower(second[i])) return true;
      else if (tolower(first[i]) > tolower(second[i])) return false;
      ++i;
   }
   return ( first.length() < second.length() );
}

void SensorData::addSensor(SensorData &sensor)
{
//   ESP_LOGI(TAG, "Taking semaphore");
   if (xSemaphoreTake(m_sdMutex, 50 / portTICK_RATE_MS))
   {
      m_known_sensors[sensor.name()] = sensor;
      xSemaphoreGive(m_sdMutex);
   }
   else
      ESP_LOGE(TAG, "Semaphore blocked");
//   ESP_LOGI(TAG, "done");
}

SensorData SensorData::getSensor(std::string name)
{
   std::smatch match;
   SensorData rv;
//   ESP_LOGI(TAG, "Taking semaphore");
   if (xSemaphoreTake(m_sdMutex, 50 / portTICK_RATE_MS))
   {
      if (name.find('/') != std::string::npos)
      {
         rv = m_known_sensors[name];
      }
      else
      {
         for (auto it = m_known_sensors.begin(); it != m_known_sensors.end(); ++it )
         {
            if (std::regex_search(it->first, match, device_name_re))
            {
               ESP_LOGI(TAG, "Found match, device = %s", match[1].str().c_str());
               if (name == match[1].str()) 
               {
                  rv = m_known_sensors[it->first];
                  break;
               }
            }
         }
      }
      xSemaphoreGive(m_sdMutex);
   }
   else
      ESP_LOGE(TAG, "Semaphore blocked");
//   ESP_LOGI(TAG, "done");
   
   return(rv);
}

SensorData::SensorList_t SensorData::getAllSensors()
{
   SensorData::SensorList_t rv;
//   ESP_LOGI(TAG, "Taking semaphore");
   if (xSemaphoreTake(m_sdMutex, 50 / portTICK_RATE_MS))
   {
      for (auto it = m_known_sensors.begin(); it != m_known_sensors.end(); ++it )
      {
         rv.push_back(it->first);
      }
      xSemaphoreGive(m_sdMutex);
      rv.sort(nc_compare);
   }
   else
      ESP_LOGE(TAG, "Semaphore blocked");
//   ESP_LOGI(TAG, "done");
   return(rv);
}
