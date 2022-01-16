# include "IOPort.h"

uint8_t IOPort::m_ddr[NUM_PORTS];
uint8_t IOPort::m_pins[NUM_PORTS];

volatile uint8_t* IOPort::m_portregs[NUM_PORTS] = 
                     {
                        &PORTA,
                        &PORTB,
                        &PORTC,
                        &PORTD
                     };

volatile uint8_t* IOPort::m_ddregs[NUM_PORTS] =
                     {
                        &DDRA,
                        &DDRB,
                        &DDRC,
                        &DDRD
                     };

volatile uint8_t* IOPort::m_pinregs[NUM_PORTS] =
                     {
                        &PINA,
                        &PINB,
                        &PINC,
                        &PIND
                     };

