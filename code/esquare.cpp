# include "esquare.h"

# include <avr/pgmspace.h>

extern char *p_buf;

static char *p_strcpy(const char* str)
{
   static  char buf[256];
   return(strcpy_P(buf, str));
}

#define  P_STR(str)  p_strcpy(PSTR(str))

const char  ESquare::GOOD_SIGNATURE[] = "!BLOK";
const char  ESquare::BAD_SIGNATURE[]  = "!TOST";
const char  VERSION_STR[]     PROGMEM = "$Id$";

const char  CURRENT_MODE[]       PROGMEM = "current_mode    = ";
const char  AUTO_MODE[]          PROGMEM = "auto_mode       = ";
const char  VACATION_MODE[]      PROGMEM = "vacation_mode   = ";
const char  HEAT_HYST[]          PROGMEM = "heat_hyst       = ";
const char  COOL_HYST[]          PROGMEM = "cool_hyst       = ";
const char  HEAT_CYCLE_TIME[]    PROGMEM = "heat_cycle_time = ";
const char  COOL_CYCLE_TIME[]    PROGMEM = "cool_cycle_time = ";
const char  MAX_CYCLE_TIME[]     PROGMEM = "max_cycle_time  = ";
const char  CYCLE_SENSOR[]       PROGMEM = "cycle_sensor    = ";
const char  OUTSIDE_SENSOR[]     PROGMEM = "outside_sensor  = ";
const char  TARGET_SENSOR[]      PROGMEM = "target_sensor   = ";
const char  CYCLE_MAXIMUM[]      PROGMEM = "cycle_max       = ";
const char  CYCLE_MINIMUM[]      PROGMEM = "cycle_min       = ";
const char  CYCLE_TEMP[]         PROGMEM = "cycle_temp      = ";
const char  CYCLE_HYSTORISIS[]   PROGMEM = "cycle_hyst      = ";
const char  OUTSIDE_CURVE[]      PROGMEM = "outside_curve   = ";
const char  OUTSIDE_ADJ[]        PROGMEM = "outside_adj     = ";
const char  OUTSIDE_TEMP[]       PROGMEM = "outside_temp    = ";
const char  TARGET_TEMP[]        PROGMEM = "target_temp     = ";
const char  VACATION_TEMP[]      PROGMEM = "vacation_temp   = ";
const char  KP[]                 PROGMEM = "kp              = ";
const char  KI[]                 PROGMEM = "ki              = ";
const char  KD[]                 PROGMEM = "kd              = ";

const uint32_t ESquare::VERSION = 0x00000100;

//void ESquare::PrintRevisionTag()
//{
//   Serial << p_strcpy(VERSION_STR);
//}

void ESquare::DisplaySettings()
{
   Serial << strcpy_P(p_buf, CURRENT_MODE)      << (int)data.currentMode     << endl;
   Serial << strcpy_P(p_buf, VACATION_MODE)     << (int)data.vacationMode    << endl;
   Serial << strcpy_P(p_buf, AUTO_MODE)         << (int)data.autoMode        << endl;
   Serial << strcpy_P(p_buf, HEAT_HYST)         << (int)data.heatHyst        << endl;
   Serial << strcpy_P(p_buf, COOL_HYST)         << (int)data.coolHyst        << endl;
   Serial << strcpy_P(p_buf, HEAT_CYCLE_TIME)   << (int)data.heatMinCycleTime << endl;
   Serial << strcpy_P(p_buf, COOL_CYCLE_TIME)   << (int)data.coolMinCycleTime << endl;
   Serial << strcpy_P(p_buf, MAX_CYCLE_TIME)    << (int)data.maxCycleTime     << endl;
   Serial << strcpy_P(p_buf, CYCLE_SENSOR)      << (int)data.heatCycleSensor << endl;
   Serial << strcpy_P(p_buf, OUTSIDE_SENSOR)    << (int)data.outsideSensor   << endl;
   Serial << strcpy_P(p_buf, TARGET_SENSOR)     << (int)data.targetSensor    << endl;
   Serial << strcpy_P(p_buf, CYCLE_MAXIMUM)     << data.cycleMaximum    << endl;
   Serial << strcpy_P(p_buf, CYCLE_MINIMUM)     << data.cycleMinimum    << endl;
   Serial << strcpy_P(p_buf, CYCLE_TEMP)        << data.cycleTemp       << endl;
   Serial << strcpy_P(p_buf, CYCLE_HYSTORISIS)  << data.cycleHyst       << endl;
   Serial << strcpy_P(p_buf, OUTSIDE_CURVE)     << data.outsideCurve    << endl;
   Serial << strcpy_P(p_buf, OUTSIDE_ADJ)       << data.outsideAdj      << endl;
   Serial << strcpy_P(p_buf, OUTSIDE_TEMP)      << data.outsideTemp     << endl;
   Serial << strcpy_P(p_buf, TARGET_TEMP)       << data.targetTemp      << endl;
   Serial << strcpy_P(p_buf, VACATION_TEMP)     << data.vacationTemp    << endl;
   Serial << strcpy_P(p_buf, KP)                << data.Kp              << endl;
   Serial << strcpy_P(p_buf, KI)                << data.Ki              << endl;
   Serial << strcpy_P(p_buf, KD)                << data.Kd              << endl;

   Serial << strcpy_P(p_buf, PSTR("Heating Temps:")) << endl;
   for (char x = 1; x < Settings::NUM_SETTINGS; x++)
   {
      Serial << strcpy_P(p_buf, PSTR("Setting ")) << (int)x;
      Serial << strcpy_P(p_buf, PSTR(":"));
      Serial << strcpy_P(p_buf, PSTR("temp: ")) << data.heatTemps[x];
      if ( data.heatTimes[x] == 0xffff ) 
      {
         Serial << strcpy_P(p_buf, PSTR(", time: 0xffff")) << endl;
      }
      else 
      {
         uint8_t hour = data.heatTimes[x] / 60;
         uint8_t min = data.heatTimes[x] % 60;
         Serial << strcpy_P(p_buf, PSTR(", time: ")) << (int)hour;
         Serial << strcpy_P(p_buf, PSTR(":")) << (int)min;
         Serial << strcpy_P(p_buf, PSTR(" (")) << (int)data.heatTimes[x];
         Serial << strcpy_P(p_buf, PSTR(")")) << endl;
      }
   }

   Serial << strcpy_P(p_buf, PSTR("Cooling Temps:")) << endl;
   for (char x = 1; x < Settings::NUM_SETTINGS; x++)
   {
      Serial << strcpy_P(p_buf, PSTR("Setting ")) << (int)x;
      Serial << strcpy_P(p_buf, PSTR(":"));
      Serial << strcpy_P(p_buf, PSTR(" temp: ")) << data.coolTemps[x];
      if ( data.coolTimes[x] == 0xffff ) 
      {
         Serial << strcpy_P(p_buf, PSTR(", time: 0xffff")) << endl;
      }
      else 
      {
         uint8_t hour = data.coolTimes[x] / 60;
         uint8_t min = data.coolTimes[x] % 60;
         Serial << strcpy_P(p_buf, PSTR(", time: ")) << (int)hour;
         Serial << strcpy_P(p_buf, PSTR(":")) << (int)min;
         Serial << strcpy_P(p_buf, PSTR(" (")) << (int)data.heatTimes[x];
         Serial << strcpy_P(p_buf, PSTR(")")) << endl;
      }
   }
}

ESquare eeprom;

