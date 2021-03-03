/* vi: ts=8 sts=4 sw=4
 *
 * $Id: passwords.h,v 1.3 2000/07/17 15:33:35 kalle Exp $
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 */

#ifndef __Passwords_h_included__
#define __Passwords_h_included__

class QButtonGroup;
class QCheckBox;
class QSpinBox;
class KConfig;

#include <kcmodule.h>

class KPasswordConfig : public KCModule
{
    Q_OBJECT

public:
    KPasswordConfig(QWidget *parent=0, const char *name=0);
    ~KPasswordConfig();

    virtual void load();
    virtual void save();
    virtual void defaults();

    void apply();
    int buttons();
    QString quickHelp() const;

signals:
    void changed(bool);

private slots:

    void slotEchoMode(int);
    void slotKeep(bool);

private:
    QButtonGroup *m_EMGroup;
    QCheckBox *m_KeepBut;
    QSpinBox *m_TimeoutEdit;
    KConfig *m_pConfig;

    int m_Echo, m_Timeout;
    bool m_bKeep;
};

#endif
