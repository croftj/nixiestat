#include "TempTopicHandler.h"
#include "MQTT.h"
#include "QcjData/LogBuilder.h"
#include "QcjData/Logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <iostream>

using namespace std;
using namespace QcjData;

DataStore::location_map_t          DataStore::Locations;
DataStore::location_sensor_map_t   DataStore::Sensors;
DataStore::sensor_data_map_t       DataStore::SensorData;

static LogBuilder mylog("temp", 3, "");

// '{"device": "new2_board", "sensor": "28:42:fb:19:54:20:1:2", 
//   "temperature": 68.224998474121094, "timestamp": 1620817097}' to topic 'temperature'

void TempTopicHandler::processMessage(QJsonObject& json)
{
   bool new_device = false;
   QString device       = json.take("device").toString();
   QString sensor       = json.value("sensor").toString();
   QString temperature  = json.value("temperature").toString();
   QString timestamp    = json.value("timestamp").toString();
   qDebug(*log("temp", 1)) << "have message device: " << device << ", sensor: " << sensor;

}

