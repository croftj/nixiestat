/***************************************************************************
**    ui.h
**    User Interface Finite State Machine
**
** Copyright (C) 2010  Joseph Croft <joe@croftj.net>  
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/

# include <avr/pgmspace.h>
# include <string.h>

# include "EEPROM.h"
# include "WProgram.h"

#include "display.h"
#include "ui.h"

# include "TempCycle.h"

#define  FAT

extern TempSensors sensors;
extern uint8_t dbg;

//uint8_t  Ui::dbg = true;

Ui ui;

/**********************************************************************/
/*   Here we handle key presses and turn them into events             */
/**********************************************************************/
void Ui::haveKeyPress(uint16_t keys)
{
   if ( m_state == 0 ) 
   {
      m_state = UIState::initialState();
      m_state->onEntry();  
   }

//   Serial << __PRETTY_FUNCTION__ << ": m_state = " << (uint32_t)m_state << endl;
   /***********************************************/
   /*   If the keys are released, just clear the  */
   /*   current key state                         */
   /***********************************************/
   if ( keys == 0x00 ) 
   {
      m_keyState = keys;
   }

   /***********************************************/
   /*   Otherwise,    perform    a    sort    of  */
   /*   debounce/serialization to allow only one  */
   /*   keypress at a time.                       */
   /***********************************************/
   else if ( m_keyState == 0x00 )
   {
      currentDate.resetInterval(Clock::UI_LONG_INTERVAL);
      
      m_state->anyKeyEvent();       // Send this event first!

      if ( keys & KEY_MODE ) 
      {
         m_keyState = KEY_MODE;
         m_state->modeEvent();
      }
      else if ( keys & KEY_STEP ) 
      {
         m_keyState = KEY_STEP;
         m_state->stepEvent();
      }
      else if ( keys & KEY_UP ) 
      {
         m_keyState = KEY_UP;
         m_state->upEvent();
      }
      else if ( keys & KEY_DOWN ) 
      {
         m_keyState = KEY_DOWN;
         m_state->downEvent();
      }
      else if ( keys & KEY_SET ) 
      {
         m_keyState = KEY_SET;
         m_state->setEvent();
      }
      else if ( keys & KEY_CLEAR ) 
      {
         m_keyState = KEY_CLEAR;
         m_state->clearEvent();
      }
   }
}

/**********************************************************************/
/*   Here  we  handle  clock  tick  and generate long and short time  */
/*   events as needed                                                 */
/**********************************************************************/
void Ui::tick()
{
//   Serial << __FUNCTION__ << "- long interval count =  " << currentDate.getInterval(Clock::UI_LONG_INTERVAL) << endl;
//   Serial << __FUNCTION__ << "- short interval count =  " << currentDate.getInterval(Clock::UI_SHORT_INTERVAL) << endl;
   if ( currentDate.getInterval(Clock::UI_LONG_INTERVAL) >= LONG_TIME_DEFAULT )
   {
//      Serial << __FUNCTION__ << " Long interval expired, state = " << m_state->name() << endl;

      m_state->timeLongEvent();
      currentDate.resetInterval(Clock::UI_LONG_INTERVAL);
   }      
   else if ( currentDate.getInterval(Clock::UI_SHORT_INTERVAL) >= SHORT_TIME_DEFAULT )
   {
//      Serial << __FUNCTION__ << " Short interval expired, state = " << m_state->name() << endl;
      m_state->timeShortEvent();
      currentDate.resetInterval(Clock::UI_SHORT_INTERVAL);
   }
}

void Ui::moveToNextSetting()
{
   display.clearLamps();
   m_setting++;
   m_setting %= Settings::NUM_SETTINGS;
//   if ( m_setting == 0 ) 
//   {
//      m_setting = 1;
//   }
   restoreSettings();
}

void Ui::CancelOverride()
{
   thermostat.CancelOverride();
}

void Ui::DecrementCurrentTemp()
{
   thermostat.SetOverride(thermostat.getCurrentTempSetting() - 1.0);
}

void Ui::IncrementCurrentTemp()
{
   thermostat.SetOverride(thermostat.getCurrentTempSetting() + 1.0);
}

