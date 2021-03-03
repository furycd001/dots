// -*- c++ -*-

#ifndef KASABOUTDLG_H
#define KASABOUTDLG_H

#include <kdialogbase.h>

/**
 * About dialog for KasBar
 */
class KasAboutDialog : public KDialogBase
{
   Q_OBJECT

public:
   KasAboutDialog( QWidget *parent );
   ~KasAboutDialog();
};

#endif // KASABOUTDLG_H

