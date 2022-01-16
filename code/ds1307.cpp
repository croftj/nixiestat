#define  DISPLAY_OFF

# include <avr/pgmspace.h>

#ifndef DISPLAY_OFF
# include "display.h"
#endif

# include "ds1307.h"
# include "core/Wire.h"
# include "core/twi.h"
# include "core/wiring.h"

static char ibuf[16];

/**********************************************************************/
/*   Convert a byte from binary to bcd                                */
/**********************************************************************/
uint8_t DS1307::decToBcd(uint8_t val)
{
   return ( (val/10*16) + (val%10) );
}

/**********************************************************************/
/*   Convert bcd do a byte of binary                                  */
/**********************************************************************/
uint8_t DS1307::bcdToDec(uint8_t val)
{
//   Serial << "DS1307::bcdToDec(): val = 0x" << itoa(val, ibuf, 16)  << endl;
   return ( (val/16*10) + (val%16) );
}

void DS1307::readTime(uint8_t *sec,
                      uint8_t *min,
                      uint8_t *hour,
                      uint8_t *day,
                      uint8_t *dow,
                      uint8_t *month,
                      uint16_t *year)
{
#ifndef DISPLAY_OFF
   display.disable();
#endif
//   PORTD |= _BV(6);
   Wire.begin();
   Wire.enable();
//   PORTD &= ~(_BV(6));
   // Reset the register pointer
   for (int x = 3; x  > 0; x--) 
   {
//      PORTD |= _BV(6);
//      twi_init();
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.send(0x00);
      Wire.endTransmission();

      Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

      // A few of these need masks because certain bits are control bits
      *sec    = bcdToDec(Wire.receive() & 0x7f);
//      Serial << "sec " << (uint16_t)*sec << endl;
      *min    = bcdToDec(Wire.receive());
//      Serial << "min " << (uint16_t)*min << endl;
      *hour   = bcdToDec(Wire.receive() & 0x3f);  // Need to change this if 12 hour am/pm
//      Serial << "hour " << (uint16_t)*hour << endl;
      *dow    = bcdToDec(Wire.receive());
//      Serial << "dow " << (uint16_t)*dow << endl;
      *day    = bcdToDec(Wire.receive());
//      Serial << "day " << (uint16_t)*day << endl;
      *month  = bcdToDec(Wire.receive()) - 1;
//      Serial << "month " << (uint16_t)*month << endl;
      *year   = bcdToDec(Wire.receive()) + 1900;
//      Serial << "year " << (uint16_t)*year << endl;
      if ( *day != 0 || *month < 12 ) 
         break;
      delay(50);
//      PORTD &= ~(_BV(6));
   }
   Wire.disable();
//   display.touch();
}

void DS1307::setTime(uint8_t sec,
                     uint8_t min,
                     uint8_t hour,
                     uint8_t day,
                     uint8_t dow,
                     uint8_t month,
                     uint16_t year)
{
   month++;
   year -= 1900;

//#ifdef FAT
//   Serial << "setTime(): ";
//   Serial << (int)year << "/";
//   Serial << (int)month << "/";
//   Serial << (int)day << " ";
//   Serial << (int)dow << " ";
//   Serial << (int)hour << ":";
//   Serial << (int)min << ":";
//   Serial << (int)sec << endl;
//#endif
#ifndef DISPLAY_OFF
   display.disable();
#endif
   Wire.begin();
   Wire.enable();
   Wire.beginTransmission(DS1307_I2C_ADDRESS);
   Wire.send(0x00);
   Wire.send(decToBcd(sec) & 0x7f);    // 0 to bit 7 starts the clock
   Wire.send(decToBcd(min));
   Wire.send(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                    // bit 6 (also need to change readDateDs1307)
   Wire.send(decToBcd(dow));
   Wire.send(decToBcd(day));
   Wire.send(decToBcd(month));
   Wire.send(decToBcd(year));
   Wire.send(decToBcd(0x10));
   Wire.endTransmission();
   Wire.disable();
//   display.touch();
}

