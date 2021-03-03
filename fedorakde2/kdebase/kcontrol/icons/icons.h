/* vi: ts=8 sts=4 sw=4
 *
 * $Id: icons.h,v 1.6 2001/07/20 17:57:50 antlarr Exp $
 *
 * This file is part of the KDE project, module kcmdisplay.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 * with minor additions and based on ideas from
 * Torsten Rahn <torsten@kde.org>
 *
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#ifndef __icons_h__
#define __icons_h__

#include <qvaluelist.h>
#include <qcolor.h>
#include <qimage.h>

#include <kcmodule.h>
#include <kdialogbase.h>

class QColor;
class QWidget;
class QCheckBox;
class QComboBox;
class QListBox;
class QListView;
class QSlider;
class QLabel;
class QIconView;
class QTabWidget;
class QGridLayout;
class QPushButton;

class KConfig;
class KIconEffect;
class KIconTheme;
class KIconLoader;
class KColorButton;

struct Effect 
{
    int type;
    float value;
    QColor color;
    bool transparant;
};


/**
 * The General Icons tab in kcontrol.
 */
class KIconConfig: public KCModule
{
    Q_OBJECT

public:
    KIconConfig(QWidget *parent, const char *name=0);
    virtual void load();
    virtual void save();
    virtual void defaults();
    void preview();

signals:
    void changed(bool);

private slots:
    void slotEffectSetup0() { EffectSetup(0); }
    void slotEffectSetup1() { EffectSetup(1); }
    void slotEffectSetup2() { EffectSetup(2); }
    
    void slotUsage(int index);
    void slotSize(int index);
    void slotDPCheck(bool check);
    void slotAlphaBCheck(bool check);

private:
    void preview(int i);
    void EffectSetup(int state);
    QPushButton *addPreviewIcon(int i, const QString &str, QWidget *parent, QGridLayout *lay);
    void init();
    void initDefaults();
    void read();
    void apply();


    bool mbDP[6], mbAlphaB[6], mbChanged[6];
    int mSizes[6];
    QValueList<int> mAvSizes[6];

    Effect mEffects[6][3];
    Effect mDefaultEffect[3];
    
    int mUsage;
    QString mTheme, mExample;
    QStringList mGroups, mStates;

    KIconEffect *mpEffect;
    KIconTheme *mpTheme;
    KIconLoader *mpLoader;
    KConfig *mpConfig;

    typedef QLabel *QLabelPtr;
    QLabelPtr mpPreview[3];

    QListBox *mpUsageList;
    QComboBox *mpSizeBox;
    QCheckBox *mpDPCheck, *wordWrapCB, *underlineCB, *mpAlphaBCheck;
    bool disableAlphaBlending;
    QTabWidget *m_pTabWidget;
    QWidget *m_pTab1;                                    
};

class KIconEffectSetupDialog: public KDialogBase
{
    Q_OBJECT
     
public:
    KIconEffectSetupDialog(const Effect &, const Effect &,
                           const QString &, const QImage &,
			   QWidget *parent=0L, char *name=0L);
    Effect effect() { return mEffect; }

protected:
    void preview();
    void init();

protected slots:
    void slotEffectValue(int value);
    void slotEffectColor(const QColor &col);
    void slotEffectType(int type);
    void slotSTCheck(bool b);
    void slotDefault();

private:
    KIconEffect *mpEffect;
    QListBox *mpEffectBox;
    QCheckBox *mpSTCheck;
    QSlider *mpEffectSlider;
    KColorButton *mpEColButton;
    Effect mEffect;
    Effect mDefaultEffect;
    QImage mExample;
    QLabel *mpPreview;
};                      
                      
#endif
