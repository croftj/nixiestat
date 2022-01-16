#ifndef MQTT_H
#define MQTT_H

#include <stdio.h>

#include "qmqtt.h"

#include <QString>
#include <QCoreApplication>

class MQTT;

class MQTTTopicHandler : public QObject
{
   Q_OBJECT

public:
   MQTTTopicHandler(QString topic) : m_topic(topic) { }
   void setMqtt(MQTT* mqtt)
   {
      m_mqtt = mqtt;
   }

   virtual void processMessage(QJsonObject& json) = 0;

protected:
   QString  m_topic;
   MQTT*    m_mqtt;
};

class MQTT :  public QMQTT::Client
{
   Q_OBJECT

public:
   explicit MQTT(const QHostAddress& host, const quint16 port, QObject* parent = NULL)
      : QMQTT::Client(host, port, parent)
      , m_connected(false)
      , m_qout(stdout)
//      , m_db(NULL)
   {
      QObject::connect(this, SIGNAL(connected()),  this, SLOT(onConnected()));
      QObject::connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(onSubscribed(const QString&, const quint8)));
      QObject::connect(this, SIGNAL(received(const QMQTT::Message&)),   this, SLOT(onReceived(const QMQTT::Message&)));
      connectToHost();
   }
   virtual ~MQTT() {}

   void addHandler(QString topic, MQTTTopicHandler* handler)
   {
      if ( ! m_topicMap.contains(topic) )
      {
         m_topicMap.insert(topic, handler);
         handler->setMqtt(this);
         while (true)
         {
            if (m_connected)
            {
               m_qout << "Subscribing to topic: " << topic << endl; 
               subscribeTopic(topic);
               break;
            }
            else
            {
               QCoreApplication::processEvents();
            }
         }
      }
   }

   void subscribeTopic(QString topic);
   void sendTimeMessage(QString time_topic);

protected slots:
   void onConnected();
   void onSubscribed(const QString& topic, const quint8 qos = 0);
   void onReceived(const QMQTT::Message& message);

private:
   typedef QMap<QString, MQTTTopicHandler*> MQTTTopicHandler_t;

   volatile bool        m_connected;
   QTextStream          m_qout;
   MQTTTopicHandler_t   m_topicMap;
   QStringList          m_subscriptions;
};

#endif
