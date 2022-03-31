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

# include <string.h>
#include <thread>

#include "esp_log.h"

#include "display.h"
#include "MPR121.h"
#include "ui.h"

# include "DBGPorts.h"
# include "TempCycle.h"

#define KEY __PRETTY_FUNCTION__

extern TempSensors sensors;

//uint8_t  Ui::dbg = true;

Ui ui;

int      Ui::m_longInterval = 0;
int      Ui::m_shortInterval = 0;
UIState* Ui::m_state = NULL;

/**************************************************************/
/* This function serves as the timer for the user             */
/* interface. it runs on its own thread emitting two events,  */
/* short time event and long time event to the current state. */
/**************************************************************/
void *Ui::runTimer(void*)
{
   while (true)
   {
      ui.haveKeyPress(mpr121->read());
//      ESP_LOGI(KEY, "keyState = 0x%04x", ui.m_keyState);
      if (true || ui.m_keyState == 0)
      {
         if ( ++m_longInterval >= LONG_TIME_DEFAULT )
         {
//            ESP_LOGI(KEY, "long event!");
            m_state->timeLongEvent();
            m_longInterval = 0;
         }      
         else if ( ++m_shortInterval >= SHORT_TIME_DEFAULT )
         {
//            ESP_LOGI(KEY, "short event!");
            m_state->timeShortEvent();
            m_shortInterval = 0;
         }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
   }
   return(NULL);
}

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

   /***********************************************/
   /*   If the keys are released, just clear the  */
   /*   current key state                         */
   /***********************************************/
   if ( keys == 0x00 ) 
   {
//      ESP_LOGI(KEY, "clearing keyState");
      m_keyState = keys;
   }

   /***********************************************/
   /*   Otherwise,    perform    a    sort    of  */
   /*   debounce/serialization to allow only one  */
   /*   keypress at a time.                       */
   /***********************************************/
   else if ( m_keyState == 0x00 )
   {
      ESP_LOGI(KEY, "keys = 0x%04x", keys);
      m_longInterval = 0;
      
      m_state->anyKeyEvent();       // Send this event first!

      if ( keys & KEY_MODE ) 
      {
         m_keyState = KEY_MODE;
         ESP_LOGI(TAG, "modeEvent()");
         m_state->modeEvent();
      }
      else if ( keys & KEY_STEP ) 
      {
         m_keyState = KEY_STEP;
         ESP_LOGI(TAG, "stepEvent()");
         m_state->stepEvent();
      }
      else if ( keys & KEY_UP ) 
      {
         m_keyState = KEY_UP;
         ESP_LOGI(TAG, "upEvent()");
         gpio_set_level(DBG_PIN_3, 1);
         m_state->upEvent();
      }
      else if ( keys & KEY_DOWN ) 
      {
         m_keyState = KEY_DOWN;
         ESP_LOGI(TAG, "downEvent()");
         gpio_set_level(DBG_PIN_3, 1);
         m_state->downEvent();
      }
      else if ( keys & KEY_SET ) 
      {
         m_keyState = KEY_SET;
         ESP_LOGI(TAG, "setEvent()");
         m_state->setEvent();
      }
      else if ( keys & KEY_CLEAR ) 
      {
         m_keyState = KEY_CLEAR;
         ESP_LOGI(TAG, "clearEvent()");
         m_state->clearEvent();
      }
   }
//   gpio_set_level(DBG_PIN_3, 0);
}

/**********************************************************************/
/*   Here  we  handle  clock  tick  and generate long and short time  */
/*   events as needed                                                 */
/**********************************************************************/
void Ui::tick()
{
   if ( m_longInterval >= LONG_TIME_DEFAULT )
   {

      m_state->timeLongEvent();
      m_longInterval = 0;
   }      
   else if ( m_shortInterval >= SHORT_TIME_DEFAULT )
   {
      m_state->timeShortEvent();
      m_shortInterval = 0;
   }
}

void Ui::moveToNextSetting()
{
   display->clear_lamps();
   m_setting++;
   m_setting %= Ui::NUM_SETTINGS;
   restoreSettings();
}

void Ui::CancelOverride()
{
   thermostat.CancelOverride();
}

void Ui::DecrementCurrentTemp()
{
//   thermostat.SetOverride(thermostat.getCurrentTempSetting() - 1.0);
}

void Ui::IncrementCurrentTemp()
{
//   thermostat.SetOverride(thermostat.getCurrentTempSetting() + 1.0);
}

void Ui::showOffset()
{
   int8_t ofs = config->value("ovr_offset").toInt();
   // Don't allow ofs to be more than +/-2 away from 0
   ESP_LOGI(TAG, "1:ofs = %d", ofs);
   ofs = abs(ofs) > 2 ? ((ofs < 0) ? -2 : 2) : ofs;
   ESP_LOGI(TAG, "2:ofs = %d", ofs);
   uint8_t lamp = Display::LAMP_SETTING_3;
   display->clear_lamps();
   display->lamp(lamp, true);
   ESP_LOGI(TAG, "3:ofs = %d", ofs);
   while (ofs != 0)
   {
      lamp += (ofs < 0) ? 1 : -1;
      display->lamp(lamp, true);
      ofs += (ofs < 0) ? 1 : -1;
   }
}

