
#ifndef __KDesktopIface_h__
#define __KDesktopIface_h__

#include <qstringlist.h>
#include <dcopobject.h>
#include <dcopref.h>

class KDesktopIface : virtual public DCOPObject
{
    K_DCOP
public:

k_dcop:
    /**
     * Re-arrange the desktop icons.
     */
    virtual void rearrangeIcons() = 0;
    /**
     * @deprecated
     */
    void rearrangeIcons( bool ) { rearrangeIcons(); }
    /**
     * Lineup the desktop icons.
     */
    virtual void lineupIcons() = 0;
    /**
     * Select all icons
     */
    virtual void selectAll() = 0;
    /**
     * Unselect all icons
     */
    virtual void unselectAll() = 0;
    /**
     * Refresh all icons
     */
    virtual void refreshIcons() = 0;
    /**
     * @return the urls of selected icons
     */
    virtual QStringList selectedURLs() = 0;

    /**
     * Re-read KDesktop's configuration
     */
    virtual void configure() = 0;
    /**
     * Display the "Execute Command" dialog (minicli)
     */
    virtual void popupExecuteCommand() = 0;
    /**
     * Get the background dcop interface (KBackgroundIface)
     */
    DCOPRef background() { return DCOPRef( "kdesktop", "KBackgroundIface" ); }
    /**
     * Get the screensaver dcop interface (KScreensaverIface)
     */
    DCOPRef screenSaver() { return DCOPRef( "kdesktop", "KScreensaverIface" ); }
    /**
     * Full refresh
     */
    virtual void refresh() = 0;
    /**
     * Bye bye
     */
    virtual void logout() = 0;
    /**
     *
     */
    virtual bool isVRoot() = 0;
    /**
     *
     */
    virtual void setVRoot( bool enable )= 0;
};

#endif

