# include "display.h"
# include "TempSensors.h"
# include "HardwareSerial.h"
# include "WProgram.h"
# include "IOPort.h"

//#define  DBG
//#define  DBG1
//#define  DBG2
//#define  DBG3

static char p_buf[80];

//OneWire t_sens1(17);
//OneWire t_sens2(18);
//OneWire t_sens3(19);
//OneWire t_sens4(20);

OutputPort t_sense_port;

OneWire t_port1(4);
OneWire t_port2(5);
OneWire t_port3(6);

TempSensors::TempSensors() : 
   m_lastSensorCycled(0)
{
}

void TempSensors::initialize()
{
   t_sense_port.SetPort(IOPort::PORT_D, 0x70);
   t_sense_port.on();

   temp_ports[0] = &t_port1;
   temp_ports[1] = &t_port2;
   temp_ports[2] = &t_port3;
//   temp_ports[3] = &t_port4;

//   Serial << "t_init NUM_TEMP_PORTS = " << NUM_TEMP_PORTS << endl;
   discover();
   Serial << "Discovered devices:" << endl;
   for (uint8_t x = 0; x < NUM_SENSORS; x++) 
   {
      if ( devices[x].iop != NULL ) 
      {
         char tbuf[9];
         memset(tbuf, '\0', sizeof(tbuf));
         memcpy(tbuf, devices[x].addr, 8);
         Serial << "dev[ " << (int)x + 1 << "]- addr: ";
         printAddr(devices[x].addr);
         Serial << endl;
      }
      else 
      {
         break;
      }
   }
   if ( m_deviceCount > 0 ) 
   {
      startCycle(&devices[0]);
   }
   clearTotals();
}

float TempSensors::read(uint8_t sensor, SensorReading_t reading, bool celsius)
{
   float rv = 0.0;

   char type[16];

   if ( sensor > 0 ) 
   {
      sensor--; // Make the sensore 0 based.
      switch (reading) 
      {
         case MINIMUM:
            rv = minimum[sensor];
            strcpy_P(type, PSTR(" Minimum "));
            break;

         case MAXIMUM:
            rv = maximum[sensor];
            strcpy_P(type, PSTR(" Maximum "));
            break;

         case AVERAGE:
            rv = average[sensor];
            strcpy_P(type, PSTR(" Average "));
            break;

         case CURRENT:
         default:
            rv = temperatures[sensor];
            strcpy_P(type, PSTR(" Current "));
            break;
      }
   }

   if ( ! celsius ) 
   {
      rv = (rv * (9.0/5.0)) + 32.0;
   }
# ifdef DBG3
      Serial << "Request" << type << "temp[" << (int)sensor << "]:"  << rv << endl;
# endif
   return(rv);
}

void TempSensors::readNextSensor()
{
   bool err;

   OneWireDevice *device = &devices[m_lastSensorCycled];
   if ( device->iop != NULL ) 
   {
# ifdef DBG1
      Serial << __PRETTY_FUNCTION__ << "t_read pointer = " << (long)device->iop << ", addr = ";
      printAddr(device->addr);
      Serial << endl;
# endif
      float t = readSensor(device, &err);

      /***********************************************/
      /*   If   we   have   no   errors   keep  the  */
      /*   temperature.  It  looks like the sensors  */
      /*   will  return  185.000 with a good crc is  */
      /*   they  are unhappy about something, maybe  */
      /*   it  is  from  the  senor  getting  a bad  */
      /*   command from the noise.                   */
      /***********************************************/
      if ( ! err) 
      {
         if ( t < 84.0) 
         {
//            Serial << "Saving temp (" << t << ") from sensor " << (int)m_lastSensorCycled << endl;
            totals_count[m_lastSensorCycled]++;
            temperatures[m_lastSensorCycled] = t;
            if ( minimum[m_lastSensorCycled] > temperatures[m_lastSensorCycled] ) 
            {
               minimum[m_lastSensorCycled] = temperatures[m_lastSensorCycled];
            }
            if ( maximum[m_lastSensorCycled] < temperatures[m_lastSensorCycled] ) 
            {
               maximum[m_lastSensorCycled] = temperatures[m_lastSensorCycled];
            }
            sum[m_lastSensorCycled] += temperatures[m_lastSensorCycled];
            average[m_lastSensorCycled] = sum[m_lastSensorCycled] / totals_count[m_lastSensorCycled];
         }
//         else 
//         {
//            Serial << "Invalid temp (" << t << ") from sensor " << (int)m_lastSensorCycled << " Skipping" << endl;
//         }
      }

      // Otherwise, skip this temp.
//      else 
//      {
//         Serial << "crc err from sensor " << (int)m_lastSensorCycled << " Skipping" << endl;
//      }
   }
   m_lastSensorCycled++;
   if ( devices[m_lastSensorCycled].iop  == NULL ) 
   {
      m_lastSensorCycled = 0;
   }
//   Serial << "Starting cycle for sensor " << (int)m_lastSensorCycled << endl;
   startCycle(&devices[m_lastSensorCycled]);
}

