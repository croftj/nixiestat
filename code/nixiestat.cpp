//#define  MINIMAL
//#define  NO_EXEC
//#define  NO_LOG
//#define  NO_MENU_EXEC
//#define  DISPLAY_OFF
// #define  DBG
#define  POLLING  1
#define REGCHANNEL

# include <avr/pgmspace.h>
# include <ctype.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>

# include "EEPROM.h"
# include "IOPort.h"

# include "datetime.h"
# include "esquare.h"
# include "menu.h"

#ifndef MINIMAL 
# include "display.h"
# include "PID.h"
# include "SystemLogger.h"
# include "TempCycle.h"
# include "TempSensors.h"
# include "thermostat.h"
# include "WProgram.h"
# include "Wire.h"
# include "ds1307.h"
# include "mpr121.h"
# include "switches.h"
# include "ui.h"
#endif

#define  LOG_SIZE                (15 * 1024)
//#define  LOG_SIZE                (100)
#define  READ_TEMPERATURE_TIME   15
#define  READ_SENSOR_TIME        2
#define  DISP_TEMPERATURE_TIME   10
#define  LOG_TEMPERATURE_TIME    (30 * 60)
//#define  LOG_TEMPERATURE_TIME    (30)
#define  RESERVED_STACK          8192

enum
{
   TIME_LAPSE_MASK  = 0xf000,
   TIME_LAPSE_SHIFT = 12,
   CHANNEL_MASK     = 0x0c00,
   CHANNEL_SHIFT    = 10,
   TEMPERATURE_MASK = 0x03ff,
   TEMPERATURE_SIGN = 0x0200,
   LOG_LENGTH       = 12 * 1024,
   TEMP_OFFSET      = 40
};

#ifdef DBG
uint8_t dbg = 4;
#else
uint8_t dbg = 0;
#endif

#ifndef MINIMAL 
#ifndef NO_LOG
SystemLogger syslog;
#endif

TempSensors sensors;
PID pid(5, 2, 10);

MPR121 mpr121;
#endif

bool tempIsReady = 0;
uint8_t tempCounter = 12;

char p_buf[128];
char ibuf[32];
bool needPrompt = 0;
bool *pid_debug = NULL;

MenuEntry *main_menu;

const char invalidEntry_str[] PROGMEM = "\nInvalid Entry!";

# ifdef REGCHANNEL
#ifndef NO_LOG
const char OUTSIDE_SENSOR_AVG_LOG[] PROGMEM  = "OutAvg";
const char OUTSIDE_SENSOR_MIN_LOG[] PROGMEM  = "OutMin";
const char OUTSIDE_SENSOR_MAX_LOG[] PROGMEM  = "OutMax";

const char INSIDE_SENSOR_AVG_LOG[] PROGMEM   = "InAvg";
const char INSIDE_SENSOR_MIN_LOG[] PROGMEM   = "InMin";
const char INSIDE_SENSOR_MAX_LOG[] PROGMEM   = "InMax";

const char CYCLE_SENSOR_TGT_LOG[] PROGMEM    = "CycTgt";
const char CYCLE_SENSOR_AVG_LOG[] PROGMEM    = "CycAvg";
const char CYCLE_SENSOR_MIN_LOG[] PROGMEM    = "CycMin";
const char CYCLE_SENSOR_MAX_LOG[] PROGMEM    = "CycMax";
const char CYCLE_STATE_LOG[]      PROGMEM    = "CycState";

const char TARGET_TEMP_LOG[] PROGMEM         = "TgtTemp";
const char PID_ITERM_LOG[] PROGMEM           = "PIDITerm";
# endif
# endif
/**********************************************************************/
/*   Menu Strings                                                     */
/**********************************************************************/
const char MENU_MAINMENU_STR[] PROGMEM = "Main Menu";

const char MENU_MM11_STR[] PROGMEM = "Cycle State";
const char MENU_MM10_STR[] PROGMEM = "Cycle Temp.";
const char MENU_MM9_STR[]  PROGMEM = "Outside Temp.";
const char MENU_MM8_STR[]  PROGMEM = "Inside Temp.";
const char MENU_MM7_STR[]  PROGMEM = "Dump full Log";
const char MENU_MM6_STR[]  PROGMEM = "Dump Log";
const char MENU_MM5_STR[]  PROGMEM = "Save Settings";
const char MENU_MM4_STR[]  PROGMEM = "Date";
const char MENU_MM3_STR[]  PROGMEM = "Time";
const char MENU_MM2_STR[]  PROGMEM = "Mode";
const char MENU_MM1_STR[]  PROGMEM = "Set Temperature";

const char MENU_SYSMENU_STR[]    PROGMEM = "System";
const char MENU_SY7_STR[]        PROGMEM = "PID Debug";
const char MENU_SY6_STR[]        PROGMEM = "Reset Count";
const char MENU_SY5_STR[]        PROGMEM = "Clear Log";
const char MENU_SY4_STR[]        PROGMEM = "Show Esquare";
const char MENU_SY3_STR[]        PROGMEM = "Show menu Map";
const char MENU_SY2_STR[]        PROGMEM = "Debug Level";
const char MENU_SY1_STR[]        PROGMEM = "Log Interval";

const char MENU_KYPMENU_STR[]    PROGMEM = "Key Pad";
const char MENU_KP1_STR[]        PROGMEM = "Touch Thresh";
const char MENU_KP2_STR[]        PROGMEM = "Release Thresh";
const char MENU_KP3_STR[]        PROGMEM = "Initialize";


const char MENU_HYSTMENU_STR[]   PROGMEM = "Hysteresis";
const char MENU_HM2_STR[]        PROGMEM = "Cooling";
const char MENU_HM1_STR[]        PROGMEM = "Heating";

const char MENU_SENSORSMENU_STR[]   PROGMEM = "Sensors";
const char MENU_SM4_STR[]           PROGMEM = "Status";
const char MENU_SM3_STR[]           PROGMEM = "Discover";
const char MENU_SM2_STR[]           PROGMEM = "Exterior";
const char MENU_SM1_STR[]           PROGMEM = "Interior";

const char MENU_THERMOSTATMENU_STR[]   PROGMEM = "Thermostat";
const char MENU_TMS02_STR[]            PROGMEM = "Vacation";
const char MENU_TMS01_STR[]            PROGMEM = "Max Cycle Time";

const char MENU_HEATINGMENU_STR[] PROGMEM = "Heating";

const char MENU_THM12_STR[] PROGMEM = "Min Cycle Time";
const char MENU_THM11_STR[] PROGMEM = "Time4";
const char MENU_THM10_STR[] PROGMEM = "Temp4";

const char MENU_THM08_STR[] PROGMEM = "Time3";
const char MENU_THM07_STR[] PROGMEM = "Temp3";

const char MENU_THM05_STR[] PROGMEM = "Time2";
const char MENU_THM04_STR[] PROGMEM = "Temp2";

const char MENU_THM02_STR[] PROGMEM = "Time1";
const char MENU_THM01_STR[] PROGMEM = "Temp1";

const char MENU_HEATINGPIDMENU_STR[]         PROGMEM = "PID Settings";
const char MENU_HEATINGADVANCEDPIDMENU_STR[] PROGMEM = "Advanced Settings";

