#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>


#include "kaccess.h"


const char * DESCRIPTION = 
  I18N_NOOP("A utility to improve KDE's accessibility for people "
	    "with disabilities.");


int main(int argc, char *argv[])
{
  KAboutData about("kaccess", I18N_NOOP("KDE Accessibility Tool"), "0.1",
                   DESCRIPTION, KAboutData::License_QPL, 
		   "(C) 2000, Matthias Hölzer-Klüpfel");
  KCmdLineArgs::init(argc, argv, &about);  
  KAccessApp::addCmdLineOptions();
  
  if (!KAccessApp::start())
    return 0;

  KAccessApp app;
  return app.exec();		   
}
