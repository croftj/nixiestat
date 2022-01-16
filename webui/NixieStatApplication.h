#ifndef NIXIE_STAT_APPLICATION_H
#define NIXIE_STAT_APPLICATION_H

#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WTabWidget.h>
#include <Wt/WTimer.h>

#include <utility>

#include "ThreadsafeQueue.h"
#include "JsonTab.h"
#include <QString>

typedef std::pair<QString, QString> queue_entry_t;

class NixieStatApplication : public Wt::WApplication
{
public:
   NixieStatApplication(const Wt::WEnvironment& env);
   void pushMessage(QString topic, QString message)
   {
      m_messages.push(std::make_pair(topic, message));
   }
   
private:
   void              processQueue();
   
   static Wt::WTimer*                     m_timer;
   static std::list<JsonTab*>             m_tabList;
   static ThreadsafeQueue<queue_entry_t>  m_messages;
   Wt::WTabWidget*                        m_tab;
};
#endif
