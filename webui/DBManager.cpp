#include "DBManager.h"
#include "QcjLib/LogBuilder.h"
#include "QcjLib/Logger.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStringList>
#include <QSqlError>

using namespace QcjLib;

static LogBuilder mylog("dbm", 3, "");

DBManager::DBManager(QObject *parent) :
           QObject(parent)
{
}

bool DBManager::setDatabaseName(QString type, QString name)
{
   m_db = QSqlDatabase::addDatabase(type, "dbmanager");
//   m_db.setHostName("localhost");
   m_db.setHostName("127.0.0.1");
   m_db.setUserName("croftj");
   m_db.setPort(5432);
   m_db.setDatabaseName(name);
   bool rv = m_db.open();
   if (rv)
   {
      qDebug() << "setDatabasename(): db opened: rv = " << rv;
   }
   else
   {
      qDebug() << "setDatabaseName(): db err: " << m_db.lastError().databaseText() <<
                                   " drv err: " << m_db.lastError().driverText();
   }
   return(rv);
}

void DBManager::bindDeviceSensor(QString device, QString sensor, QString type)
{
   QString sql;

   int devc_fk = addDevice(device);
   int sens_fk = addSensor(sensor, type);

   sql = "select device_fk from device_sensor where sensor_fk = :sfk";
   QSqlQuery q1(m_db);
   q1.prepare(sql);
   q1.bindValue(":sfk", sens_fk);
   if (q1.exec() && q1.next())
   {
      int d_fk = q1.value(0).toInt();
      if (d_fk != devc_fk)
      {
         sql = "delete from device_sensor where device_fk = :dfk";
         qDebug(*log("dbm", 1)) << "deleting binding for sensor, sql = " << sql;
         QSqlQuery q2(m_db);
         q2.prepare(sql);
         q2.bindValue(":dfk", devc_fk);
         q2.exec();
         sql = "insert into device_sensor (device_fk, sensor_fk) values (:dfk, :sfk)";
      }
      else
      {
         sql.clear();
      }
   }
   else
   {
      sql = "insert into device_sensor (device_fk, sensor_fk) values (:dfk, :sfk)";
   }

   if (sql.length() > 0)
   {
      qDebug(*log("dbm", 1)) << "binding sensor sql = " << sql;
      qDebug(*log("dbm", 1)) << "sensor_fk: " << sens_fk << ", device_fk: " << devc_fk;
      QSqlQuery q2(m_db);
      q2.prepare(sql);
      q2.bindValue(":dfk", devc_fk);
      q2.bindValue(":sfk", sens_fk);
      q2.exec();
   }
}

int DBManager::findSensor(QString sensor)
{
   int rv = -1;
   QString sql("select ident from sensors where sensor_id = :sensor");
   qDebug(*log("dbm", 1)) << "finding sensor sql = " << sql;
   qDebug(*log("dbm", 1)) << "sensor: " << sensor;
   QSqlQuery q1(m_db);
   q1.prepare(sql);
   q1.bindValue(":sensor", sensor);
   if (q1.exec() && q1.next())
   {
      rv = q1.value(0).toInt();
   }
   qDebug(*log("dbm", 1)) << "sensor_fk: " << rv;
   return(rv);
}

int DBManager::findDevice(QString device)
{
   int rv = -1;
   QString sql("select ident from devices where name = :name");
   QSqlQuery q1(m_db);
   q1.prepare(sql);
   q1.bindValue(":name", device);
   if (q1.exec() && q1.next())
   {
      rv = q1.value(0).toInt();
   }
   return(rv);
}

int DBManager::addDevice(QString device_name, QJsonObject &json)
{
   int rv = -1;

   int dev_fk = addDevice(device_name);
   QStringList jkeys = json.keys();
   QSqlTableModel devtbl(this, m_db);
   devtbl.setTable("devices");
   QSqlRecord devrec = devtbl.record();

   QStringList values;
   foreach(QString field, jkeys)
   {
      QJsonValue jv = json.value(field);
      if (! devrec.contains(field))
      {
         addColumn("devices", field, jv);
      }
      if (jv.isBool())
      {
         values << field + " = " + (jv.toBool() ? QString::number(1) : QString::number(0));
      }
      else if (jv.isDouble())
      {
         if (field == "timestamp")
         {
            values << field + " = " + QString::number((int)jv.toDouble());
         }
         else 
         {
            values << field + " = " + QString::number(jv.toDouble());
         }
      }
      else if (jv.isString())
      {
         values << field + " = " + QString("'") + jv.toString() + QString("'");
      }
   }
   QString sql("update devices set %%values%% where ident = :dev_fk");
   sql.replace("%%values%%", values.join(","));
   qDebug(*log("dbm", 1)) << "addDevice: sql = " << sql;
   QSqlQuery q1(m_db);
   q1.prepare(sql);
   q1.bindValue(":dev_fk", dev_fk);
   rv = q1.exec();
   return(rv);
}

int DBManager::addDevice(QString name)
{
   int rv = findDevice(name);
   if (rv < 0)
   {
      QString sql;
      sql = QString("insert into devices (name) values (:name)");
      
      QSqlQuery q2(m_db);
      q2.prepare(sql);
      q2.bindValue(":name", name);
      q2.exec();
      rv = findDevice(name);
   }
   return(rv);
}

