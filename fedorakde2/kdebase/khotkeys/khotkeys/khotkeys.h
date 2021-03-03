/****************************************************************************

 KHotKeys -  (C) 2000 Lubos Lunak <l.lunak@email.cz>

 khotkeys.h  -
 
 $Id: khotkeys.h,v 1.2 2000/07/25 20:43:57 lunakl Exp $

****************************************************************************/

#ifndef __khotkeys_H
#define __khotkeys_H

#include <kuniqueapp.h>
#include <qtimer.h>
#include <dcopobject.h>
#include <kurifilter.h>

#include "khkglobalaccel.h"
#include "khotkeysglobal.h"

class KHotKeysApp
    : public KUniqueApplication
    {
    Q_OBJECT
    K_DCOP
    public:
        KHotKeysApp();
        virtual ~KHotKeysApp();
    protected:
        virtual bool x11EventFilter(XEvent *);
        KHKGlobalAccel* accel;
        KHotData_dict data;
        void start_general( const QString& action_P );
        void start_menuentry( const QString& action_P );
    protected slots:
        void accel_activated( const QString& action_P, const QString&, int );
    public:
    k_dcop:
        virtual ASYNC reread_configuration();
    };

//****************************************************************************
// Inline
//****************************************************************************

#endif
