# NixieStat Thermostat Code

This code is the main source code for the NixieStat Thermostat. It is
structured to be built using the Espressif idf.py tool.

This code requires code from the github project croftj/esp_components. This 
repository should be cloned and a symbolic link named cpp_components needs to
be created in the main folder of this code pointing to the folder with code from
esp_components.

This code can be built and loaded onto the ESP32 devboard for the thermostat
using the command

   *idf.py -p /dev/ttyUSBXX build flash monitor*

Assuming that the esp32 sdk environment was previous installed and configured.

Once the unit was flashed, it will attempt to connect to wifi and the message
bus. This cannot happen until the menus are brought up by pressing <Enter> 
and setting the wifi and the MQTT parameters are configured. The menus can be traversed
using single letters. This initialization  is a bit
cumbersome because of the debug messaging, but doable. Once the parameters are
set, reset the board and it should then connect.

A value can be set by using it's name or the single letter followed without
spaces an '=' then the value. As an example *b=72<Enter>*.


