// -*- c++ -*-

#ifndef KASPREFSDLG_H
#define KASPREFSDLG_H

#include <kdialogbase.h>

class QComboBox;
class QCheckBox;
class QSlider;
class QSpinBox;
class KColorButton;
class KConfig;
class KIntSpinBox;

class KasTasker;

/**
 * Prefs dialog for KasBar
 */
class KasPrefsDialog : public KDialogBase
{
   Q_OBJECT

public:
   KasPrefsDialog( KasTasker *kas, KConfig *config );
   ~KasPrefsDialog();

protected:
   virtual void accept();

   QComboBox *itemSizeCombo;
   QCheckBox *transCheck;
   QCheckBox *tintCheck;
   KColorButton *tintButton;
   QSlider *tintAmount;
   QCheckBox *thumbsCheck;
   QSlider *thumbSizeSlider;
   QSpinBox *thumbUpdateSpin;
   QCheckBox *notifierCheck;
   QCheckBox *modifiedCheck;
   QCheckBox *showAllWindowsCheck;
   KIntSpinBox *maxBoxesSpin;
   QCheckBox *groupWindowsCheck;

   KasTasker *kasbar;
   KConfig *conf;
};

#endif // KASPREFSDLG_H

