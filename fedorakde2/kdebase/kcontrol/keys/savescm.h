#ifndef SAVESCM_H
#define SAVESCM_H

#include <kdialogbase.h>
#include <klineedit.h>

class SaveScm : public KDialogBase {
	Q_OBJECT
public:
	SaveScm( QWidget *parent, const char *name, const QString &def );
	
	KLineEdit* nameLine;
};

#endif
