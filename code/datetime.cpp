/*
  DateTime.cpp - Arduino Date and Time library
  Copyright (c) Michael Margolis.  All right reserved.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/
# include <avr/pgmspace.h>
# include <stdio.h>

# include "WProgram.h"
#include "datetime.h"

extern char p_buf[128];
extern char ibuf[32];

// make one instance for DateTime class the user 
DateTime currentDate;

#define LEAP_YEAR(_year) ((_year%4)==0)
static  uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

//******************************************************************************
//* DateTime Public Methods
//******************************************************************************

DateTime::DateTime() : Clock()
{
   this->status = dtStatusNotSet;
}

boolean DateTime::available()
{  
// refresh time components if clock is set (even if not synced), just return false if not set
   if(status != dtStatusNotSet) 
   { 
      localTime(&seconds,&Second,&Minute,&Hour,&Day,&Dow,&Month,&Year)  ;     
      return true;
   }
   else
      return false;
}

/**********************************************************************/
/*                                                                    */
/*   This  function returns an array of bytes representing the time.  */
/*   Except for the position 0 (tens of years), each byte has a BCD   */
/*   digit  0-9  of  the  value. The tens of years is represented in  */
/*   binary  and  can  rage  from 0 for 1900 to 255 representing the  */
/*   year 2740.                                                       */
/*                                                                    */
/*   It  is the responsiblity of the caller to free/delete the array  */
/*   when  they  are  finished with it! Not deleting it will cause a  */
/*   memory leak!                                                     */
/*                                                                    */
/*   The following defines the array:                                 */
/*                                                                    */
/*   Pos: 0  => year tens (binary)                                    */
/*   Pos: 1  => year units                                            */
/*   Pos: 2  => month tens                                            */
/*   Pos: 3  => month units                                           */
/*   Pos: 4  => dow units (0 = Sunday, 6 = Saturday)                  */
/*   Pos: 5  => day tens                                              */
/*   Pos: 6  => day units                                             */
/*   Pos: 7  => hour tens                                             */
/*   Pos: 8  => hour units                                            */
/*   Pos: 9  => minute tens                                           */
/*   Pos: 10 => minute units                                          */
/*   Pos: 11 => second tens                                           */
/*   Pos: 12 => second units                                          */
/*                                                                    */
/**********************************************************************/
/*
uint8_t *DateTime::localTime()
{
   localTime(&seconds, &Second, &Minute, &Hour, &Day, &Dow, &Month, &Year);
//   uint8_t *time = new uint8_t[15];

   time[0]  = Second / 10;
   time[1]  = Second % 10;

   time[2]  = Minute / 10;
   time[3]  = Minute % 10;

   time[4]  = Hour / 10;
   time[5]  = Hour % 10;

   time[6]  = Day / 10;
   time[7]  = Day % 10;

   time[8]  = Dow % 7;

   time[9]  = Month / 10;
   time[10] = Month % 10;

   time[11] = Year / 10;
   time[12] = Year % 10;

   return(time);
}
*/

void DateTime::localTime(time_t *timep,
                         uint8_t *psec,
                         uint8_t *pmin,
                         uint8_t *phour,
                         uint8_t *pday,
                         uint8_t *pwday,
                         uint8_t *pmonth,
                         uint8_t *pyear) 
{
// convert the given time_t to time components
// this is a more compact version of the C library localtime function

  time_t long epoch=*timep;
  uint8_t year;
  uint8_t month, monthLength;
  unsigned long days;
  
  *psec=epoch%60;
  epoch/=60; // now it is minutes
  *pmin=epoch%60;
  epoch/=60; // now it is hours
  *phour=epoch%24;
  epoch/=24; // now it is days
  *pwday=(epoch+4)%7;
  
  year=70;  
  days=0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= epoch) {
    year++;
  }
//  year += 1900;
  *pyear=year; // *pyear is returned as years from 1900
  
  days -= LEAP_YEAR(year) ? 366 : 365;
  epoch -= days; // now it is days in this year, starting at 0
  //*pdayofyear=epoch;  // days since jan 1 this year
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }
    
    if (epoch>=monthLength) {
      epoch-=monthLength;
    } else {
        break;
    }
  }
  *pmonth=month;  // jan is month 0
  *pday=epoch+1;  // day of month
}

