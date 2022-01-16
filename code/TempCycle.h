#ifndef TEMPCYCLE_H
#define TEMPCYCLE_H

# include "WProgram.h"
# include "datetime.h"
# include "esquare.h"
# include "TempSensors.h"

class TempCycle 
{
public:
   TempCycle() : m_hyst(.5) { };
   void initialize(TempSensors *sensors);
   bool update();

   bool call4Cool(bool f);
   bool call4Heat(bool f);
   void call4Fan(bool f);

   void setCycleTime(uint8_t minutes)
   {
      m_minCycleTime = minutes * 60;
   }

   void setMaxCycleTime(uint8_t minutes)
   {
      m_maxCycleTime = minutes * 60;
   }

   void setSensor(uint8_t sensor)
   {
      m_sensor = sensor;
   }
   
   bool getSensor()
   {
      return(m_sensor);
   }

   bool getMode()
   {
      return(m_mode);
   }

   void setMode(bool mode)
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

   void setTemp(float temp)
   {
      m_temp = temp;
   }

   void setHysterisis(float hyst)
   {
      m_hyst = hyst;
//      Serial << __PRETTY_FUNCTION__ << "m_hyst = " << m_hyst << endl;
   }

   float getHysterisis()
   {
      return(m_hyst);
   }

   float getTemp()
   {
      return(m_temp);
   }

   bool getLastCall()
   {
      return(m_lastCall);
   }

   time_t getCurrentCycleTime()
   {
      return(currentDate.getInterval(Clock::CYCLE_TIME_INTERVAL));
   }

   uint16_t getMinimumCycleTime()
   {
      uint16_t rv = m_minCycleTime;
      if ( m_lastCall ) 
      {
         rv = rv / 2;
      }
      return(rv);
   }

protected:
private:
   char              m_fmtBuf[10];
   TempSensors*      m_sensors;
   bool              m_lastCall;
   bool              m_mode;
   int8_t            m_sensor;
   float             m_temp;
   float             m_temps[NUM_SENSORS];
   float             m_hyst;
   float             m_absMinimumTemp;
   float             m_absMaximumTemp;
   uint16_t          m_minCycleTime;
   uint16_t          m_maxCycleTime;
   bool              m_initialCall;
   bool              m_minCycleTimeReached;
};

extern TempCycle temp_cycle;

#endif
