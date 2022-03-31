# include "display.h"
# include "IOPorts.h"

#include <thread>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "DBGPorts.h"

#define  TOUCH_ENABLED  0

#define  FLASH_VALUE    60

#define TAG __PRETTY_FUNCTION__

Display* display = new Display;

Display::Display()
{
   m_enableIdleTime = 30000;
   flashCounter = 0;
   m_nextDigit = 0;
   m_speed = 100;
   digitBlankMask = 0;
   digitFlashMask = 0;

   for (uint8_t x = 0; x < 6; x++) 
   {
      m_digitValues[x] = x;
   }
   m_lamps = 0xa5;
   m_enabled = true;
   enableCounter = 0;
   m_loopCnt = 0;
}

void Display::init()
{
}

void Display::exec(void*)
{
#if 1
   std::this_thread::sleep_for(std::chrono::seconds(2));
   Digits[0]->off();
   Digits[1]->off();
   Digits[2]->off();
   Digits[3]->off();
   Digits[5]->off();
   Digits[6]->off();
   Segments[0]->off();
   Segments[1]->off();
   Segments[2]->off();
   Segments[3]->off();
   Segments[4]->off();
   Segments[5]->on();
   Segments[6]->off();
   Segments[7]->off();
   Segments[8]->off();
   Segments[9]->off();
#endif
   ESP_LOGI(TAG, "portTICK_PERIOD_MS = %d", portTICK_PERIOD_MS);
   while (true)
   {
      display->loop();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }
   return;
}

void Display::loop()
{
   bool showDigit;
//   ESP_LOGI(TAG, "Enter");
#if 1
   if ( false || ! m_enabled ) 
   {
      m_loopCnt++;
      if ( m_loopCnt >= 3 ) 
      {
         m_loopCnt = 0;
         m_nextDigit++;
         if ( m_nextDigit >= 7 ) 
         {
            m_nextDigit = 0;
         }
      }
   }
//   if ( ++enableCounter > m_enableIdleTime) 
//   {
//      disable();
//      return;
//   }

   if ( m_loopCnt >= 2 ) 
   {
//      ESP_LOGI(TAG, "All off");
      /***********************************************/
      /*   Start  things  by turning off all of the  */
      /*   m_lamps and digits.                         */
      /***********************************************/
      for (int x = 0; x < 10; x++) 
      {
         Segments[x]->off();
         if ( x < 7 ) 
         {
            Digits[x]->off();
         }
      }
      m_loopCnt = 0;
      return;
   }
   else if ( m_loopCnt != 0 )
   {
//      ESP_LOGI(TAG, "display nop");
      m_loopCnt++;
      return;
   } 
   m_loopCnt++;

#if 1
   /***********************************************/
   /*   Start by figuring out if the flash state  */
   /*   is on or off then                         */
   /***********************************************/
   flashCounter--;
   if ( flashCounter < (0 - FLASH_VALUE) )
   {
      flashCounter = FLASH_VALUE;
   }
   if ( flashCounter < 0 ) 
   {
//         PORTB &= ~_BV(0);
//      ESP_LOGI(TAG, "hide digit");
      showDigit = false;
   }
   else
   {
//         PORTB |= _BV(0);
//      ESP_LOGI(TAG, "show digit");
      showDigit = true;
   }
#else
   showDigit = true;
#endif

   /***********************************************/
   /*   Here we deal with digits                  */
   /***********************************************/
   if ( m_nextDigit < 6 ) 
   {
//      PORTB &= ~_BV(2);
      /***********************************************/
      /*   Now  be  sure  to  not  turn on any that  */
      /*   should be blanked                         */
      /***********************************************/
      if ( ! (digitBlankMask & _BV(m_nextDigit)) ) 
      {
         if ( ! (digitFlashMask & _BV(m_nextDigit)) && showDigit == false) 
            showDigit = true;
      }
      else
         showDigit = false;

      /***********************************************/
      /*   If  a digit should be shown, turn on the  */
      /*   appropriate pins to do the deed           */
      /***********************************************/

//      ESP_LOGI(TAG, "showDigit = %d", showDigit);
      if ( showDigit ) 
      {
//         ESP_LOGI(TAG, "%d: %d", (int)m_nextDigit, (int)m_digitValues[m_nextDigit]);
         /***************************************************/
         /* leave blank any digit that has a number greater */
         /* than 9                                          */
         /***************************************************/
         if (m_digitValues[m_nextDigit] <= 9)
         {
            Digits[m_nextDigit]->on();
            Segments[m_digitValues[m_nextDigit]]->on();
         }
      }
      m_nextDigit++;
   }
   else if ( m_nextDigit < 7)
   {
      uint16_t l = m_lamps;

      if ( ! showDigit ) 
      {
//         ESP_LOGI(TAG, "l = 0x%0x, lampFlashMask = 0x%0x", l, m_lampFlashMask);
         l &= ~m_lampFlashMask;
      }
      Digits[m_nextDigit]->on();

      for (int x = 0; x < 10; x++ ) 
      {
         if ( l & 0x01 ) 
         {
            Segments[x]->on();
         }
         else
         {
            Segments[x]->off();
         }
         l = l >> 1;
      }
      m_nextDigit++;
   }
   else
   {
//      PORTB |= _BV(2);
      m_nextDigit = 0;
//      switches.scan();   
//      PORTB &= ~(_BV(2));
   }
#endif
}

void Display::digitsOff()
{
   ESP_LOGI(TAG, "Turning all digits off");
   for (int x = 0; x < IOPorts::NUM_DIGITS; x++)
   {
      Digits[x]->off();
   }
}

void Display::digitOn(int digit)
{
   ESP_LOGI(TAG, "Turning on digit %d", digit);
   if (digit < IOPorts::NUM_DIGITS)
   {
      digitsOff();
      Digits[digit]->on();
   }
}

void Display::segmentsOff()
{
   ESP_LOGI(TAG, "Turning all segments off");
   for (int x = 0; x < IOPorts::NUM_SEGMENTS; x++)
   {
      Segments[x]->off();
   }
}

void Display::segmentOn(int segment)
{
   if (segment < IOPorts::NUM_SEGMENTS)
   {
      segmentsOff();
      ESP_LOGI(TAG, "Turning on segment %d", segment);
      Segments[segment]->on();
   }
}

bool Display::disable()
{
   bool rv = m_enabled;
   ESP_LOGW(TAG, "Turning off HV");
   HVEnable->on();
   m_enabled = false;
   enableCounter = 0;
   return(rv);
};

void Display::touch()
{
   if ( ! m_enabled ) 
   {
      m_loopCnt = 0;
      m_nextDigit = 0;
   }

   enableCounter = 0;
//   Serial << __PRETTY_FUNCTION__ << " here" << endl;
   HVEnable->off();
   m_enabled = true;
};


