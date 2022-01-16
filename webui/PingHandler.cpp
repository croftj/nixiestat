#include "PingTopicHandler.h"
#include <QJsonDocument>
#include <QByteArray>
#include <iostream>
using namespace std;

void PingTopicHandler::processMessage(QJsonObject& json)
{
   m_qout << "PingTopicHandler::processMessage(): have message"
          << json.value(m_topic).toString() << endl;
}


