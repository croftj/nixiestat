#include <Wt/WEnvironment.h>
#include "NixieStatApplication.h"
#include "WtApplication.h"

using namespace Wt;

static std::unique_ptr<WApplication> createApplication(const WEnvironment& env)
{
   return std::make_unique<NixieStatApplication>(env);
}

void WtApplication::run()
{
   WRun(m_argc, m_argv, &createApplication);
}

