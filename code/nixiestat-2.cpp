# include <avr/pgmspace.h>
# include <ctype.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>

# include "EEPROM.h"
# include "IOPort.h"

# include "datetime.h"
# include "display.h"
# include "thermostat.h"
# include "TempSensors.h"
# include "WProgram.h"
# include "Wire.h"
# include "ds1307.h"
# include "mpr121.h"
# include "switches.h"
//# include "ui.h"

# include "TemperatureLog.h"

//void printFSMSize();

// #define  DBG

#define  LOG_SIZE                (7 * 1024)
#define  READ_TEMPERATURE_TIME   20
#define  DISP_TEMPERATURE_TIME   60
#define  LOG_TEMPERATURE_TIME    280
//#define  LOG_TEMPERATURE_TIME    80

enum 
{
   TIME_LAPSE_MASK  = 0xf000,
   TIME_LAPSE_SHIFT = 12,
   CHANNEL_MASK     = 0x0c00,
   CHANNEL_SHIFT    = 10,
   TEMPERATURE_MASK = 0x03ff,
   TEMPERATURE_SIGN = 0x0200,
   LOG_LENGTH       = 12 * 1024,
   TEMP_OFFSET      = 40
};


struct eeprom 
{
   int   count;
   float minTemp;
   float maxTemp;
   int   lampOnTime;
   int   lampOffTime;
   int   compOnTime;
   int   compOffTime;
};

TemperatureLog tempLog;
TempSensors sensors;

MPR121 mpr121;

bool tempIsReady = 0;
uint8_t tempCounter = 12;

SIGNAL(TIMER1_OVF_vect)
//SIGNAL(TIMER1_COMPA_vect)
{
   uint8_t x = PINB;
   PORTB = x ^ 0x01;
   if ( ++tempCounter > 12 ) 
   {
      tempIsReady = 1;
      tempCounter = 0;
   }
}

char buf[128];
char ibuf[32];
bool needPrompt = 0;

const char invalidEntry_str[] PROGMEM = "\nInvalid Entry!";

/**********************************************************************/
/*   This  function  reads  a  floating point number from the serial  */
/*   port.  If  it's  a  valid  entry  valid  will  be  set to true,  */
/*   otherwise false.                                                 */
/*                                                                    */
/*   The function returns either the value entered or 0.0.            */
/**********************************************************************/
float getFloat(bool *valid)
{
   float rv = 0.0;
   char *cp = 0;

//   Serial << "getFloat(): enter" << endl;
   while (1) 
   {
      if ( Serial.available() )
      {
         int status = 0;
//         Serial << "here 1" << endl;
         cp = Serial.readLine(cp, &status);
         if ( status < 0 ) 
         {
            if ( valid != 0 ) 
            {
               *valid = false;
               break;
            }
         }
         else if ( status )
         {
            char *endp;
//            Serial << endl;
            if ( strlen(cp) > 0 ) 
            {
//               Serial << "have: |" << cp << "|" <<endl;
               rv = (float)strtod(cp, &endp);
//               Serial << "endp: " << (long)endp << ", cp: " << (long)cp << endl;
               if ( endp == cp ) 
                  *valid = false;
               else
                  *valid = true;
            }
            else
               *valid = false;
            break;
         }
      }
   }
   return(rv);
}

/**********************************************************************/
/*   This  function reads an integer number from the serial port. If  */
/*   it's a valid entry valid will be set to true, otherwise false.   */
/*                                                                    */
/*   The function returns either the value entered or 0.              */
/**********************************************************************/
int getInt(bool *valid)
{
   int rv = 0;
   char *cp = 0;

   while (1) 
   {
      if ( Serial.available() )
      {
         int status = 0;
         cp = Serial.readLine(cp, &status);
         if ( status < 0 ) 
         {
            if ( valid != 0 ) 
            {
               *valid = false;
               break;
            }
         }
         else if ( status )
         {
            char *endp;
//            Serial << endl;
            if ( strlen(cp) > 0 ) 
            {
//               Serial << "have: |" << cp << "|" <<endl;
               rv = (int)strtol(cp, &endp, 10);
//               Serial << "rv: << " << rv << ", endp: " << (long)endp << ", cp: " << (long)cp << endl;
               if ( endp == cp ) 
                  *valid = false;
               else
                  *valid = true;
            }
            else
               *valid = false;
            break;
         }
      }
   }
   return(rv);
}

