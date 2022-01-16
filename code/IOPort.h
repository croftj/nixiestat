#ifndef IOPORT_H
#define IOPORT_H

# include <avr/common.h>
# include <avr/pgmspace.h>
# include <stdint.h>
# include <stdio.h>
# include <avr/io.h>

class IOPort
{
public:
   enum 
   {
      PORT_A,
      PORT_B,
      PORT_C,
      PORT_D,
      NUM_PORTS   // Must remain last in the list
   };

   IOPort()
   {
   }

   static void confPortDDR()
   {
      for (uint8_t x = 0; x < NUM_PORTS; x++) 
      {
         *m_ddregs[x] = m_ddr[x];
      }
   }

   void on()
   {
      setPort(m_port, m_mask);
   }

   void off()
   {
      clearPort(m_port, m_mask);
   }

   void set(uint8_t value)
   {
      value &= m_mask;
      uint8_t p = readPort(m_port);
      p &= ~m_mask;
      p |= value;
//      Serial << __FUNCTION__ << " port: " << (int)m_port << ", m_portregs: " << (int) m_portregs[m_port] << endl;
      *m_portregs[m_port] = p;
   }

   uint8_t read()
   {
      return(readPort(m_port) & m_mask);
   }

   void setPin(uint8_t port, uint8_t pin)
   {
      m_pins[port] |= _BV(pin);
//      char ibuf[8];
//      Serial << __FUNCTION__ << " port: " << (int)port << ", m_pins: 0x" << itoa(m_pins[port], ibuf, 16) << endl;
      *m_portregs[port] = m_pins[port];
   }

   void setPort(uint8_t port, uint8_t pins)
   {
//      char ibuf[8];
      m_pins[port] |= pins;
//      Serial << __FUNCTION__ << " port: " << (int)port << ", m_pins: 0x" << itoa(m_pins[port], ibuf, 16) << endl;
      *m_portregs[port] = m_pins[port];
   }

   void clearPin(uint8_t port, uint8_t pin)
   {
      m_pins[port] &= ~(_BV(pin));
//      char ibuf[8];
//      Serial << __FUNCTION__ << " port: " << (int)port << ", m_pins: 0x" << itoa(m_pins[port], ibuf, 16) << endl;
      *m_portregs[port] = m_pins[port];
   }

   void clearPort(uint8_t port, uint8_t pins)
   {
      m_pins[port] &= ~pins;
//      char ibuf[8];
//      Serial << __FUNCTION__ << " port: " << (int)port << ", m_pins: 0x" << itoa(m_pins[port], ibuf, 16) << endl;
      *m_portregs[port] = m_pins[port];
   }

   bool readPin(uint8_t port, uint8_t pin)
   {
      return(*m_pinregs[port] & _BV(pin));
   }

   uint8_t readPort(uint8_t port)
   {
      return(*m_pinregs[port]);
   }

   void setPortDDR(uint8_t port, uint8_t ddr)
   {
      m_ddr[port] |= ddr;
//      Serial << __FUNCTION__ << " port: " << (int)port << ", m_ddr: " << (int) m_ddr[port] << endl;
      *m_ddregs[port] = m_ddr[port];
   }

   void setPinDDR(uint8_t port, uint8_t pin, bool ddr)
   {
      if ( ddr ) 
      {
         m_ddr[port] |= _BV(pin);
      }
      else 
      {
         m_ddr[port] &= ~(_BV(pin));
      }
//      Serial << __FUNCTION__ << " port: " << (int)port << ", m_ddr: " << (int) m_ddr[port] << endl;
      *m_ddregs[port] = m_ddr[port];
   }

   void clearPortDDR(uint8_t port, uint8_t ddr)
   {
      m_ddr[port] &= ~(ddr);
      *m_ddregs[port] = m_ddr[port];
   }

   uint8_t                          m_port;
   uint8_t                          m_mask;

private:
   static uint8_t                   m_ddr[NUM_PORTS];
   static uint8_t                   m_pins[NUM_PORTS];
   static volatile uint8_t*         m_portregs[NUM_PORTS];
   static volatile uint8_t*         m_ddregs[NUM_PORTS];
   static volatile uint8_t*         m_pinregs[NUM_PORTS];
};

class OutputPort : public IOPort
{
public:
   void SetPort(uint8_t port, uint8_t mask)
   {
//      Serial << __FUNCTION__ << " port: " << (int)port << ", mask: " << (int)mask << endl;
      m_port = port;
      m_mask = mask;
      setPortDDR(port, m_mask);
   }

private:
protected:
};

class InputPort : public IOPort
{
public:
   void SetPort(uint8_t port, uint8_t mask)
   {
//      Serial << __FUNCTION__ << " port: " << (int)port << ", mask: " << (int)mask << endl;
      m_port = port;
      m_mask = mask;
      clearPortDDR(port, m_mask);
   }

private:
protected:
};

#endif

