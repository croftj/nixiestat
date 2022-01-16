#define  DISPLAY_OFF

# include <stdint.h>

#ifndef DISPLAY_OFF
# include "display.h"
#endif
# include "datetime.h"
# include "WProgram.h"
# include "ui.h"

extern uint32_t systemSeconds;

static uint16_t array[7];
static uint8_t  zcross = 0;

SIGNAL(INT0_vect)
{
   zcross++;
   if ( zcross == 120 ) 
   {
      zcross = 0;
      currentDate.increment();
   }
}

Clock::Clock()
{
   /***********************************************/
   /*   Enable INT0 (the line freq in) interrupt  */
   /*   to fire on both edges.                    */
   /***********************************************/
   EICRA &= ~(_BV(ISC01));
   EICRA |= _BV(ISC00);
   EIMSK |= _BV(INT0);

   initialized = false;
   updateNeeded = false;
}

void Clock::readClock(uint16_t *dt)
{
   uint8_t sec, min, hour, day, dow, month;
   uint16_t year;

#ifndef DISPLAY_OFF
   uint8_t enabled = display.disable();
#endif
//   ui.enable(false);
   rtc.readTime(&sec, &min, &hour, &day, &dow, &month, &year);
//   ui.enable(true);
   dt[Second] = sec;
   dt[Minute] = min;
   dt[Hour] = hour;
   dt[Day] = day;
   dt[DayOfWeek] = dow;
   dt[Month] = month;
   dt[Year] = year;
#ifndef DISPLAY_OFF
   if ( enabled ) 
      display.touch();
#endif
}

time_t Clock::readClock()
{
   uint8_t sec, min, hour, day, dow, month;
   uint16_t year;

#ifndef DISPLAY_OFF
   uint8_t enabled = display.disable();
#endif
//   ui.enable(false);
   rtc.readTime(&sec, &min, &hour, &day, &dow, &month, &year);
//   ui.enable(true);
#ifndef DISPLAY_OFF
   if ( enabled ) 
      display.touch();
#endif
//   Serial << "Clock::readClock(): sec: " << (uint16_t)sec << endl; 
//   Serial << "Clock::readClock(): min: " << (uint16_t)min << endl; 
//   Serial << "Clock::readClock(): hour: " << (uint16_t)hour << endl; 
//   Serial << "Clock::readClock(): day: " << (uint16_t)day << endl; 
//   Serial << "Clock::readClock(): month: " << (uint16_t)month << endl; 
//   Serial << "Clock::readClock(): year: " << (uint16_t)year << endl; 
//   Serial << "Clock::readClock(): dow: " << (uint16_t)dow << endl; 
   month--;
   time_t time = makeTime(sec, min, hour, day, month, year);
   return(time);
}

void Clock::writeClock(uint16_t *dt)
{

   uint16_t month = dt[Month] - 1;
   time_t time = makeTime(dt[Second], dt[Minute], dt[Hour], dt[Day], month, dt[Year]);
//   Serial << "Clock::writeClock(): sec: " << (uint16_t)dt[Second] << endl; 
//   Serial << "Clock::writeClock(): min: " << (uint16_t)dt[Minute] << endl; 
//   Serial << "Clock::writeClock(): hour: " << (uint16_t)dt[Hour] << endl; 
//   Serial << "Clock::writeClock(): day: " << (uint16_t)dt[Day] << endl; 
//   Serial << "Clock::writeClock(): month: " << (uint16_t)month << endl; 
//   Serial << "Clock::writeClock(): year: " << (uint16_t)dt[Year] << endl; 
//   Serial << "Clock::writeClock(): time: " << (uint16_t)time << endl; 
   writeClock(time);
}

void Clock::writeClock(time_t time)
{
   uint8_t sec, min, hour, day, dow, month, year;
   uint16_t iyear;

//   Serial << __PRETTY_FUNCTION__ << "seconds = " << seconds;

   localTime(&time, &sec, &min, &hour, &day, &dow, &month, &year);
   month++;
   iyear = year + 1900;
#ifndef DISPLAY_OFF
   uint8_t enabled = display.disable();
#endif
//   ui.enable(false);
   rtc.setTime(sec, min, hour, day, dow, month, iyear);
//   ui.enable(false);
#ifndef DISPLAY_OFF
   if ( enabled ) 
      display.touch();
#endif
}

time_t Clock::getTime()
{
   if ( ! initialized ) 
      init();
//   Serial << __PRETTY_FUNCTION__ << "seconds = " << seconds;
   return(seconds);
}

void Clock::getTime(uint16_t *dt)
{
   uint8_t sec, min, hour, day, dow, month, year;
   uint16_t iyear;

   if ( ! initialized ) 
      init();

   localTime(&seconds, &sec, 
                       &min, 
                       &hour, 
                       &day, 
                       &dow, 
                       &month, 
                       &year);

   iyear = year + 1900;

   dt[Second]     = sec;
   dt[Minute]     = min;
   dt[Hour]       = hour;
   dt[Day]        = day;
   dt[DayOfWeek]  = dow;
   dt[Month]      = month;
   dt[Year]       = iyear;
}

void Clock::setTime(uint16_t *dt)
{
   zcross = 0;
   seconds = makeTime(dt[Second], dt[Minute], dt[Hour], dt[Day], dt[Month], dt[Year]);
}

void Clock::setTime(time_t time)
{
   zcross = 0;
   seconds = time;
}

bool Clock::updateClock()
{
   bool rv = updateNeeded;
   if ( updateNeeded ) 
   {
      updateNeeded = false;
      writeClock(seconds);
   }
   return(rv);
}

void Clock::increment()
{
   seconds++;
   for (int x = 0; x < INTERVAL_COUNTERS; x++) 
   {
      intervalCounter[x]++;
   }
   if ( (seconds % 600) == 0 ) 
   {
      updateNeeded = true;
   }
}


