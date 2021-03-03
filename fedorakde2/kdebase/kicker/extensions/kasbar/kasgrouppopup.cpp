#include <qlayout.h>

#include "kasgroupitem.h"
#include "kastasker.h"

#include "kasgrouppopup.h"

#ifndef KDE_USE_FINAL
const int TITLE_HEIGHT = 13;
#endif

KasGroupPopup::KasGroupPopup( KasGroupItem *item, const char *name )
    : KasPopup( item, name )
{
    this->item = item;

    QHBoxLayout *box = new QHBoxLayout( this );

    KasTasker *master = static_cast<KasTasker *> (kasbar());
    childBar_ = new KasTasker( ( kasbar()->orientation() == Horizontal ) ? Vertical : Horizontal,
			       master, this );


    box->addWidget( childBar_ );
    resize( childBar_->size() );
}

KasGroupPopup::~KasGroupPopup()
{
}

#include "kasgrouppopup.moc"
