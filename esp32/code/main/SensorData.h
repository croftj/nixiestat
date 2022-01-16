#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <string>
#include <list>
#include <unordered_map>

class SensorData
{
public:
   typedef std::list<std::string> SensorList_t;

   SensorData(const SensorData &other)
   {
      m_temp = other.m_temp;
      m_time = other.m_time;
      m_sensor = other.m_sensor;
      m_device = other.m_device;
      if ( ! m_haveMutex)
      {
         m_sdMutex = xSemaphoreCreateMutexStatic(&m_sdMutexBuf);
         xSemaphoreGive(m_sdMutex);
         m_haveMutex = true;
      }
   }

   SensorData()
   {
      m_temp = 0.0;
      m_time = 0;
      m_sensor = std::string("");
      m_device = std::string("");
      if ( ! m_haveMutex)
      {
         m_sdMutex = xSemaphoreCreateMutexStatic(&m_sdMutexBuf);
         xSemaphoreGive(m_sdMutex);
         m_haveMutex = true;
      }
   }

   SensorData(std::string sensor, std::string device, float temp, int timestamp) :
      m_sensor(sensor),
      m_device(device),
      m_temp(temp),
      m_time(timestamp)
   {
      m_sdMutex = xSemaphoreCreateMutexStatic(&m_sdMutexBuf);
      xSemaphoreGive(m_sdMutex);
   }

   bool isValid()
   {
      return(m_device.length() == 0 || m_sensor.length() == 0);
   }

   std::string name()
   {
      return (m_device + "/" + m_sensor);
   }
   
   std::string sensor()
   {
      return(m_sensor);
   }

   std::string device()
   {
      return(m_device);
   }

   float temperature()
   {
      return(m_temp);
   }

   int timestamp()
   {
      return(m_time);
   }

   static void addSensor(SensorData &sensor);
   static SensorData   getSensor(std::string name);
   static SensorList_t getAllSensors();

   void operator= (const SensorData &v)
   {
      m_sensor = v.m_sensor;
      m_device = v.m_device;
      m_temp = v.m_temp;
      m_time = v.m_time;
   }

   bool operator== (const SensorData &v) const
   {
      return (v.m_sensor == this->m_sensor && v.m_device == this->m_device);
   }

   bool operator== (const std::string &sensor_name) const
   {
      return((m_device + "/" + m_sensor) == sensor_name);
   }

private:
   typedef std::unordered_map<std::string, SensorData> SensorMap_t;

   std::string    m_sensor;
   std::string    m_device;
   float          m_temp;
   int            m_time;

   static StaticSemaphore_t   m_sdMutexBuf;
   static SemaphoreHandle_t   m_sdMutex;
   static SensorMap_t         m_known_sensors;
   static bool                m_haveMutex;
};


#endif
