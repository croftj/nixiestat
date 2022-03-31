#ifndef TEMPCYCLE_H
#define TEMPCYCLE_H

#include "esp_system.h"
#include "variant.h"
#include <string>

#include "CycleData.h"

class TempCycle 
{
public:
   TempCycle() : m_hyst(.5) { };
   void initialize();
   bool update(CycleData *cyc_data, bool force_on);

   void setCycleTime(uint16_t minutes);
   bool call4Cool(bool f, CycleData *cyc_data = NULL);
   bool call4Heat(bool f, CycleData *cyc_data = NULL);
   void call4Fan(bool f);
   void call4Valve(bool f);

   bool currentTemp(float &temp, int &timestamp);

   void setSensor(const std::string &sensor)
   {
      m_sensor = sensor;
   }
   
   std::string getSensor()
   {
      return(m_sensor);
   }

   uint8_t getMode()
   {
      return(m_mode);
   }

   void setMode(uint8_t mode)
   {
      m_mode = mode;
   }
   
   void setAbsMinimum(float temp)
   {
      m_absMinimumTemp = temp;   
   }

   void setAbsMaximum(float temp)
   {
      m_absMaximumTemp = temp;   
   }

   void setMaxCycleTime(uint16_t minutes)
   {
      m_maxCycleTime = minutes * 60;
   }

   void setTemp(float temp)
   {
      m_temp = temp;
   }

   void setHysterisis(float hyst)
   {
      m_hyst = hyst;
   }

   float getHysterisis()
   {
      return(m_hyst);
   }

   float getTemp()
   {
      return(m_temp);
   }

   bool getPrevCall()
   {
      return(m_lastCall);
   }

   time_t getCurrentCycleTime()
   {
//      return(currentDate.getInterval(Clock::CYCLE_TIME_INTERVAL));
      return(0);
   }

   uint16_t getMinimumCycleTime()
   {
      uint16_t rv = m_minCycleTime;
      if ( m_prevCall ) 
      {
         rv = rv / 2;
      }
      return(rv);
   }

protected:
private:
   char              m_fmtBuf[10];
   bool              m_prevCall;
   bool              m_alarm;
   uint8_t           m_mode;
   std::string       m_sensor;
   float             m_temp;
   float             m_hyst;
   float             m_absMinimumTemp;
   float             m_absMaximumTemp;
   uint16_t          m_minCycleTime;
   uint16_t          m_maxCycleTime;
   bool              m_initialCall;
   bool              m_lastCall;
   time_t            m_lastCallTime;
   bool              m_minCycleTimeReached;
};

extern TempCycle temp_cycle;

#endif
