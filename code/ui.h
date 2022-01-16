/***************************************************************************
**    ui.h
**    User Interface Finite State Machine
**
** Copyright (C) 2010   Joseph Croft <joe@croftj.net>  
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

#ifndef _UI_H
#define _UI_H

#include <inttypes.h>
#include <stdio.h>

# include "datetime.h"
# include "display.h"
# include "TempSensors.h"
# include "thermostat.h"
# include "UiState.h"

class Ui 
{
public:
   enum 
   {
      SHORT_TIME_DEFAULT   = 4,
      LONG_TIME_DEFAULT    = 30,

      KEY_MODE             = 0x001, // 0x400,
      KEY_STEP             = 0x002, // 0x800,
      KEY_UP               = 0x010, // 0x40,
      KEY_DOWN             = 0x020, // 0x80,
      KEY_SET              = 0x004, // 0x04,
      KEY_CLEAR            = 0x008, // 0x08,

      // The following must remain at the end of the enum
      CURRENT_SETTINGS     = 0,
      TIME_SETTING_1,
      TIME_SETTING_2,
      TIME_SETTING_3,
      TIME_SETTING_4,
      NUM_SETTINGS         // Must be last in the list
   };

   Ui()
   {
      m_showYear = false;
      m_showMonDay = false;
      m_setting = CURRENT_SETTINGS;
      m_keyState = 0x00;
      m_state = 0;
   }

   void haveKeyPress(uint16_t keys);
   void tick();

   void moveToNextSetting();
   void restoreSettings();
   void saveSettings(bool saveTime = false);
   void saveTemperature();

   void CancelOverride();
   void DecrementCurrentTemp();
   void IncrementCurrentTemp();
   void ShowTimeTemp(uint8_t index);

   void showDays(uint16_t day);
   void showHours(uint16_t hour);
   void showMinutes(uint16_t minute);
   void showMonths(uint16_t month);
   void showTemperature(int8_t temp);
   void showYears(uint16_t year);
   void showSettings();

   void clearLamps()
   {
      display.clearLamps();
   }

   UIState *getState()
   {
      return(m_state);
   }

   void setState(UIState *newState)
   {
      currentDate.resetInterval(Clock::UI_LONG_INTERVAL);
      currentDate.resetInterval(Clock::UI_SHORT_INTERVAL);
      m_state = newState;
      m_state->onEntry();  
   }

   void decrDay()
   {
   }

   void decrHours()
   {
//      if (dbg) Serial << __PRETTY_FUNCTION__ << "decrementing m_hour = " << (int)m_hour << endl;
      m_hour--;
      if ( m_hour < 0 ) 
      {
         m_hour = 23;
      }
      showHours(m_hour);
   }

   void decrMinutes()
   {
      m_minute--;
      if ( m_minute < 0 ) 
      {
         m_minute = 59;
      }
      showMinutes(m_minute);
   }

   void decrMonth()
   {
   }

   void decrTemperature()
   {
      if ( m_temp > -126 ) 
      {
         m_temp--;
         showTemperature(m_temp);
      }
   }

   void decrYear()
   {
   }

   void displayOn()
   {
      display.touch();
      currentDate.resetInterval(Clock::UI_LONG_INTERVAL);
      currentDate.resetInterval(Clock::UI_SHORT_INTERVAL);
   }

   void displayOff()
   {
      display.disable();
   }

   void flashNone()
   {
      display.flashDigits(Display::DIGIT_MASK, false);   
   }

   void flashYear()
   {
      m_showYear = true;
      m_showMonDay = false;
      showSettings();
      display.flashDigits(Display::DIGIT_MASK, false);   
      display.flashDigits(Display::FLASH_YEAR, true);   
   }

   void flashDay()
   {
      m_showYear = false;
      m_showMonDay = true;
      showSettings();
      display.flashDigits(Display::DIGIT_MASK, false);   
      display.flashDigits(Display::FLASH_DAY, true);   
   }

   void flashHeatCool()
   {
      m_showYear = false;
      m_showMonDay = false;
      display.flashDigits(Display::DIGIT_MASK, false);   
      showSettings();
//      if (dbg) Serial << __PRETTY_FUNCTION__ << "Lamp_Cool " << (int)Display::LAMP_COOL << endl;
//      if (dbg) Serial << __PRETTY_FUNCTION__ << "Lamp_Heat " << (int)Display::LAMP_HEAT << endl;
      display.flashLamps(Display::LAMP_MASK, false);   
      display.flashLamps(_BV(Display::LAMP_HEAT) | _BV(Display::LAMP_COOL), true);
   }

   void flashHours()
   {
      m_showYear = false;
      m_showMonDay = false;
      showSettings();
      display.flashDigits(Display::DIGIT_MASK, false);   
      display.flashDigits(Display::FLASH_HOUR, true);   
   }

   void flashMinutes()
   {
      m_showYear = false;
      m_showMonDay = false;
      showSettings();
      display.flashDigits(Display::DIGIT_MASK, false);   
      display.flashDigits(Display::FLASH_MINUTE, true);   
   }

   void flashMonth()
   {
      m_showYear = false;
      m_showMonDay = true;
      showSettings();
      display.flashDigits(Display::DIGIT_MASK, false);   
      display.flashDigits(Display::FLASH_MONTH, true);   
   }

   void flashTemperature()
   {
      m_showYear = false;
      m_showMonDay = false;
      showSettings();
      display.flashDigits(Display::DIGIT_MASK, false);   
      display.flashDigits(Display::FLASH_TEMP, true);   
   }

   uint8_t getSetting()
   {
      return(m_setting);
   }

   bool haveCalendar()
   {
      return(m_setting = CURRENT_SETTINGS);
   }
   void incrDay()
   {
   }

   void incrHours()
   {
//      if (dbg) Serial << __PRETTY_FUNCTION__ << "incrementing m_hour = " << (int)m_hour << endl;
      m_hour++;
      m_hour %= 24;
      showHours(m_hour);
   }

   void incrMinutes()
   {
      m_minute++;
      m_minute %= 60;
      showMinutes(m_minute);
   }

   void incrMonth()
   {
   }

   void initialize(ESquareSpace_t *settings)
   {
      m_settings = settings;

      if ( m_state == 0 ) 
      {
         m_state = UIState::initialState();
      }
//      m_temp = thermostat.getTemp(m_setting);
//      m_time = thermostat.getTime(m_setting);
      m_hour = m_time / 24;
      m_minute = m_time % 24;
      m_state->initEvent();
   }

   void incrTemperature()
   {
      if ( m_temp < 127 ) 
      {
         m_temp++;
         showTemperature(m_temp);
      }
   }

   void incrYear()
   {
   }

   void setTemp(int idx, float temp)
   {
      m_currTemps[idx] = temp;
   }

   void setSetting(uint8_t setting)
   {
      m_setting = setting;
      m_showYear = false;
      m_showMonDay = false;
//      m_temp = thermostat.getTemp(m_setting);
//      m_time = thermostat.getTime(m_setting);
   }

   uint8_t setting()
   {
      return(m_setting);
   }

   void showCurrentTemp()
   {
      int temp = (int)(m_currTemps[0] + 0.5);
      display.setDigit(4, temp / 10);
      display.setDigit(5, temp % 10);
   }

   void toggleHeatCool()
   {
      m_thermMode = ! m_thermMode;
//      thermostat.setMode(! thermostat.getMode());
      showSettings();
   }

protected:

private:
   uint8_t        m_setting;
   bool           m_showYear;
   bool           m_showMonDay;

   bool           m_thermMode;
   uint8_t        m_temp;
   uint16_t       m_time;

   uint16_t       m_year;
   uint16_t       m_month;
   uint16_t       m_day;
   uint16_t       m_hour;
   uint16_t       m_minute;

   float          m_currTemps[NUM_SENSORS];
   ESquareSpace_t *m_settings;

   uint16_t       m_keyState;
   UIState       *m_state;
//   static uint8_t dbg;
};

extern Ui ui;

#endif
