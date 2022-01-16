#include <QCoreApplication>
#include <QJsonDocument>
#include "DBManager.h"
#include "MQTT.h"
#include "PingTopicHandler.h"
#include "LogTopicHandler.h"
#include "SubTemperatures.h"
#include "WtApplication.h"
#include "QcjData/QcjTcpService.h"
#include "QcjLib/LogBuilder.h"
#include "QcjLib/Logger.h"

using namespace QcjLib;

static LogBuilder mylog("main", 3, "need we say more?");

int main(int argc, char *argv[])
{
   Logger *DebugLog = Logger::instance();
   QCoreApplication a(argc, argv);
//   SubTemperatures st(QHostAddress("athena.local"), 1883);
   DBManager db;
   db.setDatabaseName("QPSQL", "thermostat");
//   QcjSqlRestApiFactory restApiFactory;
//   restApiFactory.setDatabase(db.database());
//   QcjTcpServer tcpService(QHostAddress::AnyIPv4, 8080);
//   tcpService.setServiceFactory(&restApiFactory);

   LogTopicHandler *lh = new LogTopicHandler(QString("log"));
   lh->setDatabase(&db);
   PingTopicHandler *ph = new PingTopicHandler(QString("ping"));
   ph->setDatabase(&db);
   ph->setLogHandler(lh);
//   TempTopicHandler *ph = new TempTopicHandler(QString("temperature"));
   MQTT mqtt(QHostAddress("192.168.1.22"), 1883);
   mqtt.addHandler("ping", ph);
//   SubTemperatures st(QHostAddress("192.168.1.22"), 1883);
//   st.setDatabase(&db);
//   st.connectToHost();
   WtApplication wapp(argc, argv);
   qDebug(*log("main", 1)) << "Starting wtapp!";
   wapp.start();
   qDebug(*log("main", 1)) << "Starting main app";
   return a.exec();
}
