# ifndef DATALOGGER_H
# define DATALOGGER_H

# include <stdint.h>
# include "datetime.h"
# include "core/HardwareSerial.h"
# include "CircularBuffer.h"

class DataLogger 
{
public:
   DataLogger();

   enum 
   {
      MAX_CHANNELS      = 32,
   };

   void LogEntry(uint16_t entry);

   bool setLogSize(uint16_t size)
   {
      return(m_log.SetBufferSize(size));
   }

   void ClearLog()
   {
      m_log.Clear();
   }

   uint16_t Count(time_t time = 0)
   {
      uint16_t maxCount = m_log.Count();
      uint16_t count = 0;
      if ( time > 0 ) 
      {
         m_log.MoveToHead(3);
         time_t thisTime = m_lastTime;
         while (thisTime > time && count <= maxCount) 
         {
            char ibuf[16];
            uint16_t entry = m_log.Previous(3);
//            Serial << "entry = 0x" << itoa(entry, ibuf, 16) << endl;
//            Serial << "thisTime = " << thisTime << endl;
            thisTime = DecodeEntry(entry, 0, thisTime, 0);
            count++; 
         }
      }
      else
         count = m_log.Count();
      return(count);
   }

   void SaveFPValue(uint8_t channel, float value);
   void SaveTemperature(uint8_t channel, float temp);
   void SaveUIntValue(uint8_t channel, uint16_t value);
   
   void MoveToHead();
   time_t MoveToFirstEntry();
   time_t MoveBackToTime(time_t dest);
   bool   ReadNextLogSet(time_t *ts);

   float  Convert2Temperature(uint16_t value);
   float  Convert2Float(uint16_t value);
   int    Convert2Int(uint16_t value);
protected:
   time_t DecodeEntry(uint16_t entry, uint8_t *channel = 0, time_t time = 0, float *temp = 0);

   uint16_t                            m_channelCache[MAX_CHANNELS];

private:
   time_t                              m_lastTime;
   time_t                              m_lastLogTime;

   char                                ibuf[20];
   CircularBuffer<uint16_t, uint16_t>  m_log; 

   static const uint16_t TIME_STAMP;
   static const uint16_t TIME_STAMP_MSW;
   static const uint16_t TIME_STAMP_MASK;
   static const uint16_t TIME_DIVISOR;       // Fits a days worth of seconds in 15 bits
                  
   static const uint16_t VALUE_CHAN_MASK;
   static const uint16_t VALUE_CHAN_SHIFT;
                  
   static const uint16_t VALUE_MASK;
   static const uint16_t VALUE_SIGN;
   static const float TEMP_OFFSET;           // Range from -68 to +187

   static const uint16_t TIMESTAMP;
};


# endif