time_t DateTime::makeTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint16_t year ){
// converts time components to time_t 
// note year argument is full four digit year (or digits since 2000), i.e.1975, (year 8 is 2008)
  
   int i;
   time_t seconds;

   if (year < 69) 
      year += 2000;
   
//   Serial << "DateTime::makeTime(): year: " << year <<endl;
   // seconds from 1970 till 1 jan 00:00:00 this year
   seconds= (year-1970)*(60*60*24L*365);
//   Serial << "DateTime::makeTime(): seconds: " << (time_t) seconds << endl;

   // add extra days for leap years
   for (i=1970; i<year; i++) {
      if (LEAP_YEAR(i)) {
         seconds+= 60*60*24L;
//         Serial << "DateTime::makeTime(): leep year " << i << " seconds: " << (time_t) seconds << endl;
      }
   }
   // add days for this year
   for (i=0; i<month; i++) {
      if (i==1 && LEAP_YEAR(year)) { 
         seconds+= 60*60*24L*29;
      } else {
         seconds+= 60*60*24L*monthDays[i];
      }
//      Serial << "DateTime::makeTime(): month " << i << " seconds: " << (time_t) seconds << endl;
   }

   seconds+= (day-1)*3600*24L;
//   Serial << "DateTime::makeTime(): day seconds: " << (time_t) seconds << endl;
   seconds+= hour*3600L;
//   Serial << "DateTime::makeTime(): hour seconds: " << (time_t) seconds << endl;
   seconds+= min*60L;
//   Serial << "DateTime::makeTime(): minute seconds: " << (time_t) seconds << endl;
   seconds+= sec;
//   Serial << "DateTime::makeTime(): sec seconds: " << (time_t) seconds << endl;
//   Serial << "DateTime::makeTime(): seconds: " << (time_t) seconds << endl;
   return seconds; 
}

char *DateTime::formatDate(time_t time)
{
   static char rv[14];
   uint8_t sec, min, hour, day, dow, month, yr;
   currentDate.localTime(&time, &sec, &min, &hour, &day, &dow, &month, &yr);
   uint16_t iyear = yr + 1900;
   month++;
//   Serial << strcpy_P(p_buf, PSTR("Current Time: ")) << (int)hour << strcpy_P(p_buf, PSTR(":")) << (int)min << strcpy_P(p_buf, PSTR(":")) << (int)sec  << strcpy_P(p_buf, PSTR(" (")) << clock.secs << strcpy_P(p_buf, PSTR(")")) << endl;
   sprintf(rv, strcpy_P(p_buf, PSTR("%02d/%02d/%4d")), month, day, iyear);
   return(rv);
}

char *DateTime::formatDateTime(time_t time)
{
   static char rv[32];
   uint8_t sec, min, hour, day, dow, month, yr;
   currentDate.localTime(&time, &sec, &min, &hour, &day, &dow, &month, &yr);
   uint16_t iyear = yr + 1900;
   month++;
//   Serial << strcpy_P(p_buf, PSTR("Current Time: ")) << (int)hour << strcpy_P(p_buf, PSTR(":")) << (int)min << strcpy_P(p_buf, PSTR(":")) << (int)sec  << strcpy_P(p_buf, PSTR(" (")) << clock.secs << strcpy_P(p_buf, PSTR(")")) << endl;
   sprintf(rv, strcpy_P(p_buf, PSTR("%02d/%02d/%4d-%02d:%02d:%02d")), month, day, iyear, hour, min, sec);
   return(rv);
}

char *DateTime::formatTime(time_t time)
{
   static char rv[12];
   uint8_t sec, min, hour, day, dow, month, yr;
   currentDate.localTime(&time, &sec, &min, &hour, &day, &dow, &month, &yr);
   uint16_t iyear = yr + 1900;
   month++;
//   Serial << strcpy_P(p_buf, PSTR("Current Time: ")) << (int)hour << strcpy_P(p_buf, PSTR(":")) << (int)min << strcpy_P(p_buf, PSTR(":")) << (int)sec  << strcpy_P(p_buf, PSTR(" (")) << clock.secs << strcpy_P(p_buf, PSTR(")")) << endl;
   sprintf(rv, strcpy_P(p_buf, PSTR("%02d:%02d:%02d")), hour, min, sec);
   return(rv);
}
