#ifndef CLOCK_H
#define CLOCK_H

#include "ds1307.h"
# include "HardwareSerial.h"

typedef unsigned long time_t;

class Clock 
{
public:
   enum
   {
      READ_TEMPERATURE_INTERVAL = 0,
      READ_SENSOR_INTERVAL,
      LOG_TEMPERATURE_INTERVAL,
      UI_SHORT_INTERVAL,
      UI_LONG_INTERVAL,
      DISP_TEMPERATURE_INTERVAL,
      CYCLE_TIME_INTERVAL,

      // Must stay last!!
      INTERVAL_COUNTERS
   };

   enum 
   {
      Second = 0, Minute, Hour, Day, DayOfWeek, Month, Year, NUM_ELEMENTS
   };

   Clock();

   void init()
   {
      setTime(readClock());
      initialized = true;
   };

   void readClock(uint16_t *dt);
   time_t readClock();

   void writeClock(uint16_t *dt);
   void writeClock(time_t time);

   void getTime(uint16_t *dt);
   time_t getTime();

   uint16_t getMinutesOfDay()
   {
      uint32_t day_minutes = (uint32_t)(24 * 60) * 60;
      return((seconds % (day_minutes)) / 60);
   };

   time_t getInterval(uint8_t counter)
   {
      counter %= INTERVAL_COUNTERS;
//      if ( counter == LOG_TEMPERATURE_INTERVAL ) 
//      {
//         Serial << __PRETTY_FUNCTION__ << "intervalCounter[" << (uint32_t)counter << "]: " << intervalCounter[counter] << endl;
//      }
      return(intervalCounter[counter]);
   };

   void resetInterval(uint8_t counter)
   {
      counter %= INTERVAL_COUNTERS;
//      if ( counter == LOG_TEMPERATURE_INTERVAL ) 
//      {
//         Serial << __PRETTY_FUNCTION__ << "- resetting counter: " << (uint32_t)counter << endl;
//      }
      intervalCounter[counter] = 0;
   };

   void setTime(uint16_t *dt);
   void setTime(time_t time);

   bool updateClock();
   void increment();

   virtual void localTime(time_t  *timep,
                          uint8_t *psec,
                          uint8_t *pmin,
                          uint8_t *phour,
                          uint8_t *pday,
                          uint8_t *pwday,
                          uint8_t *pmonth,
                          uint8_t *pyear)
   {
   };

   virtual time_t makeTime(uint8_t sec, 
                           uint8_t min, 
                           uint8_t hour, 
                           uint8_t day, 
                           uint8_t month, 
                           uint16_t year )
   {
      return(0);
   };

protected:
   time_t   seconds;
   time_t   intervalCounter[INTERVAL_COUNTERS];
   bool     initialized;
   bool     updateNeeded;

private:
   DS1307   rtc;
};

#endif