void writeEEProm()
{
   struct eeprom ee;
   
   byte *cp = (byte*)&ee;
   for (int x = 0; x < sizeof(ee); x++)
      *(cp++) = EEPROM.read(x);
   if ( ee.count >= 0  ) 
      ee.count++;
   else
      ee.count = 1;

/*
   ee.minTemp     = minTemp;
   ee.maxTemp     = maxTemp;
   ee.lampOnTime  = lampOnTime;
   ee.lampOffTime = lampOffTime;
   ee.compOnTime  = compOnTime;
   ee.compOffTime = compOffTime;
*/
   cp = (byte*)&ee;
   for (int x = 0; x < sizeof(ee); x++)
      EEPROM.write(x, *(cp++));
}

void readEEProm()
{
   struct eeprom ee;
   byte *cp = (byte*)&ee;
   for (int x = 0; x < sizeof(ee); x++)
      *(cp++) = EEPROM.read(x);
/*
   if ( ee.count == -1 )
   {
      minTemp     = 33.0;
      maxTemp     = 34.0;
      lampOnTime  = 1 * 60;
      lampOffTime = 3 * 60;
      compOnTime  = 15;
      compOffTime = 15;
   }
   else 
   {
      minTemp     = ee.minTemp;
      maxTemp     = ee.maxTemp;
      lampOnTime  = ee.lampOnTime;
      lampOffTime = ee.lampOffTime;
      compOnTime  = ee.compOnTime;
      compOffTime = ee.compOffTime;
   }
*/
}

char *formatTime(time_t time)
{
   static char rv[32];
   uint8_t sec, min, hour, day, dow, month, yr;
   currentDate.localTime(&time, &sec, &min, &hour, &day, &dow, &month, &yr);
   uint16_t iyear = yr + 1900;
   month++;
//   Serial << "Current Time: " << (int)hour << ":" << (int)min << ":" << (int)sec  << " (" << clock.secs << ")" << endl;
   sprintf(rv, "%02d/%02d/%4d,%02d:%02d:%02d", month, day, iyear, hour, min, sec);
   return(rv);
}
/**********************************************************************/
/*   This function displays the time in an almost pleasant way.       */
/**********************************************************************/
void showTime()
{
   byte sec, min, hour, duh;
   time_t now = currentDate.now();
   Serial << strcpy_P(buf, PSTR("\t  Current Time: ")) << formatTime(now)  << endl;
}

char *formatLogEntry(char *buf, time_t thisTime, uint8_t channel, float temp)
{
   sprintf(buf,"%ld,%s,%d,%3.2f", thisTime, formatTime(thisTime), channel, (double)temp);
   return(buf);
}

