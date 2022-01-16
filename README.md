# NixieStat Home Thermostat

NxieStat is thermostat for the home with a nixie tube display supporting
remote sensors.

All comminications to the thermostat is over wifi using the Mosquitto MQTT
message bus. The messages are in the form of json strings. 

This prject started out as an AVR based thermostat. You will see vestiges of
that in source tree. The latest code and hardware is under the **esp32**
folder. The hope is that there will be a web ui running on a Raspberry PI. The
start of this code is in the **webui** folder. It is very early code that is
mostly to prove certain components/SDKs such as **QT** & **WT** can actually be 
compiled together on the Rasberry PI.

## Hardware

### Devboard
This is currently the most tested of the two ESP32 boards. It handle an
ESP32-S2 processor. It also has a three pin connector for easily attaching a
DS18S20 temperature sensor to it. Its  primary purpose is for the temperature
sensors.

### Devboard2
This board is similar to *DevBoard* except that it is laid out for the
ESP32-WROVER-IE processor. This board has not been tested yet. It will
hopefully be the processor for use as the thermostat itself and be able to be
upgraded *over the air*

### CPU Board
This board has the relays for the furnace as wll as the interface chips for the
keyboard an the display board. It can use either *Devboard* or *Devboard2* for
it's processor board.

## Nixie Display
This board has the High voltage supply as well as the nixie tubes and neon
bulbs for the display.

