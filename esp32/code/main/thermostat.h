#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <string>

#include "SensorData.h"
#include "variant.h"

class Thermostat 
{
public:
   typedef enum
   {
      UNSET = -1,
      NORMAL = 0,
      ALARM1,
      ALARM2,
      ALARM3
   } RunState;

   Thermostat() {};
   void initialize();
   bool update();

   SensorData selectSensor(std::string sensors, time_t now);
   void SetOverride(float temp);

   void CancelOverride();
   bool ReachedOverride(float cur_temp);
   static void *exec(void*);

protected:
private:
   void  run();
   bool              m_override;
   float             m_overrideTemp;
   uint8_t           m_overrideIndex;  // Which of the temps were being overridden
   float             m_target;
   int8_t            m_temp;
};

extern Thermostat thermostat;

#endif
