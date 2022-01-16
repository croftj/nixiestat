#include "configuration.h"
#include <map>

using namespace std;

static bool dbg = true;

namespace confMenu
{
   MenuEntry mainMenu((const char*)"Configuration",            (const char*)("main"),        MenuEntry::MainMenu);
   MenuEntry ssidEntry((const char*)"SSID",                    (const char*)("ssid"),        MenuEntry::String, &Configuration::m_ssid,       &mainMenu);
   MenuEntry wkeyEntry((const char*)"WPA KEY",                 (const char*)("wpa_key"),     MenuEntry::String, &Configuration::m_wapkey,     &mainMenu);
   MenuEntry thermMenu((const char*)"Sensor Settings",         (const char*)("thermostat"),  MenuEntry::SubMenu, &mainMenu);
   MenuEntry mqttMenu((const char*)"MQTT Settings",            (const char*)("mqtt"),        MenuEntry::SubMenu, &mainMenu);
   
   MenuEntry tsampEntry((const char*)"Sample Interval (secs)",  (const char*)("samp_int"),    MenuEntry::UInt,     &Configuration::m_sampleInterval, &thermMenu);
   MenuEntry rsampEntry((const char*)"Report Interval (secs)",  (const char*)("rept_int"),    MenuEntry::UInt,     &Configuration::m_reportInterval, &thermMenu);
   MenuEntry sensPortEntry((const char*)"Sensor IO Port",        (const char*)("sens_port"),  MenuEntry::UInt,     &Configuration::m_sensorPort,    &thermMenu);
   MenuEntry sensorMenu((const char*)"Sensors",                (const char*)("sensors"),      MenuEntry::SubMenu, &thermMenu);
   
   MenuEntry sname5Entry((const char*)"Sensor5 Name",           (const char*)("sensor_name5"),    MenuEntry::String, &Configuration::m_sensorName5, &sensorMenu);
   MenuEntry saddr5Entry((const char*)"Sensor5 Addr",           (const char*)("sensor_addr5"),    MenuEntry::String, &Configuration::m_sensorAddr5, &sensorMenu);
   MenuEntry sname4Entry((const char*)"Sensor4 Name",           (const char*)("sensor_name4"),    MenuEntry::String, &Configuration::m_sensorName4, &sensorMenu);
   MenuEntry saddr4Entry((const char*)"Sensor4 Addr",           (const char*)("sensor_addr4"),    MenuEntry::String, &Configuration::m_sensorAddr4, &sensorMenu);
   MenuEntry sname3Entry((const char*)"Sensor3 Name",           (const char*)("sensor_name3"),    MenuEntry::String, &Configuration::m_sensorName3, &sensorMenu);
   MenuEntry saddr3Entry((const char*)"Sensor3 Addr",           (const char*)("sensor_addr3"),    MenuEntry::String, &Configuration::m_sensorAddr3, &sensorMenu);
   MenuEntry sname2Entry((const char*)"Sensor2 Name",           (const char*)("sensor_name2"),    MenuEntry::String, &Configuration::m_sensorName2, &sensorMenu);
   MenuEntry saddr2Entry((const char*)"Sensor2 Addr",           (const char*)("sensor_addr2"),    MenuEntry::String, &Configuration::m_sensorAddr2, &sensorMenu);
   MenuEntry sname1Entry((const char*)"Sensor1 Name",           (const char*)("sensor_name1"),    MenuEntry::String, &Configuration::m_sensorName1, &sensorMenu);
   MenuEntry saddr1Entry((const char*)"Sensor1 Addr",           (const char*)("sensor_addr1"),    MenuEntry::String, &Configuration::m_sensorAddr1, &sensorMenu);

