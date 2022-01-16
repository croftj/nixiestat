#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QSqlDatabase>
#include <QString>

class DBManager : public QObject
{
   Q_OBJECT

public:
   DBManager(QObject *parent = NULL);

   bool importJson(QJsonObject& json);
   bool insertRecord(QString table, const QJsonValue& json);
   int  addDevice(QString device_name, QJsonObject &json);
   int  addDevice(QString device_name);
   int  addSensor(QString sensor, QString type);
   void bindDeviceSensor(QString device, QString sensor, QString type);
   int  findSensor(QString sensor);
   int  findDevice(QString device);

   bool setDatabaseName(QString type, QString name);
   QSqlDatabase* database()
   {
      return(&m_db);
   }

protected:
   bool createTable(QString table, QJsonValue& json);
   bool addColumn(QString table, QString column, QJsonValue& json);

private:
   QSqlDatabase   m_db;
   const QString  m_dbName;
};

#endif
