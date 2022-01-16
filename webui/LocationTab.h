#ifndef LOCATION_TAB_H
#define LOCATION_TAB_H
#include <Wt/WApplication.h>

#include "JsonTab.h"
#include <forward_list>

#include <QString>
#include <QVariant>

   QString device       = json.take("device").toString();
   QString sensor       = json.value("sensor").toString();
   QString temperature  = json.value("temperature").toString();
   QString timestamp    = json.value("timestamp").toString();
   
class LocationTab : public JsonTab
{
   typedef std::map<QString, QVariant> location_data_map_t;
   typedef std::map<QString, location_data_map_t> location_map_t;
   typedef std::forward_list<QString> location_list_t;

public:
   LocationTab() : JsonTab(),
         m_nextRow(0),
         m_nextCol(0)
   {
      m_locTabs = addNew<Wt::WTabWidget>(); // (std::make_unique<Wt::WTabWidget>());
   }
   virtual void processMessage(QString topic, QString json);

protected:
//   void updateLocation(QString json);
//   void updateLocationData(QString json);

private:
   int   m_nextRow;
   int   m_nextCol;
   location_map_t       m_locations;
   location_list_t      m_locNames;
   Wt::WTabWidget*      m_locTabs;
};

#endif

