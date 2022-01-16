# include "WProgram.h"
# include "TempCycle.h"
# include "clock.h"
# include "esquare.h"
# include "IOPort.h"

extern uint8_t dbg;

extern TempSensors sensors;

extern char p_buf[128];
extern char ibuf[32];

static OutputPort HeatRelay;
static OutputPort CoolRelay;
static OutputPort FanRelay;
static OutputPort ValveRelay;

void TempCycle::initialize(TempSensors *sensors)
{
   m_sensors               = sensors;
   m_lastCall              = false;
   m_initialCall           = true;
   m_minCycleTimeReached   = false;

   HeatRelay   .SetPort(IOPort::PORT_B, _BV(0)); 
   CoolRelay   .SetPort(IOPort::PORT_B, _BV(1)); 
   FanRelay    .SetPort(IOPort::PORT_B, _BV(2)); 
   ValveRelay  .SetPort(IOPort::PORT_B, _BV(3)); 
   HeatRelay   .off();
   CoolRelay   .off();
   FanRelay    .off();
   ValveRelay  .off();
}

bool TempCycle::update()
{
   bool     rv             = false;
   float    target         = m_temp;
   uint16_t min_cycle_time = m_minCycleTime;
   time_t   cycle_time     = currentDate.getInterval(Clock::CYCLE_TIME_INTERVAL);

   if (dbg > 5) Serial << __PRETTY_FUNCTION__ << endl;
   if (dbg > 5) Serial << strcpy_P(p_buf, PSTR("m_minCycleTime = ")) << m_minCycleTime << endl;
   if (dbg > 5) Serial << strcpy_P(p_buf, PSTR("min_cycle_time = ")) << min_cycle_time << endl;
   if (dbg > 5) Serial << strcpy_P(p_buf, PSTR("m_maxCycleTime = ")) << m_maxCycleTime << endl;
   if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("cycle_time = "))     << cycle_time << endl;

   target = m_temp;
   if ( target > m_absMaximumTemp ) 
   {
      target = m_absMaximumTemp - 0.5;
   }
   else if ( target < m_absMinimumTemp )
   {
      target = m_absMinimumTemp + 0.5;
   }

   if ( m_lastCall ) 
   {
      min_cycle_time = min_cycle_time / 2;
   }

   float currTemp = sensors.read(m_sensor, TempSensors::CURRENT, false);
   if ( dbg > 2 ) 
   {
      Serial << strcpy_P(p_buf, PSTR("m_sensor = ")) << m_sensor;
      Serial << strcpy_P(p_buf, PSTR(", Current Temp: ")) << currTemp;
   }

   /***********************************************/
   /*   If  we  have  exceeded  the maximum time  */
   /*   that  the  heater/ac  should  be on then  */
   /*   force whichever off                       */
   /***********************************************/
   if ( cycle_time >= m_maxCycleTime && m_lastCall) 
   {
      if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("Max run time reached, forcing system off"));
      if ( m_mode == Settings::Cool ) 
      {
         rv = call4Cool(false);
      }
      else 
      {
         rv = call4Heat(false);
      }
      if (dbg > 2) Serial << strcpy_P(p_buf, PSTR(", rv = ")) << (int)rv << endl;
   }
   else 
   {
      if ( m_mode == Settings::Cool && currTemp < m_absMinimumTemp - 2.0 )
      {
         rv = call4Cool(false);
         if ((dbg > 1 && rv) || dbg > 4) Serial << strcpy_P(p_buf, PSTR("Minimum Reached- Forcing cool off!")) << endl;
      }
      else if ( m_mode == Settings::Heat && currTemp > m_absMaximumTemp + 2.0)
      {
         rv = call4Heat(false);
         if ((dbg > 1 && rv) || dbg > 4) Serial << strcpy_P(p_buf, PSTR("Maximum Reached- Forcing heat off!")) << endl;
      }
      else if ( m_mode == Settings::Cool ) 
      {
         float target;

         if ( m_lastCall ) 
         {
            target = (m_temp - (m_hyst / 2));
         }
         else 
         {
            target = (m_temp + (m_hyst / 2));
         }
         if (dbg > 2) Serial << strcpy_P(p_buf, PSTR(", Target Temp: ")) << target << "- ";
         if ( m_initialCall || cycle_time >= min_cycle_time )
         {
            if ( currTemp > target ) 
            {
               if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("cooling on")) << endl;
               m_initialCall = false;
               rv = call4Cool(true);
            }
            else
            {
               if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("cooling off")) << endl;
               rv = call4Cool(false);
            }
         }
         else 
         {
            if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("Cycle Postponed, elapsed time: "))  
                                                      << (int)cycle_time;
            if (dbg > 2) Serial << strcpy_P(p_buf, PSTR(" seconds of "))  
                                                      << (unsigned int)min_cycle_time;
            if (dbg > 2) Serial << strcpy_P(p_buf, PSTR(" total\r\n"));
         }
      }
      else if ( m_mode == Settings::Heat ) 
      {
         float target;

         if ( m_lastCall ) 
         {
            target = (m_temp + (m_hyst / 2));
         }
         else 
         {
            target = (m_temp - (m_hyst / 2));
         }
         if (dbg > 2) Serial << strcpy_P(p_buf, PSTR(", Target Temp: ")) << target << "- ";
         if ( m_initialCall || cycle_time >= min_cycle_time ) 
         {
            if ( currTemp < target ) 
            {
               if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("heating on")) << endl;
               m_initialCall = false;
               rv = call4Heat(true);
            }
            else
            {
               if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("heating off")) << endl;
               rv = call4Heat(false);
            }
         }
         else 
         {
            if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("Cycle Postponed, elapsed time: "))  
                                                      << (int)cycle_time;
            if (dbg > 2) Serial << strcpy_P(p_buf, PSTR(" seconds of "))  
                                                      << (unsigned int)min_cycle_time;
            if (dbg > 2) Serial << strcpy_P(p_buf, PSTR(" total\r\n"));
         }
      }
   }

   /***********************************************/
   /*   If  we  turned the furnace/ac on of off,  */
   /*   start the min cycle time                  */
   /***********************************************/
   if ( rv ) 
   {
      currentDate.resetInterval(Clock::CYCLE_TIME_INTERVAL);
   }
   return(rv);
}

bool TempCycle::call4Cool(bool f)
{
   bool rv = false;
   if ( m_lastCall != f ) 
   {
      rv = true;
   }
   m_lastCall = f;

   if ( f ) 
      CoolRelay.on();
   else
      CoolRelay.off();
   return(rv);
}

bool TempCycle::call4Heat(bool f)
{
   bool rv = false;
   if ( m_lastCall != f ) 
   {
      rv = true;
   }
   m_lastCall = f;

   if ( f ) 
      HeatRelay.on();
   else
      HeatRelay.off();
   return(rv);
}

void TempCycle::call4Fan(bool f)
{
   if ( f ) 
      FanRelay.on();
   else
      FanRelay.off();
}

TempCycle temp_cycle;

