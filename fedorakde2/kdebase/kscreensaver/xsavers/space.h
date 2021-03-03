
#ifndef __SPACE_H__
#define __SPACE_H__

#include <qtimer.h>
#include <qlist.h>
#include <kdialogbase.h>
#include <qlineedit.h>
#include "saver.h"

class kSpaceSaver : public kScreenSaver
{
	Q_OBJECT
public:
	kSpaceSaver( Drawable drawable );
	virtual ~kSpaceSaver();

	void setSpeed( int spd );
	void setWarp( int l );
	void setPoints( int p );

protected:
	void readSettings();

protected slots:
	void slotTimeout();

protected:
	QTimer      timer;
	int         colorContext;

	int         counter;
	int         speed;
	int			maxLevels;
	int			numPoints;
};

class kSpaceSetup : public KDialogBase
{
	Q_OBJECT
public:
	kSpaceSetup( QWidget *parent = NULL, const char *name = NULL );

protected:
	void readSettings();

private slots:
	void slotSpeed( int );
	void slotWarp( int );
	void slotOk();
	void slotAbout();

private:
	QWidget *preview;
	kSpaceSaver *saver;

	int			speed;
	int			warpinterval;
};

#endif

