#ifndef TEMP_TOPIC_HANDLER_H
#define TEMP_TOPIC_HANDLER_H

#include <stdio.h>

#include "MQTT.h"
#include "DBManager.h"
#include "TempTopicHandler.h"

namespace DataStore
{
   typedef QStringList location_name_t ;
   typedef QMap<QString, QString> location_sensor_map_t;
   typedef QPair<QDateTime, QVariant> sensor_data_t;
   typedef QMap<QString, sensor_data_t> sensor_data_map_t;

   location_map_t          Locations;
   location_sensor_map_t   Sensors;
   sensor_data_map_t       SensorData;
}

class TempTopicHandler :  public MQTTTopicHandler
{
   Q_OBJECT

public:
   explicit TempTopicHandler(QString topic) :
         MQTTTopicHandler(topic),
         m_qout(stdout),
         m_db(NULL)
   {};
   virtual ~TempTopicHandler() {}
   
   void processMessage(QJsonObject& json);

signals:
   void newDevice(QJsonObject device);
   
protected slots:
private:
};

#endif
