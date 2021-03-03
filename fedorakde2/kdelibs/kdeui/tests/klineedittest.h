#ifndef _KLINEEDITTEST_H
#define _KLINEEDITTEST_H

#include <qwidget.h>
#include <qguardedptr.h>

class QString;
class QPushButton;

class KLineEdit;

class KLineEditTest : public QWidget
{
    Q_OBJECT

public:
   KLineEditTest ( QWidget *parent=0, const char *name=0 );
   ~KLineEditTest();
   KLineEdit* lineEdit() const { return lineedit; }

private slots:
   void quitApp();
   void slotReturnPressed();
   void resultOutput( const QString& );

protected:
   QGuardedPtr<KLineEdit> lineedit;
   QPushButton* button;
};

#endif
