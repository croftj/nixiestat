#ifndef CYCLE_DATA_H
#define CYCLE_DATA_H

#include <string>

class CycleData
{
public:
   CycleData() {};

   std::string toString();


   void setTgtRoomTemp(double temp)
   {
      m_tgtRoomTemp = temp;
   }

   void setCurRoomTemp(double temp)
   {
      m_curRoomTemp = temp;
   }

   void setTgtCycleTemp(double temp)
   {
      m_tgtCycleTemp = temp;
   }

   void setCurCycleTemp(double temp)
   {
      m_curCycleTemp = temp;
   }

   void setOutsideTemp(double temp)
   {
      m_outsideTemp = temp;
   }

   void setAlarm(int alarm)
   {
      m_alarm = alarm;
   }

   void setCycleTime(int c_time)
   {
      m_cycleTime = c_time;
   }

   void setCoolState(bool state)
   {
      m_heatState = state;
   }

   void setHeatState(bool state)
   {
      m_heatState = state;
   }

   void setPidEnabled(bool state)
   {
      m_pidEnabled = state;
   }

private:
   double m_tgtRoomTemp;
   double m_curRoomTemp;
   double m_tgtCycleTemp;
   double m_curCycleTemp;
   double m_outsideTemp;
   int    m_cycleTime;
   int    m_alarm;
   bool   m_coolState;
   bool   m_heatState;
   bool   m_pidEnabled;
};

#endif