   MenuEntry passEntry((const char*)"MQTT Password",           (const char*)("mqtt_pass"),    MenuEntry::String, &Configuration::m_mqttPass,   &mqttMenu);
   MenuEntry userEntry((const char*)"MQTT User",               (const char*)("mqtt_user"),    MenuEntry::String, &Configuration::m_mqttUser,   &mqttMenu);
   MenuEntry brkrEntry((const char*)"MQTT Broker",             (const char*)("mqtt_broker"),  MenuEntry::String, &Configuration::m_mqttBroker, &mqttMenu);
   MenuEntry ltpcEntry((const char*)"MQTT Log Topic",          (const char*)("mqtt_log_topic"),  MenuEntry::String, &Configuration::m_mqttLogTopic,  &mqttMenu);
   MenuEntry ttpcEntry((const char*)"MQTT Temp Topic",         (const char*)("mqtt_temp_topic"), MenuEntry::String, &Configuration::m_mqttTempTopic,  &mqttMenu);
   MenuEntry pingEntry((const char*)"MQTT Ping Topic",        (const char*)("ping_topic"),       MenuEntry::String, &Configuration::m_pingTopic,  &mqttMenu);
   MenuEntry idntEntry((const char*)"MQTT Identity",           (const char*)("mqtt_ident"),      MenuEntry::String, &Configuration::m_mqttIdent,  &mqttMenu);
}

Variant Configuration::m_ssid;
Variant Configuration::m_wapkey;

Variant Configuration::m_mqttBroker;
Variant Configuration::m_mqttUser;
Variant Configuration::m_mqttPass;
Variant Configuration::m_mqttIdent;
Variant Configuration::m_mqttTopic;
Variant Configuration::m_mqttLogTopic;
Variant Configuration::m_mqttTempTopic;
Variant Configuration::m_pingTopic;

Variant Configuration::m_radiatorDevId;
Variant Configuration::m_roomTempDevId;

Variant Configuration::m_sampleInterval;
Variant Configuration::m_reportInterval;
Variant Configuration::m_outsideTemp;
Variant Configuration::m_setTemp;
Variant Configuration::m_sensorPort;

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

Configuration::SensorDefList_t Configuration::m_sensors;

nvs_handle_t Configuration::m_nvsHandle;

Configuration::flash_map_t Configuration::m_confMap;
Configuration::DefaultValue_t Configuration::m_defaultValues;

Configuration::Configuration(uart_port_t port_num) :
               MenuConsole(port_num),
               m_ready(false)
{
   setMenu(&confMenu::mainMenu);

   /************************************/
   /* Setup the list of default values */
   /************************************/
#if 1
   m_defaultValues["ssid"]                = string("athena");
   m_defaultValues["wpa_key"]             = string("my_knowlege_goddess");
   m_defaultValues["mqtt_broker"]         = string("mqtt://athena.local:1883");
   m_defaultValues["ping_topic"]          = string("ping");
   m_defaultValues["mqtt_temp_topic"]     = string("temperature");
   m_defaultValues["mqtt_log_topic"]      = string("log");
   m_defaultValues["sens_port"]           = string("18");
#endif
   m_sensors.push_back(std::make_pair(&m_sensorAddr1, &m_sensorName1));
   m_sensors.push_back(std::make_pair(&m_sensorAddr2, &m_sensorName2));
   m_sensors.push_back(std::make_pair(&m_sensorAddr3, &m_sensorName3));
   m_sensors.push_back(std::make_pair(&m_sensorAddr4, &m_sensorName4));
   m_sensors.push_back(std::make_pair(&m_sensorAddr5, &m_sensorName5));
}

