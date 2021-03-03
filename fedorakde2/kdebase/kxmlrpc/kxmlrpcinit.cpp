
#include <kconfig.h>
#include <kapp.h>

extern "C"
{

    void init_xmlrpcd()
    {
        KConfig *config = new KConfig( "kxmlrpcdrc", true, false );
        config->setGroup( "General" );

        if ( config->readBoolEntry( "StartServer", false ) )
            kapp->startServiceByDesktopName( "kxmlrpcd" );

	delete config;
    }

};
