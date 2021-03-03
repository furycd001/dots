#include "helpers.h"

#include <kapp.h>

void min_width(QWidget *w) {
  w->setMinimumWidth(w->sizeHint().width());
}

void fixed_width(QWidget *w) {
  w->setFixedWidth(w->sizeHint().width());
}

void min_height(QWidget *w) {
  w->setMinimumHeight(w->sizeHint().height());
}

void fixed_height(QWidget *w) {
  w->setFixedHeight(w->sizeHint().height());
}

void min_size(QWidget *w) {
  w->setMinimumSize(w->sizeHint());
}

void fixed_size(QWidget *w) {
  w->setFixedSize(w->sizeHint());
}

KConfig *klock_config()
{
    QString name( kapp->argv()[0] );
    int slash = name.findRev( '/' );
    if ( slash )
	name = name.mid( slash+1 );

    return new KConfig( name + "rc" );
}

