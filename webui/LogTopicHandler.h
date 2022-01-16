#ifndef LOG_TOPIC_HANDLER_H
#define LOG_TOPIC_HANDLER_H

#include <stdio.h>

#include "MQTT.h"
#include "DBManager.h"

class LogTopicHandler :  public MQTTTopicHandler
{
   Q_OBJECT

public:
   explicit LogTopicHandler(QString topic) :
         MQTTTopicHandler(topic),
         m_qout(stdout),
         m_db(NULL)
   {};
   void processMessage(QJsonObject& json);

   void setDatabase(DBManager *db)
   {
      m_db = db;
   }

   virtual ~LogTopicHandler() {}

signals:
   void newDevice(QJsonObject device);
   
protected slots:
private:
   QTextStream    m_qout;
   DBManager*     m_db;
};

#endif