void Ui::ShowTimeTemp(uint8_t index)
{
   int8_t sensor;

   display.clearLamps();
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "Index = " << (int)index << endl;
   display.lamp(Display::LAMP_SETTING_1 + index, true);
   switch (index) 
   {
      case 1:
         sensor = m_settings->outsideSensor;
         break;   

      case 2:
         sensor = m_settings->targetSensor;
         break;   

      case 3:
         sensor = m_settings->cycleSensor;
         break;   

      case 0:
         sensor = -1;
      default:
         break;
   }
   if (dbg) Serial << __PRETTY_FUNCTION__ << "sensor = " << (int)sensor << endl;
   int8_t temp = (sensor < 0) ? thermostat.getCurrentTempSetting()
                              : (uint8_t)(sensors.read(sensor) + 0.5);
   if ( dbg > 3 ) 
   {
      temp = sensor;
   }
   if (dbg) Serial << __PRETTY_FUNCTION__ << "temp = " << (int)temp << endl;
   showTemperature(temp);
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "here 1" << endl;

   uint16_t dt[Clock::NUM_ELEMENTS];
   currentDate.getTime(dt);
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "here 1.2" << endl;
   uint8_t minutes = dt[Clock::Minute];
   uint8_t hours = dt[Clock::Hour];
   showMinutes(minutes);
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "here 2" << endl;
   showHours(hours);
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "here 3" << endl;

   bool last_call = temp_cycle.getLastCall();
   if ( (m_thermMode = m_settings->currentMode) == Settings::Cool ) 
   {
      display.lamp(Display::LAMP_HEAT, false);
      display.lamp(Display::LAMP_COOL, true);
      display.flashLamps((uint16_t)(1 << Display::LAMP_COOL), last_call);
   }
   else
   {
      display.lamp(Display::LAMP_HEAT, true);
      display.lamp(Display::LAMP_COOL, false);
      display.flashLamps((uint16_t)(1 << Display::LAMP_HEAT), last_call);
   }
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "exit" << endl;
}

void Ui::restoreSettings()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << "m_setting = " << (int)m_setting << ", m_temp = " << (int)m_temp << endl;
   if ( m_setting == Ui::CURRENT_SETTINGS ) 
   {
//      m_temp = thermostat.getCurrentTempSetting();
      uint16_t dt[Clock::NUM_ELEMENTS];
      currentDate.getTime(dt);
      m_year     = dt[Clock::Year];
      m_month    = dt[Clock::Month];
      m_day      = dt[Clock::Day];
      m_hour     = dt[Clock::Hour];
      m_minute   = dt[Clock::Minute];

//      m_thermMode = thermostat.getMode();
   }
   else 
   {
//      m_temp = thermostat.getTemp(m_setting);
//      if (dbg) Serial << __PRETTY_FUNCTION__ << "thermostat.getTime(" << (int)m_setting << ") = " << thermostat.getTime(m_setting) << endl;
//      if ( thermostat.getTime(m_setting) != 0xffff ) 
      {
//         if (dbg) Serial << __PRETTY_FUNCTION__ << "Defined time!! thermostat.getTime(" << (int)m_setting << ") = " << thermostat.getTime(m_setting) << endl;
//         m_hour   = thermostat.getTime(m_setting) / 60;
//         m_minute = thermostat.getTime(m_setting) % 60;
      }
//      else 
      {
         if (dbg) Serial << __PRETTY_FUNCTION__ << "Undefined time!!" << endl;
         uint16_t dt[Clock::NUM_ELEMENTS];
         currentDate.getTime(dt);
         m_year     = dt[Clock::Year];
         m_month    = dt[Clock::Month];
         m_day      = dt[Clock::Day];
         m_hour     = dt[Clock::Hour];
         m_minute   = dt[Clock::Minute];
      }
      if (dbg) Serial << __PRETTY_FUNCTION__ << "m_hour = " << (int)m_hour << ", m_minute = " << (int)m_minute << endl;
   }
}