void TempSensors::readSensors()
{
   for (uint8_t x = 0; x < NUM_SENSORS; x++) 
   {
      if ( devices[x].iop != 0 ) 
      {
# ifdef DBG2
         Serial << __PRETTY_FUNCTION__ << "t_read x = " << (int)x << ", pointer = " << (long)devices[x].iop << ", addr = " << endl;
         printAddr(devices[x].addr);
         Serial << endl;
# endif
//         delay(200);
         /***********************************************/
         /*   Save  the  minimum,  maximum and average  */
         /*   value for each sensor                     */
         /***********************************************/
         totals_count[x]++;
         temperatures[x] = readSensor(&devices[x]);
         if ( minimum[x] > temperatures[x] ) 
         {
            minimum[x] = temperatures[x];
         }
         if ( maximum[x] < temperatures[x] ) 
         {
            maximum[x] = temperatures[x];
         }
         sum[x] += temperatures[x];
         average[x] = sum[x] / totals_count[x];
      }
   }
}

uint8_t* TempSensors::deviceAddress(uint8_t device)
{
   uint8_t* rv;
   device = device % NUM_SENSORS;
   rv = devices[device].addr;
   return(rv);
}

float TempSensors::readSensor(OneWireDevice *device, bool *err)
{
   uint8_t data[12];

   if ( device == 0 || device->iop == 0) 
   {
      return(0.0);
   }
   OneWire* ts = device->iop;
# ifdef DBG1
   Serial << __PRETTY_FUNCTION__ << "read temp pointer = " << (long)devices->iop << ", addr = ";
   printAddr(device->addr);
   Serial << endl;
# endif

   uint8_t present = ts->reset();
   ts->select(device->addr);    
   ts->write(0xBE);           // Read Scratchpad

# ifdef DBG
   Serial << "rs.P=" << (int) present << " ";
# endif

   for ( uint8_t i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ts->read();
# ifdef DBG
      Serial.print(data[i], HEX);
      Serial.print(" ");
# endif
   }
//   ts->reset();           // Read Scratchpad

# ifdef DBG
   Serial.print(" rs.CRC=");
   Serial.print( OneWire::crc8( data, 8), HEX);
   Serial.println();
# endif

   if ( OneWire::crc8(data, 8) != data[8] ) 
   {
      *err = true;
   }
   else 
   {
      *err = false;
   }
   uint8_t msb = data[1];
   uint8_t lsb = data[0];
   int16_t value = (msb << 8) + lsb;
   float temp = value * 0.0625;

# ifdef DBG
   Serial.print("msb = 0x");
   Serial.print( msb, HEX);
   Serial.print(", lsb = 0x");
   Serial.print( lsb, HEX);
   Serial.print(", value = 0x");
   Serial.print( value, HEX);
   Serial << "msb: " << (uint16_t)msb << endl;
   Serial << "lsb: " << (uint16_t)lsb << endl;
   Serial << "val: " << value << endl;
# endif
# ifdef DBG1
   Serial << "temp: " << temp << endl;
# endif
   return(temp);
}

