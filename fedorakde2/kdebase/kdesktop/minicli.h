/*
*
* minicli.h
* Copyright (C) 1997 Matthias Ettrich <ettrich@kde.org>
* copyright (C) 1997 Torben Weis [ Added command completion ]
* Copyright (C) 1999 Preston Brown <pbrown@kde.org>
*
* Complete Re-write:
* Copyright (C) 1999,2000 Dawit Alemayehu <adawit@kde.org>
* Copyright (C) 2000 Malte Starostik <starosti@zedat.fu-berlin.de>
*
* Kdesu integration:
* Copyright (C) 2000 Geert Jansen <jansen@kde.org>
*/

#ifndef MINICLI_H
#define MINICLI_H

#include <qstring.h>
#include <qstringlist.h>
#include <qgroupbox.h>

#include <kdialog.h>
#include <kurifilter.h>

class KLineEdit;
class KComboBox;
class KCompletion;
class KHistoryCombo;
class QSlider;
class QLabel;
class QCheckBox;
class MinicliAdvanced;
class KPasswordEdit;

class Minicli : public KDialog
{
    Q_OBJECT

public:
    Minicli( QWidget *parent=0, const char *name=0 );
    virtual ~Minicli();
    void reset();
    void saveConfig();

protected slots:
    virtual void accept();
    virtual void reject();

protected:
    virtual void keyPressEvent( QKeyEvent* );
    void loadConfig();
    void loadGUI();

private slots:
    void slotCmdChanged( const QString& );
    void slotParseTimer();
    void slotAdvanced();
    int run_command();

private:
    QString m_IconName;

    QLabel *m_runIcon;
    QCheckBox* m_terminalBox;
    QPushButton* m_btnOptions;
    QPushButton* m_btnCancel;
    QTimer* m_parseTimer;

    bool mbAdvanced;
    MinicliAdvanced *mpAdvanced;
    KHistoryCombo* m_runCombo;
    KURIFilterData* m_filterData;
    QWidget* m_FocusWidget;
};

class MinicliAdvanced : public QGroupBox
{
    Q_OBJECT

public:
    MinicliAdvanced( QWidget *parent=0, const char *name=0 );
    ~MinicliAdvanced();

    bool terminal() { return mbTerminal; }
    bool changeUid() { return mbChangeUid; }
    QString username() { return mUsername; }
    bool changeScheduler() { return mbChangeScheduler; }
    int priority() { return mPriority; }
    int scheduler() { return mScheduler; }
    const char *password();

    bool needsKDEsu();
    void reset();

private slots:
    void slotTerminal(bool);
    void slotChangeUid(bool);
    void slotChangeScheduler(bool);
    void slotScheduler(int);
    void slotPriority(int);
    void slotUsername(const QString &);

private:
    void updateAuthLabel();

    bool mbTerminal, mbChangeUid, mbChangeScheduler;
    int mScheduler, mPriority;

    QSlider *mpSlider;
    QCheckBox *mpCBTerm, *mpCBUser, *mpCBPrio;
    QLabel *mpAuthLabel;
    QString mUsername;
    KLineEdit *mpEdit;
    KComboBox *mpCombo;
    KPasswordEdit *mpPassword;
};

#endif
