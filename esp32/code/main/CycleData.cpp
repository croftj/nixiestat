#include "CycleData.h"
#include "sys/time.h"

#include "configuration.h"
#include "json11.hpp"

extern Configuration *config;

std::string CycleData::toString()
{
   time_t timestamp;
   time(&timestamp);
   json11::Json js = json11::Json::object {
      { "message_type", "status" }, 
      { "device", config->value("mqtt_ident").toString() }, 
      { "target_room_temp",   m_tgtRoomTemp}, 
      { "current_room_temp",  m_curRoomTemp}, 
      { "target_cycle_temp",  m_tgtCycleTemp}, 
      { "current_cycle_temp", m_curCycleTemp}, 
      { "outside_temp",       m_outsideTemp}, 
      { "cycle_time",         m_cycleTime}, 
      { "heat_state",         m_heatState}, 
      { "pid_enabled",        m_pidEnabled}, 
      { "alarm",              m_alarm}, 
      { "timestamp",          (int)timestamp}
   };
   return(js.dump());
}

