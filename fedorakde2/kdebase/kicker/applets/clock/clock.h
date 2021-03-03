/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __CLOCK_H
#define __CLOCK_H

#include <qlcdnumber.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qguardedptr.h>
#include <qdatetime.h>
#include <qvbox.h>

#include <kpanelapplet.h>

class QTimer;
class QBoxLayout;
class KConfig;
class KDatePicker;


class DatePicker : public QVBox
{
public:
	DatePicker(QWidget*);
	~DatePicker();
private:
	KDatePicker *picker;
};


class ClockConfDialog;

class ClockSettings : public QObject
{
  Q_OBJECT

public:
	enum ClockType { Plain = 0, Digital, Analog, Fuzzy };

	ClockSettings(QWidget* app, KConfig* conf);
	~ClockSettings();
	void writeSettings();

	void openPreferences();

	void setType(ClockType type);

	ClockType type()     { return _type; }
	bool lcdStyle()      { return _type == Digital?_lcdStyleDig:_lcdStyleAna; }
	bool showSeconds();
        bool showDate();
	QColor foreColor();
	QColor shadowColor();
	QColor backColor();
	bool blink()         { return _blink; }
	QFont font()         { return _type == Plain? _fontPlain:_fontFuz; }
	int fuzzyness()      { return _fuzzynessFuz; }

        QFont dateFont()     { return _fontDate; }
        QColor dateForeColor();

signals:
	void newSettings();

protected slots:
	void dlgOkClicked();
	void dlgApplyClicked();
	void dlgCancelClicked();
	void dlgDeleted();
        void dlgLCDDigitalToggled(bool);
        void dlgLCDAnalogToggled(bool);
	void dlgChooseFontButtonClicked();

protected:
        QWidget *applet;
	KConfig *config;
	ClockType _type;
	QGuardedPtr<ClockConfDialog> confDlg;

	bool _lcdStyleDig, _lcdStyleAna,
	     _showSecsPlain, _showSecsDig, _showSecsAna,
	     _showDatePlain, _showDateDig, _showDateAna,  _showDateFuz,
             _useColDate, _useColPlain, _useColDig, _useColAna, _useColFuz,
	     _blink;
	QColor _foreColorDate, _foreColorPlain, _foreColorDig, _foreColorAna,  _foreColorFuz,
	       _backColorPlain, _backColorDig, _backColorAna,  _backColorFuz,
	       _shadowColorAna, _shadowColorDig;
	int _fuzzynessFuz;
	QFont _fontDate, _fontPlain, _fontFuz;
};


class ClockApplet;

// base class for all clock types
class ClockWidget
{

public:
	ClockWidget(ClockApplet *applet, ClockSettings* settings);
	virtual ~ClockWidget();

	virtual QWidget* widget()=0;
	virtual int preferedWidthForHeight(int h) const =0;
	virtual int preferedHeightForWidth(int w) const =0;
	virtual void updateClock()=0;

protected:
	ClockApplet *_applet;
	ClockSettings *_settings;
};


class PlainClock : public QLabel, public ClockWidget
{
  Q_OBJECT

public:
	PlainClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent=0, const char *name=0);
	~PlainClock();

	QWidget* widget()    { return this; }
	int preferedWidthForHeight(int h) const;
	int preferedHeightForWidth(int w) const;
	void updateClock();

protected:
	QString _timeStr;
};


class DigitalClock : public QLCDNumber, public ClockWidget
{
  Q_OBJECT

public:
	DigitalClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent=0, const char *name=0);
	~DigitalClock();

	QWidget* widget()    { return this; }
	int preferedWidthForHeight(int h) const;
	int preferedHeightForWidth(int w) const;
	void updateClock();

protected:
	void paintEvent( QPaintEvent*);
	void drawContents( QPainter * p);
	void resizeEvent ( QResizeEvent *ev);
	void styleChange( QStyle& );

	QPixmap *_buffer;
	QString _timeStr;
};


class AnalogClock : public QFrame, public ClockWidget
{
  Q_OBJECT

public:
	AnalogClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent=0, const char *name=0);
	~AnalogClock();

	QWidget* widget()                        { return this; }
	int preferedWidthForHeight(int h) const  { return h; }
	int preferedHeightForWidth(int w) const  { return w; }
	void updateClock();

protected:
	virtual void paintEvent(QPaintEvent *);
	void styleChange(QStyle&);

	QTime _time;
};


class FuzzyClock : public QFrame, public ClockWidget
{
  Q_OBJECT

public:
	FuzzyClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent=0, const char *name=0);
	~FuzzyClock();

	QWidget* widget()    { return this; }
	int preferedWidthForHeight(int h) const;
	int preferedHeightForWidth(int w) const;
	void updateClock();

protected:
	virtual void drawContents(QPainter *p);

	QTime _time;
	QString _timeStr;
};


class ClockApplet : public KPanelApplet
{
  Q_OBJECT
  friend class ClockSettings;

public:
	ClockApplet(const QString& configFile, Type t = Normal, int actions = 0,
	            QWidget *parent = 0, const char *name = 0);
	~ClockApplet();

	int widthForHeight(int h) const;
	int heightForWidth(int w) const;
	void preferences();
	Orientation getOrientation()    { return orientation(); }
	void resizeRequest()            { emit(updateLayout()); }

protected slots:
	void slotApplySettings();
	void slotUpdate();
	void slotCalendarDeleted();
	void slotEnableCalendar();
	void slotCopyMenuActivated( int id );

protected:
	void openCalendar();
	void openContextMenu();

	void paletteChange(const QPalette &)   { slotApplySettings(); }
	void mousePressEvent(QMouseEvent *ev);
	bool eventFilter(QObject *, QEvent *);

	ClockSettings *_settings;
	DatePicker *_calendar;
	bool _disableCalendar;
	ClockWidget *_clock;
	QLabel *_date;
	QDate _lastDate;
	QTimer *_timer;
};

#endif
