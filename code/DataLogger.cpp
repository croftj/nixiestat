# include "datetime.h"
# include "DataLogger.h"
# include "WProgram.h"
# include <math.h>

const uint16_t DataLogger::TIME_STAMP        = 0x8000;
const uint16_t DataLogger::TIME_STAMP_MSW    = 0x4000;
const uint16_t DataLogger::TIME_STAMP_MASK   = 0x3fff;
const uint16_t DataLogger::TIME_DIVISOR      = 8;        // Fits a time stamp in 28 bits
               
const uint16_t DataLogger::VALUE_CHAN_MASK   = 0x7c00;
const uint16_t DataLogger::VALUE_CHAN_SHIFT  = 10;
               
const uint16_t DataLogger::VALUE_MASK        = 0x03ff;
const uint16_t DataLogger::VALUE_SIGN        = 0x0200;
const float DataLogger::TEMP_OFFSET          = 60.0;     // Range from -68 to +187

static char ibuf[32];

static bool DBG_I = false;

DataLogger::DataLogger()
{
   m_lastTime = -1;
}

/**********************************************************************/
/*   Stores the passed in float as a 10bit fixed point number with a  */
/*   two bits to the right of the 'decimal' point. In other words an  */
/*   8 bit mantissa.                                                  */
/*                                                                    */
/*   This function is different than the SaveFPValue in that it will  */
/*   shift  the  value  by adding TEMP_OFFSET to it. This places the  */
/*   temperature in a more reasonable range.                          */
/**********************************************************************/
void DataLogger::SaveTemperature(uint8_t channel, float temp)
{
   /***********************************************/
   /*   Scale  this  so  that  we  get something  */
   /*   better  than  -128  to + 128. Not that I  */
   /*   would  ever  want  to  see  +128  in the  */
   /*   summer,  but  lord  willing I will never  */
   /*   see -128 in the winter!                   */
   /***********************************************/
   float temp_calc = temp;
   temp_calc -= TEMP_OFFSET;
   SaveFPValue(channel, temp_calc);
}

/**********************************************************************/
/*   Stores the passed in float as a 10bit fixed point number with a  */
/*   two bits to the right of the 'decimal' point. In other words an  */
/*   8 bit mantissa.                                                  */
/**********************************************************************/
void DataLogger::SaveFPValue(uint8_t channel, float value)
{
   /***********************************************/
   /*   Next  get  a  reasonable  integer of the  */
   /*   temp and 2 decimal places                 */
   /***********************************************/
   uint16_t t_fixedp = (int)(fabs(value) * 4.0);

   if ( value < 0 ) 
   {
      t_fixedp |= VALUE_SIGN;
   }

   /***********************************************/
   /*   Mask it off to fit the bits we need.      */
   /***********************************************/
   t_fixedp &= VALUE_MASK;

   SaveUIntValue(channel, t_fixedp);
}

void DataLogger::SaveUIntValue(uint8_t channel, uint16_t value)
{
   /***********************************************/
   /*   Shift and isolate the channel number      */
   /***********************************************/
   uint16_t chan = channel;
   if (DBG_I) Serial << __PRETTY_FUNCTION__ << " Saving channel: 0x" << itoa(channel, ibuf, 16) << endl;

   chan = (chan << VALUE_CHAN_SHIFT) & VALUE_CHAN_MASK;
   if (DBG_I) Serial << __PRETTY_FUNCTION__ << " Shifted channel: 0x" << itoa(chan, ibuf, 16) << endl;
   if (DBG_I) Serial << __PRETTY_FUNCTION__ << " Value: 0x" << itoa(value, ibuf, 16) << endl;

   /***********************************************/
   /*   Add  the channel and sensor type in with  */
   /*   the temperature;                          */
   /***********************************************/
   value |= chan;
   LogEntry(value);
}

