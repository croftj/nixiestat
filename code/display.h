#ifndef  DISPLAY_H
#define  DISPLAY_H

# include <avr/common.h>
# include "WProgram.h"
# include <stdint.h>
# include <stdio.h>

class Display 
{
public:
   enum 
   {
      /***********************************************/
      /*   These define the lamps                    */
      /***********************************************/
      LAMP_SETTING_MASK = 0x1E,
      LAMP_MASK         = 0x3ff,
      LAMP_AMPM         = 0U,
      LAMP_LOWER_COLON,
      LAMP_UPPER_COLON,
      LAMP_COOL,
      LAMP_HEAT,
      LAMP_MINUS,
      LAMP_SETTING_1,
      LAMP_SETTING_2,
      LAMP_SETTING_3,
      LAMP_SETTING_4,
      LAMP_SETTING_5,
      NUM_LAMPS,        // Must remain at the end of the lamp definitions

      /***********************************************/
      /*   These  are  used  for flashing different  */
      /*   sets of digits on the display             */
      /***********************************************/
      FLASH_HOUR     = _BV(0) | _BV(1),
      FLASH_MINUTE   = _BV(2) | _BV(3),
      FLASH_MONTH    = _BV(0) | _BV(1),
      FLASH_DAY      = _BV(2) | _BV(3),
      FLASH_YEAR     = _BV(0) | _BV(1) | _BV(2) | _BV(3),
      FLASH_TEMP     = _BV(4) | _BV(5),

      /***********************************************/
      /*   These are used for setting the values of  */
      /*   the digits on the display.                */
      /***********************************************/
      DIGIT_HOUR_TENS      = 0,
      DIGIT_HOUR_UNITS     = 1,
      DIGIT_MINUTE_TENS    = 2,
      DIGIT_MINUTE_UNITS   = 3,
      DIGIT_TEMP_TENS      = 4,
      DIGIT_TEMP_UNITS     = 5,
      DIGIT_MONTH_TENS     = 0,
      DIGIT_MONTH_UNITS    = 1,
      DIGIT_DAY_UNITS      = 2,
      DIGIT_DAY_TENS       = 3,
      DIGIT_YEAR_THOUSANDS = 0,
      DIGIT_YEAR_HUNDREDS  = 1,
      DIGIT_YEAR_TENS      = 2,
      DIGIT_YEAR_UNITS     = 3,
      DIGIT_COUNT          = 6,
      DIGIT_MASK           = 0x3f
   };

   Display();
   
   void init();

   void setSpeed(uint16_t spd)
   {
      speed = spd;
   };

   void setDigit(uint8_t digit, uint8_t val)
   {
      digit %= 6;
//      Serial << __PRETTY_FUNCTION__ << "setting display, digit = " << (int)digit << ", value = " << (int)val << endl;
      digitValues[digit] = val % 10;
   };

   uint8_t getDigit(uint8_t pos)
   {
      return(digitValues[pos]);
   };

   void flashDigits(uint8_t digits, bool flash)
   {
      if ( flash ) 
         digitFlashMask |= digits;
      else
         digitFlashMask &= ~digits;
   };

   void blankDigits(uint8_t digits, bool blank)
   {
      if ( blank ) 
         digitBlankMask |= digits;
      else
         digitBlankMask &= ~digits;
   };

   void clearLamps()
   {
      lamps = 0;
      lampFlashMask = 0;
   }

   void lamp(uint8_t lamp, bool on)
   {
      static char ibuf[6];
      uint16_t mask = _BV((lamp % 10));
      if ( on ) 
         lamps |= mask;
      else
         lamps &= ~(mask);
//      Serial << __PRETTY_FUNCTION__ << " lamp = " << (int)lamp << ", on = " << (int)on << ", mask = " << itoa(lamps, ibuf, 16) << ", lamps = 0x" << itoa(lamps, ibuf, 16) << endl;
   };

   uint8_t getLamps()
   {
      return(lamps);
   };

   void flashLamps(uint16_t lamps, bool on)
   {
      if ( on ) 
         lampFlashMask |= lamps;
      else
         lampFlashMask &= ~lamps;
   };

   void touch();

   bool isEnabled()
   {
      return(m_enabled);
   }

   bool setEnabled(bool enable)
   {
//      Serial << "enable = " << enable;
      bool rv = m_enabled;
      if ( enable ) 
      {
         touch();
      }
      else 
      {
         disable();
      }
      return(rv);
   }

   bool disable();

   void setEnableIdleTime(uint16_t val)
   {
      enableIdleTime = val;
      enableCounter = 0;
   };

   uint16_t getNextDigit()
   {
      return(nextDigit);
   };

   void loop();

   uint8_t  digitFlashMask;
   uint8_t  digitBlankMask;
   int16_t  flashCounter;
   uint16_t enableCounter;

protected:
//private:
public:
   uint8_t  digitValues[6];
   uint16_t lamps;

   uint16_t lampFlashMask;

   uint16_t enableIdleTime;
   uint16_t speed;
   uint8_t  nextDigit;
   uint8_t  loopCnt;
   bool     m_enabled;
};

extern Display display;

# endif



