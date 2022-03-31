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
   enum 
   {
      Heat         = 0,
      Cool         = 1,
      CURRENT = 0,
      SETTING_1,
      SETTING_2,
      SETTING_3,
      SETTING_4,
      NUM_SETTINGS
   };

   typedef std::map<std::string, std::string*> flash_map_t;
   typedef std::map<std::string, std::string> DefaultValue_t;
   typedef std::pair<Variant*, Variant*> SensorDef_t;
   typedef std::vector<SensorDef_t> SensorDefList_t;

   Configuration(uart_port_t port_num);

   static Variant m_SendStatus;
   static Variant m_sampleInterval;
   static Variant m_reportInterval;
   static Variant m_outsideTemp;
   static Variant m_setTemp;
   static Variant m_sensorPort;
   static Variant m_mode;
   static Variant m_remoteRelays;

   static Variant m_outsideSensor;
   static Variant m_roomTempSensor;

   static Variant m_radiatorSensor;
   static Variant m_coolSensor;

   static Variant m_ssid;
   static Variant m_wapkey;
   static Variant m_mqttBroker;
   static Variant m_mqttUser;
   static Variant m_mqttPass;
   static Variant m_mqttIdent;
   static Variant m_mqttTopic;
   static Variant m_mqttLogTopic;
   static Variant m_mqttStatusTopic;
   static Variant m_mqttFurnaceTopic;
   static Variant m_mqttTempTopic;
   static Variant m_pingTopic;
   static Variant m_tzMinutes;

   static Variant m_coolTimes[5];
   static Variant m_coolTemps[5];

   static Variant m_coolMaxCycleTime;
   static Variant m_coolMinCycleTime;
   static Variant m_coolCycleMaximum;
   static Variant m_coolCycleMinimum;

   static Variant m_coolCycleHyst;
   static Variant m_coolPidEnabled;
   static Variant m_coolKp;
   static Variant m_coolKi;
   static Variant m_coolKd;
   static Variant m_coolOutsideCurve;
   static Variant m_coolOutsideAdj;
   static Variant m_coolCycleSensor;

   static Variant m_heatTimes[5];
   static Variant m_heatTemps[5];

   static Variant m_heatMaxCycleTime;
   static Variant m_heatMinCycleTime;
   static Variant m_heatCycleMaximum;
   static Variant m_heatCycleMinimum;

   static Variant m_heatCycleHyst;
   static Variant m_heatPidEnabled;
   static Variant m_heatKp;
   static Variant m_heatKi;
   static Variant m_heatKd;
   static Variant m_heatOutsideCurve;
   static Variant m_heatOutsideAdj;
   static Variant m_heatCycleSensor;

   static Variant m_vacationTemp;
   static Variant m_vacationMode;
   static Variant m_currentMode;

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
   static Variant m_resetCount;
   static Variant m_displayEnb;
   static Variant m_forceOn;

   static Variant m_cycleHyst;
   static Variant m_pidEnabled;
   static Variant m_Kp;
   static Variant m_Ki;
   static Variant m_Kd;
   static Variant m_outsideCurve;
   static Variant m_outsideAdj;
   static Variant m_cycleSensor;
   static Variant m_cycleMinTime;
   static Variant m_cycleMaxTime;
   static Variant m_cycleMin;
   static Variant m_cycleMax;
   static Variant m_targetSensor;
   static Variant m_overideOffset;

   static const char GET_KEYS_CMD[];
   static const char GET_KNOWN_SENSORS_CMD[];

   std::string get_known_sensors();
   std::string get_all_keys();
   int CalculateCurrentSetting();
   float getCurrentTempSetting();
   void *process();
   virtual void changeMade();
   Variant value(std::string key);
   void setValue(std::string key, Variant value, bool keep = true);
   int findSensorAddr(std::string device_addr);
   void updateSensors(std::vector<std::string> devices);
   void setModeValues();

   int adjustedTimeOfDay();
   void setTzMinutes(int tz_mins)
   {
      setValue("tz_minutes", Variant(tz_mins));
   }

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
   volatile bool              m_ready;
   static flash_map_t         m_confMap;
   static DefaultValue_t      m_defaultValues;
   static nvs_handle_t        m_nvsHandle;
   static SensorDefList_t     m_sensors;
};

namespace ConfigurationThread
{
   void *exec(void* obj);
}

#endif