void Ui::saveTemperature()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " saving current temp of " << (int)m_temp << endl;
//   thermostat.setTemp(Ui::CURRENT_SETTINGS, m_temp);
}

void Ui::saveSettings(bool saveTime)
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " saving temp of " << (int)m_temp << " for setting " << (int)m_setting << endl;
//   thermostat.setTemp(m_setting, m_temp);
//   thermostat.setMode(m_thermMode);
   if ( saveTime ) 
   {
      if (dbg) Serial << __PRETTY_FUNCTION__ << " saving time!" << endl;
      if ( m_setting == 0 ) 
      {
         if (dbg) Serial << __PRETTY_FUNCTION__ << " setting current date and time" << endl;
         uint16_t dt[Clock::NUM_ELEMENTS];
         currentDate.getTime(dt);
         dt[Clock::Minute] = m_minute;
         dt[Clock::Hour]   = m_hour;
         dt[Clock::Day]    = m_day;
         dt[Clock::Month]  = m_month;
         dt[Clock::Year]   = m_year;
         currentDate.setTime(dt);
         currentDate.writeClock(currentDate.readClock());
      }
      else 
      {
//         if (dbg) Serial << __PRETTY_FUNCTION__ << " setting thermostat time" << endl;
//         thermostat.setTime(m_setting, (m_hour * 60) + m_minute);
      }
   }
//   thermostat.save();
   if (dbg) Serial << __PRETTY_FUNCTION__ << " Exit" << endl;
}

void Ui::showDays(uint16_t day)
{
   display.setDigit(Display::DIGIT_DAY_UNITS,   day % 10);
   display.setDigit(Display::DIGIT_DAY_TENS,    day / 10);
   display.lamp(Display::LAMP_LOWER_COLON, true);
   display.lamp(Display::LAMP_UPPER_COLON, false);
}

void Ui::showHours(uint16_t hour)
{
   if ( hour > 12 ) 
   {
      hour %= 12;
      display.lamp(Display::LAMP_AMPM, true);
   }
   else 
   {
      display.lamp(Display::LAMP_AMPM, false);
   }
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "showing hour = " << (int)hour << endl;
   display.setDigit(Display::DIGIT_MONTH_UNITS, hour % 10);
   display.setDigit(Display::DIGIT_MONTH_TENS,  hour / 10);
   display.lamp(Display::LAMP_LOWER_COLON, true);
   display.lamp(Display::LAMP_UPPER_COLON, true);
}

void Ui::showMinutes(uint16_t minute)
{
   display.setDigit(Display::DIGIT_MINUTE_UNITS, minute % 10);
   display.setDigit(Display::DIGIT_MINUTE_TENS,  minute / 10);
   display.lamp(Display::LAMP_LOWER_COLON, true);
   display.lamp(Display::LAMP_UPPER_COLON, true);
}

void Ui::showMonths(uint16_t month)
{
   display.setDigit(Display::DIGIT_MONTH_UNITS, month % 10);
   display.setDigit(Display::DIGIT_MONTH_TENS,  month / 10);
   display.lamp(Display::LAMP_LOWER_COLON, true);
   display.lamp(Display::LAMP_UPPER_COLON, false);
}

void Ui::showTemperature(int8_t temp)
{
   /***********************************************/
   /*   Start  by  showing  if  it's  below 0 or  */
   /*   above 100                                 */
   /***********************************************/
   if ( temp < 0 || temp > 99 ) 
   {
      if (dbg) Serial << __PRETTY_FUNCTION__ << " Have minus or 100+ temp: " << (int)temp << endl;
      display.lamp(Display::LAMP_MINUS, true);
   }
   else 
   {
      display.lamp(Display::LAMP_MINUS, false);
   }

   /***********************************************/
   /*   Now put the temp into the display         */
   /***********************************************/
   temp = abs(temp);
   temp %= 100;
   display.setDigit(Display::DIGIT_TEMP_UNITS, temp % 10);
   display.setDigit(Display::DIGIT_TEMP_TENS,  temp / 10);
}

