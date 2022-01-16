#ifndef SUB_TEMPERATURES_H
#define SUB_TEMPERATURES_H

#include <stdio.h>

#include "qmqtt.h"
#include "DBManager.h"

class SubTemperatures :  public QMQTT::Client
{
   Q_OBJECT

public:
   explicit SubTemperatures(const QHostAddress& host, const quint16 port, QObject* parent = NULL)
      : QMQTT::Client(host, port, parent)
      , m_qout(stdout)
      , m_db(NULL)
   {
      m_qout << "SIGNAL(" << SIGNAL(connected()) << endl;
      connect(this, SIGNAL(connected()),  this, SLOT(onConnected()));
      connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(onSubscribed(const QString&, const quint8)));
      connect(this, SIGNAL(received(const QMQTT::Message&)),   this, SLOT(onReceived(const QMQTT::Message&)));
   }

   void setDatabase(DBManager *db)
   {
      m_db = db;
      m_qout << "setDatabase(): *db = " << hex << (uint64_t)m_db << endl;
   }

   virtual ~SubTemperatures() {}

protected slots:
   void onConnected();
   void onSubscribed(const QString& topic, const quint8 qos = 0);
   void onReceived(const QMQTT::Message& message);

private:
   QTextStream    m_qout;
   DBManager*     m_db;
};

#endif
