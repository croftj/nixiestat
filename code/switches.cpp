# include <avr/pgmspace.h>

# include "WProgram.h"
# include "switches.h"

Switches switches;

#define  SW_BUFFER_SIZE 16
#define  SW_DEBOUNCE    5
#define  SW_PAUSE       50
#define  SW_REPEAT      40

struct ring_buffer {
  unsigned char buffer[SW_BUFFER_SIZE];
  int head;
  int tail;
};

ring_buffer swBuf = { { 0 }, 0, 0 };

static inline void put_char(unsigned char c)
{
   int i = (swBuf.head + 1) % SW_BUFFER_SIZE;

   // if we should be storing the received character into the location
   // just before the tail (meaning that the head would advance to the
   // current location of the tail), we're about to overflow the buffer
   // and so we don't write the character or advance the head.
   if ( i != swBuf.tail ) {
//      Serial << "put_char(" << (int)c << ")" <<endl;
//      Serial << "put_char(): head: " << (int)swBuf.head  << ", i: " << (int)i << endl;

      swBuf.buffer[swBuf.head] = c;
      swBuf.head = i;
   }
}

static inline unsigned char get_char(void)
{
   // if the head isn't ahead of the tail, we don't have any characters
   unsigned char c = 0;
//   noInterrupts();
   if (swBuf.head != swBuf.tail)
   {
//      Serial << "get_char(): head: " << (int)swBuf.head  << ", tail: " << (int)swBuf.tail << endl;
      c = swBuf.buffer[swBuf.tail];
      swBuf.tail = (swBuf.tail + 1) % SW_BUFFER_SIZE;
   }
//   interrupts();
   return c;
}

Switches::Switches()
{
   for (uint8_t x = 0; x < 8; x++) 
      sw_count[x] = 0;

   memset(&swBuf, '\0', sizeof(swBuf));
}

void Switches::scan()
{
   uint8_t switch_state;

   uint8_t porta = PORTA;

   DDRA = 0x80;
   DDRB |= _BV(3);
   PORTB |= _BV(3);
   PORTC = 0x00;
   PORTD &= ~_BV(7);
//   PORTA = 0;
   uint32_t y;
//   delay(10);
   sw_state = PINA;
   PORTB &= ~_BV(3);
   DDRB &= ~_BV(3);
   DDRA = 0xff;
   PORTA = porta;
//   uint8_t sw_state = 0;
      
   for (uint8_t x = 0; x < 6; x++) 
   {
      if ( sw_state & 0x01 ) 
      {
         PORTB |= _BV(0);
         sw_count[x]++;
         PORTB &= ~_BV(0);
      }
      else 
      {
         sw_count[x] = 0;
      }
      sw_state = sw_state >> 1;
   }
   for (uint8_t x = 0; x < 6; x++) 
   {
      if ( sw_count[x] == SW_DEBOUNCE ) 
      {
         PORTB |= _BV(1);
         put_char(x + 1);         
         PORTB &= ~_BV(1);
      }
      else if ( sw_count[x] > SW_PAUSE && (sw_count[x] % SW_REPEAT == 0) )
      {
         PORTB |= _BV(2);
         put_char(x + 1);         
         PORTB &= ~_BV(2);
      }
   }
}

bool Switches::haveButton()
{
   return(swBuf.head != swBuf.tail);
}

char Switches::getButton()
{
   char rv = get_char();
   if ( rv == 0 ) 
     rv = -1;
   return(rv);
}
