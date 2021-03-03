#ifndef __LEGACY_IMPORT_H
#define __LEGACY_IMPORT_H

class QPushButton;
class QLineEdit;

#include <qwizard.h>

class KLegacyImport : public QWizard
{
    Q_OBJECT
public:
    KLegacyImport(QWidget *parent=0, const char *name=0);
    void finished();
protected slots:
    void slotBrowsePixClicked();
    void slotPixEdit();
    void slotBrowseGtkrcClicked();
    void slotGtkrcEdit();

protected:
    QWidget *firstPage, *secondPage;
    QLineEdit *pixDirEdit;
    QLineEdit *gtkrcDirEdit;
    QPushButton *goBtn, *browsePixBtn, *browseGtkrcBtn;
    bool firstStep;

    QString pixmapStr, gtkrcStr;
};

#endif

