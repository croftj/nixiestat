#include "thermostat.h"

#include <math.h>

#include "configuration.h"
#include "CycleData.h"
#include "json11.hpp"
#include "mqtt.h"
#include "PID.h"
#include "SensorData.h"
#include "TempCycle.h"

#include "esp_system.h"
#include "esp_log.h"
//#include "sys/time.h"

#define TAG __PRETTY_FUNCTION__

using namespace std;
using namespace json11;

extern Configuration *config;

PID pid(0, 0, 0);

void Thermostat::initialize()
{
   m_override  = false;
}

void Thermostat::SetOverride(float temp)
{
   Variant* tempSettings;

   if ( Configuration::m_currentMode.toInt() == Configuration::Heat ) 
   {
      tempSettings = Configuration::m_heatTemps;
   }
   else
   {
      tempSettings = Configuration::m_coolTemps;
   }

   m_overrideIndex = config->CalculateCurrentSetting();
   tempSettings[0].setValue(Variant(temp));
   m_overrideTemp = temp;
   m_override = true;
}

bool Thermostat::ReachedOverride(float cur_temp)
{
   bool rv = false;

//   if (dbg > 1) Serial  << __PRETTY_FUNCTION__ << " m_override =  " << m_override << endl;
   if ( m_override == true ) 
   {
//      if (dbg) Serial  << __PRETTY_FUNCTION__ << "m_target = " << m_target << endl;
//      if (dbg) Serial  << __PRETTY_FUNCTION__ << "m_overrideTemp = " << m_overrideTemp << endl;
//      if (dbg) Serial  << __PRETTY_FUNCTION__ << "cur_temp = " << cur_temp << endl;
      if ( (m_target > m_overrideTemp && 
            cur_temp <= m_overrideTemp) ||
           (m_target <= m_overrideTemp &&
            cur_temp >= m_overrideTemp) )
      {
         rv = true;
      }
   }
//   if (dbg > 1) Serial  << __PRETTY_FUNCTION__ << "rv = " << rv << endl;
   return(rv);
}

#if MULTI_SENSOR
SensorData selectSensor(QString sensors)
{
   SensorData rv;

   QStringList sens_list = sensors.split(QRegExp("\\s*,\\s*"));
   foreach (SensorData sensor, sens_list)
   {
      if (sensor.isValid())
      {
         if (rv.timestamp() < sensor.timestamp() && rv.temperature() < sensor.temperature())
         {
            rv = sensor;
         }
      }
   }
   return(rv);
}
#endif

