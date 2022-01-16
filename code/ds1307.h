#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>

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
   uint8_t decToBcd(uint8_t val);
   /**********************************************************************/
   /*   Convert bcd do a byte of binary                                  */
   /**********************************************************************/
   uint8_t bcdToDec(uint8_t val);
   void readTime(uint8_t *sec,
                 uint8_t *min,
                 uint8_t *hour,
                 uint8_t *day,
                 uint8_t *dow,
                 uint8_t *month,
                 uint16_t *year);
   void setTime(uint8_t sec,
                uint8_t min,
                uint8_t hour,
                uint8_t day,
                uint8_t dow,
                uint8_t month,
                uint16_t year);
protected:
private:
};

#endif
