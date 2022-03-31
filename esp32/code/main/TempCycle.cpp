#include "TempCycle.h"
#include "configuration.h"
#include "DBGPorts.h"
#include "IOPorts.h"
#include "json11.hpp"
#include "mqtt.h"
#include "SensorData.h"

#include "esp_log.h"

#define TAG __PRETTY_FUNCTION__

using namespace json11;

extern Configuration *config;

void TempCycle::initialize()
{
   m_prevCall              = false;
   m_lastCall              = false;
   m_initialCall           = true;
   m_minCycleTimeReached   = false;
}

void TempCycle::setCycleTime(uint16_t minutes)
{
   ESP_LOGI(TAG, "minutes = %d", minutes);
   m_minCycleTime = minutes * 60;
   ESP_LOGI(TAG, "set min cycle time to %d", m_minCycleTime);
}

bool TempCycle::update(CycleData *cyc_data, bool force_on)
{
   bool     rv             = false;
   float    target         = m_temp;
   uint16_t min_cycle_time = m_minCycleTime;

   struct timeval tv;
   gettimeofday(&tv, NULL);
   time_t cur_time = tv.tv_sec;
   time_t cycle_time = cur_time - m_lastCallTime;
   if (cycle_time > (time_t)10000)
   {
      cycle_time = (time_t)10 * 60;
   }
   cyc_data->setCycleTime(cycle_time);
   ESP_LOGI(TAG, "target = %f, min_cycle_time = %d, cycle_time = %d, m_initialCall = %d", m_temp, min_cycle_time, (int)cycle_time, (int)m_initialCall);

   /**********************************************************/
   /* If the cycle if forced on, just to the deed and return */
   /**********************************************************/
   if (force_on)
   {
      ESP_LOGW(TAG, "Cycle force on!!!");
      if ( m_mode == Configuration::Cool ) 
      {
         rv = call4Cool(true, cyc_data);
      }
      else 
      {
         rv = call4Heat(true, cyc_data);
      }
   }

   /**********************************************/
   /* Otherwise, see what really needs to happen */
   /**********************************************/
   else
   {
      /*********************************************************/
      /* get the current tempurature. If no sensors are found, */
      /* turn off everything and set an alarm condition        */
      /*********************************************************/
      SensorData sd_tgt = SensorData::getSensor(m_sensor);
      float currTemp = sd_tgt.temperature();
      ESP_LOGI(TAG, "currTemp = %f", currTemp);

      /**********************************************************/
      /* Now figure out what our true target temperature should */
      /* be taking into account hysterisis.                     */
      /**********************************************************/
      target = m_temp;
      if ( target > m_absMaximumTemp ) 
      {
         ESP_LOGW(TAG, "Target exceeded max temp");
         target = m_absMaximumTemp - 0.5;
      }
      else if ( target < m_absMinimumTemp )
      {
         ESP_LOGW(TAG, "Target fell below min temp");
         target = m_absMinimumTemp + 0.5;
      }

      if ( m_prevCall ) 
      {
         min_cycle_time = min_cycle_time / 2;
      }

      /***********************************************/
      /*   If  we  have  exceeded  the maximum time  */
      /*   that  the  heater/ac  should  be on then  */
      /*   force whichever off                       */
      /***********************************************/
      if ( cycle_time >= m_maxCycleTime && m_lastCall) 
      {
         ESP_LOGW(TAG, "Exceeded max cycle time");
         if ( m_mode == Configuration::Cool ) 
         {
            rv = call4Cool(false, cyc_data);
         }
         else 
         {
            rv = call4Heat(false, cyc_data);
         }
      }
      else 
      {
         /*************************************************/
         /* Test if the measured temperature exceeded the */
         /* limits                                        */
         /*************************************************/
         if ( m_mode == Configuration::Cool && currTemp < m_absMinimumTemp - 2.0 )
         {
            ESP_LOGW(TAG, "Forcing cooling off");
            rv = call4Cool(false, cyc_data);
         }
         else if ( m_mode == Configuration::Heat && currTemp > m_absMaximumTemp + 2.0)
         {
            ESP_LOGW(TAG, "Forcing heating off");
            rv = call4Heat(false, cyc_data);
         }
         
         /***************************************************/
         /* Otherwise, if we are cooling, figure out if the */
         /* cooler should be on or off and do the needful   */
         /***************************************************/
         else if ( m_mode == Configuration::Cool ) 
         {
            float target;
            ESP_LOGI(TAG, "Mode = cool");

            if ( m_lastCall ) 
            {
               target = (m_temp - (m_hyst / 2));
            }
            else 
            {
               target = (m_temp + (m_hyst / 2));
            }
            if ( m_initialCall || cycle_time >= min_cycle_time )
            {
               if ( currTemp > target ) 
               {
                  m_initialCall = false;
                  rv = call4Cool(true, cyc_data);
               }
               else
               {
                  rv = call4Cool(false, cyc_data);
               }
            }
         }

         /***********************************************/
         /* If we are heating, figure out if the heater */
         /* should be on or off and do the needful      */
         /***********************************************/
         else if ( m_mode == Configuration::Heat ) 
         {
            float target;
            ESP_LOGI(TAG, "Mode = heat");

            if ( m_lastCall ) 
            {
               target = (m_temp + (m_hyst / 2));
            }
            else 
            {
               target = (m_temp - (m_hyst / 2));
            }
            ESP_LOGI(TAG, "Hyst Adjusted target: %f, cycle_time: %d, initialCall: %d, lastCall = %d", target, (int)cycle_time, (int)m_initialCall, (int)m_lastCall);

            if ( m_initialCall || cycle_time >= min_cycle_time ) 
            {
               if ( currTemp < target ) 
               {
                  m_initialCall = false;
                  ESP_LOGI(TAG, "Call for heat");
                  rv = call4Heat(true, cyc_data);
               }
               else
               {
                  ESP_LOGI(TAG, "Call for NO heat");
                  rv = call4Heat(false, cyc_data);
               }
            }
            else
            {
               ESP_LOGI(TAG, "Call for %sheat", (m_lastCall) ? "" : "NO ");
               rv = call4Heat(m_lastCall, cyc_data);
            }
         }
      }

      /***********************************************/
      /*   If  we  turned the furnace/ac on of off,  */
      /*   start the min cycle time                  */
      /***********************************************/
      if ( rv ) 
      {
         m_lastCallTime =  cur_time;
      }
   }
   return(rv);
}

