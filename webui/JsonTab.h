#ifndef JSON_TAB_H
#define JSON_TAB_H

#include <WTabWidget.h>

#include <QString>

class JsonTab : public Wt::WTabWidget
{
public:
   JsonTab() : Wt::WTabWidget() {}

   virtual void processMessage(QString topic, QString json) = 0;
};

#endif
