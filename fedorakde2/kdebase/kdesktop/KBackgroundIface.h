/**
 * DCOP interface for the background manager.
 */

#ifndef __KBackgroundIface_h__
#define __KBackgroundIface_h__

#include <dcopobject.h>

class KBackgroundIface : virtual public DCOPObject
{
    K_DCOP
public:

k_dcop:
    /** Reread the configuration */
    virtual void configure() = 0;

    /** Enable/disable export of the background pixmap. */
    virtual void setExport(int xport) = 0;

    /** Returns the export desktop pixmap state. */
    virtual bool isExport() = 0;

    /** Enable/disable common desktop background. */
    virtual void setCommon(int common) = 0;

    /** Returns the common desktop background state. */
    virtual bool isCommon() = 0;

    /** Change caching behaviour.
     * @param bLimit Limit cache if not equal to zero.
     * @param size Cache size in kilobytes. */
    virtual void setCache(int bLimit, int size) = 0;

    /** Change the wallpaper.
     * @param wallpaper The (local) path to the wallpaper.
     * @param mode The tiling mode. */
    virtual void setWallpaper(QString wallpaper, int mode) = 0;

    /** Change the wallpaper in "multi mode". */
    virtual void changeWallpaper() = 0;
};

#endif // __KBackgroundIface_h__
