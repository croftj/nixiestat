#include "PingTopicHandler.h"
#include "MQTT.h"
#include "QcjLib/LogBuilder.h"
#include "QcjLib/Logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <iostream>

using namespace std;
using namespace QcjLib;

static LogBuilder mylog("ping", 3, "");
void PingTopicHandler::processMessage(QJsonObject& json)
{
   bool new_device = false;
   QString device        = json.take("device").toString();
   QString type         = json.value("type").toString();
   QString time_topic   = json.value("time_topic").toString();
   QString log_topic    = json.value("log_topic").toString();
   QString sens_topic   = json.value("sens_topic").toString();
   int timestamp        = json.value("timestamp").toInt();
   qDebug(*log("ping", 1)) << "have message device: " << device << ", type: " << type;

   if (m_db != NULL && type.length() == 0 || type == "temp_sensor")
   {
      m_db->addDevice(device, json);
      QJsonArray sensors = json.take("sensors").toArray();
      for (QJsonArray::const_iterator it = sensors.begin(); it != sensors.end(); ++it)
      {
         qDebug(*log("ping", 1)) << "binding sensor" << (*it).toString();
         m_db->bindDeviceSensor(device, (*it).toString(), type);
      }
      if (sens_topic.length() > 0 && m_logHandler != NULL)
      {
         qDebug(*log("ping", 0)) << "Adding log handler for topic: " << sens_topic;
         m_mqtt->addHandler(sens_topic, m_logHandler);
      }
      if (log_topic.length() > 0 && m_logHandler != NULL)
      {
         qDebug(*log("ping", 0)) << "Adding log handler for topic: " << log_topic;
         m_mqtt->addHandler(log_topic, m_logHandler);
      }
   }
   
   QDateTime t;
   qint64 sys_time = t.currentSecsSinceEpoch();
   if ((timestamp < sys_time - 600) || (timestamp > sys_time + 600))
   {
      m_mqtt->sendTimeMessage(time_topic);
   }
}

