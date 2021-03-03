/**************************************************************************

    config.h  - KPager config dialog
    Copyright (C) 2000  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to larrosa@kde.org

***************************************************************************/
#ifndef __KPCONFIG_H
#define __KPCONFIG_H

#include <kdialogbase.h>

class QCheckBox;
class QButtonGroup;

class KPagerConfigDialog : public KDialogBase
{
    Q_OBJECT

public:
    KPagerConfigDialog(QWidget *parent);

public slots:
    void setShowName(bool show);
    void setShowNumber(bool show);
    void setShowBackground(bool show);
    void setShowWindows(bool show);

    void setWindowDrawMode(int mode);
    void setLayout(int layout);

    void enableWindowDragging(bool);
    
    void loadConfiguration();
    void slotOk();
public:
    static void initConfiguration(bool bIsEmbedded=false);
    static bool m_showName;
    static bool m_showNumber;
    static bool m_showBackground;
    static bool m_showWindows;
    static int m_windowDrawMode;
    static int m_layoutType;
    static bool m_windowDragging;

protected:
    QCheckBox *m_chkShowName;
    QCheckBox *m_chkShowNumber;
    QCheckBox *m_chkShowBackground;
    QCheckBox *m_chkShowWindows;
    QButtonGroup *m_grpWindowDrawMode;
    QButtonGroup *m_grpLayoutType;
    QCheckBox* m_chkWindowDragging;
    bool m_tmpShowName;
    bool m_tmpShowNumber;
    bool m_tmpShowBackground;
    bool m_tmpShowWindows;
    int m_tmpWindowDrawMode;
    int m_tmpLayoutType;
    bool m_tmpWindowDragging;
};

#endif
