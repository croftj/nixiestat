#ifndef RTC_H
#define RTC_H

# include "datetime.h"
# include "Wire.h"

#define DS1307_I2C_ADDRESS 0x68 

class DS1307 
{
public:
   DS1307()
   {
   };

   /**********************************************************************/
   /*   Convert a byte from binary to bcd                                */
   /**********************************************************************/
   byte decToBcd(byte val)
   {
     return ( (val/10*16) + (val%10) );
   }

   /**********************************************************************/
   /*   Convert bcd do a byte of binary                                  */
   /**********************************************************************/
   byte bcdToDec(byte val)
   {
     return ( (val/16*10) + (val%16) );
   }

   time_t readTime()
   {
      byte sec, min, hour, day, dow, month, year;
      time_t rv;

      Wire.enable();
      // Reset the register pointer
      for (int x = 3; x  > 0; x--) 
      {
         Wire.beginTransmission(DS1307_I2C_ADDRESS);
         Wire.send(0x00);
         Wire.endTransmission();

         Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

         // A few of these need masks because certain bits are control bits
         sec    = bcdToDec(Wire.receive() & 0x7f);
         min    = bcdToDec(Wire.receive());
         hour   = bcdToDec(Wire.receive() & 0x3f);  // Need to change this if 12 hour am/pm
         dow    = bcdToDec(Wire.receive());
         day    = bcdToDec(Wire.receive());
         month  = bcdToDec(Wire.receive());
         year   = bcdToDec(Wire.receive());
         int iyear = year + 1900;
# if 0
         Serial << "readTime(): ";
         Serial << (int)iyear << "/";
         Serial << (int)month << "/";
         Serial << (int)day << " ";
         Serial << (int)dow << " ";
         Serial << (int)hour << ":";
         Serial << (int)min << ":";
         Serial << (int)sec << endl;
# endif
         if ( day == 0 || month > 12 ) 
            rv = 3600;
         else 
         {
            month--;
            rv = DateTime.makeTime(sec, min, hour, day, month, iyear);
            break;
         }
         delay(50);
      }
      Wire.disable();
      return(rv);
   };

   void setTime(time_t time)
   {
      byte sec, min, hour, day, dow, month, year;
      DateTime.localTime(&time, &sec, &min, &hour, &day, &dow, &month, &year);

      month++;
# if FAT
      Serial << "setTime(): ";
      Serial << (int)year << "/";
      Serial << (int)month << "/";
      Serial << (int)day << " ";
      Serial << (int)dow << " ";
      Serial << (int)hour << ":";
      Serial << (int)min << ":";
      Serial << (int)sec << endl;
#endif
      Wire.enable();
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.send(0x00);
      Wire.send(decToBcd(sec));    // 0 to bit 7 starts the clock
      Wire.send(decToBcd(min));
      Wire.send(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                      // bit 6 (also need to change readDateDs1307)
      Wire.send(decToBcd(dow));
      Wire.send(decToBcd(day));
      Wire.send(decToBcd(month));
      Wire.send(decToBcd(year));
      Wire.endTransmission();
      Wire.disable();
   };

protected:
private:
};

#endif