const char MENU_HPM4_STR[] PROGMEM = "Cycle Maximum";
const char MENU_HPM3_STR[] PROGMEM = "Cycle Minimum";
const char MENU_HPM2_STR[] PROGMEM = "Cycle Hysteresis";
const char MENU_HPM1_STR[] PROGMEM = "Enabled";

const char MENU_HAPM6_STR[] PROGMEM = "Kp";
const char MENU_HAPM5_STR[] PROGMEM = "Ki";
const char MENU_HAPM4_STR[] PROGMEM = "Kd";
const char MENU_HAPM3_STR[] PROGMEM = "Curve";
const char MENU_HAPM2_STR[] PROGMEM = "Adjust";
const char MENU_HAPM1_STR[] PROGMEM = "Sensor";

const char MENU_COOLINGMENU_STR[] PROGMEM = "Cooling";

/**********************************************************************/
/*   This function displays the time in an almost pleasant way.       */
/**********************************************************************/
char *formatLogEntry(char *buf, time_t thisTime, uint8_t channel, float temp)
{
#ifndef MINIMAL
//   sprintf(buf,strcpy_P(p_buf, PSTR("%ld,%s,%d,%s,%3.2f")), thisTime, DateTime::formatTime(thisTime), channel, thermostat.formatTempSensorAddr(channel), (double)temp);
#else
   *buf = '\0';
#endif
   return(buf);
}

/**********************************************************************/
/*   Function to update the settings in the settings area. This will  */
/*   also  send  required  settings  out to the various objects that  */
/*   depend on the settings                                           */
/**********************************************************************/
int updateThermostatSettings(ESquareSpace_t *settings)
{
#ifndef MINIMAL
   Serial << strcpy_P(p_buf, PSTR("Updating thermostat settings...")) << endl;
   temp_cycle.setSensor(settings->targetSensor);
   temp_cycle.setMode(settings->currentMode);
   if ( settings->currentMode == Settings::Cool ) 
   {
      temp_cycle.setAbsMinimum(settings->coolCycleMinimum);
      temp_cycle.setHysterisis(settings->coolHyst);
      if ( settings->coolPidEnabled == true ) 
      {
         temp_cycle.setSensor(settings->coolCycleSensor);
         temp_cycle.setHysterisis(settings->coolCycleHyst);
         pid.SetControllerDirection(REVERSE);
      }
      settings->cycleSensor   = settings->coolCycleSensor;
      settings->cycleMaximum  = settings->coolCycleMaximum;
      settings->cycleMinimum  = settings->coolCycleMinimum;
      settings->cycleTime     = settings->coolMinCycleTime;
      settings->outsideCurve  = settings->coolOutsideCurve;
      settings->outsideAdj    = settings->coolOutsideAdj;
      settings->Ki            = settings->coolKi;
      settings->Kp            = settings->coolKp;
      settings->Kd            = settings->coolKd;
      settings->pidEnabled    = settings->coolPidEnabled;
   }
   else 
   {
      temp_cycle.setAbsMaximum(settings->heatCycleMaximum);
      temp_cycle.setHysterisis(settings->heatHyst);
      if ( settings->heatPidEnabled == true ) 
      {
         temp_cycle.setSensor(settings->heatCycleSensor);
         temp_cycle.setHysterisis(settings->heatCycleHyst);
         pid.SetControllerDirection(DIRECT);
      }
      settings->cycleSensor   = settings->heatCycleSensor;
      settings->cycleMaximum  = settings->heatCycleMaximum;
      settings->cycleMinimum  = settings->heatCycleMinimum;
      settings->cycleTime     = settings->heatMinCycleTime;
      settings->outsideCurve  = settings->heatOutsideCurve;
      settings->outsideAdj    = settings->heatOutsideAdj;
      settings->Ki            = settings->heatKi;
      settings->Kp            = settings->heatKp;
      settings->Kd            = settings->heatKd;
      settings->pidEnabled    = settings->heatPidEnabled;
   }
   temp_cycle.setTemp(settings->cycleTemp);
   Serial << __FUNCTION__ << strcpy_P(p_buf, PSTR("Setting minimum cycle time ")) << (unsigned int)settings->cycleTime << endl;
   temp_cycle.setCycleTime(settings->cycleTime);
   Serial << __FUNCTION__ << strcpy_P(p_buf, PSTR("Setting maximum cycle time ")) << (unsigned int)settings->maxCycleTime << endl;
   temp_cycle.setMaxCycleTime(settings->maxCycleTime);

   pid.SetSampleTime(READ_TEMPERATURE_TIME * 1000);
   pid.SetOutputLimits(-15.0, 105.0);
   pid.SetTarget(settings->targetTemp);
   pid.SetTunings(settings->Kp, settings->Ki, settings->Kd);
   if ( settings->currentMode == Settings::Heat ) 
   {
      Serial << __FUNCTION__ << strcpy_P(p_buf, PSTR("Setting pid direct")) << endl;
      pid.SetControllerDirection(DIRECT);
   }
   else 
   {
      Serial << __FUNCTION__ << strcpy_P(p_buf, PSTR("Setting pid reverse")) << endl;
      pid.SetControllerDirection(REVERSE);
   }
#endif
}

bool discoverSensors()
{
   Serial << strcpy_P(p_buf, PSTR("Discovering sensors...")) << endl;
   sensors.discover();
   Serial << strcpy_P(p_buf, PSTR("Sensors found")) << endl;
   sensors.dumpSensors();
   return(false);
}

bool dump_esquare()
{
   eeprom.DisplaySettings();
   return(false);
}

bool show_menu_map()
{
   main_menu->PrintChildren();
}

/**********************************************************************/
/*   Menu  helper  function  which writes the eeprom data out to the  */
/*   eeprom                                                           */
/**********************************************************************/
bool write_eeprom()
{
   eeprom.write();
   return(false);
}

void showCycleStatus()
{
   uint16_t cycle_time = temp_cycle.getMinimumCycleTime();
   int16_t time_remaining = cycle_time - temp_cycle.getCurrentCycleTime();
   if ( time_remaining < 0 ) 
   {
      time_remaining = 0;
   }
   Serial << (temp_cycle.getLastCall() ? strcpy_P(p_buf, PSTR("on")) : strcpy_P(p_buf, PSTR("off")));
   Serial << strcpy_P(p_buf, PSTR(": "));
   if ( time_remaining > 0 ) 
   {
      Serial << time_remaining << strcpy_P(p_buf, PSTR(" of "));
      Serial << cycle_time << strcpy_P(p_buf, PSTR(" secs remaining"));
   }
   else 
   {
      Serial << strcpy_P(p_buf, PSTR("for "));
      Serial << temp_cycle.getCurrentCycleTime() << strcpy_P(p_buf, PSTR(" secs"));
   }
}

void showCycleTemp()
{
   ESquareSpace_t *settings = eeprom.settings();
   if ( settings->pidEnabled ) 
   {
      if ( settings->currentMode == Settings::Heat ) 
      {
         Serial << sensors.read(settings->heatCycleSensor, TempSensors::CURRENT);
      }
      else 
      {
         Serial << sensors.read(settings->coolCycleSensor, TempSensors::CURRENT);
      }
   }
   else 
   {
      Serial << "N/A";
   }
}

void showOutsideTemp()
{
   ESquareSpace_t *settings = eeprom.settings();
   Serial << sensors.read(settings->outsideSensor, TempSensors::CURRENT);
}

