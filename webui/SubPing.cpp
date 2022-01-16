#include "SubPing.h"
#include <QJsonDocument>
#include <QByteArray>
#include <iostream>
using namespace std;

void SubPing::onConnected()
{
   m_qout << "connected" << endl;
   subscribe("ping", 0);
}

void SubPing::onSubscribed(const QString& topic, const quint8 qos)
{
   m_qout << "subscribed: " << topic << endl;
}

void SubPing::onReceived(const QMQTT::Message& message)
{
   QString jbuf = QString::fromUtf8(message.payload());
   m_qout << "message received: \"" << jbuf << "\", len = " << jbuf.length() << endl;
   QJsonDocument jdoc = QJsonDocument::fromJson(jbuf.toUtf8());
   if (m_db != NULL)
   {
      int x;
      QJsonObject json = jdoc.object();
      m_db->importJson(json);
   }
}


