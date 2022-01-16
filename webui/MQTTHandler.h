#ifndef MQTT_SUBSCRIPTION_HANDLER_H
#define MQTT_SUBSCRIPTION_HANDLER_H

class MQTTSubscriptionHandler : public QObject
{
   Q_OBJECT

public:
   MQTTSubscriptionHandler(QString topic) : m_topic(topic) { }

   void processMessage(QJsonObject& json) = 0;
};

#endif
