//-----------------------------------------------------------------------------
//
// KDE Display color scheme setup module
//
// Copyright (c)  Mark Donohoe 1997
//

#ifndef __COLORSCM_H__
#define __COLORSCM_H__

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <kdialogbase.h>

#include <kcmodule.h>

#include "widgetcanvas.h"

class QSlider;
class QComboBox;
class QPushButton;
class QResizeEvent;
class KLineEdit;
class QPalette;
class KListBox;
class KColorButton;
class KConfig;
class KStdDirs;

/**
 * The Desktop/Colors tab in kcontrol.
 */
class KColorScheme: public KCModule
{
    Q_OBJECT

public:
    KColorScheme(QWidget *parent, const char *name);
    ~KColorScheme();

    virtual void load();
    virtual void save();
    virtual void defaults();
    QString quickHelp() const;

signals:
    void changed(bool);

private slots:
    void sliderValueChanged(int val);
    void slotSave();
    void slotAdd();
    void slotRemove();
    void slotSelectColor(const QColor &col);
    void slotWidgetColor(int);
    void slotColorForWidget(int, const QColor &);
    void slotPreviewScheme(int);

private:
    void setColorName( const QString &name, int id );
    void readScheme(int index=0);
    void readSchemeNames();
    int findSchemeByName(const QString &scheme);
    QPalette createPalette();
    
    QColor &color(int index);

    int nSysSchemes;
    bool m_bChanged, useRM;

    QColor colorPushColor;
    QSlider *sb;
    QComboBox *wcCombo;
    QPushButton *addBt, *removeBt;
    KListBox *sList;
    QStringList sFileList;
    QString sCurrentScheme;

    KColorButton *colorButton;
    WidgetCanvas *cs;
};


/**
 * A little dialog which prompts for a name for a color scheme.
 */
class SaveScm: public KDialogBase
{
    Q_OBJECT

public:
    SaveScm(QWidget *parent, const char *name, const QString &def);
	
    KLineEdit* nameLine;
};

#endif