int main()
{
   uint8_t  cbuf_count = 1;
   time_t   lastStartTime = 0;

   bool  compOn;
   bool  lampOn;
   float temp = 32.0;
   int x = 0;
   uint16_t lcnt = 0;

   /***********************************************/
   /*   Initialize  the  serial port and show we  */
   /*   had a reset!                              */
   /***********************************************/
   init();

   Serial.begin(115200); //hello
   Serial << strcpy_P(buf, PSTR("/************           R  E  S  E  T            ************/")) << endl;
//   printFSMSize();
//   ui.init();
   Serial << strcpy_P(buf, PSTR("UI FSM Initialized")) << endl;

   IOPort::confPortDDR();

   /*********************************************************/
   /*********************************************************/
   /*********************************************************/
   /*********************************************************/
# if 0
   float test_temp;

   for (test_temp = -100.0; test_temp < 165.0; test_temp += .1) 
   {
      Serial << "test_temp = " << test_temp;
      float temp_calc = test_temp - TEMP_OFFSET;
//      temp_calc -= TEMP_OFFSET;
      Serial << ", temp_calc = " << temp_calc;
      temp_calc = fabs(temp_calc);
      Serial << ", adj temp_calc = " << temp_calc;

      /***********************************************/
      /*   Next  get  a  reasonable  integer of the  */
      /*   temp and 2 decimal places                 */
      /***********************************************/
      uint16_t x1 = temp_calc * 100;
      Serial << ", x1 = " << x1;

      /***********************************************/
      /*   Get  the integer value of the real whole  */
      /*   number of the temp                        */
      /***********************************************/
      float t1 = (float)x1 / 100;
      Serial << ", t1 = " << t1;
      int16_t t_whole = t1;
      Serial << ", t_whole = " << t_whole;
//      t_whole *= 4;
//      Serial << ", adj t_whole = 0x" << itoa(t_whole, ibuf, 16);

      /***********************************************/
      /*   Now  get the fractional portion as an in  */
      /*   and  keep it to only 2 bits of precision  */
      /*   (1/4 degree).                             */
      /***********************************************/
      float t2 = fabs(t1 - t_whole);
      Serial << ", t2 = " << t2;
      t2 *= 4;
      Serial << ", adj t2 = " << t2;
      uint16_t t_frac = (uint16_t)t2 & 0x03;
      Serial << ", t_frac = " << t_frac;
//      if ( t_frac != 0 ) 
//      {
//         t_frac = (~t_frac) & 0x03;
//      }
      Serial << ", adj t_frac = " << t_frac;

      /***********************************************/
      /*   Now    construct    the    fixed   point  */
      /*   representation of the temperature         */
      /***********************************************/
      int16_t t_fixedp = (t_whole * 4) + t_frac;
      if ( (test_temp - TEMP_OFFSET) < 0 ) 
      {
         t_fixedp *= -1;
      }

      t_fixedp &= TEMPERATURE_MASK;
      Serial << ", tfixed = 0x" << itoa(t_fixedp, ibuf, 16);

      int16_t t = t_fixedp & TEMPERATURE_MASK;
      Serial << ", t: 0x" << itoa(t, ibuf, 16);
      if ( t_fixedp & TEMPERATURE_SIGN ) 
      {
         t |= ~TEMPERATURE_MASK;
      }
      float new_temp = ((float)t / 4);
      Serial << ", new_temp: " << new_temp;
      new_temp += TEMP_OFFSET;
      Serial << ", adj new_temp: " << new_temp;
      Serial << endl;
      delay(50);
   }
   
# endif
   /*********************************************************/
   /*********************************************************/
   /*********************************************************/
   /*********************************************************/

   /***********************************************/
   /*   Set th Log buffer size. The idea here is  */
   /*   to  make  it  as  larg  as possible, but  */
   /*   still  leave  enough  room for the stack  */
   /*   and  it's  autovariables. There will not  */
   /*   be much room left, but who needs memory?  */
   /***********************************************/
   if ( true ) 
   {
      char xxx_buf[512]; // filler just to ensure space for other things later
      uint16_t size = LOG_SIZE;
      while ( ! tempLog.setLogSize(size) ) 
         size -= 512;
      Serial << strcpy_P(buf, PSTR("Log size is ")) << size << strcpy_P(buf, PSTR(" entries")) << endl;
   }

   CircularBuffer<uint8_t, uint8_t> cbuf;
   cbuf.SetBufferSize(10);

   needPrompt = 1;   // We need a prompt
   display.init();
   display.disable();

   /***********************************************/
   /*   Set up the display timer:                 */
   /*                                             */
   /*   WGM Mode:   0                             */
   /*                                             */
   /*   OC2A/B:     Disconnected                  */
   /***********************************************/

   TCCR2A = 0x00;
//   TCCR2B = _BV(CS22); // | _BV(CS21) | _BV(CS20);
   TCCR2B = _BV(CS21) | _BV(CS20);
   TIMSK2 = _BV(TOIE2);

//   DDRB = 0xff;
//   PORTB = 0x00;
   Serial << "Initializing touch sensors" << endl;
   mpr121.initialize();

   Serial << "Initializing temp sensors" << endl;
   sensors.initialize();

   delay(10);

   Serial << "Starting temp timer" << endl;

//   TCNT1 = 31250;
   TCNT1 = 15000;
   TCCR1A = 0x00;
   TCCR1B = _BV(CS12) | _BV(CS10);
   TIMSK1 = _BV(TOIE1);
//   TIMSK1 = _BV(OCIE1B);

   display.setEnabled(true);
   for (uint8_t x = 0; x < Clock::INTERVAL_COUNTERS; x++) 
   {
      currentDate.resetInterval(x);
   }

   display.flashDigits(_BV(4) | _BV(5), true);
   while (1) 
   {
      char *cp;
      uint16_t dt[7];

      /***********************************************/
      /*   Now  see  if we need a prompt displayed.  */
      /*   If  so,  reset the serial input timeout,  */
      /*   the   line   buffer   and  send  out  an  */
      /*   appropriate prompt.                       */
      /***********************************************/
      if ( needPrompt ) 
      {
         Serial.clearLineBuf();
         while (Serial.read() >= 0);
//         time_t time = currentDate.getTime();
         currentDate.getTime(dt);
//         Serial << endl;
         char buf[48];
         lcnt++;
         sprintf(buf, "%d %02d/%02d/%4d %02d:%02d:%02d", lcnt, 
                           dt[Clock::Month] + 1, 
                           dt[Clock::Day], 
                           dt[Clock::Year],
                           dt[Clock::Hour],
                           dt[Clock::Minute],
                           dt[Clock::Second]);
//         Serial << "[" << buf << "] " << endl;
         Serial << strcpy_P(buf, PSTR("$$")) << endl;
         delay(150);
         needPrompt = 0;
         cp = 0;
//         ui.procEvent(TC_WAKEN);
//         display.touch();
      }

      /***********************************************/
      /*   Otherwise,  see  if  there  is  any data  */
      /*   available from the rial port.             */
      /***********************************************/
      else if ( Serial.available() )
      {
//         display.touch();
         /***********************************************/
         /*   Try to read a full line                   */
         /***********************************************/
         int status = 0;
         cp = Serial.readLine(cp, &status);
//         Serial << "Have serial, status: " << (int)status<< endl;
         /***********************************************/
         /*   If any errors, say so!                    */
         /***********************************************/
         if ( status < 0 ) 
         {
            Serial.println(strcpy_P(buf, PSTR("\nInvalid entry!")));
            needPrompt = 1;
         }

         /***********************************************/
         /*   Otherwise,  if  we  have  a  full  line,  */
         /*   process it.                               */
         /***********************************************/
         else if ( status )
         {
            /***********************************************/
            /*   If  we  sre  not in a special mode, then  */
            /*   check what command they entered.          */
            /***********************************************/
            Serial << endl;
            needPrompt = 1;
            bool valid;
            if ( strlen(cp) > 0 )
            {
               char *cp1;
               if ( (cp1 = strcasestr(cp, "settime")) != 0 ) 
               {
                  currentDate.getTime(dt);
                  dt[Clock::Month]++;
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     while (*cp1 && isspace(*cp1)) 
                        cp1++;
                     if ( strlen(cp1) > 0 ) 
                     {
                        Serial << "hours: " << cp1;
                        dt[Clock::Hour] = atol(cp1);
                        while (*cp1 != '\0' and *cp1 != ':') 
                           cp1++;
                        if ( *(cp1++) != 0 && *cp1 != 0) 
                        {
                           Serial << ", minutes: " << cp1;
                           dt[Clock::Minute] = atol(cp1);
                           while (*cp1 != '\0' and *cp1 != ':') 
                              cp1++;
                           if ( *(cp1++) != 0 && *cp1 != 0) 
                           {
                              Serial << ", seconds: " << cp1 << endl;
                              dt[Clock::Second] = atol(cp1);
                              currentDate.setTime(dt);
                              currentDate.writeClock(dt);
                              currentDate.setTime(currentDate.readClock());
//                                    ports[1]->setBitMode(4, 1);
//                                    ports[1]->setBitMode(5, 1);
//                                 clock.secs = 0;
                           }
                        }
                     }
                  }
               }
               else if ( (cp1 = strcasestr(cp, "setdate")) != 0 ) 
               {
                  currentDate.readClock(dt);
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     while (*cp1 && isspace(*cp1)) 
                        cp1++;
                     if ( strlen(cp1) > 0 ) 
                     {
                        dt[Clock::Month] = atol(cp1);
                        while (*cp1 != '\0' and *cp1 != '/') 
                           cp1++;
                        if ( *(cp1++) != 0 && *cp1 != 0) 
                        {
                           dt[Clock::Day] = atol(cp1);
                           while (*cp1 != '\0' and *cp1 != '/') 
                              cp1++;
                           if ( *(cp1++) != 0 && *cp1 != 0) 
                           {
                              dt[Clock::Year] = atol(cp1);
                              if ( dt[Clock::Year] < 100 ) 
                                 dt[Clock::Year] += 2000;
                              currentDate.writeClock(dt);
                              currentDate.setTime(currentDate.readClock());
//                                    ports[1]->setBitMode(4, 1);
//                                    ports[1]->setBitMode(5, 1);
//                                 clock.secs = 0;
                           }
                        }
                     }
                  }
               }
               else if ( (cp1 = strcasestr(cp, "lamp")) != 0 ) 
               {
                  Serial << "lamp argument = |" << cp1 << "|" << endl;
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     while (*cp1 && isspace(*cp1)) 
                        cp1++;
                     if ( strlen(cp1) > 0 ) 
                     {
                        if ( (strcasestr(cp1, "off")) != 0 ) 
                        {
                           Serial << "Lamps OFF" << endl;
                           for (uint8_t x = 0; x < 10; x++) 
                           {
                              display.lamp(x, false);
                           }
                        }
                        else
                        {
                           uint8_t dig = atol(cp1);
                           Serial << "Turning on lamp " << (char)dig << endl;
                           display.lamp(dig, true);
                        }
                     }
                  }
               }
               else if ( (cp1 = strcasestr(cp, "lflash")) != 0 ) 
               {
                  Serial << "Flash argument = |" << cp1 << "|" << endl;
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     while (*cp1 && isspace(*cp1)) 
                        cp1++;
                     if ( strlen(cp1) > 0 ) 
                     {
                        if ( (strcasestr(cp1, "off")) != 0 ) 
                        {
                           Serial << "Flashing lamps OFF" << endl;
                           display.flashLamps(0x3ff, false);
                        }
                        else
                        {
                           uint8_t dig = atol(cp1);
                           Serial << "Flashing lamp " << (char)dig << endl;
                           display.flashLamps(_BV(dig), true);
                        }
                     }
                  }
                  uint8_t val = display.lampFlashMask;
                  Serial << "Flash Mask = %" << itoa(val, ibuf, 2) << endl;
                  Serial << "Flash Counter = " << display.flashCounter << endl;
               }
               else if ( (cp1 = strcasestr(cp, "flash")) != 0 ) 
               {
                  Serial << "Flash argument = |" << cp1 << "|" << endl;
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     while (*cp1 && isspace(*cp1)) 
                        cp1++;
                     if ( strlen(cp1) > 0 ) 
                     {
                        if ( (strcasestr(cp1, "off")) != 0 ) 
                        {
                           Serial << "Flashing digits OFF" << endl;
                           display.flashDigits(0xff, false);
                        }
                        else
                        {
                           uint8_t dig = atol(cp1);
                           Serial << "Flashing digit " << (char)dig << endl;
                           display.flashDigits(_BV(dig), true);
                        }
                     }
                  }
                  uint8_t val = display.digitFlashMask;
                  Serial << "Flash Mask = %" << itoa(val, ibuf, 2) << endl;
                  Serial << "Flash Counter = " << display.flashCounter << endl;
               }
               else if ( (cp1 = strcasestr(cp, "blank")) != 0 ) 
               {
                  Serial << "Blank argument = |" << cp1 << "|" << endl;
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     while (*cp1 && isspace(*cp1)) 
                        cp1++;
                     if ( strlen(cp1) > 0 ) 
                     {
                        if ( (strcasestr(cp1, "off")) != 0 ) 
                        {
                           Serial << "Blanking digits OFF" << endl;
                           display.blankDigits(0xff, false);
                        }
                        else
                        {
                           uint8_t dig = atol(cp1);
                           Serial << "Blanking digit " << (char)dig << endl;
                           display.blankDigits(_BV(dig), true);
                        }
                     }
                  }
                  uint8_t val = display.digitBlankMask;
                  Serial << "Blank Mask = %" << itoa(val, ibuf, 2) << endl;
               }
               else if ( (cp1 = strcasestr(cp, "digit")) != 0 ) 
               {
                  Serial << "Flash argument = |" << cp1 << "|" << endl;
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     while (*cp1 && isspace(*cp1)) 
                        cp1++;
                     if ( strlen(cp1) > 0 ) 
                     {
                        uint8_t dig = atol(cp1);

                        while (*cp1 && !isspace(*cp1)) 
                           cp1++;
                        if ( *cp1 != '\0' ) 
                        {
                           while (*cp1 && isspace(*cp1)) 
                           cp1++;
                           uint8_t val = atol(cp1);

                           Serial << "Set digit " << (int)dig << " to val: " << (int)val << endl;
                           display.setDigit(dig, val);
                        }
                     }
                  }
               }
               else if ( (cp1 = strcasestr(cp, "push")) != 0 ) 
               {
                  cbuf.Push(cbuf_count);
                  Serial << "Pushed = " << (int)cbuf_count << ", buf has " << (int)cbuf.Count() << " elements" << endl;
                  cbuf_count++;
               }
               else if ( (cp1 = strcasestr(cp, "next")) != 0 ) 
               {
                  if ( cbuf.HasNext() ) 
                  {
                     Serial << "next element = " << (int)cbuf.Next() << endl;
                  }
                  else 
                  {
                     Serial << "at the end of the list, element = " << (int)cbuf.Next() << endl;
                  }
               }
               else if ( (cp1 = strcasestr(cp, "prev")) != 0 ) 
               {
                  if ( cbuf.HasPrevious() ) 
                  {
                     Serial << "previous element = " << (int)cbuf.Previous() << endl;
                  }
                  else 
                  {
                     Serial << "at the start of the list, element = " << (int)cbuf.Next() << endl;
                  }
               }
               else if ( (cp1 = strcasestr(cp, "first")) != 0 ) 
               {
                  Serial << "reset next pointer" << endl;
                  cbuf.MoveToHead();
               }
               else if ( (cp1 = strcasestr(cp, "dump")) != 0 ) 
               {
                  uint16_t logCount = tempLog.Count();
                  time_t logTime = 0;

                  char tbuf[48];
                  while (*cp1 && !isspace(*cp1)) 
                     cp1++;
                  while (*cp1 && isspace(*cp1)) 
                     cp1++;
                  if ( *cp1 != '\0' ) 
                  {
                     uint8_t tcnt = sizeof(tbuf) - 1;
                     char *cp2 = tbuf;
                     while (*cp1 && isdigit(*cp1) && tcnt > 0) 
                     {
                        *(cp2++) = *(cp1++);
                        *cp2 = '\0';
                        tcnt--;
                     }
                     Serial << "Have number: '" << tbuf << "'" << endl;
                     if ( atol(tbuf) > 10000 ) 
                     {
                        logCount = tempLog.Count((time_t)atol(tbuf));
                     }
                     else if ( atoi(tbuf) < logCount )
                     {
                        if ( atoi(tbuf) == 0 ) 
                        {
                           logCount = tempLog.Count();
                        }
                        else 
                        {
                           logCount = atoi(tbuf);
                        }
                     }
                  }
                  else 
                  {
                     logCount = tempLog.Count(lastStartTime);
                  }
                  Serial << "Count: " << logCount << endl;
                  time_t thisTime = tempLog.MoveToHead();
                  lastStartTime = thisTime;
                  uint8_t channel;
                  float temp;
                  for (int x = 0; x < logCount; x++)
                  {
                     time_t time = thisTime;
                     thisTime = tempLog.ReadPreviousEntry(thisTime, &channel, &temp);
                     Serial << formatLogEntry(tbuf, time, channel, temp) << endl;
                  }
               }
               else if ( (cp1 = strcasestr(cp, "cool")) != 0 ) 
               {
                  thermostat.call4Heat(false);
                  thermostat.call4Fan(false);
                  thermostat.call4Cool(true);
               }

               else if ( (cp1 = strcasestr(cp, "heat")) != 0 ) 
               {
                  thermostat.call4Cool(false);
                  thermostat.call4Fan(false);
                  thermostat.call4Heat(true);
               }

               else if ( (cp1 = strcasestr(cp, "fan")) != 0 ) 
               {
                  thermostat.call4Heat(false);
                  thermostat.call4Cool(false);
                  thermostat.call4Fan(true);
               }

               else if ( (cp1 = strcasestr(cp, "off")) != 0 ) 
               {
                  thermostat.call4Heat(false);
                  thermostat.call4Cool(false);
                  thermostat.call4Fan(false);
               }

               else if ( (cp1 = strcasestr(cp, "readclock")) != 0 ) 
               {
                  time_t time = currentDate.readClock();
                  Serial << "hw clock = " << time << " [" << formatTime(time) << "]" << endl;
               }
               else if ( (cp1 = strcasestr(cp, "lamps")) != 0 ) 
               {
                  uint8_t val = display.getLamps();
                  Serial << "lamps = 0x" << itoa(val, ibuf, 16) << endl;
               }
               else if ( (cp1 = strcasestr(cp, "switches")) != 0 ) 
               {
                  uint8_t val = switches.sw_state;
                  Serial << "switches = %" << itoa(val, ibuf, 2) << endl;
               }
               else if ( (cp1 = strcasestr(cp, "digits")) != 0 ) 
               {
                  for (uint8_t x = 0; x < 6 ; x++) 
                  {
                     Serial << "digit[" << (int)x << "] = 0x" << itoa(display.getDigit(x), ibuf, 16) << endl;
                  }
               }
               else if ( (cp1 = strcasestr(cp, "touch")) != 0 ) 
               {
                  display.touch();
               }