void showInsideTemp()
{
   ESquareSpace_t *settings = eeprom.settings();
   Serial << sensors.read(settings->targetSensor, TempSensors::CURRENT);
}

bool voidAction(const char *buf)
{
   return(false);
}

/**********************************************************************/
/*   Menu  helper  program  the shows the current time all formatted  */
/*   pretty.                                                          */
/**********************************************************************/
void showTime()
{
   time_t now = currentDate.now();
   Serial << DateTime::formatTime(now);
}

/**********************************************************************/
/*   Menu  helper  program  the shows the current date all formatted  */
/*   pretty.                                                          */
/**********************************************************************/
void showDate()
{
   time_t now = currentDate.now();
   Serial << DateTime::formatDate(now);
}

/**********************************************************************/
/*   Menu  helper  program  that  parses the passed buffer as a time  */
/*   value and sets the clock                                         */
/**********************************************************************/
bool parseTime(const char *buf)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(p_buf, PSTR("buf: ")) << buf;
   uint16_t dt[7];
   currentDate.getTime(dt);
   dt[Clock::Month]++;
   while (*buf && isspace(*buf))
      buf++;
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(p_buf, PSTR("buf: '")) << buf << "'" << endl;
   if ( *buf != '\0' )
   {
      if ( strlen(buf) > 0 )
      {
//         Serial << strcpy_P(p_buf, PSTR("hours: ")) << buf;
         dt[Clock::Hour] = atol(buf);
         while (*buf != '\0' and *buf != ':')
            buf++;
         if ( *(buf++) != 0 && *buf != 0)
         {
//            Serial << strcpy_P(p_buf, PSTR(", minutes: ")) << buf;
            dt[Clock::Minute] = atol(buf);
            while (*buf != '\0' and *buf != ':')
               buf++;
            if ( *(buf++) != 0 && *buf != 0)
            {
//               Serial << strcpy_P(p_buf, PSTR(", seconds: ")) << buf << endl;
               dt[Clock::Second] = atol(buf);
               currentDate.setTime(dt);
               bool disp_en = display.setEnabled(false);
               currentDate.writeClock(dt);
               currentDate.setTime(currentDate.readClock());
               display.setEnabled(disp_en);
            }
         }
      }
   }
   return(false);
}

void ShowTargetTemp()
{
   float temp = thermostat.getCurrentTempSetting();
   Serial << temp;
}

bool ParseTargetTemp(const char *buf)
{
   float fv = atof(buf);
   if ( fv > 50 && fv < 90 ) 
   {
      ESquareSpace_t *settings = eeprom.settings();
      updateThermostatSettings(settings);
      thermostat.SetOverride(fv);
   }
   return(false);
}


/**********************************************************************/
/*   Menu  helper  program  that  parses the passed buffer as a date  */
/*   value and sets the clock                                         */
/**********************************************************************/
bool parseDate(const char *buf)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(p_buf, PSTR("buf: ")) << buf;
   uint16_t dt[7];
   currentDate.readClock(dt);
   while (*buf && isspace(*buf))
      buf++;
   if ( *buf != '\0' )
   {
      if ( strlen(buf) > 0 )
      {
         dt[Clock::Month] = atol(buf);
         while (*buf != '\0' and *buf != '/')
            buf++;
         if ( *(buf++) != 0 && *buf != 0)
         {
            dt[Clock::Day] = atol(buf);
            while (*buf != '\0' and *buf != '/')
               buf++;
            if ( *(buf++) != 0 && *buf != 0)
            {
               dt[Clock::Year] = atol(buf);
               if ( dt[Clock::Year] < 100 )
                  dt[Clock::Year] += 2000;
               bool disp_en = display.setEnabled(false);
               currentDate.writeClock(dt);
               currentDate.setTime(currentDate.readClock());
               display.setEnabled(disp_en);
            }
         }
      }
   }
   return(false);
}

bool showSensors()
{
   sensors.dumpSensors();
   return(false);
}

bool dump_log()
{
#ifndef NO_LOG
   syslog.DumpLog(false);
#endif
   return(false);
}

bool dump_full_log()
{
#ifndef NO_LOG
   syslog.DumpLog(true);
#endif
   return(false);
}

bool clear_log()
{
#ifndef NO_LOG
   syslog.ClearLog();
#endif
   return(false);
}

bool parse_current_mode(const char *buf)
{
   bool save_settings = true;
   ESquareSpace_t *settings = eeprom.settings();

   while (*buf && isspace(*buf))
      buf++;
   if ( toupper(*buf) == 'H' ) 
   {
      settings->currentMode = Settings::Heat;
      settings->autoMode = false;
      settings->vacationMode = false;
   }
   else if ( toupper(*buf) == 'C' ) 
   {
      settings->currentMode = Settings::Cool;
      settings->autoMode = false;
      settings->vacationMode = false;
   }
   else if ( toupper(*buf) == 'A' ) 
   {
      settings->autoMode = true;
      settings->vacationMode = false;
   }
   else if ( toupper(*buf) == 'V' ) 
   {
      settings->autoMode = false;
      settings->vacationMode = true;
   }
   else 
   {
      Serial << strcpy_P(p_buf, PSTR("Error!! Expecting 'Heat', 'Cool', 'Automatic' or 'Vacation'")) << endl;
      save_settings = false;
   }
   if ( save_settings == true ) 
   {
      updateThermostatSettings(settings);
      write_eeprom();
   }
   return(false);
}

void show_current_mode()
{
   ESquareSpace_t *settings = eeprom.settings();
   
   if ( settings->vacationMode == true ) 
   {
      Serial << strcpy_P(p_buf, PSTR("Vacation- "));
   }
   else if ( settings->autoMode == true ) 
   {
      Serial << strcpy_P(p_buf, PSTR("Automatic- "));
   }
   if ( settings->currentMode == Settings::Heat ) 
   {
      Serial << strcpy_P(p_buf, PSTR("Heat"));
   }
   else
   {
      Serial << strcpy_P(p_buf, PSTR("Cool"));
   }
}

bool initialize_keypad()
{
   ESquareSpace_t *settings = eeprom.settings();
   write_eeprom();
   mpr121.setTTH(settings->keyTouch);
   mpr121.setRTH(settings->keyRelease);
   mpr121.initialize();
   return(false);
}

