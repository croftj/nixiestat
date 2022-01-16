#include "LocationTab.h"

#include <Wt/WApplication>

#include <QDebug>
#include <QJsonDocument>
#include <QObject>
#include <QJsonValue>
#include <QString>

using namespace Wt;

void LocationTab::processMessage(QString topic, QString json)
{
   Wt::log("info") << __PRETTY_FUNCTION__ << " new message, topic " << qPrintable(topic) << ", json: " << qPrintable(json);
#if 1
   QJsonDocument jdoc;
   QJsonParseError jerr;
   jdoc = QJsonDocument::fromJson(json.toLatin1(), &jerr);
   if ( ! jdoc.isNull() )
   {
      QJson json = jdoc.object();
      QString message_type   = json.value("message_type").toString();
      if (message_type == "ping")
      {
         QString device       = json.take("device").toString();
         QString temp_topic   = json.value("temp_topic").toString();
         QString config_topic = json.value("config_topic").toString();
         QString timestamp    = json.value("timestamp").toString();
      }
      if (message_type == "temperature")
      {
         QString device       = json.take("device").toString();
         QString sensor       = json.value("sensor").toString();
         QString temperature  = json.value("temperature").toString();
         QString timestamp    = json.value("timestamp").toString();

         WTable *locTable = dynamic_cast<WTabWidget*>(app->find(device.toStdString()));
         if (locTable == NULL)
         {
            locTable = addTab(std::make_unique<Wt::WTable>(), qPrintable(device));
         }
         std::string dev_sensor = QString(device + '-' + sensor).toStdString();
         SensorData *sens_data = dynamic_cast<SensorData*>(app->find(dev_sensor));
         if (sens_data == NULL)
         {
            WTableCell sens_cell = locTable->elementAt(m_nextRow, m_nextCol);
            sens_data = sens_cell->addWidget(std:make_unique<SensorData>(dev_sensor)
         }
         if (sens_data != NULL)
         {
            sens_data->processMessage(message);
         }
      }
   }
#endif
}
