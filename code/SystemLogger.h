# ifndef SYSTEMLOGGER_H
# define SYSTEMLOGGER_H

# include <stdint.h>
# include "datetime.h"
# include "core/HardwareSerial.h"
# include "CircularBuffer.h"
# include "DataLogger.h"


class SystemLogger : public DataLogger
{
public:
   enum 
   {
      TEMPERATURE = 0,
      FLOATING_POINT,
      UNSIGNED_INT,
   };

   SystemLogger();

   int8_t registerChannel_P(const char *name, uint8_t type);
   int8_t registerChannel(char *name, uint8_t type);
   void LogData(char *name, float value);
   void LogData(char *name, uint16_t value);
   void LogData(uint8_t channel, float value);
   void LogData(uint8_t channel, uint16_t value);
   void DumpLog(bool full);

private:
   char*          m_chanNames[MAX_CHANNELS];
   uint8_t        m_chanTypes[MAX_CHANNELS];
   uint16_t       m_DateTimeMSB;
   uint16_t       m_DateTimeLSB;
   uint8_t        m_nextChannel;
   time_t         m_lastTimeDumped;
};


# endif

