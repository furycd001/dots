// -*- c++ -*-

#ifndef KASGROUPPOPUP_H
#define KASGROUPPOPUP_H

#include "kaspopup.h"

class KasGroupItem;
class KasTasker;

/**
 * A subclass of KasPopup which shows info about a group.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id: kasgrouppopup.h,v 1.1 2001/05/21 20:59:41 rich Exp $
 */
class KasGroupPopup : public KasPopup
{
   Q_OBJECT

public:
   KasGroupPopup( KasGroupItem *item, const char *name=0 );
   virtual ~KasGroupPopup();

   KasTasker *childBar() { return childBar_; }

private:
   KasGroupItem *item;
   KasTasker *childBar_;
   QTimer *killTimer_;
};

#endif // KASGROUPPOPUP_H