void Ui::showYears(uint16_t year)
{
   uint8_t y1    =  year % 10;
   uint8_t y10   = (year / 10) % 10;
   uint8_t y100  = (year / 100) % 10;
   uint8_t y1000 =  year / 1000;

   display.setDigit(Display::DIGIT_YEAR_UNITS,     y1);
   display.setDigit(Display::DIGIT_YEAR_TENS,      y10);
   display.setDigit(Display::DIGIT_YEAR_HUNDREDS,  y100);
   display.setDigit(Display::DIGIT_YEAR_THOUSANDS, y1000);

   display.lamp(Display::LAMP_LOWER_COLON, false);
   display.lamp(Display::LAMP_UPPER_COLON, false);
}

/**********************************************************************/
/*   This  function  displays the appropriate values for the current  */
/*   settings which are being manipulated by the user interface       */
/**********************************************************************/
void Ui::showSettings()
{
   static char ibuf[6];
   /***********************************************/
   /*   Start  by  turning  off all of the lamps  */
   /*   that  show  the setting. (The four lamps  */
   /*   next to the temp at this time)            */
   /***********************************************/
   display.clearLamps();

   /***********************************************/
   /*   If  the  year  is  being set, go load it  */
   /*   into the display                          */
   /***********************************************/
   if ( m_showYear ) 
   {
      uint16_t dt[Clock::NUM_ELEMENTS];
      currentDate.getTime(dt);
      showYears(dt[Clock::Year]);
   }

   /***********************************************/
   /*   Otherwise  if  they  are  working on the  */
   /*   month   and  day,  load  them  into  the  */
   /*   display.                                  */
   /***********************************************/
   else if ( m_showMonDay ) 
   {
      uint16_t dt[Clock::NUM_ELEMENTS];
      currentDate.getTime(dt);

      showDays(dt[Clock::Day]);
      showMonths(dt[Clock::Month]);
   }

   /***********************************************/
   /*   If  neither of the above were being set,  */
   /*   assume  we  are  setting  the  time  and  */
   /*   temperature of whatever setting they are  */
   /*   on.                                       */
   /***********************************************/
   else 
   {
      /***********************************************/
      /*   Now  set  the  appropriate  lamp for the  */
      /*   current     setting.     The     setting  */
      /*   CURRENT_SETTINGS does not have any lamps  */
      /*   lit.                                      */
      /***********************************************/
      if ( m_setting > 0 ) 
      {
         uint8_t y = m_setting - 1 + Display::LAMP_SETTING_1;
         if (dbg) Serial << __PRETTY_FUNCTION__ << " m_setting = " << (int)m_setting << ", y = " << (int)y << endl;
         display.lamp(y, true);
      }
      if ( m_thermMode == Settings::Cool ) 
      {
         display.lamp(Display::LAMP_HEAT, false);
         display.lamp(Display::LAMP_COOL, true);
      }
      else
      {
         display.lamp(Display::LAMP_HEAT, true);
         display.lamp(Display::LAMP_COOL, false);
      }

      /***********************************************/
      /*   Now get the temperature and display it    */
      /***********************************************/
      int8_t temp;
      if ( m_setting == Ui::CURRENT_SETTINGS ) 
      {
//         temp = thermostat.getCurrentTempSetting();
      }
      else 
      {
//         temp = thermostat.getTemp(m_setting);
      }
      if (dbg) Serial << __PRETTY_FUNCTION__ << "showing temp[" << (int)m_setting << "] = " << (int)temp << endl;

      int t = (int)(m_currTemps[0] + 0.5);
      showTemperature(t);

      /***********************************************/
      /*   Fetch  and  display  the  time  for  the  */
      /*   current setting                           */
      /***********************************************/
      uint8_t hours;
      uint8_t minutes;
//      if ( m_setting == 0 || thermostat.getTime(m_setting) == 0xffff ) 
      {
         uint16_t dt[Clock::NUM_ELEMENTS];
         currentDate.getTime(dt);
         minutes = dt[Clock::Minute];
         hours = dt[Clock::Hour];
      }
//      else 
      {
//         uint16_t time = thermostat.getTime(m_setting);
//         minutes = time % 60;
//         hours = time / 60;
      }
      showMinutes(minutes);
      showHours(hours);
   }
}
