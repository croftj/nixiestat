#ifndef PING_TOPIC_HANDLER_H
#define PING_TOPIC_HANDLER_H

#include <stdio.h>

#include "qmqtt.h"

class PingTopicHandler :  public MQTTTopicHandler
{
   Q_OBJECT

public:
   explicit PingTopicHandler(QString topic) :
         MQTTTopicHandler(topic),
         m_qout(stdout)
   {};
   void processMessage(QJsonObject& json);

   void setDatabase(DBManager *db)
   {
      m_db = db;
   }

   virtual ~PingTopicHandler() {}

protected slots:
private:
   QTextStream    m_qout;
   DBManager*     m_db;
};

#endif
