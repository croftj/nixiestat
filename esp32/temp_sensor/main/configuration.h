#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <map>
#include <string>
#include "menu_console.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "variant.h"

class Configuration : public MenuConsole
{
public:
   typedef std::map<std::string, std::string*> flash_map_t;
   typedef std::map<std::string, std::string> DefaultValue_t;
   typedef std::pair<Variant*, Variant*> SensorDef_t;
   typedef std::vector<SensorDef_t> SensorDefList_t;

   Configuration(uart_port_t port_num);

   static Variant m_sampleInterval;
   static Variant m_reportInterval;
   static Variant m_outsideTemp;
   static Variant m_setTemp;
   static Variant m_sensorPort;

   static Variant m_radiatorDevId;
   static Variant m_roomTempDevId;

   static Variant m_ssid;
   static Variant m_wapkey;
   static Variant m_mqttBroker;
   static Variant m_mqttUser;
   static Variant m_mqttPass;
   static Variant m_mqttIdent;
   static Variant m_mqttTopic;
   static Variant m_mqttLogTopic;
   static Variant m_mqttTempTopic;
   static Variant m_pingTopic;

   static Variant m_sensorName1;
   static Variant m_sensorName2;
   static Variant m_sensorName3;
   static Variant m_sensorName4;
   static Variant m_sensorName5;

   static Variant m_sensorAddr1;
   static Variant m_sensorAddr2;
   static Variant m_sensorAddr3;
   static Variant m_sensorAddr4;
   static Variant m_sensorAddr5;

   void *process();
   virtual void changeMade();
   Variant value(std::string key);
   int findSensorAddr(std::string device_addr);
   void updateSensors(std::vector<std::string> devices);
   SensorDefList_t &sensorList()
   {
      return(m_sensors);
   }

   std::string sensorName(const SensorDef_t &sensor)
   {
      return(sensor.second->toString());
   }

   std::string sensorAddress(const SensorDef_t &sensor)
   {
      return(sensor.first->toString());
   }

   void readyWait()
   {
      while (!m_ready);
   }

private:
   volatile bool           m_ready;
   static flash_map_t      m_confMap;
   static DefaultValue_t   m_defaultValues;
   static nvs_handle_t     m_nvsHandle;
   static SensorDefList_t  m_sensors;
};

namespace ConfigurationThread
{
   void *exec(void* obj);
}

#endif
