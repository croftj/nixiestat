# include "display.h"
# include "IOPorts.h"

#include <thread>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define  TOUCH_ENABLED  0

#define  FLASH_VALUE    90

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
   m_lamps = 0x00;
   m_enabled = true;
   enableCounter = 0;
   m_loopCnt = 0;
}

void Display::init()
{
}

void *Display::exec(void*)
{
#if 0
   std::this_thread::sleep_for(std::chrono::seconds(2));
   Digits[0]->off();
   Digits[1]->off();
   Digits[2]->on();
   Digits[3]->off();
   Digits[5]->off();
   Digits[6]->off();
   Segments[0]->off();
   Segments[1]->off();
   Segments[2]->on();
   Segments[3]->off();
   Segments[4]->off();
   Segments[5]->off();
   Segments[6]->off();
   Segments[7]->off();
   Segments[8]->off();
   Segments[9]->off();
#endif
   while (true)
   {
      display->loop();
      std::this_thread::sleep_for(std::chrono::milliseconds(600));
   }
   return(NULL);
}

void Display::loop()
{
   bool showDigit;
#if 0
   if ( true || ! m_enabled ) 
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
      return;
   }
//   if ( ++enableCounter > m_enableIdleTime) 
//   {
//      disable();
//      return;
//   }

   if ( m_loopCnt >= 3 ) 
   {
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
      m_loopCnt++;
      return;
   } 
   m_loopCnt++;

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
      showDigit = false;
   }
   else
   {
//         PORTB |= _BV(0);
      showDigit = true;
   }

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

      if ( showDigit ) 
      {
//         Serial << (int)m_nextDigit << ":" << (int)m_digitValues[m_nextDigit] << endl;
         Digits[m_nextDigit]->on();
         Segments[m_digitValues[m_nextDigit]]->on();
      }
      m_nextDigit++;
   }
   else if ( m_nextDigit < 7)
   {
      uint16_t l = m_lamps;
      if ( ! showDigit ) 
         l &= ~m_lampFlashMask;
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

bool Display::disable()
{
   bool rv = m_enabled;
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


