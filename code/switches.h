#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdint.h>

class Switches
{
public:
   Switches();

   void scan();

   char getButton();
   bool haveButton();
   uint8_t sw_state;

protected:
private:
   uint8_t sw_count[8];
};

extern Switches switches;

#endif
