#include "LogTopicHandler.h"
#include "MQTT.h"
#include "QcjLib/LogBuilder.h"
#include "QcjLib/Logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <iostream>

using namespace std;
using namespace QcjLib;

static LogBuilder mylog("loghandler", 3, "");
void LogTopicHandler::processMessage(QJsonObject& json)
{
   bool new_device = false;
   QJsonObject temp;

   temp                 = json.value("temp").toObject();

   QString device       = temp.value("device").toString();
   QString sensor       = temp.value("sensor").toString();
   int timestamp        = temp.value("timestamp").toInt();
   qDebug(*log("loghandler", 1)) << "have message device: " << device << ", sensor: " << sensor;

   if (m_db != NULL)
   {
      m_db->insertRecord("temperatures", temp);
   }
}

