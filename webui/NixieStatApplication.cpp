#include "NixieStatApplication.h"
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTextArea.h>
#include <Wt/WTimer.h>
#include <iostream>

#include "LocationTab.h"

#include <QDebug>

Wt::WTimer*                   NixieStatApplication::m_timer = NULL;
std::list<JsonTab*>           NixieStatApplication::m_tabList;
ThreadsafeQueue<queue_entry_t> NixieStatApplication::m_messages;

NixieStatApplication::NixieStatApplication(const Wt::WEnvironment &env) : WApplication(env),
   m_tab(nullptr)
{
   /*********************************************************/
   /* Do not create anything here! Do that sort of stuff in */
   /* the create() function.                                */
   /*********************************************************/
   setTitle("NixieStat Control");
   auto container = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
   /********************************************/
   /* Create the main Tab Widget and it's tabs */
   /********************************************/
   m_tab = container->addNew<Wt::WTabWidget>(); // (std::make_unique<Wt::WTabWidget>());
   m_tab->addTab(std::make_unique<LocationTab>(), "Locations");
   Wt::log("info") << __PRETTY_FUNCTION__ << "m_timer = " << (unsigned long)m_timer;

   if (m_timer == NULL)
   {
      m_timer = root()->addChild(std::make_unique<Wt::WTimer>());
      m_timer->timeout().connect(this, &NixieStatApplication::processQueue);
      m_timer->setInterval(std::chrono::seconds(5));
      m_timer->start();
      Wt::log("info") << __PRETTY_FUNCTION__ << "Timer created";
   }
}

void NixieStatApplication::processQueue()
{
   Wt::log("info") << __PRETTY_FUNCTION__ << ": Enter";
   while (true)
   {
      queue_entry_t message = m_messages.pop();
      if (message.first.length() == 0)
      {
         break;
      }
      for (int x = 0; x < m_tab->count(); x++)
      {
         JsonTab *tab = dynamic_cast<JsonTab*>(m_tab->widget(x));
         tab->processMessage(message.first, message.second);
      }
   }
   Wt::log("info") << __PRETTY_FUNCTION__ << ": Exit";
}