/**********************************************************************/
/*   Here  we  log  a pre-assembled entry. If enough time has passed  */
/*   that  we  need  a  new  timestamp  to  be  logged,  one will be  */
/*   generated and pushed onto the log.                               */
/**********************************************************************/
void DataLogger::LogEntry(uint16_t entry)
{
   /***********************************************/
   /*   Get  the current time and see if so much  */
   /*   time  has  passed  we  need to log a new  */
   /*   timestamp                                 */
   /***********************************************/
   time_t timeNow = currentDate.now() / TIME_DIVISOR;
   if (DBG_I) Serial << __PRETTY_FUNCTION__ << " timeNow = " << timeNow << ", m_lastTime " << m_lastTime << endl;
   if ( timeNow != m_lastTime ) 
   {
      if (DBG_I) Serial << __PRETTY_FUNCTION__ << " Adding time stamp, timeNow = " << timeNow << ", 0x" << itoa((long)((timeNow >> 16) & 0xffff), ibuf, 16);
      if (DBG_I) Serial << itoa((long)(timeNow & 0xffff), ibuf, 16) << endl;
      unsigned int msw = ((timeNow >> 14) & TIME_STAMP_MASK);
      unsigned int lsw = (timeNow & TIME_STAMP_MASK);
      if (DBG_I) Serial << __PRETTY_FUNCTION__ << "1 msw = " << itoa(msw, ibuf, 16);
      if (DBG_I) Serial << ", lsw = " << itoa(lsw, ibuf, 16) << endl;
      msw = (msw & TIME_STAMP_MASK) | TIME_STAMP | TIME_STAMP_MSW;
      lsw = lsw | TIME_STAMP;
      if (DBG_I) Serial << __PRETTY_FUNCTION__ << "2 msw = " << itoa(msw, ibuf, 16);
      if (DBG_I) Serial << ", lsw = " << itoa(lsw, ibuf, 16) << endl;

      m_log.Push((uint16_t)msw); // | TIME_STAMP | TIME_STAMP_MSW);
      m_log.Push((uint16_t)lsw); // | TIME_STAMP);
      m_lastTime = timeNow;
   }
   if (DBG_I) Serial << __PRETTY_FUNCTION__ << " Saving entry value: 0x" << itoa(entry, ibuf, 16) << endl;
   m_log.Push(entry);
}

/**********************************************************************/
/*   void MoveToHead()                                                */
/*                                                                    */
/*   This function moves cursor 2 to the head of the buffer.          */
/**********************************************************************/
void DataLogger::MoveToHead()
{
   m_log.MoveToHead(2);
}

time_t DataLogger::MoveToFirstEntry()
{
   time_t rv = 0;

   /***********************************************/
   /*   Invalidate the channel cache entries.     */
   /***********************************************/
   for (uint8_t x = 0; x < MAX_CHANNELS; x++) 
   {
      m_channelCache[x] = 0x8000;   
   }

   m_log.MoveToTail(2);
   while ( rv = 0 && m_log.HasNext(2) ) 
   {
      unsigned int lv1 = m_log.Next(2);
      if ( lv1 & TIME_STAMP )
      {
         Serial << __PRETTY_FUNCTION__ << " lv1 = 0x" << itoa(lv1, ibuf, 16) << endl;
         if ( m_log.HasNext(2) ) 
         {
            // Now reconstruct the time
            unsigned int lv2 = m_log.Next(2);   // lsw second
            if ( lv2 & TIME_STAMP )    
            {
               Serial << __PRETTY_FUNCTION__ << " lv2 = 0x" << itoa(lv2, ibuf, 16) << endl;
               /***********************************************/
               /*   We  made  it!  reconstruct the timestamp  */
               /*   and  move  back  to  the  start  of  the  */
               /*   timestamp.                                */
               /***********************************************/
               if ( lv1 & TIME_STAMP_MSW && ! (lv2 & TIME_STAMP_MSW) ) 
               {
                  rv = (lv1 & TIME_STAMP_MASK) << 14 | lv2 & TIME_STAMP_MASK;
               }
               else if ( lv2 & TIME_STAMP_MSW && ! (lv1 & TIME_STAMP_MSW) ) 
               {
                  rv = (lv2 & TIME_STAMP_MASK) << 14 | lv1 & TIME_STAMP_MASK;
               }
               else 
               {
                  Serial << __PRETTY_FUNCTION__ << " Did not find a valid time signature" << endl;
                  rv = 0;
               }
            }
            else 
            {
               Serial << __PRETTY_FUNCTION__ << " lv2 missing time tag" << endl;
            }
         }
         else 
         {
            Serial << __PRETTY_FUNCTION__ << " no lv2" << endl;
         }
      }
   }
   if ( rv != 0 ) 
   {
      /***********************************************/
      /*   Keep the time stamp                       */
      /***********************************************/
      m_log.Previous(2);
      m_log.Previous(2);
   }
   Serial << __PRETTY_FUNCTION__ << " rv = " << (int)rv << endl;

   return(rv);
}
      
