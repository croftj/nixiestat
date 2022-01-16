#ifndef WT_APPLICATION_H
#define WT_APPLICATION_H

#include <QObject>
#include <QThread>

class WtApplication : public QThread
{
   Q_OBJECT

public:
   WtApplication(int argc, char** argv) : QThread(),
         m_argc(argc),
         m_argv(argv)
   {}

   void run() override;

private:
   int      m_argc;
   char**   m_argv;
};

#endif
