#ifndef _IO_PORTS_H
#define _IO_PORTS_H

#include "MAX7320.h"

namespace IOPorts
{
   enum 
   {
      SEGS_LSB = 0x56,
      SEGS_MSB = 0x57,
      RELAYS   = 0x57,
      DIGITS   = 0x58
   };
}

namespace IOMasks
{
   enum
   {
      HV_OFF   = _BV(2),
      HEAT     = _BV(4),
      COOL     = _BV(5),
      FAN      = _BV(6),
      VALVE    = _BV(7),
   };
};

extern MAX7320* Heat;
extern MAX7320* Cool;
extern MAX7320* Fan;
extern MAX7320* Valve;
extern MAX7320* HVEnable;

#if 0
extern MAX7320* Digits[];
extern MAX7320* Segments[];
#endif
#endif
