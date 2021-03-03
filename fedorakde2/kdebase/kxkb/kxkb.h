#ifndef __K_XKB_H__
#define __K_XKB_H__


#include <qwidget.h>
#include <qstringlist.h>


class XKBExtension;
class KeyRules;

#include <kuniqueapp.h>
#include <ksystemtray.h>

class KGlobalAccel;

class TrayWindow : public KSystemTray
{
  Q_OBJECT

public:

  TrayWindow(QWidget *parent=0, const char *name=0);

  void setLayout(QString layout);
  void setLayouts(QStringList layouts, QString rule="xfree86");

  KPopupMenu* contextMenu() { return KSystemTray::contextMenu(); };


signals:

  void toggled();


protected:

  void mouseReleaseEvent(QMouseEvent *);

};


class KXKBApp : public KUniqueApplication
{
  Q_OBJECT

public:

  KXKBApp(bool allowStyles=true, bool GUIenabled=true);
  ~KXKBApp();

  int newInstance() { readSettings(); return 0; };


protected slots:

  void menuActivated(int id);
  void toggled();


protected:

  void readSettings();


private:

  QString _rule, _model, _layout, _encoding;
    unsigned int _group;
    QStringList _list;
  QStringList _encList;
  XKBExtension *extension;
    KeyRules *rules;
    
  TrayWindow *tray;
  KGlobalAccel *keys;

};


#endif
