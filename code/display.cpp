# include "display.h"
# include "IOPort.h"
# include "switches.h"

# include "esquare.h"

#define  TOUCH_ENABLED  1

#define  FLASH_VALUE    90

extern char ibuf[];

static bool dbg = 0;

Display display;

static OutputPort Segments[10];
static OutputPort Digits[7];
static OutputPort HvDisable;

SIGNAL(TIMER2_OVF_vect)
{
//   PORTB |= _BV(0);
   display.loop();   
//   PORTB &= ~_BV(0);
}

Display::Display()
{
   enableIdleTime = 30000;
   flashCounter = 0;
   nextDigit = 0;
   speed = 100;
   digitBlankMask = 0;
   digitFlashMask = 0;

   for (uint8_t x = 0; x < 6; x++) 
   {
      digitValues[x] = x;
   }
   lamps = 0x00;
   m_enabled = true;
   enableCounter = 0;
   loopCnt = 0;
}

void Display::init()
{
   Segments[0].SetPort(IOPort::PORT_B, _BV(6)); 
   Segments[1].SetPort(IOPort::PORT_B, _BV(7)); 
   Segments[2].SetPort(IOPort::PORT_C, _BV(2)); 
   Segments[3].SetPort(IOPort::PORT_C, _BV(3)); 
   Segments[4].SetPort(IOPort::PORT_C, _BV(4)); 
   Segments[5].SetPort(IOPort::PORT_C, _BV(5)); 
   Segments[6].SetPort(IOPort::PORT_C, _BV(6)); 
   Segments[7].SetPort(IOPort::PORT_C, _BV(7)); 
   Segments[8].SetPort(IOPort::PORT_D, _BV(7)); 
   Segments[9].SetPort(IOPort::PORT_A, _BV(7));

   Digits[0].SetPort(IOPort::PORT_A, _BV(0));
   Digits[1].SetPort(IOPort::PORT_A, _BV(1));
   Digits[2].SetPort(IOPort::PORT_A, _BV(2));
   Digits[3].SetPort(IOPort::PORT_A, _BV(3));
   Digits[4].SetPort(IOPort::PORT_A, _BV(4));
   Digits[5].SetPort(IOPort::PORT_A, _BV(5));
   Digits[6].SetPort(IOPort::PORT_A, _BV(6));

   HvDisable.SetPort(IOPort::PORT_B, _BV(4));
}

void Display::loop()
{
   bool showDigit;
   if ( ! m_enabled ) 
   {
      loopCnt++;
      if ( loopCnt >= 3 ) 
      {
         loopCnt = 0;
         nextDigit++;
         if ( nextDigit >= 7 ) 
         {
            nextDigit = 0;
         }
      }
      return;
   }
//   if ( ++enableCounter > enableIdleTime) 
//   {
//      disable();
//      return;
//   }

   if ( loopCnt >= 3 ) 
   {
      /***********************************************/
      /*   Start  things  by turning off all of the  */
      /*   lamps and digits.                         */
      /***********************************************/
      for (int x = 0; x < 10; x++) 
      {
         Segments[x].off();
         if ( x < 7 ) 
         {
            Digits[x].off();
         }
      }
      loopCnt = 0;
      return;
   }
   else if ( loopCnt != 0 )
   {
      loopCnt++;
      return;
   } 
   loopCnt++;

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
   if ( nextDigit < 6 ) 
   {
//      PORTB &= ~_BV(2);
      /***********************************************/
      /*   Now  be  sure  to  not  turn on any that  */
      /*   should be blanked                         */
      /***********************************************/
      if ( ! (digitBlankMask & _BV(nextDigit)) ) 
      {
         if ( ! (digitFlashMask & _BV(nextDigit)) && showDigit == false) 
            showDigit = true;
      }
      else
         showDigit = false;

      /***********************************************/
      /*   If  a digit should be shown, turn on the  */
      /*   appropriate pins to do the deed           */
      /***********************************************/

# if 0
      if ( dbg ) Serial << "nd = " << (int)nextDigit 
                        << ", dbm = " << (int)digitBlankMask
                        << ", dfm = " << (int)digitFlashMask
                        << ", sd = " << (int)showDigit
                        << endl;
# endif

      if ( showDigit ) 
      {
//         Serial << (int)nextDigit << ":" << (int)digitValues[nextDigit] << endl;
         if ( dbg ) Serial << "dv = 0x" << itoa((int)digitValues[nextDigit], ibuf, 16)
                           << ", nd = " << (int)nextDigit
                           << endl;
         Digits[nextDigit].on();
         Segments[digitValues[nextDigit]].on();
      }
      nextDigit++;
   }
   else if ( nextDigit < 7)
   {
      uint16_t l = lamps;
      if ( ! showDigit ) 
         l &= ~lampFlashMask;
      Digits[nextDigit].on();

      for (int x = 0; x < 10; x++ ) 
      {
         if ( l & 0x01 ) 
         {
            Segments[x].on();
         }
         else
         {
            Segments[x].off();
         }
         l = l >> 1;
      }
      nextDigit++;
   }
   else
   {
//      PORTB |= _BV(2);
      nextDigit = 0;
//      switches.scan();   
//      PORTB &= ~(_BV(2));
   }
}

bool Display::disable()
{
   bool rv = m_enabled;
   HvDisable.on();
   PORTC = 0x00;
   DDRC = 0x00;
   m_enabled = false;
   enableCounter = 0;
   return(rv);
};

void Display::touch()
{
   if ( ! m_enabled ) 
   {
      loopCnt = 0;
      nextDigit = 0;
   }

   enableCounter = 0;
//   Serial << __PRETTY_FUNCTION__ << " here" << endl;
   HvDisable.off();
   m_enabled = true;
};