# if 1
               else if ( (cp1 = strcasestr(cp, "tpoll")) != 0 ) 
               {
                  Serial << (mpr121.haveChange() ? "Have Key" : "Dont have key") << endl;
               }

               else if ( (cp1 = strcasestr(cp, "tread")) != 0 ) 
               {
                  display.disable();
                  delay(50);
                  uint16_t val = mpr121.readKeyboard();
                  Serial << "keyboard = 0x" << itoa(val, ibuf, 16) << endl;
                  display.touch();
               }
# endif
               else
                  Serial << strcpy_P(buf, PSTR("What??")) << endl;
            }
         }
      }
      else 
      {
         uint16_t dt[7];

         if ( currentDate.updateClock() ) 
         {
//            Serial << "time updated @ " << formatTime(currentDate.getTime()) << endl;
//            Serial << "hw clock =     " << formatTime(currentDate.readClock()) << endl;
         }

         if ( mpr121.haveChange() ) 
         {
            display.disable();
            delay(50);
            Serial << "keyboard = 0x" << itoa(mpr121.readKeyboard(), ibuf, 16) << endl;
            display.touch();
         }

//         ui.scan();

         if ( switches.haveButton() ) 
         {
            char btn = switches.getButton();
            Serial << "Button: " << btn << endl;
         }

         currentDate.getTime(dt);
# if 1
         display.setDigit(0, dt[Clock::Hour] / 10);
         display.setDigit(1, dt[Clock::Hour] % 10);
         display.setDigit(2, dt[Clock::Minute] / 10);
         display.setDigit(3, dt[Clock::Minute] % 10);
//         display.setDigit(4, dt[Clock::Second] / 10);
//         display.setDigit(5, dt[Clock::Second] % 10);
# else 
         display.lamp(0, false);
         display.lamp(1, false);
         display.lamp(2, false);
         display.lamp(3, false);
         display.lamp(4, false);
         display.lamp(5, false);
         display.lamp(6, false);
         display.lamp(7, false);
         display.lamp(8, false);
         display.lamp(9, false);
# endif

         bool log_temps = false;
         if ( currentDate.getInterval(Clock::READ_TEMPERATURE_INTERVAL) >= READ_TEMPERATURE_TIME ) 
         {
            currentDate.resetInterval(Clock::READ_TEMPERATURE_INTERVAL);

            if ( currentDate.getInterval(Clock::LOG_TEMPERATURE_INTERVAL) >= LOG_TEMPERATURE_TIME ) 
            {
               currentDate.resetInterval(Clock::LOG_TEMPERATURE_INTERVAL);
               log_temps = true;
            }

            sensors.readSensors();
            tempIsReady = 0;
            currentDate.getTime(dt);
            if ( currentDate.getInterval(Clock::DISP_TEMPERATURE_INTERVAL) >= DISP_TEMPERATURE_TIME ) 
            {
               sprintf(buf, "%d %02d/%02d/%4d %02d:%02d:%02d", lcnt, 
                                 dt[Clock::Month] + 1, 
                                 dt[Clock::Day], 
                                 dt[Clock::Year],
                                 dt[Clock::Hour],
                                 dt[Clock::Minute],
                                 dt[Clock::Second]);
               Serial << "[" << buf << "] " << strcpy_P(buf, PSTR(": "));
            }
            
            for (int x = 0; x < NUMSENSORS; x++) 
            {
               if ( sensors.validSensor(x) ) 
               {
                  float temp1 = sensors.read(x, false);
                  if ( log_temps ) 
                  {
                     tempLog.SaveTemp(x, temp1);
                  }
                  if ( currentDate.getInterval(Clock::DISP_TEMPERATURE_INTERVAL) >= DISP_TEMPERATURE_TIME ) 
                  {
                     Serial << strcpy_P(buf, PSTR(" temp ")) 
                            << x 
                            << strcpy_P(buf, PSTR(": ")) 
                            << temp1;
                  }
                  if ( x == 0 ) 
                  {
                     display.flashDigits(_BV(4) | _BV(5), false);
                     display.setDigit(4, (int)temp1 / 10);
                     display.setDigit(5, (int)temp1 % 10);
                  }
               }
            }
            if ( currentDate.getInterval(Clock::DISP_TEMPERATURE_INTERVAL) >= DISP_TEMPERATURE_TIME ) 
            {
               if ( log_temps ) 
                  Serial << " **";
               Serial << endl;
            }
            currentDate.resetInterval(Clock::DISP_TEMPERATURE_INTERVAL);
            sensors.startCycle();
         }
      }
   }
}