bool TempCycle::call4Cool(bool f, CycleData *cyc_data)
{
   bool rv = false;
   time_t timestamp;
   time(&timestamp);
   json11::Json js = Json::object {
      { "message_type", "furnace" }, 
      { "device", config->value("mqtt_ident").toString() }, 
      { "mode", "cool"}, 
      { "cycle", f ? "On" : "Off"}, 
      { "timestamp",  (int)timestamp}
   };
   mqtt_bus->sendData("thermostat", js.dump().c_str());

   if ( ! config->value("rmt_relays").toBool())
   {
      if ( f ) 
         Cool->off();
      else
         Cool->on();
   }
   if (f != m_lastCall)
   {
      m_lastCall = f;
      rv = true;
   }
   cyc_data->setCoolState(f);
   return(rv);
}

bool TempCycle::call4Heat(bool f, CycleData *cyc_data)
{
   bool rv = false;
   time_t timestamp;
   time(&timestamp);
   json11::Json js = Json::object {
      { "message_type", "furnace" }, 
      { "device", config->value("mqtt_ident").toString() }, 
      { "mode", "heat"}, 
      { "cycle", f ? "On" : "Off"}, 
      { "timestamp",  (int)timestamp}
   };
   mqtt_bus->sendData(config->value("mqtt_furn_topic").toString().c_str(), js.dump().c_str());

   if ( ! config->value("rmt_relays").toBool())
   {
      if ( f ) 
         Heat->off();
      else
         Heat->on();
   }
   if (f != m_lastCall)
   {
      m_lastCall = f;
//      gpio_set_level(DBG_PIN_4, m_lastCall);
      rv = true;
   }
   cyc_data->setHeatState(f);
   return(rv);
}

void TempCycle::call4Valve(bool f)
{
   time_t timestamp;
   time(&timestamp);
   json11::Json js = Json::object {
      { "message_type", "furnace" }, 
      { "device", config->value("mqtt_ident").toString() }, 
      { "mode", "valve"}, 
      { "cycle", f ? "On" : "Off"}, 
      { "timestamp",  (int)timestamp}
   };
   mqtt_bus->sendData("thermostat", js.dump().c_str());

   if ( ! config->value("rmt_relays").toBool())
   {
      if ( f ) 
         Valve->off();
      else
         Valve->on();
   }
}

void TempCycle::call4Fan(bool f)
{
   if ( ! config->value("rmt_relays").toBool())
   {
      if ( f ) 
         Fan->off();
      else
         Fan->on();
   }
}

bool TempCycle::currentTemp(float &temp, int &timestamp)
{
   SensorData sd = SensorData::getSensor(m_sensor);
   if (sd.isValid())
   {
      temp        = sd.temperature();
      timestamp   = sd.timestamp();
   }
   return(sd.isValid());
}

TempCycle temp_cycle;