void Thermostat::run()
{
   SensorData  sd_cyc;
   SensorData  sd_out;
   SensorData  sd_tgt;
   bool        pid_enabled;
   bool        force_on;
   RunState    run_state;
   RunState    old_state = UNSET;

   old_state = NORMAL;
   this_thread::sleep_for(std::chrono::seconds(15));
   while (true)
   {
      time_t now;
      time(&now);
      CycleData cyc_data;

      if (now < 1636000000)
      {
         ESP_LOGI(TAG, "Waiting for system time to be set");
         this_thread::sleep_for(std::chrono::seconds(30));
         continue;
      }
         
//      config->lockVars();
      /*************************************************/
      /* Assume all is well and operating until proven */
      /* otherwise.                                    */
      /*************************************************/
      run_state = NORMAL;
      pid_enabled = config->value("pid_enable").toBool();
      force_on = config->value("force_on").toBool();
      cyc_data.setPidEnabled(pid_enabled);
      
      /*****************************************************/
      /* Check the sensors, if we lost contact with any of */
      /* them, set the operating state approriately.       */
      /*****************************************************/
      ESP_LOGI(TAG, "Fetching sensors");
      auto sd_cyc = SensorData::getSensor(config->value("cyc_sensor").toString());
      auto sd_out = SensorData::getSensor(config->value("outside_sensor").toString());
      auto sd_tgt = SensorData::getSensor(config->value("target_sensor").toString());
#if MULTI_SENSOR
      auto sd_tgt = selectSensor(config->value("target_sensor").toString());
#endif
      cyc_data.setOutsideTemp(sd_out.temperature());
      cyc_data.setCurRoomTemp(sd_tgt.temperature());
      cyc_data.setCurCycleTemp(sd_cyc.temperature());
      if (now - sd_cyc.timestamp() > 5 * 60)
      {
         pid_enabled = false;
         run_state = ALARM2;
      }
      if (now - sd_out.timestamp() > 5 * 60)
      {
         run_state = ALARM1;
      }
      if (now - sd_tgt.timestamp() > 3 * 60)
      {
         run_state = ALARM3;
      }
      cyc_data.setAlarm(run_state);

      ESP_LOGI(TAG, "pid_enabled: %d, run_state: %d", pid_enabled, run_state);

      /*******************************************************/
      /* If we can at at least see the target sensor, assume */
      /* we can stil maintain control even if not perfectly  */
      /*******************************************************/
      if (run_state < ALARM3)
      {
         float cur_cycle;
         float target_temp = config->getCurrentTempSetting();
         cyc_data.setTgtRoomTemp(target_temp);
         if (ReachedOverride(sd_tgt.temperature()))
         {
            ESP_LOGI(TAG, "Override temp reached");
            CancelOverride();
         }

         if (pid_enabled)
         {
            pid.SetTarget(target_temp);

            /***************************************************/
            /* If we are cooling the house, invert the outside */
            /* temp so it rises as the temperature drops       */
            /***************************************************/
            float outside_temp = (config->value("mode").toInt() == Configuration::Heat) ?
                                 sd_out.temperature() :
                                 -(sd_out.temperature());
            cur_cycle = pid.Compute(sd_tgt.temperature());

            ESP_LOGI(TAG, "target = %f, room = %f, cur_cycle = %f", target_temp, sd_tgt.temperature(), cur_cycle);
            float offset = config->value("out_adj").toDouble() - outside_temp;
            offset = pow(offset, 1 + config->value("out_curve").toDouble());
//            cur_cycle += offset + config->value("set_point").toDouble();
            cur_cycle += offset + target_temp;
            ESP_LOGI(TAG, " updated cur_cycle = %f, offset = %f", cur_cycle, offset);
            temp_cycle.setSensor(config->value("cyc_sensor").toString());
         }
         else
         {
            cur_cycle = config->getCurrentTempSetting();
            temp_cycle.setSensor(config->value("target_sensor").toString());
         }
         cyc_data.setTgtCycleTemp(cur_cycle);
         ESP_LOGI(TAG, "Hystorisis = %f", (float)config->value("cyc_hyst").toDouble());
         ESP_LOGI(TAG, "Min. CycleTime = %d", config->value("cyc_min_time").toInt());
         temp_cycle.setCycleTime(config->value("cyc_min_time").toInt());
         temp_cycle.setTemp(cur_cycle);
         temp_cycle.setAbsMaximum(config->value("cyc_max_temp").toDouble());
         temp_cycle.setAbsMinimum(config->value("cyc_min_temp").toDouble());
         temp_cycle.setHysterisis(config->value("cyc_hyst").toDouble());
         ESP_LOGI(TAG, "Max. CycleTime = %d", config->value("cyc_max_time").toInt());
         temp_cycle.setMaxCycleTime(config->value("cyc_max_time").toInt());
         temp_cycle.setMode(config->value("mode").toInt());
         temp_cycle.update(&cyc_data, force_on);
      }

      /*********************************************************/
      /* Otherwise, switch to the external failsafe thermostat */
      /*********************************************************/
      else
      {
         
      }
      if (old_state != run_state)
      {
         time_t timestamp;
         time(&timestamp);
         json11::Json js = Json::object {
            { "message_type", "alarm" }, 
            { "device", config->value("mqtt_ident").toString() }, 
            { "run_state", (int)run_state}, 
            { "timestamp",  (int)timestamp}
         };
         mqtt_bus->sendData("thermostat", js.dump().c_str());
      }
//      config->releaseVars();
      old_state = run_state;

      ESP_LOGI(TAG, "Looping, run_state = %d", run_state);
      if (config->value("send_status").toBool())
      {
         std::string cyc_message = cyc_data.toString();
         mqtt_bus->sendData("status", cyc_message.c_str());
      }
//      this_thread::sleep_for(std::chrono::seconds(Configuration::m_sampleInterval.toInt()));
      this_thread::sleep_for(std::chrono::seconds(30));
   }
}

void *Thermostat::exec(void *obj)
{
   static_cast<Thermostat*>(obj)->run();
   return(NULL);
}

Thermostat thermostat;