void Ui::showMode()
{
   bool last_call = temp_cycle.getPrevCall();
   ESP_LOGI(TAG, "last_call = %d", last_call);
   if ( (m_thermMode = config->value("mode").toInt()) == Configuration::Heat ) 
   {
      display->lamp(Display::LAMP_HEAT, true);
      display->lamp(Display::LAMP_COOL, false);
      display->flash_lamps((uint16_t)(1 << Display::LAMP_HEAT), last_call);
   }
   else
   {
      display->lamp(Display::LAMP_HEAT, false);
      display->lamp(Display::LAMP_COOL, true);
      display->flash_lamps((uint16_t)(1 << Display::LAMP_COOL), last_call);
   }
}

void Ui::showTemperature(uint8_t index, bool top)
{
   SensorData sensor;
   float temp = 0.0;
   time_t now;
   int ofs = 0;

   time(&now);
   switch (index) 
   {
      case 1:
         sensor = SensorData::getSensor(config->value("outside_sensor").toString());
         break;   

      case 2:
         sensor = thermostat.selectSensor(config->value("target_sensor").toString(), now);
         break;   

      case 3:
         sensor = SensorData::getSensor(config->value("cyc_sensor").toString());
         break;   

      default:
         sensor = SensorData("target", "", config->getCurrentTempSetting(), -1);
         ofs = config->value("ovr_offset").toInt();
         break;
   }
   ESP_LOGI(TAG, "sensor = %s, ofs = %d", sensor.name().c_str(), ofs);

   if (sensor.isValid())
   {
      temp = sensor.temperature();
   }
   ESP_LOGI(TAG, "temp = %f", temp);
   showTemperature(temp, top);
}

void Ui::restoreSettings()
{
#if 0
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
//      if ( thermostat.getTime(m_setting) != 0xffff ) 
      {
//         m_hour   = thermostat.getTime(m_setting) / 60;
//         m_minute = thermostat.getTime(m_setting) % 60;
      }
//      else 
      {
         uint16_t dt[Clock::NUM_ELEMENTS];
         currentDate.getTime(dt);
         m_year     = dt[Clock::Year];
         m_month    = dt[Clock::Month];
         m_day      = dt[Clock::Day];
         m_hour     = dt[Clock::Hour];
         m_minute   = dt[Clock::Minute];
      }
   }
#endif
}

void Ui::decrTemperature()
{
   if ( true|| m_temp > 40 ) 
   {
      int ofs = config->value("ovr_offset").toInt() - 1;
      ESP_LOGI(TAG, "ofs = %d", ofs);
      config->setValue("ovr_offset", Variant(ofs), false);
      showOffset();
      showTemperature((uint8_t)CURRENT_SETTINGS, true);
      gpio_set_level(DBG_PIN_3, 0);
   }
}

void Ui::incrTemperature()
{
   if ( true || m_temp < 127 ) 
   {
      int ofs = config->value("ovr_offset").toInt() + 1;
      ESP_LOGI(TAG, "ofs = %d", ofs);
      config->setValue("ovr_offset", Variant(ofs), false);
      showOffset();
      showTemperature((uint8_t)CURRENT_SETTINGS, true);
      gpio_set_level(DBG_PIN_3, 0);
   }
}

void Ui::saveTemperature()
{
//   thermostat.setTemp(Ui::CURRENT_SETTINGS, m_temp);
}

void Ui::saveSettings(bool saveTime)
{
//   thermostat.setTemp(m_setting, m_temp);
//   thermostat.setMode(m_thermMode);
   if ( saveTime ) 
   {
#if 0
      if ( m_setting == 0 ) 
      {
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
//         thermostat.setTime(m_setting, (m_hour * 60) + m_minute);
      }
#endif
   }
//   thermostat.save();
}

void Ui::showDays(uint16_t day)
{
#if 0
   display->setDigit(Display::DIGIT_DAY_UNITS,   day % 10);
   display->setDigit(Display::DIGIT_DAY_TENS,    day / 10);
   display->lamp(Display::LAMP_LOWER_COLON, true);
   display->lamp(Display::LAMP_UPPER_COLON, false);
#endif
}

void Ui::showHours(uint16_t hour)
{
   display->setDigit(Display::DIGIT_HOURS_UNITS, hour % 10);
   display->setDigit(Display::DIGIT_HOURS_TENS,  hour / 10);
   display->setDigit(Display::DIGIT_HOURS_X, 99);
}

