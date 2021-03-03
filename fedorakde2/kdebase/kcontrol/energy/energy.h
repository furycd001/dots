/* vi: ts=8 sts=4 sw=4
 *
 * $Id: energy.h,v 1.3 2001/01/17 06:58:55 jones Exp $
 *
 * This file is part of the KDE project, module kcontrol.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 * 
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 *
 * Based on kcontrol1 energy.h, Copyright (c) 1999 Tom Vijlbrief.
 */

#ifndef __Energy_h_Included__
#define __Energy_h_Included__

#include <qobject.h>
#include <kcmodule.h>

class QCheckBox;
class KIntNumInput;
class KConfig;

extern "C" void init_energy();

/**
 * The Desktop/Energy tab in kcontrol.
 */
class KEnergy: public KCModule
{
    Q_OBJECT
	
public:
    KEnergy(QWidget *parent, const char *name);
    ~KEnergy();

    int buttons();

    virtual void load();
    virtual void save();
    virtual void defaults();

    QString quickHelp() const;

signals:
    void changed(bool);

private slots:
    void slotChangeEnable(bool);
    void slotChangeStandby(int);
    void slotChangeSuspend(int);
    void slotChangeOff(int);

private:
    void readSettings();
    void writeSettings();
    void showSettings();

    static void applySettings(bool, int, int, int);
    friend void init_energy();
	
    bool m_bChanged, m_bEnabled, m_bDPMS, m_bMaintainSanity;
    int m_Standby, m_Suspend, m_Off;
    int m_StandbyDesired, m_SuspendDesired, m_OffDesired;

    QCheckBox *m_pCBEnable;
    KIntNumInput *m_pStandbySlider;
    KIntNumInput *m_pSuspendSlider;
    KIntNumInput *m_pOffSlider;
    KConfig *m_pConfig;
};

#endif // __Energy_h_Included__
