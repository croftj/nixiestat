#ifndef PING_TOPIC_HANDLER_H
#define PING_TOPIC_HANDLER_H

#include <stdio.h>

#include "MQTT.h"
#include "DBManager.h"
#include "LogTopicHandler.h"

class PingTopicHandler :  public MQTTTopicHandler
{
   Q_OBJECT

public:
   explicit PingTopicHandler(QString topic) :
         MQTTTopicHandler(topic),
         m_qout(stdout),
         m_logHandler(NULL),
         m_db(NULL)
   {};
   void processMessage(QJsonObject& json);

   void setDatabase(DBManager *db)
   {
      m_db = db;
   }

   void setLogHandler(LogTopicHandler* log_handler)
   {
      m_logHandler = log_handler;
   }
   virtual ~PingTopicHandler() {}

signals:
   void newDevice(QJsonObject device);
   
protected slots:
private:
   QTextStream       m_qout;
   LogTopicHandler*  m_logHandler;
   DBManager*        m_db;
};

#endif