int DBManager::addSensor(QString sensor, QString type)
{
   int rv = findSensor(sensor);
   if (rv < 0)
   {
      QString sql = QString("insert into sensors (sensor_id, type) values (:sensor, :type)");
      qDebug(*log("dbm", 1)) << "Adding sensor sql = " << sql;
      QSqlQuery q2(m_db);
      q2.prepare(sql);
      q2.bindValue(":sensor", sensor);
      q2.bindValue(":type", type);
      q2.exec();
      rv = findSensor(sensor);
   }
   return(rv);
}

/************************************************************/
/* This funstion iterates through the passed in json object */
/* and adds the records to the appropriate table. If the    */
/* table does not exist, it will be created.                */
/*                                                          */
/* A false will be returned if there are any problems       */
/************************************************************/
bool DBManager::importJson(QJsonObject &json)
{
   bool rv = true;

   /*******************************************************/
   /* Get the existing names in the database and the list */
   /*    of tables represented in the json object         */
   /*******************************************************/
   QStringList tbl_names = m_db.tables();
   QStringList json_keys = json.keys();

   qDebug() << "tbl_names = " << tbl_names;
   /******************************************************/
   /* Iterate through each table represented in the json */
   /* object                                             */
   /******************************************************/
   foreach(QString table, json_keys)
   {
      qDebug() << "importJson(): table = " << table;
      if (! rv)
      {
         break;
      }

      /*************************************/
      /* Get the record data for the table */
      /*************************************/
      QJsonValue json_record = json.value(table);

      /*************************************************/
      /* if the table does not exist, try to create it */
      /*************************************************/
      if ( ! tbl_names.contains(table))
      {
         qDebug() << "Creating table: " << json_record;
         rv = createTable(table, json_record);
      }
      /**************************************/
      /* If we have a table, add the record */
      /**************************************/
      rv = rv && insertRecord(table, json_record);
   }
   return(rv);
}

bool DBManager::createTable(QString table, QJsonValue& json)
{
   bool rv = false;
   if (json.isObject())
   {
      QString sql("create table %%table%% (%%fields%%);");
      QJsonObject json_record = json.toObject();
      QStringList field_names = json_record.keys();
      QStringList fields;
      fields << "ident integer primary key autoincrement";
      fields << "read bool";
      foreach(QString field, field_names)
      {
         QJsonValue jv = json_record.value(field);
         if (jv.isBool())
         {
            fields << field + QString(" bool");
         }
         else if (jv.isDouble())
         {
            fields << field + QString(" double");
         }
         else if (jv.isString())
         {
            fields << field + QString(" text");
         }
      }
      sql.replace("%%table%%", table);
      sql.replace("%%fields%%", fields.join(","));
      QSqlQuery q1(m_db);
      q1.prepare(sql);
      qDebug() << "createTable(): sql = " << sql;
      rv = q1.exec();
   }
   return(rv);
}

bool DBManager::insertRecord(QString table, const QJsonValue& json)
{
   bool rv = false;
   QSqlTableModel tbl(this, m_db);
   tbl.setTable(table);
   QSqlRecord rec = tbl.record();
   if (json.isObject())
   {
      QString sql("insert into %%table%% (%%fields%%) values (%%values%%)");
      QJsonObject json_record = json.toObject();
      QStringList field_names = json_record.keys();
      QStringList values;
      QSqlRecord rec = m_db.record(table);
      foreach(QString field, field_names)
      {
         QJsonValue jv = json_record.value(field);
         if (! rec.contains(field))
         {
            addColumn(table, field, jv);
         }
         if (jv.isBool())
         {
            values << (jv.toBool() ? QString::number(1) : QString::number(0));
         }
         else if (jv.isDouble())
         {
            if (field == "timestamp")
            {
               values << QString::number((int)jv.toDouble());
            }
            else 
            {
               values << QString::number(jv.toDouble());
            }
         }
         else if (jv.isString())
         {
            values << QString("'") + jv.toString() + QString("'");
         }
      }
      sql.replace("%%table%%", table);
      sql.replace("%%fields%%", field_names.join(","));
      sql.replace("%%values%%", values.join(","));
      qDebug() << "insertRecord(): sql = " << sql;
      QSqlQuery q1(m_db);
      q1.prepare(sql);
      rv = q1.exec();
   }
   return(rv);
}

bool DBManager::addColumn(QString table, QString column, QJsonValue& json)
{
   bool rv = false;
   if (json.isObject())
   {
      QString sql("alter table %%table%% add column %%fields%%;");
      QJsonObject json_record = json.toObject();
      QStringList field_names = json_record.keys();
      QStringList fields;
      QJsonValue jv = json_record.value(column);
      if (jv.isBool())
      {
         fields << column + QString(" bool");
      }
      else if (jv.isDouble())
      {
         fields << column + QString(" double");
      }
      else if (jv.isString())
      {
         fields << column + QString(" text");
      }
      sql.replace("%%table%%", table);
      sql.replace("%%fields%%", fields.join(","));
      QSqlQuery q1(m_db);
      q1.prepare(sql);
      rv = q1.exec();
   }
   return(rv);
}