int TempSensors::discover()
{
   OneWire *t_temp_ports[3] = 
   { 
      &t_port1 , &t_port2, &t_port3
   };

   uint8_t device_cnt = 0;
   uint8_t sensor_cnt = 0;

   byte data[12];
   byte addr[8];
   bool rv = false;


   for (sensor_cnt; device_cnt < NUM_SENSORS && sensor_cnt < 3; sensor_cnt++) 
   {
# ifdef DBG1
      Serial << "discovering port " << (int)sensor_cnt << endl;
# endif
      OneWire *ts = t_temp_ports[sensor_cnt];
      if ( ts != 0 ) 
      {
         int i;
         while ( ts->search(addr) ) 
         {
# ifdef DBG1
            Serial << "sensor found sensor_cnt: " << (int)sensor_cnt << endl;
            Serial.print("sc.R=");
            for( i = 0; i < 8; i++) {
               Serial.print(addr[i], HEX);
               Serial.print(" ");
            }
            Serial << endl;
# endif

            if ( OneWire::crc8(addr, 7) != addr[7]) {
               continue;
            }
            
            if ( addr[0] != 0x28) {
               continue;
            }
            else 
            {
               devices[device_cnt].initialize(ts, addr);
               device_cnt++;
            }
         }
         ts->reset_search();
      }
   }
   m_deviceCount = device_cnt;
# ifdef DBG1
   Serial << "Device count = " << (int)device_cnt << endl; 
#endif
   return(device_cnt);
}

void TempSensors::startCycle()
{
//   Serial << __PRETTY_FUNCTION__  << endl;
   for (uint8_t x = 0; x < NUM_SENSORS; x++) 
   {
      if ( devices[x].iop != 0 ) 
      {
# ifdef DBG1
         Serial << "t_start x = " << (int)x << ", pointer = " << (long)devices[x].iop << " ";
         printAddr(devices[x].addr);
         Serial << endl;
#endif
         startCycle(&devices[x]);
         delay(1000);
      }
//      Serial << endl;
   }
}

void TempSensors::startCycle(OneWireDevice *device)
{
   byte data[12];
   OneWire *ts = device->iop;
   ts->reset();
   ts->select(device->addr);
   ts->write(0x44,1);         // start conversion, with parasite power on at the end
}


bool TempSensors::validSensor(uint8_t sensor)
{
   return(devices[sensor % NUM_SENSORS].iop != 0);
}

void TempSensors::printAddr(uint8_t* addr)
{
   Serial.print("sc.R=");
   for( uint8_t i = 0; i < 8; i++) {
      Serial.print(addr[i], HEX);
      Serial.print(" ");
   }
}

void TempSensors::dumpSensors()
{
   for (uint8_t x = 0; x < NUM_SENSORS; x++) 
   {
      if ( devices[x].iop != NULL ) 
      {
         char tbuf[9];
         Serial << strcpy_P(p_buf, PSTR("sensor [ ")) << (int)x + 1;
         Serial << strcpy_P(p_buf, PSTR("]- addr: "));
         printAddr(devices[x].addr);
         Serial << strcpy_P(p_buf, PSTR(", temperature: ")) << read(x + 1, CURRENT);
         Serial << strcpy_P(p_buf, PSTR(", minimum: "))     << read(x + 1, MINIMUM);
         Serial << strcpy_P(p_buf, PSTR(", maximum: "))     << read(x + 1, MAXIMUM);
         Serial << strcpy_P(p_buf, PSTR(", average: "))     << read(x + 1, AVERAGE);
         Serial << strcpy_P(p_buf, PSTR("\r\n"));
      }
      else 
      {
         break;
      }
   }
}
