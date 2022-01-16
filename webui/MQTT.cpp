#include "MQTT.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <iostream>

using namespace std;
using namespace QMQTT;

void MQTT::onConnected()
{
   m_qout << "connected" << endl;
   m_connected = true;
}

void MQTT::onSubscribed(const QString& topic, const quint8 qos)
{
   m_qout << "subscribed: " << topic << endl;
}

void MQTT::onReceived(const QMQTT::Message& message)
{
   QString jbuf = QString::fromUtf8(message.payload());
   QString topic = message.topic();
   
   m_qout << "message fron topic '" << topic << "' received: \"" << jbuf << "\", len = " << jbuf.length() << endl;
   QJsonDocument jdoc = QJsonDocument::fromJson(jbuf.toUtf8());
   QJsonObject json = jdoc.object();

   if (m_topicMap.contains(topic))
   {
      MQTTTopicHandler *th = m_topicMap.value(topic);
      th->processMessage(json);
   }
#if 0
   else if (false && m_db != NULL)
   {
      int x;
      QJsonObject json = jdoc.object();
//      m_db->importJson(json);
   }
#endif
   else
   {
      m_qout << "ACKK!!! Unhandled message from topic '" << topic << "' received: \"" << jbuf << "\", len = " << jbuf.length() << endl;
   }
}

void MQTT::sendTimeMessage(QString time_topic)
{
   QDateTime t;
   qint64 sys_time = t.currentSecsSinceEpoch();
   QJsonObject json { {"system_time", (int)sys_time }};
   QJsonDocument jdoc(json);
   m_qout << "Sending to topic " << time_topic << "' message: \"" << qPrintable(jdoc.toJson()) << endl;
   Message t_msg(0, time_topic, qPrintable(jdoc.toJson()));
   publish(t_msg);
}

void MQTT::subscribeTopic(QString topic)
{
   if (! m_subscriptions.contains(topic ))
   {
      subscribe(topic, 0);
      m_subscriptions.append(topic);
   }
}


