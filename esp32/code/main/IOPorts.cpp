#include "IOPorts.h"

using namespace IOPorts;

MAX7320* Heat        = new MAX7320(I2CPort::OUTPUT, IOPorts::RELAYS, IOMasks::HEAT);
MAX7320* Cool        = new MAX7320(I2CPort::OUTPUT, IOPorts::RELAYS, IOMasks::COOL);
MAX7320* Fan         = new MAX7320(I2CPort::OUTPUT, IOPorts::RELAYS, IOMasks::FAN);
MAX7320* Valve       = new MAX7320(I2CPort::OUTPUT, IOPorts::RELAYS, IOMasks::VALVE);
MAX7320* HVEnable    = new MAX7320(I2CPort::OUTPUT, IOPorts::RELAYS, IOMasks::HV_OFF);

#if 1
MAX7320* Segments[] =
{
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(0)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(1)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(2)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(3)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(4)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(5)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(6)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_LSB, _BV(7)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_MSB, _BV(0)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::SEGS_MSB, _BV(1)),
   NULL
};

MAX7320* Digits[] =
{
   new MAX7320(I2CPort::OUTPUT, IOPorts::DIGITS, _BV(0)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::DIGITS, _BV(1)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::DIGITS, _BV(2)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::DIGITS, _BV(3)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::DIGITS, _BV(4)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::DIGITS, _BV(5)),
   new MAX7320(I2CPort::OUTPUT, IOPorts::DIGITS, _BV(6)),
   NULL
};
#endif
