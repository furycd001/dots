
#ifndef __SAVER_H__
#define __SAVER_H__

#include <qobject.h>
#include <X11/Xlib.h>

extern void startScreenSaver( Drawable d );
extern void stopScreenSaver();
extern int setupScreenSaver();

//-----------------------------------------------------------------------------
class kScreenSaver : public QObject
{
	Q_OBJECT
public:
	kScreenSaver( Drawable drawable );
	virtual ~kScreenSaver();

protected:
	Drawable mDrawable;
	GC mGc;
	unsigned mWidth;
	unsigned mHeight;
};

#endif

