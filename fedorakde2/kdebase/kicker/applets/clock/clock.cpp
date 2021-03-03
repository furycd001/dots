/************************************************************

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

#include "clock.h"
#include "conf.h"

#include <qtimer.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qcolor.h>
#include <qtooltip.h>
#include <qclipboard.h>
#include <qtabwidget.h>
#include <qslider.h>
#include <qcombobox.h>

#include <kdatepik.h>
#include <kstddirs.h>
#include <kcolorbutton.h>
#include <kapp.h>
#include <kprocess.h>
#include <kwin.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kfontdialog.h>
#include <kglobalsettings.h>

extern "C"
{
    KPanelApplet* init(QWidget *parent, const QString& configFile)
    {
        KGlobal::locale()->insertCatalogue("clockapplet");
        return new ClockApplet(configFile, KPanelApplet::Normal, KPanelApplet::Preferences, parent, "clockapplet");
    }
}


//************************************************************


DatePicker::DatePicker(QWidget *parent)
    : QVBox( parent, 0,  WType_Popup | WDestructiveClose )
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    picker = new KDatePicker(this);
}


DatePicker::~DatePicker()
{
}


//************************************************************


ClockSettings::ClockSettings(QWidget* app, KConfig* conf)
    : applet(app), config(conf), confDlg(0)
{
    config->setGroup("General");

    QString s = conf->readEntry("Type", "Digital");
    if (s == "Plain")
        _type = Plain;
    else if (s == "Digital")
        _type = Digital;
    else if (s == "Analog")
        _type = Analog;
    else
        _type = Fuzzy;

    config->setGroup("Date");
    _useColDate = config->readBoolEntry("Use_Custom_Colors",false);
    _foreColorDate = config->readColorEntry("Foreground_Color", &KApplication::palette().active().text());
    QFont defFont=KGlobalSettings::generalFont();
    defFont.setPixelSize(10);
    _fontDate = config->readFontEntry("Font",&defFont);

    config->setGroup("Plain");
    _useColPlain = config->readBoolEntry("Use_Custom_Colors",false);
    _foreColorPlain = config->readColorEntry("Foreground_Color", &KApplication::palette().active().text());
    _backColorPlain = config->readColorEntry("Background_Color", &KApplication::palette().active().background());
    _showSecsPlain = config->readBoolEntry("Show_Seconds",false);
    _showDatePlain = config->readBoolEntry("Show_Date",true);
    defFont=KGlobalSettings::generalFont();
    defFont.setPixelSize(19);
    defFont.setBold(true);
    _fontPlain = config->readFontEntry("Font",&defFont);

    config->setGroup("Digital");
    _lcdStyleDig = config->readBoolEntry("LCD_Style",true);
    _useColDig = config->readBoolEntry("Use_Custom_Colors",false);
    _foreColorDig = config->readColorEntry("Foreground_Color", &KApplication::palette().active().text());
    _shadowColorDig = config->readColorEntry("Shadow_Color", &KApplication::palette().active().mid());
    _backColorDig = config->readColorEntry("Background_Color", &KApplication::palette().active().background());
    _showSecsDig = config->readBoolEntry("Show_Seconds",false);
    _showDateDig = config->readBoolEntry("Show_Date",true);
    _blink = config->readBoolEntry("Blink",true);

    config->setGroup("Analog");
    _lcdStyleAna = config->readBoolEntry("LCD_Style",true);
    _useColAna = config->readBoolEntry("Use_Custom_Colors",false);
    _foreColorAna = config->readColorEntry("Foreground_Color", &KApplication::palette().active().text());
    _shadowColorAna = config->readColorEntry("Shadow_Color", &KApplication::palette().active().mid());
    _backColorAna = config->readColorEntry("Background_Color", &KApplication::palette().active().background());
    _showSecsAna = config->readBoolEntry("Show_Seconds",true);
    _showDateAna = config->readBoolEntry("Show_Date",false);

    config->setGroup("Fuzzy");
    _useColFuz = config->readBoolEntry("Use_Custom_Colors",false);
    _foreColorFuz = config->readColorEntry("Foreground_Color", &KApplication::palette().active().text());
    _backColorFuz = config->readColorEntry("Background_Color", &KApplication::palette().active().background());
    _showDateFuz = config->readBoolEntry("Show_Date", false);
    defFont=KGlobalSettings::generalFont();
    _fontFuz = config->readFontEntry("Font",&defFont);
    _fuzzynessFuz = config->readNumEntry("Fuzzyness", 0);
}


ClockSettings::~ClockSettings()
{
    delete confDlg;
}


void ClockSettings::writeSettings()
{
    config->setGroup("General");

    switch (_type) {
        case Plain:
            config->writeEntry("Type", "Plain");
            break;
        case Digital:
            config->writeEntry("Type", "Digital");
            break;
        case Analog:
            config->writeEntry("Type", "Analog");
            break;
        case Fuzzy:
            config->writeEntry("Type", "Fuzzy");
            break;
    }

    config->setGroup("Date");
    config->writeEntry("Use_Custom_Colors",_useColDate);
    config->writeEntry("Foreground_Color",_foreColorDate);
    config->writeEntry("Font", _fontDate);

    config->setGroup("Plain");
    config->writeEntry("Use_Custom_Colors",_useColPlain);
    config->writeEntry("Foreground_Color",_foreColorPlain);
    config->writeEntry("Background_Color",_backColorPlain);
    config->writeEntry("Show_Seconds",_showSecsPlain);
    config->writeEntry("Show_Date",_showDatePlain);
    config->writeEntry("Font", _fontPlain);

    config->setGroup("Digital");
    config->writeEntry("LCD_Style",_lcdStyleDig);
    config->writeEntry("Use_Custom_Colors",_useColDig);
    config->writeEntry("Foreground_Color",_foreColorDig);
    config->writeEntry("Shadow_Color",_shadowColorDig);
    config->writeEntry("Background_Color",_backColorDig);
    config->writeEntry("Show_Seconds",_showSecsDig);
    config->writeEntry("Show_Date",_showDateDig);
    config->writeEntry("Blink",_blink);

    config->setGroup("Analog");
    config->writeEntry("LCD_Style",_lcdStyleAna);
    config->writeEntry("Use_Custom_Colors",_useColAna);
    config->writeEntry("Foreground_Color",_foreColorAna);
    config->writeEntry("Shadow_Color",_shadowColorAna);
    config->writeEntry("Background_Color",_backColorAna);
    config->writeEntry("Show_Seconds",_showSecsAna);
    config->writeEntry("Show_Date",_showDateAna);

    config->setGroup("Fuzzy");
    config->writeEntry("Use_Custom_Colors",_useColFuz);
    config->writeEntry("Foreground_Color", _foreColorFuz);
    config->writeEntry("Background_Color", _backColorFuz);
    config->writeEntry("Show_Date", _showDateFuz);
    config->writeEntry("Font", _fontFuz);
    config->writeEntry("Fuzzyness", _fuzzynessFuz);

    config->sync();
}


void ClockSettings::openPreferences()
{
    if (confDlg) {
        KWin::setActiveWindow( confDlg->winId());
        return;
    }

    confDlg = new ClockConfDialog( applet, 0, FALSE, WDestructiveClose );
    connect(confDlg->buttonOk, SIGNAL(clicked()), this, SLOT(dlgOkClicked()));
    connect(confDlg->buttonApply, SIGNAL(clicked()), this, SLOT(dlgApplyClicked()));
    connect(confDlg->buttonCancel, SIGNAL(clicked()), this, SLOT(dlgCancelClicked()));
    connect(confDlg, SIGNAL( destroyed() ), SLOT( dlgDeleted() ));
    connect(confDlg->chooseFontDate, SIGNAL(clicked()), this, SLOT(dlgChooseFontButtonClicked()));
    connect(confDlg->chooseFontPlain, SIGNAL(clicked()), this, SLOT(dlgChooseFontButtonClicked()));
    connect(confDlg->chooseFontFuzzy, SIGNAL(clicked()), this, SLOT(dlgChooseFontButtonClicked()));

    confDlg->clockCombo->setCurrentItem(_type);
    confDlg->useColorsDate->setChecked(_useColDate);
    confDlg->foregroundDate->setColor(_foreColorDate);
    confDlg->chooseFontDate->setFont(_fontDate);

    confDlg->showDatePlain->setChecked(_showDatePlain);
    confDlg->showSecsPlain->setChecked(_showSecsPlain);
    confDlg->useColorsPlain->setChecked(_useColPlain);
    confDlg->foregroundPlain->setColor(_foreColorPlain);
    confDlg->backgroundPlain->setColor(_backColorPlain);
    confDlg->chooseFontPlain->setFont(_fontPlain);

    confDlg->showDateDigital->setChecked(_showDateDig);
    confDlg->showSecsDigital->setChecked(_showSecsDig);
    confDlg->blinkingDigital->setChecked(_blink);
    confDlg->useColorsDigital->setChecked(_useColDig);
    confDlg->foregroundDigital->setColor(_foreColorDig);
    confDlg->shadowDigital->setColor(_shadowColorDig);
    confDlg->backgroundDigital->setColor(_backColorDig);
    connect(confDlg->lcdDigital, SIGNAL(toggled(bool)), SLOT(dlgLCDDigitalToggled(bool)));
    dlgLCDDigitalToggled(_lcdStyleDig);
    confDlg->lcdDigital->setChecked(_lcdStyleDig);
    confDlg->plainDigital->setChecked(!_lcdStyleDig);

    confDlg->showDateAnalog->setChecked(_showDateAna);
    confDlg->showSecsAnalog->setChecked(_showSecsAna);
    confDlg->useColorsAnalog->setChecked(_useColDig);
    confDlg->foregroundAnalog->setColor(_foreColorAna);
    confDlg->shadowAnalog->setColor(_shadowColorAna);
    confDlg->backgroundAnalog->setColor(_backColorAna);
    connect(confDlg->lcdAnalog, SIGNAL(toggled(bool)), SLOT(dlgLCDAnalogToggled(bool)));
    dlgLCDAnalogToggled(_lcdStyleAna);
    confDlg->lcdAnalog->setChecked(_lcdStyleAna);
    confDlg->plainAnalog->setChecked(!_lcdStyleAna);

    confDlg->showDateFuzzy->setChecked(_showDateFuz);
    confDlg->fuzzyness->setValue(_fuzzynessFuz);
    confDlg->useColorsFuzzy->setChecked(_useColFuz);
    confDlg->foregroundFuzzy->setColor(_foreColorFuz);
    confDlg->backgroundFuzzy->setColor(_backColorFuz);
    confDlg->chooseFontFuzzy->setFont(_fontFuz);

    confDlg->show();
}


void ClockSettings::setType(ClockType type)
{
    _type = type;
    if (confDlg)
        confDlg->clockCombo->setCurrentItem(_type);
}


bool ClockSettings::showSeconds()
{
    switch (_type) {
        case Plain:
            return _showSecsPlain;
        case Digital:
            return _showSecsDig;
        case Analog:
            return _showSecsAna;
        default:
            return false;
    }
}


bool ClockSettings::showDate()
{
    switch (_type) {
        case Plain:
            return _showDatePlain;
        case Digital:
            return _showDateDig;
        case Analog:
            return _showDateAna;
        default:         // fuzzy clock
            return _showDateFuz;
    }
}


QColor ClockSettings::foreColor()
{
    switch (_type) {
        case Plain:
            if (_useColPlain)
                return _foreColorPlain;
            else
                return KApplication::palette().active().text();
        case Digital:
            if (_useColDig)
                return _foreColorDig;
            else
                return KApplication::palette().active().text();
        case Analog:
            if (_useColAna)
                return _foreColorAna;
            else
                return KApplication::palette().active().text();
        default:       // fuzzy clock
            if (_useColFuz)
                return _foreColorFuz;
            else
                return KApplication::palette().active().text();
    }
}


QColor ClockSettings::shadowColor()
{
    if (_type == Digital) {
        if (_useColDig)
            return _shadowColorDig;
        else
           return KApplication::palette().active().mid();
    } else {
        if (_useColAna)
            return _shadowColorAna;
        else
           return KApplication::palette().active().mid();
    }
}


QColor ClockSettings::backColor()
{
    switch (_type) {
        case Plain:
            if (_useColPlain)
                return _backColorPlain;
            else
                return KApplication::palette().active().background();
        case Digital:
            if (_useColDig)
                return _backColorDig;
            else
                return KApplication::palette().active().background();
        case Analog:
            if (_useColAna)
                return _backColorAna;
            else
                return KApplication::palette().active().background();
        default:      // fuzzy clock
            if (_useColFuz)
                return _backColorFuz;
            else
                return KApplication::palette().active().background();
    }
}


QColor ClockSettings::dateForeColor()
{
    if (_useColDate)
        return _foreColorDate;
    else
        return KApplication::palette().active().text();
}


void ClockSettings::dlgOkClicked()
{
    dlgApplyClicked();
    delete confDlg;
}


void ClockSettings::dlgApplyClicked()
{
    _type = (ClockType)confDlg->clockCombo->currentItem();
    _useColDate = confDlg->useColorsDate->isChecked();
    _foreColorDate = confDlg->foregroundDate->color();
    _fontDate = confDlg->chooseFontDate->font();

    _showDatePlain = confDlg->showDatePlain->isChecked();
    _showSecsPlain = confDlg->showSecsPlain->isChecked();
    _useColPlain = confDlg->useColorsPlain->isChecked();
    _foreColorPlain = confDlg->foregroundPlain->color();
    _backColorPlain = confDlg->backgroundPlain->color();
    _fontPlain = confDlg->chooseFontPlain->font();

    _showDateDig = confDlg->showDateDigital->isChecked();
    _showSecsDig = confDlg->showSecsDigital->isChecked();
    _blink = confDlg->blinkingDigital->isChecked();
    _lcdStyleDig = confDlg->lcdDigital->isChecked();
    _useColDig = confDlg->useColorsDigital->isChecked();
    _foreColorDig = confDlg->foregroundDigital->color();
    _shadowColorDig = confDlg->shadowDigital->color();
    _backColorDig = confDlg->backgroundDigital->color();

    _showDateAna = confDlg->showDateAnalog->isChecked();
    _showSecsAna = confDlg->showSecsAnalog->isChecked();
    _lcdStyleAna = confDlg->lcdAnalog->isChecked();
    _useColAna = confDlg->useColorsAnalog->isChecked();
    _foreColorAna = confDlg->foregroundAnalog->color();
    _shadowColorAna = confDlg->shadowAnalog->color();
    _backColorAna = confDlg->backgroundAnalog->color();

    _showDateFuz = confDlg->showDateFuzzy->isChecked();
    _fuzzynessFuz = confDlg->fuzzyness->value();
    _useColFuz = confDlg->useColorsFuzzy->isChecked();
    _foreColorFuz = confDlg->foregroundFuzzy->color();
    _backColorFuz = confDlg->backgroundFuzzy->color();
    _fontFuz = confDlg->chooseFontFuzzy->font();

    writeSettings();
    emit(newSettings());
}


void ClockSettings::dlgCancelClicked()
{
    delete confDlg;
}


void ClockSettings::dlgDeleted()
{
    confDlg = 0;
}


void ClockSettings::dlgLCDDigitalToggled(bool b)
{
    confDlg->useColorsDigital->setEnabled(!b);
    confDlg->foregroundDigitalLabel->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->foregroundDigital->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->shadowDigitalLabel->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->shadowDigital->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->backgroundDigitalLabel->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->backgroundDigital->setEnabled(!b && confDlg->useColorsDigital->isChecked());
}


void ClockSettings::dlgLCDAnalogToggled(bool b)
{
    confDlg->useColorsAnalog->setEnabled(!b);
    confDlg->foregroundAnalogLabel->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->foregroundAnalog->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->shadowAnalogLabel->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->shadowAnalog->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->backgroundAnalogLabel->setEnabled(!b && confDlg->useColorsDigital->isChecked());
    confDlg->backgroundAnalog->setEnabled(!b && confDlg->useColorsDigital->isChecked());
}


void ClockSettings::dlgChooseFontButtonClicked()
{
    const QObject *button=sender();
    KFontDialog *fd = new KFontDialog(0L, "Font Dialog", false, true);

    if (button==confDlg->chooseFontDate)
      fd->setFont(confDlg->chooseFontDate->font());
    else if (button==confDlg->chooseFontPlain)
      fd->setFont(confDlg->chooseFontPlain->font());
    else
      fd->setFont(confDlg->chooseFontFuzzy->font());

    if (fd->exec() == KFontDialog::Accepted) {
        if (button==confDlg->chooseFontDate) {
            _fontDate = fd->font();
            confDlg->chooseFontDate->setFont(_fontDate);
        } else if (button==confDlg->chooseFontPlain) {
            _fontPlain = fd->font();
            confDlg->chooseFontPlain->setFont(_fontPlain);
        } else {
            _fontFuz = fd->font();
            confDlg->chooseFontFuzzy->setFont(_fontFuz);
        }
    }

    delete fd;
}


//************************************************************


ClockWidget::ClockWidget(ClockApplet *applet, ClockSettings* settings)
    : _applet(applet), _settings(settings)
{}


ClockWidget::~ClockWidget()
{}


//************************************************************


PlainClock::PlainClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent, const char *name)
    : QLabel(parent, name), ClockWidget(applet, settings)
{
    setFrameStyle(Panel | Sunken);
    setAlignment(AlignVCenter | AlignHCenter | WordBreak);
    setFont(_settings->font());
    QPalette pal = palette();
    pal.setColor( QColorGroup::Foreground, _settings->foreColor());
    pal.setColor( QColorGroup::Background, _settings->backColor());
    setPalette( pal );
    updateClock();
}


PlainClock::~PlainClock()
{
}


int PlainClock::preferedWidthForHeight(int ) const
{
    return sizeHint().width()-4;
}


int PlainClock::preferedHeightForWidth(int w) const
{
    return heightForWidth(w)-7;
}


void PlainClock::updateClock()
{
    QString newStr = KGlobal::locale()->formatTime(QTime::currentTime(),_settings->showSeconds());

    if (newStr != _timeStr) {
        _timeStr = newStr;
        setText(_timeStr);
    }
}


//************************************************************


DigitalClock::DigitalClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent, const char *name)
    : QLCDNumber(parent, name), ClockWidget(applet, settings)
{
    setFrameStyle(Panel | Sunken);
    setMargin( 4 );
    setSegmentStyle(QLCDNumber::Flat);

    if (_settings->lcdStyle())
        setBackgroundPixmap(KIconLoader("clockapplet").loadIcon("lcd",KIcon::User));
    else
        setBackgroundColor(settings->backColor());

    setNumDigits(settings->showSeconds()? 8:5);

    _buffer = new QPixmap( width(), height() );

    updateClock();
}


DigitalClock::~DigitalClock()
{
}


int DigitalClock::preferedWidthForHeight(int h) const
{
    if (h > 29) h = 29;
    return (numDigits()*h*5/11)+2;
}


int DigitalClock::preferedHeightForWidth(int w) const
{
   return((w / numDigits() * 2) + 6);
}


void DigitalClock::updateClock()
{
    static bool colon = true;
    QString newStr;
    QTime t(QTime::currentTime());

    int h = t.hour();
    int m = t.minute();
    int s = t.second();

    QString format("%02d");

    QString sep(!colon && _settings->blink() ? " " : ":");

    if (_settings->showSeconds())
        format += sep + "%02d";

    if (KGlobal::locale()->use12Clock()) {
        if (h > 12)
            h -= 12;
        else if( h == 0)
            h = 12;

        format.prepend("%2d" + sep);
    } else
        format.prepend("%02d" + sep);


    if (_settings->showSeconds())
        newStr.sprintf(format.latin1(), h, m, s);
    else
        newStr.sprintf(format.latin1(), h, m);

    if (newStr != _timeStr){
        _timeStr = newStr;
        setUpdatesEnabled( FALSE );
        display(_timeStr);
        setUpdatesEnabled( TRUE );
        repaint( FALSE );
    }
    if (_settings->blink())
        colon = !colon;
}


void DigitalClock::paintEvent( QPaintEvent*)
{
    _buffer->fill( this, 0, 0 );
    QPainter p( _buffer );
    drawFrame( &p );
    drawContents( &p );
    p.end();
    bitBlt( this, 0, 0, _buffer, 0, 0);
}


// yes, the colors for the lcd-lock are hardcoded,
// but other colors would break the lcd-lock anyway
void DigitalClock::drawContents( QPainter * p)
{
    setUpdatesEnabled( FALSE );
    QPalette pal = palette();
    if (_settings->lcdStyle())
        pal.setColor( QColorGroup::Foreground, QColor(128,128,128));
    else
        pal.setColor( QColorGroup::Foreground, _settings->shadowColor());
    setPalette( pal );
    p->translate( +1, +1 );
    QLCDNumber::drawContents( p );
    if (_settings->lcdStyle())
        pal.setColor( QColorGroup::Foreground, Qt::black);
    else
        pal.setColor( QColorGroup::Foreground, _settings->foreColor());
    setPalette( pal );
    p->translate( -2, -2 );
    setUpdatesEnabled( TRUE );
    QLCDNumber::drawContents( p );
}


// reallocate buffer pixmap
void DigitalClock::resizeEvent ( QResizeEvent *)
{
    delete _buffer;
    _buffer = new QPixmap( width(), height() );
}


// the background pixmap disappears during a style change
void DigitalClock::styleChange ( QStyle &)
{
    if (_settings->lcdStyle())
        setBackgroundPixmap(KIconLoader("clockapplet").loadIcon("lcd",KIcon::User));
}


//************************************************************


AnalogClock::AnalogClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent, const char *name)
    : QFrame(parent, name), ClockWidget(applet, settings)
{
    setFrameStyle(Panel | Sunken);
    if (_settings->lcdStyle())
        setBackgroundPixmap(KIconLoader("clockapplet").loadIcon("lcd",KIcon::User));
    else
        setBackgroundColor(settings->backColor());

    _time = QTime::currentTime();
    repaint( );
}


AnalogClock::~AnalogClock()
{
}


void AnalogClock::updateClock()
{
    if (!_settings->showSeconds())
        if (_time.minute()==QTime::currentTime().minute())
            return;

    _time = QTime::currentTime();
    repaint( );
}


void AnalogClock::paintEvent( QPaintEvent * )
{
    if ( !isVisible() )
        return;

    QPainter paint;
    paint.begin( this );

    drawFrame( &paint );

    QPointArray pts;
    QPoint cp = rect().center();

    int d = QMIN(width(),height())-10;

    if (_settings->lcdStyle()) {
        paint.setPen( QColor(100,100,100) );
        paint.setBrush( QColor(100,100,100) );
    } else {
        paint.setPen( _settings->shadowColor() );
        paint.setBrush( _settings->shadowColor() );
    }

    paint.setViewport(2,2,width(),height());

    for ( int c=0 ; c < 2 ; c++ ) {
        QWMatrix matrix;
        matrix.translate( cp.x(), cp.y() );
        matrix.scale( d/1000.0F, d/1000.0F );

        // hour
        float h_angle = 30*(_time.hour()%12-3) + _time.minute()/2;
        matrix.rotate( h_angle );
        paint.setWorldMatrix( matrix );
        pts.setPoints( 4, -20,0,  0,-20, 300,0, 0,20 );
        paint.drawPolygon( pts );
        matrix.rotate( -h_angle );

        // minute
        float m_angle = (_time.minute()-15)*6;
        matrix.rotate( m_angle );
        paint.setWorldMatrix( matrix );
        pts.setPoints( 4, -10,0, 0,-10, 400,0, 0,10 );
        paint.drawPolygon( pts );
        matrix.rotate( -m_angle );

        if (_settings->showSeconds()) {   // second
            float s_angle = (_time.second()-15)*6;
            matrix.rotate( s_angle );
            paint.setWorldMatrix( matrix );
            pts.setPoints(4,0,0,0,0,400,0,0,0);
            paint.drawPolygon( pts );
            matrix.rotate( -s_angle );
        }

        QWMatrix matrix2;
        matrix2.translate( cp.x(), cp.y() );
        matrix2.scale( d/1000.0F, d/1000.0F );

        // quadrante
        for ( int i=0 ; i < 12 ; i++ ) {
            paint.setWorldMatrix( matrix2 );
            paint.drawLine( 460,0, 500,0 );	// draw hour lines
            // paint.drawEllipse( 450, -15, 30, 30 );
            matrix2.rotate( 30 );
        }

        if (_settings->lcdStyle()) {
            paint.setPen( Qt::black );
            paint.setBrush( Qt::black );
        } else {
            paint.setPen( _settings->foreColor() );
            paint.setBrush( _settings->foreColor() );
        }

        paint.setViewport(0,0,width(),height());
    }
    paint.end();
}


// the background pixmap disappears during a style change
void AnalogClock::styleChange(QStyle &)
{
    if (_settings->lcdStyle())
        setBackgroundPixmap(KIconLoader("clockapplet").loadIcon("lcd",KIcon::User));
}


//************************************************************


FuzzyClock::FuzzyClock(ClockApplet *applet, ClockSettings* settings, QWidget *parent, const char *name)
    : QFrame(parent, name), ClockWidget(applet, settings)
{
    setFrameStyle(Panel | Sunken);
    setBackgroundColor(_settings->backColor());

    _time = QTime::currentTime();
    repaint( );
}


FuzzyClock::~FuzzyClock()
{
}


int FuzzyClock::preferedWidthForHeight(int ) const
{
    QFontMetrics fm(_settings->font());
    return fm.width(_timeStr) + 8;
}


int FuzzyClock::preferedHeightForWidth(int ) const
{
    QFontMetrics fm(_settings->font());
    return fm.width(_timeStr) + 8;
}


void FuzzyClock::updateClock()
{
  if (_time.minute()==QTime::currentTime().minute())
     return;
  else {
     _time = QTime::currentTime();
     repaint( );
  }
}


void FuzzyClock::drawContents(QPainter *p)
{
    if (!isVisible())
        return;

    QString newTimeStr;

    if (_settings->fuzzyness() == 1 || _settings->fuzzyness() == 2) {
      QStringList hourNames = QStringList() << i18n("hour","one") << i18n("hour","two")
                << i18n("hour","three") << i18n("hour","four") << i18n("hour","five")
                << i18n("hour","six") << i18n("hour","seven") << i18n("hour","eight")
                << i18n("hour","nine") << i18n("hour","ten") << i18n("hour","eleven")
                << i18n("hour","twelve");

      QStringList normalFuzzy; // xgettext:no-c-format
      normalFuzzy << i18n("%0 o'clock") // xgettext:no-c-format
                  << i18n("five past %0") // xgettext:no-c-format
                  << i18n("ten past %0") // xgettext:no-c-format
                  << i18n("quarter past %0") // xgettext:no-c-format
                  << i18n("twenty past %0") // xgettext:no-c-format
                  << i18n("twenty five past %0") // xgettext:no-c-format
                  << i18n("half past %0") // xgettext:no-c-format
                  << i18n("twenty five to %1") // xgettext:no-c-format
                  << i18n("twenty to %1") // xgettext:no-c-format
                  << i18n("quarter to %1") // xgettext:no-c-format
                  << i18n("ten to %1") // xgettext:no-c-format
                  << i18n("five to %1") // xgettext:no-c-format
                  << i18n("%1 o'clock"); // xgettext:no-c-format
      QStringList normalFuzzyOne; // xgettext:no-c-format
      normalFuzzyOne << i18n("one","%0 o'clock") // xgettext:no-c-format
                     << i18n("one","five past %0") // xgettext:no-c-format
                     << i18n("one","ten past %0") // xgettext:no-c-format
                     << i18n("one","quarter past %0") // xgettext:no-c-format
                     << i18n("one","twenty past %0") // xgettext:no-c-format
                     << i18n("one","twenty five past %0") // xgettext:no-c-format
                     << i18n("one","half past %0") // xgettext:no-c-format
                     << i18n("one","twenty five to %1") // xgettext:no-c-format
                     << i18n("one","twenty to %1") // xgettext:no-c-format
                     << i18n("one","quarter to %1") // xgettext:no-c-format
                     << i18n("one","ten to %1") // xgettext:no-c-format
                     << i18n("one","five to %1") // xgettext:no-c-format
                     << i18n("one","%1 o'clock"); // xgettext:no-c-format
      int minute = _time.minute();
      int sector = 0;
      int realHour = 0;

      if (_settings->fuzzyness() == 1) {
          if (minute > 2)
              sector = (minute - 3) / 5 + 1;
      } else {
          if (minute > 6)
              sector = ((minute - 7) / 15 + 1) * 3;
      }

      newTimeStr = normalFuzzy[sector];
      int phStart = newTimeStr.find("%");
      int phLength = newTimeStr.find(" ", phStart) - phStart;

      // larrosa: we want the exact length, in case the translation needs it,
      // in other case, we would cut off the end of the translation.
      if (phLength < 0) phLength = newTimeStr.length() - phStart;
      int deltaHour = newTimeStr.mid(phStart + 1, phLength - 1).toInt();

      if ((_time.hour() + deltaHour) % 12 > 0)
          realHour = (_time.hour() + deltaHour) % 12 - 1;
      else
          realHour = 12 - ((_time.hour() + deltaHour) % 12 + 1);
      if (realHour==0) {
          newTimeStr = normalFuzzyOne[sector];
          phStart = newTimeStr.find("%");
         // larrosa: Note that length is the same,
         // so we only have to update phStart
      }
      newTimeStr.replace(phStart, phLength, hourNames[realHour]);
      newTimeStr.replace(0, 1, QString(newTimeStr.at(0).upper()));

    } else if (_settings->fuzzyness() == 3) {
        QStringList dayTime = QStringList() << i18n("Night")
                   << i18n("Early morning") << i18n("Morning") << i18n("Almost noon")
                   << i18n("Noon") << i18n("Afternoon") << i18n("Evening")
                   << i18n("Late evening");
        newTimeStr = dayTime[_time.hour() / 3];
    } else {
        int dow = QDate::currentDate().dayOfWeek();

        if (dow == 1)
            newTimeStr = i18n("Start of week");
        else if (dow >= 2 && dow <= 4)
            newTimeStr = i18n("Middle of week");
        else if (dow == 5)
            newTimeStr = i18n("End of week");
        else
            newTimeStr = i18n("Weekend!");
    }

    if (_timeStr != newTimeStr) {
        _timeStr = newTimeStr;
        _applet->resizeRequest();
    }

    p->setFont(_settings->font());
    p->setPen(_settings->foreColor());
    if (_applet->getOrientation() == Vertical) {
        p->rotate(90);
        p->drawText(4, -2, height() - 8, -(width()) + 2, AlignCenter, _timeStr);
    } else {
        p->drawText(4, 2, width() - 8, height() - 4, AlignCenter, _timeStr);
    }
}


//************************************************************


ClockApplet::ClockApplet(const QString& configFile, Type t, int actions,
                         QWidget *parent, const char *name)
    : KPanelApplet(configFile, t, actions, parent, name),
      _calendar(0), _disableCalendar(false), _clock(0)
{
    _settings = new ClockSettings(this, config());
    connect(_settings, SIGNAL(newSettings()), SLOT(slotApplySettings()));

    setBackgroundMode(QWidget::X11ParentRelative);

    _date = new QLabel(this);
    _date->setAlignment(AlignVCenter | AlignHCenter | WordBreak);
    _date->setBackgroundMode(QWidget::X11ParentRelative);
    _date->installEventFilter(this);   // catch mouse clicks
    _lastDate = QDate::currentDate();
    _date->setText(KGlobal::locale()->formatDate(_lastDate, true));
    QToolTip::add(_date, KGlobal::locale()->formatDate(_lastDate, false));

    _timer = new QTimer(this);

    slotApplySettings();    // initialize clock widget

    connect(_timer, SIGNAL(timeout()), SLOT(slotUpdate()));
    _timer->start(1000);
}


ClockApplet::~ClockApplet()
{
    if (_calendar)
        _calendar->close();
    delete _settings;
}


int ClockApplet::widthForHeight(int h) const
{
    int shareDateHeight = 0;
    bool dateToSide = false;
    if (_settings->showDate()) {
        if (h < 32)
            dateToSide = true;
        else   // put date underneath
            shareDateHeight = _date->sizeHint().height();
    }

    int clockWidth = _clock->preferedWidthForHeight(h-shareDateHeight);

    int w;
    if (!_settings->showDate()) {
        w = clockWidth;
        _clock->widget()->setFixedSize(w, h);
    } else {
        int dateWidth = _date->sizeHint().width() + 4;
        if (dateToSide) {
            w = clockWidth + dateWidth;
            _clock->widget()->setFixedSize(clockWidth, h);
            _date->setFixedSize(dateWidth, h);
            _date->move(clockWidth, 0);
        } else {
            w = (clockWidth > dateWidth ? clockWidth : dateWidth);
            _clock->widget()->setFixedSize(w, h - shareDateHeight);
            _date->setFixedSize(w, shareDateHeight);
            _date->move(0, _clock->widget()->height());
        }
    }

    return w;
}


int ClockApplet::heightForWidth(int w) const
{
    int clockHeight = _clock->preferedHeightForWidth(w);
    _clock->widget()->setFixedSize(w, clockHeight);

    // add in height for date, if visible
    if (_settings->showDate()) {
        _date->setFixedSize(w,_date->heightForWidth(w));
        _date->move(0, clockHeight);
        clockHeight += _date->height();
    }

    return clockHeight;
}


void ClockApplet::preferences()
{
    _settings->openPreferences();
}


void ClockApplet::slotApplySettings()
{
    delete _clock;

    switch (_settings->type()) {
        case ClockSettings::Plain:
            _clock = new PlainClock(this, _settings, this);
            break;
        case ClockSettings::Digital:
            _clock = new DigitalClock(this, _settings, this);
            break;
        case ClockSettings::Analog:
            _clock = new AnalogClock(this, _settings, this);
            break;
        case ClockSettings::Fuzzy:
            _clock = new FuzzyClock(this, _settings, this);
            break;
    }

    QToolTip::add(_clock->widget(),KGlobal::locale()->formatDate(_lastDate, false));
    _clock->widget()->installEventFilter(this);   // catch mouse clicks
    _clock->widget()->show();

    _date->setFont(_settings->dateFont());
    QPalette pal = _date->palette();
    pal.setColor(QColorGroup::Foreground, _settings->dateForeColor());
    _date->setPalette(pal);
    if (_settings->showDate()) {
        _date->show();
        _date->repaint(true);
    } else
        _date->hide();

    emit(updateLayout());
}


void ClockApplet::slotUpdate()
{
    // timer fires every second, update date-label when
    // necessary...
    if (_lastDate != QDate::currentDate()) {
        _lastDate = QDate::currentDate();
        _date->setText(KGlobal::locale()->formatDate(_lastDate, true));

        QString dateStr = KGlobal::locale()->formatDate(_lastDate, false);
        QToolTip::add(_clock->widget(),dateStr);
        QToolTip::add(_date,dateStr);
    }
    _clock->updateClock();
}


void ClockApplet::slotCalendarDeleted()
{
    _calendar = 0L;
    // don't reopen the calendar immediately ...
    _disableCalendar = true;
    QTimer::singleShot(100, this, SLOT(slotEnableCalendar()));
}


void ClockApplet::slotEnableCalendar()
{
    _disableCalendar = false;
}


void ClockApplet::openCalendar()
{
    if (_disableCalendar)
        return;

    _calendar = new DatePicker(this);
    connect( _calendar, SIGNAL( destroyed() ), SLOT( slotCalendarDeleted() ));
    Direction d  = popupDirection();
    QRect deskR = QApplication::desktop()->rect();

    // some extra spacing is included if aligned on a desktop edge
    QPoint c = mapToGlobal(pos());;

    if (d == KPanelApplet::Up){
        c.setY(c.y()-_calendar->sizeHint().height()-2);
        if(c.x() + _calendar->sizeHint().width() > deskR.right())
            c.setX(deskR.right()-_calendar->sizeHint().width()-1);
    }
    else if (d == KPanelApplet::Down){
        c.setY(c.y()+height()+2);
        if(c.x() + _calendar->sizeHint().width() > deskR.right())
            c.setX(deskR.right()-_calendar->sizeHint().width()-1);
    }
    else if (d == KPanelApplet::Right){
        c.setX(c.x()+width()+2);
        if(c.y() + _calendar->sizeHint().height() > deskR.bottom())
            c.setY(deskR.bottom()-_calendar->sizeHint().height()-1);
    }
    else{ // left
        c.setX(c.x()-_calendar->sizeHint().width()-2);
        if(c.y() + _calendar->sizeHint().height() > deskR.bottom())
            c.setY(deskR.bottom()-_calendar->sizeHint().height()-1);
    }

    _calendar->move(c);
    qApp->processEvents(); // make sure it is moved before showing
    _calendar->show();
}


void ClockApplet::openContextMenu()
{
    KPopupMenu *copyMenu = new KPopupMenu();
    KLocale *loc = KGlobal::locale();

    QDateTime dt = QDateTime::currentDateTime();
    copyMenu->insertItem(loc->formatDateTime(dt), 201);
    copyMenu->insertItem(loc->formatDate(dt.date()), 202);
    copyMenu->insertItem(loc->formatDate(dt.date(), true), 203);
    copyMenu->insertItem(loc->formatTime(dt.time()), 204);
    copyMenu->insertItem(loc->formatTime(dt.time(), true), 205);
    copyMenu->insertItem(dt.date().toString(), 206);
    copyMenu->insertItem(dt.time().toString(), 207);
    copyMenu->insertItem(dt.toString(), 208);
    connect( copyMenu, SIGNAL( activated(int) ), this, SLOT( slotCopyMenuActivated(int) ) );

    KPopupMenu *menu = new KPopupMenu();
    menu->insertTitle( SmallIcon( "clock" ), i18n( "Clock" ) );

    KPopupMenu *type = new KPopupMenu(menu);
    type->insertItem(i18n("&Plain"), ClockSettings::Plain, 1);
    type->insertItem(i18n("&Digital"), ClockSettings::Digital, 2);
    type->insertItem(i18n("&Analog"), ClockSettings::Analog, 3);
    type->insertItem(i18n("&Fuzzy"), ClockSettings::Fuzzy, 4);
    type->setItemChecked((int)_settings->type(),true);

    menu->insertItem(i18n("&Type"), type, 101, 1);
    menu->insertItem(SmallIcon("configure"), i18n("&Preferences..."), 102, 2);
    menu->insertItem(SmallIcon("date"), i18n("&Adjust Date && Time..."), 103, 3);
    menu->insertItem(i18n("Date && Time &Format..."), 104, 4);
    menu->insertItem(SmallIcon("editcopy"), i18n("&Copy"), copyMenu, 105, 5 );

    int result = menu->exec( QCursor::pos() );

    if ((result >= 0) && (result < 100)) {
        _settings->setType((ClockSettings::ClockType) result);
        _settings->writeSettings();
        slotApplySettings();
    }
   else if ( result == 102 ) {
        _settings->openPreferences();
    }
    else if( result == 103 ) {
        KProcess proc;
        proc << locate("exe", "kdesu2");
        proc << locate("exe", "kcmshell");
        proc << "clock";
        proc.start(KProcess::DontCare);
    }
    else if(result == 104) {
        KProcess proc;
        proc << locate("exe", "kcmshell");
        proc << "language";
        proc.start(KProcess::DontCare);
    }

    delete copyMenu;
    delete menu;
}


void ClockApplet::slotCopyMenuActivated( int id )
{
    QPopupMenu *m = (QPopupMenu *) sender();
    QString s = m->text(id);
    QApplication::clipboard()->setText(s);
}


void ClockApplet::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == QMouseEvent::LeftButton)
        openCalendar();
    if (ev->button() == QMouseEvent::RightButton)
        openContextMenu();
}


// catch the mouse clicks of our child widgets
bool ClockApplet::eventFilter( QObject *o, QEvent *e )
{
    if ( ( o == _clock->widget() || o == _date ) && e->type() == QEvent::MouseButtonPress ) {
	mousePressEvent(static_cast<QMouseEvent*>(e) );
	return TRUE;
    }

    return KPanelApplet::eventFilter(o, e);
}


//************************************************************

#include "clock.moc"
