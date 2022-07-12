#include "configuration.h"
#include "esp_log.h"
#include <iostream>
#include <map>

#include "display.h"
#include "DBGPorts.h"
#include "IOPorts.h"
#include "PID.h"
#include "PT7C4339.h"
#include "SensorData.h"

using namespace std;

#define TAG __PRETTY_FUNCTION__

static int     times[5];
static float   temps[5];

extern PID pid;
extern Configuration *config;

void show_current_sensors(std::ostream& d_out);
void showTemp(Variant& sensor, std::ostream& d_out);
void showOutsideTemp(std::ostream& d_out);
void showCycleTemp(std::ostream& d_out);
void showRoomTemp(std::ostream& d_out);
void showTargetTemp(std::ostream& d_out);
void showMode(std::ostream& d_out);
bool setMode(std::string);
void showTime(std::ostream& d_out);
bool setTime(std::string);
bool voidAction(std::string);

namespace confMenu
{
   /*************/
   /* Main Mena */
   /*************/
   MenuEntry mainMenu((const char*)"Configuration",            (const char*)"main",       MenuEntry::MainMenu);

   MenuEntry hdnEntry00((const char*)"x", (const char*)"cyc_hyst",      MenuEntry::Int,   &Configuration::m_cycleHyst,    &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry01((const char*)"x", (const char*)"pid_enable",    MenuEntry::YesNo, &Configuration::m_pidEnabled,   &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry02((const char*)"x", (const char*)"kp",            MenuEntry::Float, &Configuration::m_Kp,           &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry03((const char*)"x", (const char*)"ki",            MenuEntry::Float, &Configuration::m_Ki,           &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry04((const char*)"x", (const char*)"kd",            MenuEntry::Float, &Configuration::m_Kd,           &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry05((const char*)"x", (const char*)"out_curve",     MenuEntry::Float, &Configuration::m_outsideCurve, &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry06((const char*)"x", (const char*)"out_adj",       MenuEntry::Float, &Configuration::m_outsideAdj,   &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry07((const char*)"x", (const char*)"cyc_sensor",    MenuEntry::String, &Configuration::m_cycleSensor, &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry08((const char*)"x", (const char*)"cyc_min_time",  MenuEntry::Int,   &Configuration::m_cycleMinTime, &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry09((const char*)"x", (const char*)"cyc_max_time",  MenuEntry::Int,   &Configuration::m_cycleMaxTime, &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry11((const char*)"x", (const char*)"cyc_min_temp",  MenuEntry::Float, &Configuration::m_cycleMin,     &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry12((const char*)"x", (const char*)"cyc_max_temp",  MenuEntry::Float, &Configuration::m_cycleMax,     &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry13((const char*)"x", (const char*)"heat_override", MenuEntry::Float, &Configuration::m_heatTimes[0], &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry14((const char*)"x", (const char*)"cool_override", MenuEntry::Float, &Configuration::m_coolTimes[0], &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry15((const char*)"x", (const char*)"heat_temp",     MenuEntry::Float, &Configuration::m_heatTemps[0], &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry16((const char*)"x", (const char*)"heat_time",     MenuEntry::Float, &Configuration::m_heatTimes[0], &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry17((const char*)"x", (const char*)"cool_temp",     MenuEntry::Float, &Configuration::m_coolTemps[0], &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry18((const char*)"x", (const char*)"cool_time",     MenuEntry::Int,   &Configuration::m_coolTimes[0], &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry19((const char*)"x", (const char*)"vacation_mode", MenuEntry::YesNo, &Configuration::m_vacationMode, &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));
   MenuEntry hdnEntry20((const char*)"x", (const char*)"ovr_offset",    MenuEntry::Float, &Configuration::m_overideOffset, &mainMenu, NULL, (MenuEntry::Invisible | MenuEntry::NoPermanence));


   MenuEntry resetEntry((const char*)"reset_count",            (const char*)"rst_count",  MenuEntry::String,   &Configuration::m_resetCount, &mainMenu);
   MenuEntry tzOffsetEntry((const char*)"Tz Minutes",          (const char*)"tz_minutes", MenuEntry::Int,      &Configuration::m_tzMinutes,  &mainMenu);
   MenuEntry dateTimeEntry((const char*)"Date Time",           (const char*)"date_time",  MenuEntry::CustomIO, setTime, showTime,            &mainMenu);
   MenuEntry desplyEnbl((const char*)"Force On",               (const char*)"force_on",   MenuEntry::YesNo,    &Configuration::m_forceOn,    &mainMenu);
   MenuEntry ssidEntry((const char*)"SSID",                    (const char*)"ssid",       MenuEntry::String,   &Configuration::m_ssid,       &mainMenu);
   MenuEntry wkeyEntry((const char*)"WPA KEY",                 (const char*)"wpa_key",    MenuEntry::String,   &Configuration::m_wapkey,     &mainMenu);
   MenuEntry modeEntry((const char*)"Mode (Heat,Cool)",        (const char*)"mode",       MenuEntry::CustomIO, setMode,    showMode,         &mainMenu);
   MenuEntry outTempEntry((const char*)"Outside Temperature",  (const char*)"outside",    MenuEntry::CustomIO, voidAction, showOutsideTemp,  &mainMenu);
   MenuEntry cycleTempEntry((const char*)"Cycle Temperature",  (const char*)"cycle",      MenuEntry::CustomIO, voidAction, showCycleTemp,    &mainMenu);
   MenuEntry roomTempEntry((const char*)"Room Temperature",    (const char*)"room",       MenuEntry::CustomIO, voidAction, showRoomTemp,      &mainMenu);
   MenuEntry tgtTempEntry((const char*)"Set Temperature",     (const char*)"target",     MenuEntry::CustomIO, voidAction, showTargetTemp,   &mainMenu);
   MenuEntry thermMenu((const char*)"Thermostat Settings",     (const char*)"thermostat", MenuEntry::SubMenu,  &mainMenu);
   MenuEntry mqttMenu((const char*)"MQTT Settings",            (const char*)"mqtt",       MenuEntry::SubMenu,  &mainMenu);
   
   /******************************/
   /* General Themostat settings */
   /******************************/
   MenuEntry tme01((const char*)"Send Status  Messages",    (const char*)"send_status",       MenuEntry::YesNo,    &Configuration::m_SendStatus,      &thermMenu);
   MenuEntry tme02((const char*)"Sample Interval (secs)",   (const char*)"samp_int",         MenuEntry::UInt,     &Configuration::m_sampleInterval,  &thermMenu);
   MenuEntry tme03((const char*)"Report Interval (secs)",   (const char*)"rept_int",         MenuEntry::UInt,     &Configuration::m_reportInterval,  &thermMenu);
   MenuEntry tme04((const char*)"Set Temperature",          (const char*)"man_temp",         MenuEntry::Float,    &Configuration::m_setTemp,         &thermMenu);
   MenuEntry tme05((const char*)"Room Device Id",           (const char*)"target_sensor",    MenuEntry::String,   &Configuration::m_targetSensor,    &thermMenu);
   MenuEntry tme06((const char*)"Outside Device Id",        (const char*)"outside_sensor",   MenuEntry::String,   &Configuration::m_outsideSensor,   &thermMenu);
   MenuEntry tme07((const char*)"Sensor IO Port",           (const char*)"sens_port",        MenuEntry::UInt,     &Configuration::m_sensorPort,      &thermMenu);
   MenuEntry tme08((const char*)"Vacation Temp",            (const char*)"vacation_temp",    MenuEntry::Float,    &Configuration::m_vacationTemp,    &thermMenu);
   MenuEntry tme09((const char*)"Remote Relays",            (const char*)"rmt_relays",       MenuEntry::YesNo,    &Configuration::m_remoteRelays,    &thermMenu);
   MenuEntry sensorMenu((const char*)"Local Sensors",       (const char*)"sensors",          MenuEntry::SubMenu,  &thermMenu);
   MenuEntry rmtSensorMenu((const char*)"RemoteSensors",    (const char*)"rmt_sensors",      MenuEntry::SubMenu,  &thermMenu);
   MenuEntry heatingMenu((const char*)"Heater Settings",    (const char*)"heater_set",       MenuEntry::SubMenu,  &thermMenu);
   MenuEntry coolingMenu((const char*)"Cooler Settings",    (const char*)"cooler_set",       MenuEntry::SubMenu,  &thermMenu);
   /****************************/
   /* Heater Specific Settings */
   /****************************/
   MenuEntry thm12((const char*)"Time4",           (const char*)"heat_time4", MenuEntry::TimeMin, &Configuration::m_heatTimes[4], &heatingMenu);
   MenuEntry thm11((const char*)"Temp4",           (const char*)"heat_temp4", MenuEntry::Float,   &Configuration::m_heatTemps[4], &heatingMenu);

   MenuEntry thm10((const char*)"Time3",           (const char*)"heat_time3", MenuEntry::TimeMin, &Configuration::m_heatTimes[3], &heatingMenu);
   MenuEntry thm09((const char*)"Temp3",           (const char*)"heat_temp3", MenuEntry::Float,   &Configuration::m_heatTemps[3], &heatingMenu);

   MenuEntry thm08((const char*)"Time2",           (const char*)"heat_time2", MenuEntry::TimeMin, &Configuration::m_heatTimes[2], &heatingMenu);
   MenuEntry thm07((const char*)"Temp2",           (const char*)"heat_temp2", MenuEntry::Float,   &Configuration::m_heatTemps[2], &heatingMenu);

   MenuEntry thm06((const char*)"Time1",           (const char*)"heat_time1", MenuEntry::TimeMin, &Configuration::m_heatTimes[1], &heatingMenu);
   MenuEntry thm05((const char*)"Temp1",           (const char*)"heat_temp1", MenuEntry::Float,   &Configuration::m_heatTemps[1], &heatingMenu);

   MenuEntry thm04((const char*)"Max Cycle Time",  (const char*)"heat_max_time", MenuEntry::UInt,  &Configuration::m_heatMaxCycleTime, &heatingMenu);
   MenuEntry thm03((const char*)"Min Cycle Time",  (const char*)"heat_min_time", MenuEntry::UInt,  &Configuration::m_heatMinCycleTime, &heatingMenu);
   MenuEntry thm02((const char*)"Max Cycle Temp",  (const char*)"heat_max_temp", MenuEntry::Float, &Configuration::m_heatCycleMaximum, &heatingMenu);
   MenuEntry thm01((const char*)"Min Cycle Temp",  (const char*)"heat_min_temp", MenuEntry::Float, &Configuration::m_heatCycleMinimum, &heatingMenu);
   MenuEntry thm00((const char*)"Hystorisis",      (const char*)"heat_hyst", MenuEntry::Float, &Configuration::m_heatCycleHyst, &heatingMenu);

   /***********************/
   /* Heater Pid Settings */
   /***********************/
   MenuEntry heatingPidMenu((const char*)"PID Settings", (const char*)"heat_pid_set", MenuEntry::SubMenu, &heatingMenu);
   MenuEntry heatingAdvancedPidMenu((const char*)"AdvancedSettings", (const char*)"heat_pid_advanced", MenuEntry::SubMenu, &heatingPidMenu);

   MenuEntry hpm1((const char*)"Enabled",       (const char*)"heat_pid_en",   MenuEntry::YesNo, &Configuration::m_heatPidEnabled, &heatingPidMenu);

   MenuEntry hapm6((const char*)"Kp",           (const char*)"heat_kp",       MenuEntry::Float,  &Configuration::m_heatKp,           &heatingAdvancedPidMenu);
   MenuEntry hapm5((const char*)"Ki",           (const char*)"heat_ki",       MenuEntry::Float,  &Configuration::m_heatKi,           &heatingAdvancedPidMenu);
   MenuEntry hapm4((const char*)"Kd",           (const char*)"heat_kd",       MenuEntry::Float,  &Configuration::m_heatKd,           &heatingAdvancedPidMenu);
   MenuEntry hapm3((const char*)"Curve",        (const char*)"heat_curve",    MenuEntry::Float,  &Configuration::m_heatOutsideCurve, &heatingAdvancedPidMenu);
   MenuEntry hapm2((const char*)"Adjust",       (const char*)"heat_adjust",   MenuEntry::Float,  &Configuration::m_heatOutsideAdj,   &heatingAdvancedPidMenu);
   MenuEntry hapm1((const char*)"Sensor",       (const char*)"heat_sensor",   MenuEntry::String, &Configuration::m_heatCycleSensor,  &heatingAdvancedPidMenu);

   /*****************************/
   /* Cooling Specific Settings */
   /*****************************/
   MenuEntry chm12((const char*)"Time4",           (const char*)"cool_time4", MenuEntry::TimeMin, &Configuration::m_coolTimes[4], &coolingMenu);
   MenuEntry chm11((const char*)"Temp4",           (const char*)"cool_temp4", MenuEntry::Float,   &Configuration::m_coolTemps[4], &coolingMenu);

   MenuEntry chm10((const char*)"Time3",           (const char*)"cool_time3", MenuEntry::TimeMin, &Configuration::m_coolTimes[3], &coolingMenu);
   MenuEntry chm09((const char*)"Temp3",           (const char*)"cool_temp3", MenuEntry::Float,   &Configuration::m_coolTemps[3], &coolingMenu);

   MenuEntry chm08((const char*)"Time2",           (const char*)"cool_time2", MenuEntry::TimeMin, &Configuration::m_coolTimes[2], &coolingMenu);
   MenuEntry chm07((const char*)"Temp2",           (const char*)"cool_temp2", MenuEntry::Float,   &Configuration::m_coolTemps[2], &coolingMenu);

   MenuEntry chm06((const char*)"Time1",           (const char*)"cool_time1", MenuEntry::TimeMin, &Configuration::m_coolTimes[1], &coolingMenu);
   MenuEntry chm05((const char*)"Temp1",           (const char*)"cool_temp1", MenuEntry::Float,   &Configuration::m_coolTemps[1], &coolingMenu);

   MenuEntry chm04((const char*)"Max Cycle Time",  (const char*)"cool_max_time", MenuEntry::UInt,  &Configuration::m_coolMaxCycleTime, &coolingMenu);
   MenuEntry chm03((const char*)"Min Cycle Time",  (const char*)"cool_min_time", MenuEntry::UInt,  &Configuration::m_coolMinCycleTime, &coolingMenu);
   MenuEntry chm02((const char*)"Max Cycle Temp",  (const char*)"cool_max_temp", MenuEntry::Float, &Configuration::m_coolCycleMaximum, &coolingMenu);
   MenuEntry chm01((const char*)"Min Cycle Temp",  (const char*)"cool_min_temp", MenuEntry::Float, &Configuration::m_coolCycleMinimum, &coolingMenu);
   MenuEntry chm00((const char*)"Hystorisis",      (const char*)"cool_hyst", MenuEntry::Float,     &Configuration::m_coolCycleHyst,    &coolingMenu);

   MenuEntry coolingPidMenu((const char*)"PID Settings", (const char*)"cool_pid_set", MenuEntry::SubMenu, &coolingMenu);
   MenuEntry coolingAdvancedPidMenu((const char*)"AdvancedSettings", (const char*)"cool_pid_advanced", MenuEntry::SubMenu, &coolingPidMenu);

   MenuEntry cpm1((const char*)"Enabled",       (const char*)"cool_pid_en",      MenuEntry::YesNo, &Configuration::m_coolPidEnabled, &coolingPidMenu);

   MenuEntry capm6((const char*)"Kp",              (const char*)"cool_kp",       MenuEntry::Float,  &Configuration::m_coolKp,           &coolingAdvancedPidMenu);
   MenuEntry capm5((const char*)"Ki",              (const char*)"cool_ki",       MenuEntry::Float,  &Configuration::m_coolKi,           &coolingAdvancedPidMenu);
   MenuEntry capm4((const char*)"Kd",              (const char*)"cool_kd",       MenuEntry::Float,  &Configuration::m_coolKd,           &coolingAdvancedPidMenu);
   MenuEntry capm3((const char*)"Curve",           (const char*)"cool_curve",    MenuEntry::Float,  &Configuration::m_coolOutsideCurve, &coolingAdvancedPidMenu);
   MenuEntry capm2((const char*)"Adjust",          (const char*)"cool_adjust",   MenuEntry::Float,  &Configuration::m_coolOutsideAdj,   &coolingAdvancedPidMenu);
   MenuEntry capm1((const char*)"Sensor",          (const char*)"cool_sensor",   MenuEntry::String, &Configuration::m_coolCycleSensor,  &coolingAdvancedPidMenu);

   /*************************/
   /* Remote Sensor Display */
   /*************************/
   MenuEntry rsensEntry((const char*)"RemoteSensors",     (const char*)"rsensors",    MenuEntry::CustomIO, voidAction, show_current_sensors, &rmtSensorMenu);

   /*****************/
   /* Local Sensors */
   /*****************/
   MenuEntry sname5Entry((const char*)"Sensor5 Name",     (const char*)"sensor_name5",    MenuEntry::String, &Configuration::m_sensorName5, &sensorMenu);
   MenuEntry saddr5Entry((const char*)"Sensor5 Addr",     (const char*)"sensor_addr5",    MenuEntry::String, &Configuration::m_sensorAddr5, &sensorMenu);
   MenuEntry sname4Entry((const char*)"Sensor4 Name",     (const char*)"sensor_name4",    MenuEntry::String, &Configuration::m_sensorName4, &sensorMenu);
   MenuEntry saddr4Entry((const char*)"Sensor4 Addr",     (const char*)"sensor_addr4",    MenuEntry::String, &Configuration::m_sensorAddr4, &sensorMenu);
   MenuEntry sname3Entry((const char*)"Sensor3 Name",     (const char*)"sensor_name3",    MenuEntry::String, &Configuration::m_sensorName3, &sensorMenu);
   MenuEntry saddr3Entry((const char*)"Sensor3 Addr",     (const char*)"sensor_addr3",    MenuEntry::String, &Configuration::m_sensorAddr3, &sensorMenu);
   MenuEntry sname2Entry((const char*)"Sensor2 Name",     (const char*)"sensor_name2",    MenuEntry::String, &Configuration::m_sensorName2, &sensorMenu);
   MenuEntry saddr2Entry((const char*)"Sensor2 Addr",     (const char*)"sensor_addr2",    MenuEntry::String, &Configuration::m_sensorAddr2, &sensorMenu);
   MenuEntry sname1Entry((const char*)"Sensor1 Name",     (const char*)"sensor_name1",    MenuEntry::String, &Configuration::m_sensorName1, &sensorMenu);
   MenuEntry saddr1Entry((const char*)"Sensor1 Addr",     (const char*)"sensor_addr1",    MenuEntry::String, &Configuration::m_sensorAddr1, &sensorMenu);

   /*********************/
   /* MQTT Bus settings */
   /*********************/
   MenuEntry passEntry((const char*)"MQTT Password",        (const char*)"mqtt_pass",        MenuEntry::String, &Configuration::m_mqttPass,   &mqttMenu);
   MenuEntry userEntry((const char*)"MQTT User",            (const char*)"mqtt_user",        MenuEntry::String, &Configuration::m_mqttUser,   &mqttMenu);
   MenuEntry brkrEntry((const char*)"MQTT Broker",          (const char*)"mqtt_broker",      MenuEntry::String, &Configuration::m_mqttBroker, &mqttMenu);
   MenuEntry ltpcEntry((const char*)"MQTT Log Topic",       (const char*)"mqtt_log_topic",   MenuEntry::String, &Configuration::m_mqttLogTopic,  &mqttMenu);
   MenuEntry stpcEntry((const char*)"MQTT Status Topic",    (const char*)"mqtt_stat_topic",  MenuEntry::String, &Configuration::m_mqttStatusTopic,  &mqttMenu);
   MenuEntry ftpcEntry((const char*)"MQTT Furnace Topic",   (const char*)"mqtt_furn_topic",  MenuEntry::String, &Configuration::m_mqttFurnaceTopic,  &mqttMenu);
   MenuEntry ttpcEntry((const char*)"MQTT Temp Topic",      (const char*)"mqtt_temp_topic",  MenuEntry::String, &Configuration::m_mqttTempTopic,  &mqttMenu);
   MenuEntry pingEntry((const char*)"MQTT Ping Topic",      (const char*)"ping_topic",       MenuEntry::String, &Configuration::m_pingTopic,  &mqttMenu);
   MenuEntry idntEntry((const char*)"MQTT Identity",        (const char*)"mqtt_ident",       MenuEntry::String, &Configuration::m_mqttIdent,  &mqttMenu);
}

Variant Configuration::m_ssid;
Variant Configuration::m_wapkey;

Variant Configuration::m_mqttBroker;
Variant Configuration::m_mqttUser;
Variant Configuration::m_mqttPass;
Variant Configuration::m_mqttIdent;
Variant Configuration::m_mqttTopic;
Variant Configuration::m_mqttLogTopic;
Variant Configuration::m_mqttStatusTopic;
Variant Configuration::m_mqttFurnaceTopic;
Variant Configuration::m_mqttTempTopic;
Variant Configuration::m_pingTopic;
Variant Configuration::m_tzMinutes;

Variant Configuration::m_outsideSensor;
Variant Configuration::m_targetSensor;

Variant Configuration::m_SendStatus;
Variant Configuration::m_sampleInterval;
Variant Configuration::m_reportInterval;
Variant Configuration::m_outsideTemp;
Variant Configuration::m_setTemp;
Variant Configuration::m_sensorPort;
Variant Configuration::m_mode;
Variant Configuration::m_remoteRelays;

Variant Configuration::m_coolTimes[5];
Variant Configuration::m_coolTemps[5];
                       
Variant Configuration::m_coolMaxCycleTime;
Variant Configuration::m_coolMinCycleTime;
Variant Configuration::m_coolCycleMaximum;
Variant Configuration::m_coolCycleMinimum;
                       
Variant Configuration::m_coolCycleHyst;
Variant Configuration::m_coolPidEnabled;
Variant Configuration::m_coolKp;
Variant Configuration::m_coolKi;
Variant Configuration::m_coolKd;
Variant Configuration::m_coolOutsideCurve;
Variant Configuration::m_coolOutsideAdj;
Variant Configuration::m_coolCycleSensor;

Variant Configuration::m_heatTimes[5];
Variant Configuration::m_heatTemps[5];

Variant Configuration::m_heatMaxCycleTime;
Variant Configuration::m_heatMinCycleTime;
Variant Configuration::m_heatCycleMaximum;
Variant Configuration::m_heatCycleMinimum;

Variant Configuration::m_heatCycleHyst;
Variant Configuration::m_heatPidEnabled;
Variant Configuration::m_heatKp;
Variant Configuration::m_heatKi;
Variant Configuration::m_heatKd;
Variant Configuration::m_heatOutsideCurve;
Variant Configuration::m_heatOutsideAdj;
Variant Configuration::m_heatCycleSensor;

Variant Configuration::m_vacationTemp;
Variant Configuration::m_vacationMode;
Variant Configuration::m_currentMode;

Variant Configuration::m_sensorName1;
Variant Configuration::m_sensorName2;
Variant Configuration::m_sensorName3;
Variant Configuration::m_sensorName4;
Variant Configuration::m_sensorName5;

Variant Configuration::m_sensorAddr1;
Variant Configuration::m_sensorAddr2;
Variant Configuration::m_sensorAddr3;
Variant Configuration::m_sensorAddr4;
Variant Configuration::m_sensorAddr5;

Variant Configuration::m_displayEnb;
Variant Configuration::m_forceOn;
Variant Configuration::m_resetCount;

/******************************************************/
/* This next block a values, have no menus, nor flash */
/* veriables associated with them                     */
/******************************************************/
Variant Configuration::m_cycleHyst;
Variant Configuration::m_pidEnabled;
Variant Configuration::m_Kp;
Variant Configuration::m_Ki;
Variant Configuration::m_Kd;
Variant Configuration::m_outsideCurve;
Variant Configuration::m_outsideAdj;
Variant Configuration::m_cycleSensor;
Variant Configuration::m_cycleMinTime;
Variant Configuration::m_cycleMaxTime;
Variant Configuration::m_cycleMin;
Variant Configuration::m_cycleMax;
Variant Configuration::m_overideOffset;

const char Configuration::GET_KEYS_CMD[] = "show_keys";
const char Configuration::GET_KNOWN_SENSORS_CMD[] = "known_sensors";
Configuration::SensorDefList_t Configuration::m_sensors;

nvs_handle_t Configuration::m_nvsHandle;

Configuration::flash_map_t Configuration::m_confMap;
Configuration::DefaultValue_t Configuration::m_defaultValues;

void showTemp(Variant& sensor, std::ostream& d_out)
{
   SensorData sd = SensorData::getSensor(sensor.toString());
   float temp = sd.temperature();
//   int timestamp = sd.timestamp();
   d_out << temp;
}
void showOutsideTemp(std::ostream& d_out)
{
   showTemp(Configuration::m_outsideSensor, d_out);
}

void showCycleTemp(std::ostream& d_out)
{
   showTemp(Configuration::m_cycleSensor, d_out);
}

void showTargetTemp(std::ostream& d_out)
{
   float offset = config->value("ovr_offset").toDouble();
   float target = config->getCurrentTempSetting() - offset;
   d_out << target << ", ofs: " << offset;
}

void showRoomTemp(std::ostream& d_out)
{
   showTemp(Configuration::m_targetSensor, d_out);
}

void show_current_sensors(std::ostream& d_out)
{
   SensorData::SensorList_t s_names;
   s_names = SensorData::getAllSensors();
   for (auto it = s_names.begin(); it != s_names.end(); ++it)
   {
      time_t now;
      time(&now);

      string name = *it;
      SensorData sd(SensorData::getSensor(name));
      float temp = sd.temperature();
      time_t ts = (time_t)sd.timestamp();
      time_t age = (time_t)((now - ts) / 24 * 3600);
      d_out << "Sensor: " << name << "\t" << temp << '\t' << age << endl;
   }
}

void showMode(std::ostream& d_out)
{
   switch (Configuration::m_mode.toInt())
   {
      case Configuration::Heat:
         d_out << "Heat";
         break;
      case Configuration::Cool:
         d_out << "Cool";
         break;
   }
   if (Configuration::m_vacationMode.toBool())
   {
      d_out << " - Vacation";
   }
}

bool setMode(std::string input)
{
   bool rv = true;

   if (std::toupper(input[0]) == 'H')
   {
      ESP_LOGI(TAG, "seting heat mode");
      Configuration::m_mode.setValue(Variant(Configuration::Heat));
      Configuration::m_vacationMode.setValue(Variant(false));
   }
   else if (std::toupper(input[0]) == 'C')
   {
      ESP_LOGI(TAG, "seting cool mode");
      Configuration::m_mode.setValue(Variant(Configuration::Cool));
      Configuration::m_vacationMode.setValue(Variant(false));
   }
   else if (std::toupper(input[0]) == 'V')
   {
      ESP_LOGI(TAG, "seting vacation mode");
      Configuration::m_vacationMode.setValue(Variant(true));
   }
   else
   {
      rv = false;
   }
   return(rv);
}

void showTime(std::ostream& d_out)
{
   char buf[80];
   time_t now;
   struct tm *cur_time;
//   now = rtc_4339->readRTCTime();   
   time(&now);
//   ESP_LOGI(TAG, "now: %d, tz_mins: %d", (int)now, Configuration::m_tzMinutes.toInt());
   now += Configuration::m_tzMinutes.toInt() * 60;
//   ESP_LOGI(TAG, "local now: %d", (int)now);
   cur_time = gmtime(&now);
   strftime(buf, sizeof(buf), "%T %d/%m/%Y", cur_time);
   d_out << buf;
}

bool setTime(std::string input)
{
   return(false);
}

bool voidAction(std::string)
{
   return(false);
}

Configuration::Configuration(uart_port_t port_num) :
               MenuConsole(port_num),
               m_ready(false)
{
   setMenu(&confMenu::mainMenu);

   /************************************/
   /* Setup the list of default values */
   /************************************/
#if 1
   m_defaultValues["rst_count"]           = string("0");
   m_defaultValues["ssid"]                = string("athena");
   m_defaultValues["wpa_key"]             = string("my_knowlege_goddess");
   m_defaultValues["mqtt_broker"]         = string("mqtt://athena.local:1883");
   m_defaultValues["ping_topic"]          = string("ping");
   m_defaultValues["mqtt_temp_topic"]     = string("temperature");
   m_defaultValues["mqtt_log_topic"]      = string("log");
   m_defaultValues["mqtt_stat_topic"]     = string("status");
   m_defaultValues["mqtt_furn_topic"]     = string("furnace");
   m_defaultValues["sens_port"]           = string("18");
   m_defaultValues["sens_port"]           = string("18");

   m_defaultValues["heat_pid_en"]         = string("true");
   m_defaultValues["heat_kp"]             = string("50.0");
   m_defaultValues["heat_ki"]             = string("0.05");
   m_defaultValues["heat_kd"]             = string("55.0");
   m_defaultValues["heat_hyst"]           = string("2.0");
   m_defaultValues["heat_max_time"]       = string("40");
   m_defaultValues["heat_min_time"]       = string("10");
   m_defaultValues["heat_max_temp"]       = string("60.0");
   m_defaultValues["heat_min_temp"]       = string("140.0");

   m_defaultValues["send_status"]         = string("true");

   m_defaultValues["heat_time1"]          = string("240");
   m_defaultValues["heat_temp1"]          = string("70.0");
   m_defaultValues["heat_time2"]          = string("300");
   m_defaultValues["heat_temp2"]          = string("71.0");
   m_defaultValues["heat_time3"]          = string("360");
   m_defaultValues["heat_temp3"]          = string("72.0");
   m_defaultValues["heat_time4"]          = string("1200");
   m_defaultValues["heat_temp4"]          = string("69.0");
   m_defaultValues["mode"]                = string("0");
   m_defaultValues["vac_mode"]            = string("false");
   m_defaultValues["vac_temp"]            = string("55");
   m_defaultValues["ovr_offset"]          = string("0");

   m_defaultValues["rmt_relays"]          = string("false");
#endif
   m_sensors.push_back(std::make_pair(&m_sensorAddr1, &m_sensorName1));
   m_sensors.push_back(std::make_pair(&m_sensorAddr2, &m_sensorName2));
   m_sensors.push_back(std::make_pair(&m_sensorAddr3, &m_sensorName3));
   m_sensors.push_back(std::make_pair(&m_sensorAddr4, &m_sensorName4));
   m_sensors.push_back(std::make_pair(&m_sensorAddr5, &m_sensorName5));
   xSemaphoreGive(m_varMutex);
}

/**************************************************************/
/* This function sets the command values such as pid enabled, */
/* cycle sensor that have separate setting for the heating    */
/* and cooling modes to the current mode in force.            */
/**************************************************************/
void Configuration::setModeValues()
{
   ESP_LOGI(TAG, "Enter, mode = %d", m_mode.toInt());
   if (m_mode.toInt() == Heat)
   {
      ESP_LOGI(TAG, "Heat Mode");
      m_cycleSensor  = m_heatCycleSensor;
      m_cycleHyst    = m_heatCycleHyst;
      m_cycleMinTime = m_heatMinCycleTime;
      m_cycleMaxTime = m_heatMaxCycleTime;
      m_cycleMin     = m_heatCycleMinimum;
      m_cycleMax     = m_heatCycleMaximum;
      m_pidEnabled   = m_heatPidEnabled;
      m_Kp           = m_heatKp;
      m_Ki           = m_heatKi;
      m_Kd           = m_heatKd;
      m_outsideCurve = m_heatOutsideCurve;
      m_outsideAdj   = m_heatOutsideAdj;

#if 1
      times[0] = 0;
      times[1] = value("heat_time1").toInt();
      times[2] = value("heat_time2").toInt();
      times[3] = value("heat_time3").toInt();
      times[4] = value("heat_time4").toInt();

      temps[0] = value("man_temp").toDouble();
      temps[1] = value("heat_temp1").toDouble();
      temps[2] = value("heat_temp2").toDouble();
      temps[3] = value("heat_temp3").toDouble();
      temps[4] = value("heat_temp4").toDouble();
#endif

      pid.SetControllerDirection(DIRECT);
   }
   else
   {
      ESP_LOGI(TAG, "Cool Mode");
      m_cycleSensor  = m_coolCycleSensor;
      m_cycleHyst    = m_coolCycleHyst;
      m_cycleMinTime = m_coolMinCycleTime;
      m_cycleMaxTime = m_coolMaxCycleTime;
      m_cycleMin     = m_coolCycleMinimum;
      m_cycleMax     = m_coolCycleMaximum;
      m_pidEnabled   = m_coolPidEnabled;
      m_Kp           = m_coolKp;
      m_Ki           = m_coolKi;
      m_Kd           = m_coolKd;
      m_outsideCurve = m_coolOutsideCurve;
      m_outsideAdj   = m_coolOutsideAdj;

#if 1
      times[0] = 0;
      times[1] = value("cool_time1").toInt();
      times[2] = value("cool_time2").toInt();
      times[3] = value("cool_time3").toInt();
      times[4] = value("cool_time4").toInt();

      temps[0] = value("man_temp").toInt();
      temps[1] = value("cool_time1").toInt();
      temps[2] = value("cool_time2").toInt();
      temps[3] = value("cool_time3").toInt();
      temps[4] = value("cool_time4").toInt();
#endif
      pid.SetControllerDirection(REVERSE);
   }
   pid.SetSampleTime(Configuration::m_sampleInterval.toInt() * 1000);
   pid.SetOutputLimits(-15.0, 105.0);
   pid.SetTunings(Configuration::m_Kp.toDouble(), Configuration::m_Ki.toDouble(), Configuration::m_Kd.toDouble());
}

/*************************************************************/
/* Returns the number of seconds since midnight. The time is */
/* adjusted by adding the value of m_tzMinutes               */
/*************************************************************/
int Configuration::adjustedTimeOfDay()
{
   time_t now;
   time(&now);
   int cst_adj = (m_tzMinutes.toInt() * 60);
   return((((int)now + cst_adj) % (24 * 60 * 60)));
}

int Configuration::CalculateCurrentSetting()
{
   int setting = -1;   // Our return value.
#if 1
   /***********************************************/
   /*   Start  by  getting the time setting that  */
   /*   is  closest  to the current time without  */
   /*   being earlier than it.                    */
   /***********************************************/
   struct timeval tv;
   gettimeofday(&tv, NULL);
   int cst_adj = (m_tzMinutes.toInt() * 60);
   int currTime = (((int)tv.tv_sec + cst_adj) % (24 * 60 * 60)) / 60;
   int bestTime = 2000;
//   ESP_LOGI(TAG, "secs from epoc: %d, currTime: %d, adj: %d", (int)tv.tv_sec, currTime, cst_adj);
   for (int x = 1; x < Configuration::NUM_SETTINGS; x++) 
   {
      if ( temps[x] >= 0 ) 
      {
//         ESP_LOGI(TAG, "currTime: %d, bestTime: %d, timeSettings[%d]: %d", currTime, bestTime, x, times[x]);
         if ( temps[x] > 0.0 && currTime - times[x] >= 0 &&
               currTime - times[x] < bestTime  ) 
         {
            setting = x;
            bestTime = currTime - times[x];
//            ESP_LOGI(TAG, "best time- setting: %d, time: %d", setting, bestTime);
         }
      }
   }

//   Serial << __PRETTY_FUNCTION__ << "Entering phase2- setting: " << (int)setting << endl;

   /***********************************************/
   /*   If  we  didn't  get  a  time in the step  */
   /*   above,  assume  that the current time is  */
   /*   past  all of the time settings and fetch  */
   /*   the setting with the latest time.         */
   /***********************************************/
   if ( setting < 0 ) 
   {
      bestTime = 0;
      for (int x = 1; x < Configuration::NUM_SETTINGS; x++) 
      {
         if ( temps[x] > 0.0 && times[x] > bestTime ) 
         {
            bestTime = times[x];
            setting = x;
         }
      }
   }
#endif
   return(setting);
}

float Configuration::getCurrentTempSetting()
{
   float    target = 65.0;

#if 1
   /***********************************************/
   /*   If  we  are  in  vacation mode, just get  */
   /*   it's setting and go for it.               */
   /***********************************************/
//   ESP_LOGI(TAG, "vacation_mode: %s", value("vac_mode").toString().c_str());
   bool vac_mode = value("vac_mode").toBool();
   if (vac_mode)
   {
      target = value("vac_temp").toDouble();
   }
#endif
#if 1
   else
   {

      /***********************************************/
      /*   Set  the  temperature to the one that is  */
      /*   set as the default.                       */
      /***********************************************/
      target = temps[0];

      /***********************************************/
      /*   Fetch  the  current  setting,  if any we  */
      /*   should be using                           */
      /***********************************************/
      int setting = CalculateCurrentSetting();
         
      /***********************************************/
      /*   If  we found one (if there were no temps  */
      /*   set,  we  won't  have  anything)  us it,  */
      /*   otherwise,  go  with the default time we  */
      /*   set above.                                */
      /***********************************************/
      if ( setting > 0 ) 
      {
         target = temps[setting];
      }
      target += value("ovr_offset").toDouble();
   }
#endif
   return(target);
}

std::string Configuration::get_known_sensors()
{
   std::stringstream str_value;
   SensorData::SensorList_t s_names;
   s_names = SensorData::getAllSensors();

   str_value << "{\"message_type\" : \"discovery\", \"sensors\" : [";
   for (auto it = s_names.begin(); it != s_names.end(); ++it)
   {
      time_t now;
      time(&now);

      string name = *it;
      SensorData sd(SensorData::getSensor(name));
      float temp = sd.temperature();
      time_t ts = (time_t)sd.timestamp();
      time_t age = (time_t)((now - ts) / 24 * 3600);
      str_value << "{ \"sensor\" : " << "\"" << name << "\", ";
      str_value << "\"temperature\" : " << temp << ", ";
      str_value << "\"age\" : " << age << " },\n";
   }
   str_value << "] }\n";
   return(str_value.str());
}

std::string Configuration::get_all_keys()
{
   std::stringstream str_value;
   MenuEntry::printEntries(str_value);
   return(str_value.str());
}


void Configuration::changeMade()
{
   esp_err_t err;
   bool commit_needed = false;
   MenuEntry::KeyList_t menu_keys = keys();
   MenuEntry::KeyList_t::iterator pos;
   gpio_set_level(DBG_PIN_4, 1);
   for (pos = menu_keys.begin(); pos != menu_keys.end(); ++pos)
   {
      char buf[256];
      memset(buf, '\0', sizeof(buf));
      size_t   len = sizeof(buf);
 //     ESP_LOGI(TAG, "config parm %s, type %d", pos->c_str(), m_menu->type());
      MenuEntry *menu = m_menu->findEntry(*pos);
      if (menu != NULL && ! (menu->options() & MenuEntry::NoPermanence) && menu->type() != MenuEntry::SubMenu && menu->type() != MenuEntry::MainMenu)
      {
         Variant val = m_menu->entryValue(*pos);
         err = nvs_get_str(m_nvsHandle, pos->c_str(), buf, &len); 
//         ESP_LOGI(TAG, "config parm %s, value %s", pos->c_str(), val.toString().c_str());
         if (strcmp(buf, val.toString().c_str()) != 0)
         {
            ESP_LOGI(TAG, "Setting config parm %s, to value %s", pos->c_str(), val.toString().c_str());
            err = nvs_set_str(m_nvsHandle, pos->c_str(), val.toString().c_str()); 
            switch (err)
            {
               case ESP_OK:
                  commit_needed = true;
                  break;
               default:
                  ESP_LOGE(TAG, "Error (%s) writing NVS", esp_err_to_name(err));
                  assert(0);
            }
         }
      }
   }      
   gpio_set_level(DBG_PIN_4, 0);
   if (commit_needed)
   {
      ESP_LOGI(TAG, "Commiting NVS");
      err = nvs_commit(m_nvsHandle);
   }
   gpio_set_level(DBG_PIN_4, 1);
   setModeValues();
   I2CPort::setOutputEnable( ! value("rmt_relays").toBool());
   gpio_set_level(DBG_PIN_4, 0);
//   display->setEnabled(m_displayEnb.toBool());
}

void* Configuration::process()
{
   esp_err_t err;
   ESP_LOGI(TAG, "Initializing config values");
   if (nvs_open("configuration", NVS_READWRITE, &m_nvsHandle) == ESP_OK)
   {
      MenuEntry::KeyList_t menu_keys = keys();
      MenuEntry::KeyList_t::iterator pos;
      for (pos = menu_keys.begin(); pos != menu_keys.end(); ++pos)
      {
         char     buf[256];
         size_t   len = sizeof(buf);
#if 1
         MenuEntry *mentry = MenuEntry::findEntry(*pos);
         if (mentry->type() != MenuEntry::MainMenu &&
             mentry->type() != MenuEntry::SubMenu &&
             mentry->type() != MenuEntry::Action &&
             mentry->type() != MenuEntry::CustomIO)
#endif
         {
//            ESP_LOGI(TAG, "Searching NVS for var (%s)", pos->c_str());
            string str;
            Variant var;
            err = nvs_get_str(m_nvsHandle, pos->c_str(), buf, &len); 
            switch (err)
            {
               case ESP_OK:
//                  ESP_LOGI(TAG, "NVS var %s found!!!", pos->c_str());
                  var.setValue(Variant(buf));
                  m_menu->setValue(*pos, var);
                  break;
               case ESP_ERR_NVS_NOT_FOUND:
//                  ESP_LOGI(TAG, "NVS var %s NOT found!!!", pos->c_str());
#if 1
                  if (m_defaultValues.find(*pos) != m_defaultValues.end())
                  {
                     var.setValue(Variant(m_defaultValues.at(*pos)));
 //                    ESP_LOGI(TAG, "NVS var %s default value %s", pos->c_str(), var.toString().c_str());
                     m_menu->setValue(*pos, var);
                  }
#endif
                  memset(buf, '\0', sizeof(buf));
                  break;

               default:
                  ESP_LOGE(TAG, "Error (%s) reading NVS", esp_err_to_name(err));
                  assert(0);
            }
         }
         else
         {
//            ESP_LOGI(TAG, "Skipping entry (%s)", pos->c_str());
         }
            
      }
      int rst_count = value("rst_count").toInt() + 1;
      setValue("rst_count", Variant(rst_count));
      changeMade();
      setModeValues();
      m_ready = true;
      MenuConsole::exec(NULL);
      nvs_close(m_nvsHandle);
      return(NULL);
   }
   assert(0);
}

Variant Configuration::value(string key)
{
//   cout << __PRETTY_FUNCTION__ << ": enter" << endl;
   string s_val = MenuEntry::entryValue(key).toString();
//   cout << __PRETTY_FUNCTION__ << ": s_val = " << s_val << endl;
   MenuEntry::KeyList_t keys = MenuEntry::keys();
   MenuEntry::KeyList_t::iterator s_key;
   for (s_key = keys.begin(); s_key != keys.end(); ++s_key)
   {
//      cout << __PRETTY_FUNCTION__ << ": s_key = " << *s_key << endl;
      string s_repl = MenuEntry::entryValue(*s_key).toString();
      string key_name = "%%" + *s_key + "%%";
      int pos = -1;
      while ((pos = s_val.find(key_name)) != string::npos)
      {
//         cout << __PRETTY_FUNCTION__ << ": found = " << key_name << " in " << s_val << endl;
          s_val.replace(pos, key_name.length(), s_repl);
      }
   }
   return(Variant(s_val));
}

void Configuration::setValue(string key, Variant val, bool keep)
{
   MenuEntry *mentry = MenuEntry::findEntry(key);
   mentry->setValue(val);
   if (keep)
   {
      changeMade();
   }
}

int Configuration::findSensorAddr(std::string device_addr)
{
   int rv = -1;

   SensorDefList_t::iterator sensors;
   int idx = 0;
   for (sensors = m_sensors.begin(); rv < 0 && sensors != m_sensors.end(); ++sensors)
   {
      if (*(*sensors).first == Variant(device_addr))
      {
         rv = idx;
         break;
      }
      idx++;
   }
   return(rv);
}
    
void Configuration::updateSensors(std::vector<std::string> devices)
{
   std::vector<std::string>::iterator dev;
   for (dev = devices.begin(); dev != devices.end(); ++dev)
   {
      ESP_LOGI(TAG, "Testing for sensor addr: %s", dev->c_str());
      int sensor = findSensorAddr(*dev);
      ESP_LOGI(TAG, "sensor: %d", sensor);
      if (sensor < 0)
      {
         SensorDefList_t::iterator sensors;
         for (sensors = m_sensors.begin(); sensors != m_sensors.end(); ++sensors)
         {
            ESP_LOGI(TAG, "Sensor %s exists", (*sensors).first->toString().c_str());
            if ((*sensors).first->toString().length() == 0)
            {
               (*sensors).first->setValue(*dev);
               break;
            }
         }
      }
   }
}

void *ConfigurationThread::exec(void* obj)
{
   static_cast<Configuration*>(obj)->process();
   return(NULL);
}

