// -*- c++ -*-

#ifndef KASTASKPOPUP_H
#define KASTASKPOPUP_H

#include <kpixmap.h>
#include "kaspopup.h"

class KasTaskItem;

/**
 * A subclass of KasPopup which shows info about a task.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id: kastaskpopup.h,v 1.3 2001/07/17 15:47:48 rich Exp $
 */
class KasTaskPopup : public KasPopup
{
    Q_OBJECT

public:
    KasTaskPopup( KasTaskItem *item, const char *name=0 );
    virtual ~KasTaskPopup();

public slots:
    void refresh();

protected:
    virtual void paintEvent( QPaintEvent * );

 private:
    KasTaskItem *item;
    KPixmap titleBg;
};

#endif // KASTASKPOPUP_H


