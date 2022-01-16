#ifndef  DISPLAY_H
#define  DISPLAY_H

#include "IOPorts.h"

class Display 
{
public:
   enum 
   {
      /***********************************************/
      /*   These define the m_lamps                    */
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
      NUM_m_lamps,        // Must remain at the end of the lamp definitions

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

   void setm_speed(uint16_t spd)
   {
      m_speed = spd;
   };

   void setDigit(uint8_t digit, uint8_t val)
   {
      digit %= 6;
//      Serial << __PRETTY_FUNCTION__ << "setting display, digit = " << (int)digit << ", value = " << (int)val << endl;
      m_digitValues[digit] = val % 10;
   };

   uint8_t getDigit(uint8_t pos)
   {
      return(m_digitValues[pos]);
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

   void clearm_lamps()
   {
      m_lamps = 0;
      m_lampFlashMask = 0;
   }

   void lamp(uint8_t lamp, bool on)
   {
      uint16_t mask = _BV((lamp % 10));
      if ( on ) 
         m_lamps |= mask;
      else
         m_lamps &= ~(mask);
   };

   uint8_t getm_lamps()
   {
      return(m_lamps);
   };

   void flashm_lamps(uint16_t m_lamps, bool on)
   {
      if ( on ) 
         m_lampFlashMask |= m_lamps;
      else
         m_lampFlashMask &= ~m_lamps;
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
      m_enableIdleTime = val;
      enableCounter = 0;
   };

   uint16_t getNextDigit()
   {
      return(m_nextDigit);
   };

   static void *exec(void*);

   uint8_t  digitFlashMask;
   uint8_t  digitBlankMask;
   int16_t  flashCounter;
   uint16_t enableCounter;

protected:
   void loop();
//private:
public:
   uint8_t  m_digitValues[6];
   uint16_t m_lamps;

   uint16_t m_lampFlashMask;

   uint16_t m_enableIdleTime;
   uint16_t m_speed;
   uint8_t  m_nextDigit;
   uint8_t  m_loopCnt;
   bool     m_enabled;
};

extern Display* display;

# endif



