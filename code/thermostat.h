#ifndef THERMOSTAT_H
#define THERMOSTAT_H

# include "esquare.h"
# include "TempSensors.h"

class Thermostat 
{
public:
   Thermostat() {};
   void initialize(TempSensors *sensors);
   bool update();

   void initialize();

   int8_t CalculateCurrentSetting(int16_t* timeSettings);
   void SetOverride(float temp);
   float getCurrentTempSetting();

   void CancelOverride()
   {
      m_override = false;
   }

   bool ReachedOverride(float cur_temp);
protected:
private:
   ESquareSpace_t*   m_settings;
   TempSensors*      m_sensors;
   float             m_temps[NUM_SENSORS];
   bool              m_lastCall;
   bool              m_override;
   float             m_overrideTemp;
   uint8_t           m_overrideIndex;  // Which of the temps were being overridden
   float             m_target;
   int8_t            m_temp;
};

extern Thermostat thermostat;

#endif
