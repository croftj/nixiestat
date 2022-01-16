#include "SystemLogger.h"
# include <avr/pgmspace.h>

extern char *p_buf;

SystemLogger::SystemLogger() : m_nextChannel(0)
{
}

int8_t SystemLogger::registerChannel_P(const char *name, uint8_t type)
{
   uint8_t rv = -1;
   char tmp_name[128];
   strcpy_P(tmp_name, name);
   rv = registerChannel(tmp_name, type);
   return(rv); 
}
int8_t SystemLogger::registerChannel(char *name, uint8_t type)
{
   uint8_t rv = -1;
   if ( m_nextChannel < MAX_CHANNELS ) 
   {
      rv = m_nextChannel;
//      Serial << "name: " << strcpy_P(p_buf, name) << endl;
//      Serial << "name: " << name << endl;
      size_t name_len = strlen(name);
      char *cp = (char*)malloc(name_len + 1);
      strcpy(cp, name);
//      Serial << "cp: " << cp << endl;
      m_chanNames[m_nextChannel] = cp;
      m_chanTypes[m_nextChannel] = (uint8_t)type;
      Serial << "Registered log entry[" << (int)rv;
      Serial << "] name: " << (char*)m_chanNames[m_nextChannel];
//      Serial << "] name: " << strcpy_P(p_buf, m_chanNames[m_nextChannel]);
      Serial << " as type: " << (int)m_chanTypes[m_nextChannel];
      Serial << ", m_nextChannel = " << (int)m_nextChannel;
      Serial << endl;
      m_nextChannel++;
   }
   return(rv);
}


void SystemLogger::LogData(uint8_t channel, float value)
{
   if ( m_chanTypes[channel] == TEMPERATURE ) 
   {
      SaveTemperature(channel, value);
   }
   else if ( m_chanTypes[channel] == FLOATING_POINT ) 
   {
      SaveFPValue(channel, value);
   }
   else if ( m_chanTypes[channel] == UNSIGNED_INT ) 
   {
      SaveUIntValue(channel, (uint16_t)value);
   }
}

void SystemLogger::LogData(char *name, float value)
{
   for (uint8_t x = 0; x < m_nextChannel; x++) 
   {
      if ( strcmp_P(m_chanNames[x], name) == 0 ) 
      {
         LogData(x, value);
         break;
      }
   }
}

void SystemLogger::LogData(uint8_t channel, uint16_t value)
{
   if ( m_chanTypes[channel] == TEMPERATURE ) 
   {
      SaveTemperature(channel, (float)value);
   }
   else if ( m_chanTypes[channel] == FLOATING_POINT ) 
   {
      SaveFPValue(channel, (float)value);
   }
   else if ( m_chanTypes[channel] == UNSIGNED_INT ) 
   {
      SaveUIntValue(channel, value);
   }
}

void SystemLogger::LogData(char *name, uint16_t value)
{
   for (uint8_t x = 0; x < m_nextChannel; x++) 
   {
      if ( strcmp_P(m_chanNames[x], name) == 0 ) 
      {
         LogData(x, value);
         break;
      }
   }
}

void SystemLogger::DumpLog(bool full)
{
   if ( full == true ) 
   {
      m_lastTimeDumped = 0;
   }
   time_t et = DataLogger::MoveToFirstEntry();
   Serial << __PRETTY_FUNCTION__ << " first time " << et << ", last time " << m_lastTimeDumped << endl;
//   if ( et < m_lastTimeDumped) 
   {
      MoveBackToTime(m_lastTimeDumped);
   }
   Serial << __PRETTY_FUNCTION__ << " Log Entries: " << (int)Count() << endl;
   while (ReadNextLogSet(&m_lastTimeDumped)) 
   {
      Serial << "timestamp=" << DateTime::formatDateTime(m_lastTimeDumped);
//      Serial << ", date=" << DateTime::formatDateTime(m_lastTimeDumped);
//      Serial << ", time=" << DateTime::formatTime(m_lastTimeDumped);
      for (uint8_t channel = 0; channel < m_nextChannel; channel++) 
      {
         if ( m_channelCache[channel] != 0x8000 ) 
         {
            uint8_t chan_type = m_chanTypes[channel];
//            Serial << ", channel: " << (int)channel << " type: " << (int)chan_type ; // << ", TEMPERATURE: " << (int)TEMPERATURE;
            Serial << ", " << m_chanNames[channel]; 

//            Serial << "," << strcpy_P(p_buf,  cn) << m_lastTimeDumped;
            if ( chan_type == TEMPERATURE ) 
            {
               Serial << "=" << Convert2Temperature(m_channelCache[channel]);
            }
            else if ( chan_type == FLOATING_POINT ) 
            {
               Serial << "=" << Convert2Float(m_channelCache[channel]);
            }
            else 
            {
               Serial << "=" << (int)m_channelCache[channel];
            }
         }
      }
      Serial << endl;
   }
}