void Configuration::changeMade()
{
   esp_err_t err;
   bool commit_needed = false;
   if (dbg) cout << __PRETTY_FUNCTION__ << "Enter!" << endl;
   MenuEntry::KeyList_t menu_keys = keys();
   MenuEntry::KeyList_t::iterator pos;
   for (pos = menu_keys.begin(); pos != menu_keys.end(); ++pos)
   {
      char buf[256];
      memset(buf, '\0', sizeof(buf));
      size_t   len = sizeof(buf);
      if (dbg) cout << __PRETTY_FUNCTION__ << "config parm " << pos->c_str() << ", type: " << m_menu->type() << endl;
      MenuEntry *menu = m_menu->findEntry(*pos);
      if (menu != NULL && menu->type() != MenuEntry::SubMenu && menu->type() != MenuEntry::MainMenu)
      {
         Variant val = m_menu->entryValue(*pos);
         err = nvs_get_str(m_nvsHandle, pos->c_str(), buf, &len); 
         if (dbg) cout << __PRETTY_FUNCTION__ << "config parm " << pos->c_str() 
                                              << ", val: " << val.toString() << endl;
//                                              << endl;
         if (strcmp(buf, val.toString().c_str()) != 0)
         {
            if (dbg) cout << __PRETTY_FUNCTION__ << "Setting config parm " << pos->c_str() 
                        << " to value " << val.toString() << endl;

            err = nvs_set_str(m_nvsHandle, pos->c_str(), val.toString().c_str()); 
            switch (err)
            {
               case ESP_OK:
                  commit_needed = true;
                  break;
               default:
                  std::cout << "Error (%) writing NVS" << esp_err_to_name(err) << std::endl;
                  assert(0);
            }
         }
      }
   }      
   if (commit_needed)
   {
      if (dbg) cout << __PRETTY_FUNCTION__ << "Commiting nvs" << endl; 
      err = nvs_commit(m_nvsHandle);
   }
}

void* Configuration::process()
{
   esp_err_t err;
   if (nvs_open("configuration", NVS_READWRITE, &m_nvsHandle) == ESP_OK)
   {
      bool changes = false;
      MenuEntry::KeyList_t menu_keys = keys();
      MenuEntry::KeyList_t::iterator pos;
      for (pos = menu_keys.begin(); pos != menu_keys.end(); ++pos)
      {
         char     buf[256];
         size_t   len = sizeof(buf);
#if 1
         MenuEntry *mentry = MenuEntry::findEntry(*pos);
         if (mentry->type() != MenuEntry::MainMenu ||
             mentry->type() != MenuEntry::SubMenu)
#endif
         {
            string str;
            Variant var;
            err = nvs_get_str(m_nvsHandle, pos->c_str(), buf, &len); 
            switch (err)
            {
               case ESP_OK:
                  std::cout << "NVS var (" << *pos << ") found!!!" <<std::endl;
                  var.setValue(Variant(buf));
                  m_menu->setValue(*pos, var);
                  break;

               case ESP_ERR_NVS_NOT_FOUND:
                  std::cout << "NVS var (" << *pos << ") not found" <<std::endl;
#if 1
                  if (m_defaultValues.find(*pos) != m_defaultValues.end())
                  {
                     var.setValue(Variant(m_defaultValues.at(*pos)));
                     std::cout << "NVS var (" << *pos << ") using default val: " << var.toString() << std::endl;
                     m_menu->setValue(*pos, var);
                     changes = true;
                  }
#endif
                  memset(buf, '\0', sizeof(buf));
                  break;

               default:
                  std::cout << "Error (%) reading NVS" << esp_err_to_name(err) << std::endl;
                  assert(0);
            }
         }
      }      
      if (changes)
      {
         changeMade();
      }
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
   bool change = false;
   std::vector<std::string>::iterator dev;
   for (dev = devices.begin(); dev != devices.end(); ++dev)
   {
      cout << __PRETTY_FUNCTION__ << ": Testing for sensor addr: " << *dev << endl;
      int sensor = findSensorAddr(*dev);
      cout << __PRETTY_FUNCTION__ << ": sensor: " << sensor << endl;
      if (sensor < 0)
      {
         SensorDefList_t::iterator sensors;
         for (sensors = m_sensors.begin(); sensors != m_sensors.end(); ++sensors)
         {
            cout << __PRETTY_FUNCTION__ << ": existing sensor: |" << (*sensors).first->toString() << "|" << endl;
            if ((*sensors).first->toString().length() == 0)
            {
               (*sensors).first->setValue(*dev);
               change = true;
               break;
            }
         }
      }
   }
   if (change)
   {
      changeMade();
   }
}

void *ConfigurationThread::exec(void* obj)
{
   static_cast<Configuration*>(obj)->process();
   return(NULL);
}

