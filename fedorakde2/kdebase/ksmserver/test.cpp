#include <shutdown.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapp.h>

int
main(int argc, char *argv[])
{
   KAboutData about("kapptest", "kapptest", "version");
   KCmdLineArgs::init(argc, argv, &about);

   KApplication a;
   KSMShutdownFeedback::start();
   QObject::connect( KSMShutdownFeedback::self(), SIGNAL( aborted() ), &a, SLOT( quit() ) );

   bool saveSession;
   (void)KSMShutdownDlg::confirmShutdown( saveSession );
   a.exec();
}