int main()
{
   uint32_t   lastStartTime = 0;
   uint32_t   lastTime = 0;

   float temp = 32.0;
   int x = 0;
   uint16_t lcnt = 0;

   pid_debug = pid.GetDebugPointer();

   /***********************************************/
   /*   Initialize  the  serial port and show we  */
   /*   had a reset!                              */
   /***********************************************/
   init();
   Serial.begin(115200); //hello
   Serial << endl;
   Serial << endl;
   Serial << strcpy_P(p_buf, PSTR("/************                                    ************/")) << endl;
   Serial << strcpy_P(p_buf, PSTR("/************           R  E  S  E  T            ************/")) << endl;
   Serial << strcpy_P(p_buf, PSTR("/************                                    ************/")) << endl;

   /***********************************************/
   /*   Get the current esquare settings          */
   /***********************************************/
   Serial << strcpy_P(p_buf, PSTR("Fetching settings...")) << endl;

   // Read the ESquare
   ESquareSpace_t *settings = eeprom.settings();
//   if (dbg) Serial << __PRETTY_FUNCTION__ << strcpy_P(p_buf, PSTR("Reading eeprom")) << endl;
   eeprom.read();

   currentDate.getTime();

   /***********************************************/
   /*   If  this  is our very first time to wake  */
   /*   up  after being flashed, set some og the  */
   /*   settings to some sensible values          */
   /***********************************************/
   Serial << strcpy_P(p_buf, PSTR("coolCycleSensor = 0x")) << itoa((uint8_t)settings->coolCycleSensor, ibuf, 16) << endl;

#ifdef PID_DBG
      *pid_debug = true;
#else
      *pid_debug = false;
#endif

   if ( (uint8_t)settings->coolCycleSensor == 0xff ) 
   {
      Serial << strcpy_P(p_buf, PSTR("EEProm empty, creating initial settings:")) << endl;
      settings->heatPidEnabled   = true;
      settings->heatCycleMinimum = 60.0;
      settings->heatCycleMaximum = 140.0;
      settings->heatCycleHyst    = 15.0;
      settings->heatHyst         = 2.0;

      settings->coolPidEnabled   = false;
      settings->coolCycleMinimum = 30.0;
      settings->coolCycleMaximum = 90.0;
      settings->coolCycleHyst    = 5.0;
      settings->coolHyst         = 2.0;

      settings->heatCycleSensor  = 3;
      settings->coolCycleSensor  = 6;
      settings->targetSensor     = 6;
      settings->outsideSensor    = 2;

      settings->heatMinCycleTime = 10;
      settings->coolMinCycleTime = 7;
      settings->maxCycleTime     = 40;

      settings->heatTemps[0]     = 69.0;
      settings->heatTemps[1]     = 72.0;
      settings->heatTemps[2]     = 68.0;
      settings->heatTemps[3]     = 72.0;
      settings->heatTemps[4]     = 68.0;
      settings->heatTimes[1]     = 240;
      settings->heatTimes[2]     = 600;
      settings->heatTimes[3]     = 960;
      settings->heatTimes[4]     = 1260;
      settings->outsideTemp      = 25.0;
      settings->heatOutsideAdj   = 50.0;
      settings->coolOutsideAdj   = 0.0;
      settings->heatOutsideCurve = 0;
      settings->coolOutsideCurve = 0;
      settings->heatKp           = 50.0;
      settings->heatKi           = 0.05;
      settings->heatKd           = 55.0;
      settings->coolKp           = 50.0;
      settings->coolKi           = 0.05;
      settings->coolKd           = 55.0;

      settings->vacationTemp     = 60.0;

      settings->currentMode      = Settings::Heat;
      settings->logTime          = LOG_TEMPERATURE_TIME;
      settings->vacationMode     = false;
      settings->autoMode         = false;
      settings->resetCounter     = -1;

      settings->keyTouch         = 0x3f;
      settings->keyRelease       = 0x2a;
   }
   else 
   {
      settings->resetCounter++;
   }
   eeprom.write();

   /***********************************************/
   /*   Create the Menus                          */
   /***********************************************/
   Serial << strcpy_P(p_buf, PSTR("Building menus...")) << endl;

   MenuEntry mainMenu(MENU_MAINMENU_STR, MenuEntry::MainMenu, NULL);

   MenuEntry mm11(MENU_MM11_STR, MenuEntry::CustomIO, voidAction, showCycleStatus, &mainMenu);
   MenuEntry mm10(MENU_MM10_STR, MenuEntry::CustomIO, voidAction, showCycleTemp, &mainMenu);
   MenuEntry  mm9(MENU_MM9_STR,  MenuEntry::CustomIO, voidAction, showOutsideTemp, &mainMenu);
   MenuEntry  mm8(MENU_MM8_STR,  MenuEntry::CustomIO, voidAction, showInsideTemp, &mainMenu);
   MenuEntry  mm7(MENU_MM7_STR,  MenuEntry::Action, dump_full_log, &mainMenu);
   MenuEntry  mm6(MENU_MM6_STR,  MenuEntry::Action, dump_log, &mainMenu);
   MenuEntry  mm5(MENU_MM5_STR,  MenuEntry::Action, write_eeprom, &mainMenu);
   MenuEntry  mm4(MENU_MM4_STR,  MenuEntry::CustomIO, parseDate, showDate, &mainMenu);
   MenuEntry  mm3(MENU_MM3_STR,  MenuEntry::CustomIO, parseTime, showTime, &mainMenu);
   MenuEntry  mm2(MENU_MM2_STR,  MenuEntry::CustomIO, parse_current_mode, show_current_mode, &mainMenu);
   MenuEntry  mm1(MENU_MM1_STR,  MenuEntry::CustomIO, ParseTargetTemp, ShowTargetTemp, &mainMenu);

   MenuEntry systemMenu(MENU_SYSMENU_STR, MenuEntry::SubMenu, &mainMenu);
   MenuEntry sy7(MENU_SY6_STR, MenuEntry::Int32, (unsigned int*)&settings->resetCounter, &systemMenu);
   MenuEntry sy6(MENU_SY5_STR, MenuEntry::Action, clear_log, &systemMenu);
   MenuEntry sy5(MENU_SY4_STR, MenuEntry::Action, dump_esquare, &systemMenu);
   MenuEntry sy4(MENU_SY3_STR, MenuEntry::Action, show_menu_map, &systemMenu);
   MenuEntry sy3(MENU_SY7_STR, MenuEntry::YesNo, (unsigned int*)pid_debug, &systemMenu);
   MenuEntry sy2(MENU_SY2_STR, MenuEntry::Int8, (unsigned int*)&dbg, &systemMenu);
   MenuEntry sy1(MENU_SY1_STR, MenuEntry::Int32, (unsigned int*)&settings->logTime, &systemMenu);

   MenuEntry keypadMenu(MENU_KYPMENU_STR, MenuEntry::SubMenu, &systemMenu);
   MenuEntry ky3(MENU_KP3_STR, MenuEntry::Action, initialize_keypad, &keypadMenu);
   MenuEntry ky2(MENU_KP2_STR, MenuEntry::Int8, (unsigned int*)&settings->keyRelease, &keypadMenu);
   MenuEntry ky1(MENU_KP1_STR, MenuEntry::Int8, (unsigned int*)&settings->keyTouch, &keypadMenu);

   MenuEntry hystMenu(MENU_HYSTMENU_STR, MenuEntry::SubMenu, &mainMenu);
   MenuEntry hm2(MENU_HM2_STR, MenuEntry::Float, (unsigned int*)&settings->coolHyst, &hystMenu);
   MenuEntry hm1(MENU_HM1_STR, MenuEntry::Float, (unsigned int*)&settings->heatHyst, &hystMenu);

   MenuEntry sensorsMenu(MENU_SENSORSMENU_STR, MenuEntry::SubMenu, &mainMenu);
   MenuEntry sm4(MENU_SM4_STR, MenuEntry::Action, showSensors, &sensorsMenu);
   MenuEntry sm3(MENU_SM3_STR, MenuEntry::Action, discoverSensors, &sensorsMenu);
   MenuEntry sm2(MENU_SM2_STR, MenuEntry::Int8, (unsigned int*)&settings->outsideSensor,  &sensorsMenu);
   MenuEntry sm1(MENU_SM1_STR, MenuEntry::Int8, (unsigned int*)&settings->targetSensor,   &sensorsMenu);

   MenuEntry thermostatMenu(MENU_THERMOSTATMENU_STR, MenuEntry::SubMenu, &mainMenu);

   MenuEntry tms02(MENU_TMS02_STR, MenuEntry::Float, &settings->vacationTemp, &thermostatMenu);
   MenuEntry tms01(MENU_TMS01_STR, MenuEntry::UInt8, (unsigned int*)&settings->maxCycleTime, &thermostatMenu);

   MenuEntry heatingMenu(MENU_HEATINGMENU_STR, MenuEntry::SubMenu, &thermostatMenu);

   MenuEntry thm11(MENU_THM11_STR, MenuEntry::TimeMin, (unsigned int*)&settings->heatTimes[4], &heatingMenu);
   MenuEntry thm10(MENU_THM10_STR, MenuEntry::Float,   &settings->heatTemps[4], &heatingMenu);

   MenuEntry thm09(MENU_THM08_STR, MenuEntry::TimeMin, (unsigned int*)&settings->heatTimes[3], &heatingMenu);
   MenuEntry thm08(MENU_THM07_STR, MenuEntry::Float,   &settings->heatTemps[3], &heatingMenu);

   MenuEntry thm07(MENU_THM05_STR, MenuEntry::TimeMin, (unsigned int*)&settings->heatTimes[2], &heatingMenu);
   MenuEntry thm06(MENU_THM04_STR, MenuEntry::Float,   &settings->heatTemps[2], &heatingMenu);

   MenuEntry thm05(MENU_THM02_STR, MenuEntry::TimeMin, (unsigned int*)&settings->heatTimes[1], &heatingMenu);
   MenuEntry thm04(MENU_THM01_STR, MenuEntry::Float,   &settings->heatTemps[1], &heatingMenu);

   MenuEntry thm03(MENU_THM12_STR, MenuEntry::UInt8,  (unsigned int*)&settings->heatMinCycleTime, &heatingMenu);
   MenuEntry thm02(MENU_HPM4_STR,  MenuEntry::Float,  &settings->heatCycleMaximum, &heatingMenu);
   MenuEntry thm01(MENU_HPM3_STR,  MenuEntry::Float,  &settings->heatCycleMinimum, &heatingMenu);

   MenuEntry heatingPidMenu(MENU_HEATINGPIDMENU_STR, MenuEntry::SubMenu, &heatingMenu);
   MenuEntry heatingAdvancedPidMenu(MENU_HEATINGADVANCEDPIDMENU_STR, MenuEntry::SubMenu, &heatingPidMenu);

   MenuEntry hpm2(MENU_HPM2_STR,    MenuEntry::Float, &settings->heatCycleHyst, &heatingPidMenu);
   MenuEntry hpm1(MENU_HPM1_STR,    MenuEntry::YesNo, (unsigned int*)&settings->heatPidEnabled, &heatingPidMenu);

   MenuEntry hapm6(MENU_HAPM6_STR,  MenuEntry::Float, &settings->heatKp, &heatingAdvancedPidMenu);
   MenuEntry hapm5(MENU_HAPM5_STR,  MenuEntry::Float, &settings->heatKi, &heatingAdvancedPidMenu);
   MenuEntry hapm4(MENU_HAPM4_STR,  MenuEntry::Float, &settings->heatKd, &heatingAdvancedPidMenu);
   MenuEntry hapm3(MENU_HAPM3_STR,  MenuEntry::Float, &settings->heatOutsideCurve, &heatingAdvancedPidMenu);
   MenuEntry hapm2(MENU_HAPM2_STR,  MenuEntry::Float, &settings->heatOutsideAdj, &heatingAdvancedPidMenu);
   MenuEntry hapm1(MENU_HAPM1_STR,  MenuEntry::Int8,  (unsigned int*)&settings->heatCycleSensor, &heatingAdvancedPidMenu);

# if 1
   MenuEntry coolingMenu(MENU_COOLINGMENU_STR, MenuEntry::SubMenu, &thermostatMenu);

   MenuEntry tcm11(MENU_THM11_STR, MenuEntry::TimeMin, (unsigned int*)&settings->coolTimes[4], &coolingMenu);
   MenuEntry tcm10(MENU_THM10_STR, MenuEntry::Float,   &settings->coolTemps[4], &coolingMenu);
                  
   MenuEntry tcm09(MENU_THM08_STR, MenuEntry::TimeMin, (unsigned int*)&settings->coolTimes[3], &coolingMenu);
   MenuEntry tcm08(MENU_THM07_STR, MenuEntry::Float,   &settings->coolTemps[3], &coolingMenu);
                  
   MenuEntry tcm07(MENU_THM05_STR, MenuEntry::TimeMin, (unsigned int*)&settings->coolTimes[2], &coolingMenu);
   MenuEntry tcm06(MENU_THM04_STR, MenuEntry::Float,   &settings->coolTemps[2], &coolingMenu);
                  
   MenuEntry tcm05(MENU_THM02_STR, MenuEntry::TimeMin, (unsigned int*)&settings->coolTimes[1], &coolingMenu);
   MenuEntry tcm04(MENU_THM01_STR, MenuEntry::Float,   &settings->coolTemps[1], &coolingMenu);
                  
   MenuEntry tcm03(MENU_THM12_STR, MenuEntry::UInt8,  (unsigned int*)&settings->heatMinCycleTime, &coolingMenu);
   MenuEntry tcm02(MENU_HPM4_STR,  MenuEntry::Float, &settings->coolCycleMaximum, &coolingMenu);
   MenuEntry tcm01(MENU_HPM3_STR,  MenuEntry::Float, &settings->coolCycleMinimum, &coolingMenu);

//   MenuEntry coolingPidMenu(PSTR("Cooling PID Settings"), MenuEntry::SubMenu, &mainMenu);
   MenuEntry coolingPidMenu(MENU_HEATINGPIDMENU_STR, MenuEntry::SubMenu, &coolingMenu);
   MenuEntry coolingAdvancedPidMenu(MENU_HEATINGADVANCEDPIDMENU_STR, MenuEntry::SubMenu, &coolingPidMenu);

   MenuEntry cpm2(MENU_HPM2_STR,  MenuEntry::Float, &settings->coolHyst, &coolingPidMenu);
   MenuEntry cpm1(MENU_HPM1_STR,  MenuEntry::YesNo, (unsigned int*)&settings->coolPidEnabled, &coolingPidMenu);

   MenuEntry capm6(MENU_HAPM6_STR,  MenuEntry::Float, &settings->coolKp, &coolingAdvancedPidMenu);
   MenuEntry capm5(MENU_HAPM5_STR,  MenuEntry::Float, &settings->coolKi, &coolingAdvancedPidMenu);
   MenuEntry capm4(MENU_HAPM4_STR,  MenuEntry::Float, &settings->coolKd, &coolingAdvancedPidMenu);
   MenuEntry capm3(MENU_HAPM3_STR,  MenuEntry::Float, &settings->coolOutsideCurve, &coolingAdvancedPidMenu);
   MenuEntry capm2(MENU_HAPM2_STR,  MenuEntry::Float, &settings->coolOutsideAdj, &coolingAdvancedPidMenu);
   MenuEntry capm1(MENU_HAPM1_STR,  MenuEntry::Int8,  (unsigned int*)&settings->coolCycleSensor, &coolingAdvancedPidMenu);
# endif

   Serial << strcpy_P(p_buf, PSTR("Done Building menus!!")) << endl;

   /***********************************************/
   /*   Register the various Logging Channels we  */
   /*   will be needing                           */
   /***********************************************/
#ifndef NO_LOG
# ifdef REGCHANNEL
   uint8_t OutsideSensorAvgLog   = syslog.registerChannel_P(OUTSIDE_SENSOR_AVG_LOG, SystemLogger::TEMPERATURE);
   uint8_t OutsideSensorMinLog   = syslog.registerChannel_P(OUTSIDE_SENSOR_MIN_LOG, SystemLogger::TEMPERATURE);
   uint8_t OutsideSensorMaxLog   = syslog.registerChannel_P(OUTSIDE_SENSOR_MAX_LOG, SystemLogger::TEMPERATURE);
                                                                         
   uint8_t InsideSensorAvgLog    =  syslog.registerChannel_P(INSIDE_SENSOR_AVG_LOG, SystemLogger::TEMPERATURE);
   uint8_t InsideSensorMinLog    =  syslog.registerChannel_P(INSIDE_SENSOR_MIN_LOG, SystemLogger::TEMPERATURE);
   uint8_t InsideSensorMaxLog    =  syslog.registerChannel_P(INSIDE_SENSOR_MAX_LOG, SystemLogger::TEMPERATURE);
                                                                         
   uint8_t CycleSensorTgtLog     =  syslog.registerChannel_P(CYCLE_SENSOR_TGT_LOG, SystemLogger::TEMPERATURE);
   uint8_t CycleSensorAvgLog     =  syslog.registerChannel_P(CYCLE_SENSOR_AVG_LOG, SystemLogger::TEMPERATURE);
   uint8_t CycleSensorMinLog     =  syslog.registerChannel_P(CYCLE_SENSOR_MIN_LOG, SystemLogger::TEMPERATURE);
   uint8_t CycleSensorMaxLog     =  syslog.registerChannel_P(CYCLE_SENSOR_MAX_LOG, SystemLogger::TEMPERATURE);
   uint8_t CycleStateLog         =  syslog.registerChannel_P(CYCLE_STATE_LOG,      SystemLogger::UNSIGNED_INT);
                                                                         
   uint8_t TargetTempLog         =  syslog.registerChannel_P(TARGET_TEMP_LOG,   SystemLogger::TEMPERATURE);
   uint8_t PidItermLog           =  syslog.registerChannel_P(PID_ITERM_LOG,     SystemLogger::FLOATING_POINT);
# endif
#endif

   // Initialize the IO Port Object 

#ifndef MINIMAL 
   IOPort::confPortDDR();

   /***********************************************/
   /*   Set the  Log buffer  size. The idea here  */
   /*   is to  make it as  larg as possible, but  */
   /*   still  leave  enough  room for the stack  */
   /*   and  its   autovariables. There will not  */
   /*   be much room left, but who needs memory?  */
   /***********************************************/
#ifndef NO_LOG
   if ( false )
   {
      char xxx_buf[RESERVED_STACK]; // filler just to ensure space for other things later
      uint16_t size = LOG_SIZE;
      while ( ! syslog.setLogSize(size) )
      {
         size -= 512;
         Serial << strcpy_P(p_buf, PSTR("Log size is ")) << size;
         Serial << strcpy_P(p_buf, PSTR(" entries")) << endl;
      }
   }
   else 
   {
      syslog.setLogSize(2600);
   }
#endif
   display.init();
   display.disable();

   /***********************************************/
   /*   Set up the timer2 for the display:        */
   /*                                             */
   /*   WGM Mode:   0                             */
   /*                                             */
   /*   OC2A/B:     Disconnected                  */
   /***********************************************/
   TCCR2A = 0x00;
   TCCR2B = _BV(CS21) | _BV(CS20);
   TIMSK2 = _BV(TOIE2);

   /***********************************************/
   /*   Set  the display to something silly just  */
   /*   to  let  folks  know  that the device is  */
   /*   alive                                     */
   /***********************************************/
   display.setDigit(0, 9);
   display.setDigit(1, 8);
   display.setDigit(2, 7);
   display.setDigit(3, 6);
   display.setDigit(4, 5);
   display.setDigit(5, 4);

   display.setEnabled(true);
   delay(500);
   display.setEnabled(false);

   /***********************************************/
   /*   Initialise the touch pad                  */
   /***********************************************/
//   if (dbg) Serial << strcpy_P(p_buf, PSTR("Initializing touch sensors")) << endl;
   mpr121.enableInterupt();
   mpr121.setTTH(settings->keyTouch);
   mpr121.setRTH(settings->keyRelease);
   mpr121.initialize();

   display.setEnabled(true);
   delay(500);
   display.setEnabled(false);

   /***********************************************/
   /*   Initialist  the  sensors  and  find  out  */
   /*   which ones we have                        */
   /***********************************************/
//   if (dbg) Serial << strcpy_P(p_buf, PSTR("Initializing temp sensors")) << endl;
   sensors.initialize();
   thermostat.initialize(&sensors);
   temp_cycle.initialize(&sensors);

   display.setEnabled(true);
   delay(800);
   for (uint8_t ts = sensors.count(); ts > 0; ts--) 
   {
      display.setEnabled(false);
      delay(50);
      sensors.readNextSensor();
      display.setEnabled(true);
      delay(800);
   }
   for (uint8_t ts = 1; ts <= sensors.count(); ts++) 
   {
      if (dbg) Serial << strcpy_P(p_buf, PSTR("Temp sensor[")) << (int)ts;
      if (dbg) Serial << strcpy_P(p_buf, PSTR("] = ")) << sensors.read(ts) << endl;
   }


   /***********************************************/
   /*   Initialize the Ui object                  */
   /***********************************************/
   ui.initialize(settings);
//   ui.initialize();

   /***********************************************/
   /*   Now reset out all of the interval timers  */
   /***********************************************/
   for (uint8_t x = 0; x < Clock::INTERVAL_COUNTERS; x++)
   {
      currentDate.resetInterval(x);
   }

   /***********************************************/
   /*   Set  the  backup  temperatures  for  the  */
   /*   inside    and   outside   to   something  */
   /*   reasonable, just in case                  */
   /***********************************************/
   Serial << strcpy_P(p_buf, PSTR("Updating Thermostat Settings")) << endl;
   updateThermostatSettings(settings);
   float last_temp = settings->targetTemp;
   float last_outside_temp = sensors.read(settings->outsideSensor);
//   ui.setTemp(0, sensors.read(settings->targetSensor));
//   ui.showSettings();
   
   Serial << endl;
   Serial << strcpy_P(p_buf, PSTR("Current Settings:")) << endl;
   eeprom.DisplaySettings();
   Serial << endl;

#endif

   Serial << strcpy_P(p_buf, PSTR("Setting the starting menu...")) << endl;

   MenuEntry *curr_entry = &mainMenu;
   main_menu = curr_entry;
   
#ifdef THEIRS
   pid.SetSampleTime(READ_TEMPERATURE_TIME * 1000);
   pid.SetOutputLimits(-15.0, 105.0);
   pid.SetTarget(settings->heatTemps[0]);
   pid.SetTunings(settings->Kp, settings->Ki, settings->Kd);
#else
#ifndef DISPLAY_OFF
   display.setEnabled(true);
#endif
#endif

   needPrompt = 1;   // We need a prompt
   while (1)
   {
      char *line_p;
      char line_buf[120];
      uint16_t dt[7];

#ifndef DISPLAY_OFF
//      if ( dbg > 2 ) Serial << "calling ui.tick()" << endl;
      ui.tick();
//      if ( dbg > 2 ) Serial << "returned" << endl;
#endif
      /***********************************************/
      /*   Now  see  if we need a prompt displayed.  */
      /*   If  so,  reset the serial input timeout,  */
      /*   the   line   buffer   and  send  out  an  */
      /*   appropriate prompt.                       */
      /***********************************************/
      if ( needPrompt )
      {
//         char dead_space3;
         Serial << strcpy_P(p_buf, PSTR("\n\n"));
//         Serial << strcpy_P(p_buf, PSTR("curr_menu ="));
//         Serial << (int)curr_entry << endl;
         curr_entry->PrintEntry();
         Serial.clearLineBuf();
         while (Serial.read() >= 0);
         Serial << strcpy_P(p_buf, PSTR("$: "));
         needPrompt = 0;
         line_p = line_buf;
         memset(line_buf, '\0', sizeof(line_buf));

#ifndef DISPLAY_OFF
//         ui.procEvent(TC_WAKEN);
#endif
      }

      /***********************************************/
      /*   Otherwise,  see  if  there  is  any data  */
      /*   available from the rial port.             */
      /***********************************************/
      else if ( Serial.available() )
      {
         /***********************************************/
         /*   Try to read a full line                   */
         /***********************************************/
         int status = 0;
         char ch = Serial.read();
         if ( ch == '\x1b' ) 
         {
            curr_entry = curr_entry->Parent();
            needPrompt = 1;
         }
         else if ( ch == '\x03' ) 
         {
            curr_entry = &mainMenu;
            needPrompt = 1;
         }
         else if ( ch == '\x7f' || ch == '\x08')
         {
            if ( line_p > line_buf ) 
            {
               Serial << '\x08' << ' ' << '\x08';
               line_p--;
               *line_p = '\0';
            }
            else 
            {
               Serial << '\x07';
            }
         }
         else if ( ch != '\r' )
         {
//            Serial << strlen(line_buf) << ".";
//            Serial << sizeof(line_buf) << ":";
            if ( strlen(line_buf) < (sizeof(line_buf) - 2) ) 
            {
               *(line_p++) = ch;
               *line_p = '\0';
               Serial << ch;
            }
            else 
            {
               Serial << '\x07';
            }
         }
         else
         {
            /***********************************************/
            /*   If  we  sre  not in a special mode, then  */
            /*   check what command they entered.          */
            /***********************************************/
//            Serial << "\r\n***********\r\n*** HERE ***\n\r***********\r\n" endl;
            needPrompt = 1;
# ifndef NO_MENU_EXEC
            bool valid;

            if ( strlen(line_buf) > 0 )
            {
               bool change_made = false;
               Serial << endl;
               curr_entry = curr_entry->Execute(line_buf, curr_entry, change_made);
               if ( change_made == true ) 
               {
                  updateThermostatSettings(settings);
               }
            }
            else 
            {
               if ( curr_entry->Parent() != NULL ) 
               {
                  curr_entry = curr_entry->Parent();
               }
            }
# endif
         }
      }

      /***********************************************/
      /*   If  no  prompt was needed, nor was there  */
      /*   any  serial  data  comming in, go handle  */
      /*   the  real  task at hand, controlling the  */
      /*   temperature!                              */
      /***********************************************/
# ifndef NO_EXEC
      else
      {
         uint16_t dt[7];

# ifndef DISPLAY_OFF
         if ( false || mpr121.haveChange() )
         {
            display.disable();
            delay(50);
            uint16_t keys = mpr121.readKeyboard();
            Serial << strcpy_P(p_buf, PSTR("keyboard = 0x")) << itoa(keys, ibuf, 16) << endl;
            ui.haveKeyPress(keys);
            display.touch();
         }
# endif

         bool log_temps = false;
         if ( currentDate.getInterval(Clock::READ_SENSOR_INTERVAL) >= READ_SENSOR_TIME )
         {
            currentDate.resetInterval(Clock::READ_SENSOR_INTERVAL);
            sensors.readNextSensor();
         }
         if ( currentDate.getInterval(Clock::READ_TEMPERATURE_INTERVAL) >= READ_TEMPERATURE_TIME )
         {
            bool force_off = false;

            currentDate.resetInterval(Clock::READ_TEMPERATURE_INTERVAL);
//            bool disp_en = display.setEnabled(false);
//            display.setEnabled(disp_en);
            float curr_temp = sensors.read(settings->targetSensor, TempSensors::CURRENT);

            /***********************************************/
            /*   Do  a sanity check, if the sensor is way  */
            /*   off   the  target,  just  use  the  last  */
            /*   reading                                   */
            /***********************************************/
            if ( curr_temp > 150 || curr_temp < -150 ) 
            {
               curr_temp = last_temp;
            }
            last_temp = curr_temp;

            if ( dbg > 3 ) 
            {
               Serial << strcpy_P(p_buf, PSTR("\r\nSensors:")) << endl;
               for (uint8_t ts = 1; ts <= sensors.count(); ts++) 
               {
                  Serial << strcpy_P(p_buf, PSTR("Temp sensor[")) << (int)ts;
                  Serial << strcpy_P(p_buf, PSTR("] = ")) << sensors.read(ts, TempSensors::CURRENT, false) << endl;
               }
            }

            if ( dbg > 0 ) 
            {
               Serial << strcpy_P(p_buf, PSTR("updating temp_cycle! Time: ")) << DateTime::formatTime(currentDate.getTime());
               Serial << strcpy_P(p_buf, PSTR(", target sensor = ")) << settings->targetSensor;
               Serial << strcpy_P(p_buf, PSTR(", target temp = ")) << settings->targetTemp;
               Serial << strcpy_P(p_buf, PSTR(", curr_temp = ")) << curr_temp << endl;
            }

            /***********************************************/
            /*   If  we  are five degrees past our target  */
            /*   temp, turn off the heat or cooling        */
            /***********************************************/
            float cur_cycle = 0.0;
            bool log_now = false;
/*
            if ( settings->currentMode == Settings::Heat && curr_temp > (settings->targetTemp + 5.0) ) 
            {
               if (dbg) Serial << strcpy_P(p_buf, PSTR("ACK!! Something is wrong! Turn off the heat")) << endl;
               temp_cycle.call4Heat(false);   
               if ( settings->autoMode ) 
               {
                  settings->currentMode = Settings::Cool;
               }
            }
            else if ( settings->currentMode == Settings::Cool && curr_temp < (settings->targetTemp - 5.0) ) 
            {
               if (dbg) Serial << strcpy_P(p_buf, PSTR("ACK!! Something is wrong! Turn off the cooling")) << endl;
               temp_cycle.call4Cool(false);   
               if ( settings->autoMode ) 
               {
                  settings->currentMode = Settings::Heat;
               }
            }
            else 
*/
            {
               /***********************************************/
               /*   Check  if we need to cancel the Override  */
               /*   assuming one is set.                      */
               /***********************************************/
               if ( thermostat.ReachedOverride(curr_temp) ) 
               {
                  thermostat.CancelOverride();
               }

               float new_setting = thermostat.getCurrentTempSetting();
               if ( settings->targetTemp != new_setting)
               {
                  if (dbg > 2) Serial << strcpy_P(p_buf, PSTR("Target changed, forcing a log entry!")) << endl;
                  log_now = true;
//                  Serial << "1: log_now = " << log_now << endl;
               }
               settings->targetTemp = new_setting;
               if ( settings->pidEnabled ) 
               {
                  /***********************************************/
                  /*   Each   of   the   pid   modes   use  the  */
                  /*   temperature    from    outside   sensor.  */
                  /*   Fortunately  for  us,  if the sensore is  */
                  /*   not  defined (set to 0), the temperature  */
                  /*   returned is 0.                            */
                  /***********************************************/
                  pid.SetTarget(settings->targetTemp);

                  /***********************************************/
                  /*   invert  the  outside temp in the case of  */
                  /*   heating  so  that it grows as  the temps  */
                  /*   drop.                                     */
                  /***********************************************/
                  float outside_temp = (settings->currentMode == Settings::Heat) ? 
                                        sensors.read(settings->outsideSensor, TempSensors::CURRENT) :
                                      -(sensors.read(settings->outsideSensor, TempSensors::CURRENT));
                  if (dbg > 1) Serial << strcpy_P(p_buf, PSTR("outside_temp = ")) << outside_temp;
                  if (dbg > 1) Serial << strcpy_P(p_buf, PSTR(", outside curve = ")) << settings->outsideCurve;
                  cur_cycle = pid.Compute(curr_temp);
                  if (dbg > 1) Serial << strcpy_P(p_buf, PSTR(", pid result = ")) << cur_cycle;
                  float offset = settings->outsideAdj - outside_temp;
                  offset = pow(offset, 1 + settings->outsideCurve);
                  cur_cycle += offset + settings->targetTemp;
                  if (dbg > 1) Serial << strcpy_P(p_buf, PSTR(", target temp = ")) << settings->targetTemp;
                  if (dbg > 1) Serial << strcpy_P(p_buf, PSTR(", offset = ")) << offset;
                  if (dbg > 1) Serial << strcpy_P(p_buf, PSTR(", raw calculated cycle_temp = ")) << cur_cycle << endl;
                  if ( cur_cycle < settings->cycleMinimum ) 
                  {
                     cur_cycle = settings->cycleMinimum;
                  }
                  if ( cur_cycle > settings->cycleMaximum ) 
                  {
                     cur_cycle = settings->cycleMaximum;
                  }
                  temp_cycle.setTemp(cur_cycle);
                  if ( temp_cycle.update() == true )
                  {
                     log_now = true;
                  }
               }
               else 
               {
                  cur_cycle = settings->targetTemp;
                  temp_cycle.setTemp(cur_cycle);
                  if ( temp_cycle.update() == true )
                  {
//                     thermostat.CancelOverride();
                     log_now = true;
                  }
               }
               if ( dbg > 2 ) 
               {
                  Serial << strcpy_P(p_buf, PSTR("limited calculated cycle_temp = ")) << cur_cycle << endl;
                  Serial << strcpy_P(p_buf, PSTR("timestamp: ")) << currentDate.getTime();
                  Serial << strcpy_P(p_buf, PSTR(", date_time: ")) << DateTime::formatTime(currentDate.getTime());
                  Serial << strcpy_P(p_buf, PSTR(", heater: ")) << (temp_cycle.getLastCall() ? "on" : "off"); 
                  Serial << strcpy_P(p_buf, PSTR(", target temp: ")) << settings->targetTemp; 
                  Serial << strcpy_P(p_buf, PSTR(", inside temp: ")) << curr_temp; 
                  Serial << strcpy_P(p_buf, PSTR(", outside temp: ")) << sensors.read(settings->outsideSensor, TempSensors::CURRENT);
                  Serial << strcpy_P(p_buf, PSTR(", radiator temp: ")) << sensors.read(settings->heatCycleSensor, TempSensors::CURRENT);
                  Serial << strcpy_P(p_buf, PSTR(", radiator target: ")) << cur_cycle;
                  Serial << endl ;
               }
            }

//            Serial << "Log interval: " << (int32_t)currentDate.getInterval(Clock::LOG_TEMPERATURE_INTERVAL) << endl;
//            Serial << "Log time: " << LOG_TEMPERATURE_TIME << endl;
//            Serial << "2: log_now = " << log_now << endl;
            if ( dbg > 2 && log_now ) 
            {
               Serial << strcpy_P(p_buf, PSTR("Logging is being forced!!")) << endl;
            }
            if ( log_now == true || (currentDate.getInterval(Clock::LOG_TEMPERATURE_INTERVAL) >= settings->logTime) ) 
            {
               float temp;
               currentDate.resetInterval(Clock::LOG_TEMPERATURE_INTERVAL);
#ifndef NO_LOG
//               Serial << "OutsideSensorAvgLog = " << (int)OutsideSensorAvgLog << endl;
               temp = sensors.read(settings->outsideSensor, TempSensors::CURRENT);
               syslog.LogData(OutsideSensorAvgLog, temp);
//                  Serial << "OutsideSensorMinLog = " << (int)OutsideSensorMinLog << endl;
               temp = sensors.read(settings->outsideSensor, TempSensors::MINIMUM);
               syslog.LogData(OutsideSensorMinLog, temp);
//                  Serial << "OutsideSensorMaxLog = " << (int)OutsideSensorMaxLog << endl;
               temp = sensors.read(settings->outsideSensor, TempSensors::MAXIMUM);
               syslog.LogData(OutsideSensorMaxLog, temp);

               temp = sensors.read(settings->targetSensor, TempSensors::AVERAGE);
//               Serial << "InsideSensorAvgLog = " << temp << endl;
               syslog.LogData(InsideSensorAvgLog, temp);

               temp = sensors.read(settings->targetSensor, TempSensors::MINIMUM);
//               Serial << "InsideSensorMinLog = " << temp << endl;
               syslog.LogData(InsideSensorMinLog, temp);

               temp = sensors.read(settings->targetSensor, TempSensors::MAXIMUM);
//               Serial << "InsideSensorMaxLog = " << temp << endl;
               syslog.LogData(InsideSensorMaxLog, temp);

//               syslog.LogData(CycleSensorTgtLog, cur_cycle);
               temp = sensors.read(settings->heatCycleSensor, TempSensors::AVERAGE);
               syslog.LogData(CycleSensorAvgLog, temp);
               temp = sensors.read(settings->heatCycleSensor, TempSensors::MINIMUM);
               syslog.LogData(CycleSensorMinLog, temp);
               temp = sensors.read(settings->heatCycleSensor, TempSensors::MAXIMUM);
               syslog.LogData(CycleSensorMaxLog, temp);
               syslog.LogData(CycleStateLog, (uint16_t)temp_cycle.getLastCall());

               syslog.LogData(TargetTempLog, settings->targetTemp);
//                  syslog.LogData(PidItermLog, settings->targetTemp);
               sensors.clearTotals();
#endif
            }
            if ( dbg > 2 && log_now ) 
            {
               Serial << strcpy_P(p_buf, PSTR("Logging complete")) << endl;
            }
         }
      }
# endif
   }
}




