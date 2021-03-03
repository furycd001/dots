#include <unistd.h>


#include <kapp.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#include <X11/Xlib.h>  


#include "bgrender.h"
#include "kdmdesktop.h"
#include "kdmdesktop.moc"

#include <config.h>

static const char *description = 
	I18N_NOOP("Fancy desktop background for kdm");

static const char *version = "v1.4";



MyApplication::MyApplication()
  : KApplication(), 
    renderer(0, new KSimpleConfig( 
	QString::fromLatin1 (KDE_CONFDIR "/kdm/kdmrc") ))
{
  connect(&renderer, SIGNAL(imageDone(int)), this, SLOT(renderDone()));
  renderer.start();
}


void MyApplication::renderDone()
{
  QPixmap pm;
  pm.convertFromImage(*renderer.image());
  desktop()->setBackgroundPixmap(pm);
  desktop()->repaint(true);
  quit();
}


int main(int argc, char *argv[])
{
  KLocale::setMainCatalogue("kdesktop");
  KCmdLineArgs::init(argc, argv, "kdmdesktop", description, version);

  MyApplication app;
  
  // Keep color resources after termination
  XSetCloseDownMode(qt_xdisplay(), RetainTemporary);

  app.exec();

  app.flushX();
  
  return 0;
}
