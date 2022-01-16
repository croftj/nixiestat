/*
  DateTime.h - Arduino library for date and time functions
  Copyright (c) Michael Margolis.  All right reserved.


  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/

#ifndef DATETIME_H
#define DATETIME_H

#include <inttypes.h>
#include "clock.h"

typedef uint8_t boolean;

/*==============================================================================*/
/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define DAYS_PER_WEEK (7L)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52L)
#define SECS_YR_2000  (946681200UL)
 
/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define dayOfWeek(_time_)  (( _time_ / SECS_PER_DAY + 4)  % DAYS_PER_WEEK) // 0 = Sunday
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  // this is number of days since Jan 1 1970
#define elapsedSecsToday(_time_)  (_time_ % SECS_PER_DAY)   // the number of seconds since last midnight 
#define previousMidnight(_time_) (( _time_ / SECS_PER_DAY) * SECS_PER_DAY)  // time at the start of the given day
#define nextMidnight(_time_) ( previousMidnight(_time_)  + SECS_PER_DAY ) // time at the end of the given day 
#define elapsedSecsThisWeek(_time_)  (elapsedSecsToday(_time_) +  (dayOfWeek(_time_) * SECS_PER_DAY) )   

// todo add date math macros
/*============================================================================*/

typedef enum {
     dtSunday, dtMonday, dtTuesday, dtWednesday, dtThursday, dtFriday, dtSaturday
} dtDays_t;

typedef enum {dtStatusNotSet, dtStatusSet, dtStatusSync
}  dtStatus_t ;

class DateTime : public Clock
{
public:
   DateTime();

   // return the current time as seconds since Jan1 1970
   time_t now()
   {
      return(seconds);
   }; 

   // returns false if not set, else refreshes the Date and Time properties and returns true 
   boolean available();  

   // functions to convert to and from time components (hrs, secs, days, years etc) to time_t  
   virtual void localTime(time_t *timep,uint8_t *psec,uint8_t *pmin,uint8_t *phour,uint8_t *pday,uint8_t *pwday,uint8_t *pmonth,uint8_t *pyear); // extracts time components from time_t
   uint8_t *localTime();
   virtual time_t makeTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint16_t year ); // returns time_t from components
   static char *formatDate(time_t time);
   static char *formatDateTime(time_t time);
   static char *formatTime(time_t time);

private:
   dtStatus_t status;
   uint8_t Hour;
   uint8_t Minute;
   uint8_t Second;
   uint8_t Day;
   uint8_t Dow; // Sunday is day 0 
   uint8_t Month; // Jan is month 0
   uint8_t Year;  // the Year minus 1900 
};

extern DateTime currentDate;  // make an instance for the user


#endif /* DateTime_h */