void Ui::showMinutes(uint16_t minute)
{
   display->setDigit(Display::DIGIT_MINS_UNITS, minute % 10);
   display->setDigit(Display::DIGIT_MINS_TENS,  minute / 10);
   display->setDigit(Display::DIGIT_MINS_X, 99);
}

void Ui::showMonths(uint16_t month)
{
#if 0
   display->setDigit(Display::DIGIT_MONTH_UNITS, month % 10);
   display->setDigit(Display::DIGIT_MONTH_TENS,  month / 10);
   display->lamp(Display::LAMP_LOWER_COLON, true);
   display->lamp(Display::LAMP_UPPER_COLON, false);
#endif
}

void Ui::showTemperature(SensorData sensor, bool top)
{
   time_t now;
   time(&now);
   uint8_t flash_mask = 0;
   if ( ! sensor.isValid() || sensor.timestamp() < 5 * 30)
   {
      flash_mask = top ? Display::TOP_TEMP_MASK : Display::BOT_TEMP_MASK;
      display->flashDigits(flash_mask, true);
   }
   else
   {
      display->flashDigits(flash_mask, true);
   }
   showTemperature(sensor.temperature(), top);
}


      
void Ui::showTemperature(float temp, bool top)
{
   /***********************************************/
   /*   Start  by  showing  if  it's  below 0 or  */
   /*   above 100                                 */
   /***********************************************/
   char buf[16];
   uint8_t ofs;

   sprintf(buf, "%4.1f", temp);
   ESP_LOGI(KEY, "temp buf = |%s|", buf);
   char *cp = buf;
   int dp = top ? Display::LAMP_TOP_DP : Display::LAMP_BOT_DP;
   if (*cp == '-')
   {
      display->lamp(Display::LAMP_MINUS, true);
      cp++;
   }
   else 
   {
      display->lamp(Display::LAMP_MINUS, false);
   }
   if (temp < 100.0)
   {
      display->lamp(dp, true);
   }
   else 
   {
      display->lamp(dp, false);
   }

   if (top)
   {
      ofs = 0;
   }
   else
   {
      ofs = Display::DIGIT_BOT_TEMP_MSD;
   }
   for (uint8_t x = 0; x < 3; x++)
   {
      while (*cp != 0x00 && (*cp < '0' || *cp > '9'))
         cp++;
      if (*cp == 0x00)
         break;
      uint8_t val = *cp - 0x30;
      ESP_LOGI(KEY, "ofs: %d, x: %d, val: %d", ofs, x, val);
      display->setDigit(ofs + x, val);
      cp++;
   }
}

void Ui::showYears(uint16_t year)
{
#if 0
   uint8_t y1    =  year % 10;
   uint8_t y10   = (year / 10) % 10;
   uint8_t y100  = (year / 100) % 10;
   uint8_t y1000 =  year / 1000;

   display->setDigit(Display::DIGIT_YEAR_UNITS,     y1);
   display->setDigit(Display::DIGIT_YEAR_TENS,      y10);
   display->setDigit(Display::DIGIT_YEAR_HUNDREDS,  y100);
   display->setDigit(Display::DIGIT_YEAR_THOUSANDS, y1000);

   display->lamp(Display::LAMP_LOWER_COLON, false);
   display->lamp(Display::LAMP_UPPER_COLON, false);
#endif
}

/**********************************************************************/
/*   This  function  displays the appropriate values for the current  */
/*   settings which are being manipulated by the user interface       */
/**********************************************************************/
void Ui::showSettings()
{
#if 1
   static char ibuf[6];
   /***********************************************/
   /*   Start  by  turning  off all of the lamps  */
   /*   that  show  the setting. (The four lamps  */
   /*   next to the temp at this time)            */
   /***********************************************/
   ESP_LOGI(KEY, "clearing first %d lamps", NUM_SETTINGS);
   for (int x = 0; x < NUM_SETTINGS; x++)
   {
      display->lamp(x, false);
   }

   /***********************************************/
   /*   If  the  year  is  being set, go load it  */
   /*   into the display                          */
   /***********************************************/
   if ( m_showYear ) 
   {
#if 0
      uint16_t dt[Clock::NUM_ELEMENTS];
      currentDate.getTime(dt);
      showYears(dt[Clock::Year]);
#endif
   }

   /***********************************************/
   /*   Otherwise  if  they  are  working on the  */
   /*   month   and  day,  load  them  into  the  */
   /*   display.                                  */
   /***********************************************/
   else if ( m_showMonDay ) 
   {
#if 0
      uint16_t dt[Clock::NUM_ELEMENTS];
      currentDate.getTime(dt);

      showDays(dt[Clock::Day]);
      showMonths(dt[Clock::Month]);
#endif
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
         display->lamp(y, true);
      }

      /***********************************************/
      /*   Now get the temperature and display it    */
      /***********************************************/
      showTemperature(m_setting);
   }
#endif
}
