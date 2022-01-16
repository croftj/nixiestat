# include "WProgram.h"
# include "thermostat.h"
# include "clock.h"
# include "datetime.h"
# include "esquare.h"
# include "IOPort.h"

extern uint8_t dbg;

extern TempSensors sensors;

static OutputPort HeatRelay;
static OutputPort CoolRelay;
static OutputPort FanRelay;
static OutputPort ValveRelay;

void Thermostat::initialize(TempSensors *sensors)
{
   m_sensors   = sensors;
   m_lastCall  = false;
   m_override  = false;

   HeatRelay   .SetPort(IOPort::PORT_B, _BV(0)); 
   CoolRelay   .SetPort(IOPort::PORT_B, _BV(1)); 
   FanRelay    .SetPort(IOPort::PORT_B, _BV(2)); 
   ValveRelay  .SetPort(IOPort::PORT_B, _BV(3)); 
   HeatRelay   .off();
   CoolRelay   .off();
   FanRelay    .off();
   ValveRelay  .off();
   m_settings = eeprom.settings();
}

float Thermostat::getCurrentTempSetting()
{
   uint8_t  *temp;
   float    target;
   float*   tempSettings;
   int16_t* timeSettings;

//   Serial  << __PRETTY_FUNCTION__ << " Current Mode: " << ((m_settings->currentMode == Settings::Heat) ? "Heat" : "Cool") << endl;
   /***********************************************/
   /*   If  we  are  in  vacation mode, just get  */
   /*   it's setting and go for it.               */
   /***********************************************/
   if ( m_settings->vacationMode )
   {
      target = m_settings->vacationTemp;
   }

   /***********************************************/
   /*  Othewise, get the correct set of times and */
   /*  temps  based of whether we  are heating or */
   /*  cooling.                                   */
   /***********************************************/
   else 
   {
      if ( m_settings->currentMode == Settings::Heat ) 
      {
         timeSettings = m_settings->heatTimes;
         tempSettings = m_settings->heatTemps;
      }
      else
      {
         timeSettings = m_settings->coolTimes;
         tempSettings = m_settings->coolTemps;
      }

      /***********************************************/
      /*   Set  the  temperature to the one that is  */
      /*   set as the default.                       */
      /***********************************************/
      target = tempSettings[0];

      /***********************************************/
      /*   Fetch  the  current  setting,  if any we  */
      /*   should be using                           */
      /***********************************************/
      int8_t setting = CalculateCurrentSetting(timeSettings);
//      Serial  << __PRETTY_FUNCTION__ << " setting: " << (int)setting << endl;
         
      /***********************************************/
      /*   If  we found one (if there were no temps  */
      /*   set,  we  won't  have  anything)  us it,  */
      /*   otherwise,  go  with the default time we  */
      /*   set above.                                */
      /***********************************************/
      if ( setting > 0 ) 
      {
         m_target = tempSettings[setting];
         if ( ! m_override || m_overrideIndex != setting ) 
         {
//            Serial  << __PRETTY_FUNCTION__ << " setting return temp to temp[" << (int)setting << "]: " << (int)tempSettings[setting] << endl;
            target = tempSettings[setting];
            m_override = false;
         }
      }
//      Serial  << __PRETTY_FUNCTION__ << " returning temp: " << (float)target << endl;
   }
   return(target);
}

void Thermostat::SetOverride(float temp)
{
   Serial  << __PRETTY_FUNCTION__ << " Current Mode: " << ((m_settings->currentMode == Settings::Heat) ? "Heat" : "Cool") << endl;
   float*  tempSettings;
   int16_t* timeSettings;

   if ( m_settings->currentMode == Settings::Heat ) 
   {
      timeSettings = m_settings->heatTimes;
      tempSettings = m_settings->heatTemps;
   }
   else
   {
      timeSettings = m_settings->coolTimes;
      tempSettings = m_settings->coolTemps;
   }
   m_overrideIndex = CalculateCurrentSetting(timeSettings);
   tempSettings[0] = temp;
   m_overrideTemp = temp;
   m_override = true;
}

int8_t Thermostat::CalculateCurrentSetting(int16_t* timeSettings)
{
   /***********************************************/
   /*   Start  by  getting the time setting that  */
   /*   is  closest  to the current time without  */
   /*   being earlier than it.                    */
   /***********************************************/
   int16_t currTime = currentDate.getMinutesOfDay();
   int16_t bestTime = 2000;
   int8_t setting = -1;   // Our return value.

   for (int8_t x = 1; x < Settings::NUM_SETTINGS; x++) 
   {
      if ( timeSettings[x] >= 0 ) 
      {
//         Serial  << __PRETTY_FUNCTION__ << " CurrTime: " << currTime << ", bestTime: " << bestTime << ", timeSettings[" << (int)x << "]: " << timeSettings[x] << endl;
         if ( currTime - timeSettings[x] >= 0 &&
               currTime - timeSettings[x] < bestTime  ) 
         {
            setting = x;
            bestTime = currTime - timeSettings[x];
//            Serial  << __PRETTY_FUNCTION__ << " Best Time- setting: " << (int)setting << ", time: " << bestTime << endl;
         }
      }
   }

//   Serial << __PRETTY_FUNCTION__ << "Entering phase2- setting: " << (int)setting << endl;

   /***********************************************/
   /*   If  we  didn't  get  a  time in the step  */
   /*   above,  assume  that the current time is  */
   /*   past  all of the time settings and fetch  */
   /*   the setting with the latest time.         */
   /***********************************************/
   if ( setting < 0 ) 
   {
      bestTime = 0;
      for (int8_t x = 1; x < Settings::NUM_SETTINGS; x++) 
      {
//         Serial  << __PRETTY_FUNCTION__ << " CurrTime: " << currTime << ", bestTime: " << bestTime << ", timeSettings[" << (int)x << "]: " << timeSettings[x] << endl;
         if ( timeSettings[x] > bestTime ) 
         {
            bestTime = timeSettings[x];
            setting = x;
//            Serial  << __PRETTY_FUNCTION__ << " Best setting: " << (int)x << ", time: " << bestTime << endl;
         }
      }
   }
   return(setting);
}

bool Thermostat::ReachedOverride(float cur_temp)
{
   bool rv = false;

   if (dbg > 1) Serial  << __PRETTY_FUNCTION__ << " m_override =  " << m_override << endl;
   if ( m_override == true ) 
   {
      if (dbg) Serial  << __PRETTY_FUNCTION__ << "m_target = " << m_target << endl;
      if (dbg) Serial  << __PRETTY_FUNCTION__ << "m_overrideTemp = " << m_overrideTemp << endl;
      if (dbg) Serial  << __PRETTY_FUNCTION__ << "cur_temp = " << cur_temp << endl;
      if ( (m_target > m_overrideTemp && 
            cur_temp <= m_overrideTemp) ||
           (m_target <= m_overrideTemp &&
            cur_temp >= m_overrideTemp) )
      {
         rv = true;
      }
   }
   if (dbg > 1) Serial  << __PRETTY_FUNCTION__ << "rv = " << rv << endl;
   return(rv);
}

Thermostat thermostat;

