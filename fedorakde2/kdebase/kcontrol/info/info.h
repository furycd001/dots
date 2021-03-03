#ifndef _INFO_H_
#define _INFO_H_

#include <qwidget.h>
#include <qwidgetstack.h>
#include <qframe.h>
#include <qlabel.h>
#include <qtabdialog.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qlistview.h>
#include <qfile.h>
#include <qevent.h>

#include <kcmodule.h>

#include "config.h"

/* function call-back-prototypes... */

bool GetInfo_CPU( QListView *lBox );
bool GetInfo_IRQ( QListView *lBox );
bool GetInfo_DMA( QListView *lBox );
bool GetInfo_PCI( QListView *lBox );
bool GetInfo_IO_Ports( QListView *lBox );
bool GetInfo_Sound( QListView *lBox );
bool GetInfo_Devices( QListView *lBox );
bool GetInfo_SCSI( QListView *lBox );
bool GetInfo_Partitions( QListView *lBox );
bool GetInfo_XServer_and_Video( QListView *lBox );

class KInfoListWidget : public KCModule
{
public:
  KInfoListWidget(const QString &_title, QWidget *parent, const char *name=0, bool _getlistbox (QListView *)=0);

  virtual void load();

  QString quickHelp() const;
  
private:
  QListView 	*lBox;
  bool 		(*getlistbox) (QListView *);
  QString title;
  
  QLabel	*NoInfoText;
  QString	ErrorString;
  QWidgetStack  *widgetStack;
};

#endif