/**********************************************************************/
/*   time_t MoveBackToTime(time_t dest)                               */
/*                                                                    */
/*   This  function  moves  cursor 2 to the position of the entry in  */
/*   the log who's time is just past the time requested.              */
/*                                                                    */
/*   A  side  effect  of this function is to invalidate the buffered  */
/*   values   for  each  channel.  This  allows  the  other  related  */
/*   functions  used  for  fetching  data from the log to know which  */
/*   channels  have actually had data fetched from the log since the  */
/*   the cursor was moved to it's starting point.                     */
/*                                                                    */
/*   It  will  return the time of this entry and leave the cursor at  */
/*   it's  position.  If  a time of 0 is returned, then there was no  */
/*   suitable entry found.                                            */
/*                                                                    */
/*   Note: if there are any entries added to the log between finding  */
/*   the  entry and making use of the cursor, there is a chance that  */
/*   the  tail  of the log will surpass the cursor position and your  */
/*   view  of the data will be corrupted (the data in the log itself  */
/*   will not be corrupted.                                           */
/**********************************************************************/
time_t DataLogger::MoveBackToTime(time_t dest)
{
   time_t rv = 0;
//   Serial << __PRETTY_FUNCTION__ << " rv = " << rv << ", dest = " << dest << endl;

   m_log.MoveToTail(2);
   while ( (rv == 0 || rv < dest) && m_log.HasNext(2) ) 
   {
      unsigned int lv1 = m_log.Next(2);
      if ( lv1 & TIME_STAMP )
      {
//         Serial << __PRETTY_FUNCTION__ << " lv1 = 0x" << itoa(lv1, ibuf, 16) << endl;
         if ( m_log.HasNext(2) ) 
         {
            // Now reconstruct the time
            unsigned int lv2 = m_log.Next(2);   // lsw second
            if ( lv2 & TIME_STAMP )    
            {
//               Serial << __PRETTY_FUNCTION__ << " lv2 = 0x" << itoa(lv2, ibuf, 16) << endl;
               /***********************************************/
               /*   We  made  it!  reconstruct the timestamp  */
               /*   and  move  back  to  the  start  of  the  */
               /*   timestamp.                                */
               /***********************************************/
               if ( lv1 & TIME_STAMP_MSW && ! (lv2 & TIME_STAMP_MSW) ) 
               {
                  rv = lv1 & TIME_STAMP_MASK;
                  rv = rv << 14;
                  rv |= lv2 & TIME_STAMP_MASK;
               }
               else if ( lv2 & TIME_STAMP_MSW && ! (lv1 & TIME_STAMP_MSW) ) 
               {
                  rv = lv2 & TIME_STAMP_MASK;
                  rv = rv << 14;
                  rv |= lv1 & TIME_STAMP_MASK;
               }
               else 
               {
//                  Serial << __PRETTY_FUNCTION__ << " Did not find a valid time signature" << endl;
                  rv = 0;
               }
            }
            else 
            {
//               Serial << __PRETTY_FUNCTION__ << " lv2 missing time tag" << endl;
            }
         }
         else 
         {
//            Serial << __PRETTY_FUNCTION__ << " no lv2" << endl;
         }
      }
   }
   if ( rv != 0 ) 
   {
      /***********************************************/
      /*   Keep the time stamp                       */
      /***********************************************/
      m_log.Previous(2);
      m_log.Previous(2);

      /***********************************************/
      /*   Invalidate the channel cache entries.     */
      /***********************************************/
      for (uint8_t x = 0; x < MAX_CHANNELS; x++) 
      {
         m_channelCache[x] = 0x8000;   
      }
   }
//   Serial << __PRETTY_FUNCTION__ << " rv = " << (int32_t)rv << "(0x" << itoa((int32_t)rv, ibuf, 16) << ")" << endl;

   return(rv);
}

