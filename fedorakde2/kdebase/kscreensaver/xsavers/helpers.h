#ifndef __HELPERS__H__
#define __HELPERS__H__

#include <qwidget.h>
#include <kconfig.h>

void min_width(QWidget *);
void fixed_width(QWidget *);
void min_height(QWidget *);
void fixed_height(QWidget *);
void min_size(QWidget *);
void fixed_size(QWidget *);

/*
 * Use this to get a KConfig object that uses a reasonable config filename.
 * KGlobal::config() will use the klockrc config file.
 *
 * Caller must delete the object when finished.
 */
KConfig *klock_config();

#endif