/**********************************************************************/
/*   bool ReadNextLogSet(time_t  *ts)                                 */
/*                                                                    */
/*   This  function  reads  in  the  data  entries  from the current  */
/*   position  to the next timestamp and loads them into the channel  */
/*   cache  m_channelCache  which can then be proccesed as needed to  */
/*   formulate a export of all of the channels of data.               */
/*                                                                    */
/*   Entries  in  the  cache that have a value of 0x8000 do not have  */
/*   any  valid data. This can be either due to no data was found in  */
/*   the  log  for  them  or there was no channel registered for the  */
/*   entry yet.                                                       */
/*                                                                    */
/*   Parameters:                                                      */
/*     ts        Pointer to the location to save the associated       */
/*               timestamp If set to 0, no data was collected, in     */
/*               words, there is no more data available.              */
/*                                                                    */
/*   Returns:    true or false, if false, the head of the buffer has  */
/*               surpassed the cursor and therefore the cursor has    */
/*               been invalidated and must be repositioned using      */
/*               either  using MoveToHead or MoveBackToTime.          */
/**********************************************************************/
bool DataLogger::ReadNextLogSet(time_t *ts)
{
   bool rv = false;
//   *ts = 0;

   /***********************************************/
   /*   Search for the next valid time tag        */
   /***********************************************/
//   Serial << __PRETTY_FUNCTION__ << " Log Count: " << m_log.Count() << endl;
   while ( rv == false && m_log.Count() > 0 && m_log.HasNext(2) ) 
   {
      unsigned long lv1 = m_log.Next(2);
      if ( lv1 & TIME_STAMP )
      {
//         Serial << __PRETTY_FUNCTION__ << " lv1 = 0x" << itoa(lv1, ibuf, 16) << endl;
         if ( m_log.HasNext(2) ) 
         {
            // Now reconstruct the time
            unsigned long lv2 = m_log.Next(2);   // lsw second
            if ( lv2 & TIME_STAMP )    
            {
//               Serial << __PRETTY_FUNCTION__ << " lv2 = 0x" << itoa(lv2, ibuf, 16) << endl;
               /***********************************************/
               /*   We  made  it!  reconstruct the timestamp  */
               /*   and  move  back  to  the  start  of  the  */
               /*   timestamp.                                */
               /***********************************************/
               if ( lv1 & TIME_STAMP_MSW && ! (lv2 & TIME_STAMP_MSW) ) 
               {
                  *ts = (lv1 & TIME_STAMP_MASK) << 14 | lv2 & TIME_STAMP_MASK;
               }
               else if ( lv2 & TIME_STAMP_MSW && ! (lv1 & TIME_STAMP_MSW) ) 
               {
                  *ts = (lv2 & TIME_STAMP_MASK) << 14 | lv1 & TIME_STAMP_MASK;
               }
               else 
               {
                  Serial << __PRETTY_FUNCTION__ << " Did not find a valid time signature" << endl;
               }
               *ts *= TIME_DIVISOR;
               rv = true;
//               Serial << __PRETTY_FUNCTION__ << " Have timestamp: "  << (long)(*ts) << endl;
            }
            else 
            {
               Serial << __PRETTY_FUNCTION__ << " lv2 missing time tag" << endl;
            }
         }
         else 
         {
            Serial << __PRETTY_FUNCTION__ << " no lv2" << endl;
         }
      }
   }

   if ( rv ) 
   {
      /***********************************************/
      /*   We are at the start of the data for this  */
      /*   time  stamp.  Start  collecting any data  */
      /*   that follows up to the next time stamp.   */
      /***********************************************/
      while ( m_log.HasNext(2) ) 
      {
         uint16_t entry = m_log.Next(2);
//         Serial << __PRETTY_FUNCTION__ << " have entry: 0x" << itoa(entry, ibuf, 16) << endl;

         /***********************************************/
         /*   Save  the  entry into the the approriate  */
         /*   channel  slot  of  cache overwriting any  */
         /*   value saved before                        */
         /***********************************************/
         if ( ! (entry & TIME_STAMP) )
         {
            uint8_t channel = (entry & VALUE_CHAN_MASK) >> VALUE_CHAN_SHIFT;
            m_channelCache[channel] = entry & VALUE_MASK;
//            Serial << __PRETTY_FUNCTION__ << " channel = " << (int)channel << ", value = " << m_channelCache[channel] << endl;
         }

         /***********************************************/
         /*   If we reached the next timestamp, stop!   */
         /***********************************************/
         else 
         {
//            Serial << __PRETTY_FUNCTION__ << " Saw it as the next timestamp, backing up!" << endl;
            m_log.Previous(2);
            break;
         }
      }
   }
//   Serial << __PRETTY_FUNCTION__ << " exiting rv = " << rv << endl;
   return(rv);
}

float DataLogger::Convert2Temperature(uint16_t value)
{
   float rv = Convert2Float(value);
   rv += TEMP_OFFSET;
   return(rv);
}

float DataLogger::Convert2Float(uint16_t value)
{
   float rv;
   uint16_t val = value;

//   Serial << __PRETTY_FUNCTION__ << "value: 0x" << itoa(val, ibuf, 16);
   val = value & VALUE_MASK;
   val &= ~(VALUE_SIGN);
   rv = (float)val;
//   Serial << ", rv: " << (float)rv;
   rv /= 4.0;
//   Serial << ", rv: " << (float)rv;
   if ( value & VALUE_SIGN ) 
   {
      rv *= -1.0;
   }
//   Serial << ", rv: " << (float)rv << endl;
   return(rv);
}

int DataLogger::Convert2Int(uint16_t value)
{
   int rv = value;
   rv &= ~(VALUE_SIGN);
   if ( value & VALUE_SIGN ) 
   {
      rv *= -1;
   }
   return(rv);
}
