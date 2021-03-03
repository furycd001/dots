#ifndef KADDRESSBOOKVIEW_H
#define KADDRESSBOOKVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qwidget.h>
#include <qlistview.h>
#include <qstring.h>
#include <qdialog.h>
#include <qpixmap.h>
#include <qtabdialog.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qtooltip.h>

#include "undo.h"

class QComboBox;
class QTabWidget;
class QBoxLayout;
class QHBoxLayout;
class QLineEdit;
class QListViewItem;
class QIconSet;
class QGridLayout;
class QListBox;

class ContactEntry;
class ContactEntryList;
class ContactListViewItem;
class KAddressBookView;
class ContactListView;


/**
 * This class is the main view for KAddressBook.  Most non-menu, non-toolbar,
 * and non-status bar GUI code should go here.
 *
 * @short Main view
 * @author Don Sanders <dsanders@kde.org>
 * @version 0.1
 */
class KAddressBookView : public QWidget
{
friend class ContactListView;

  Q_OBJECT

public:
  KAddressBookView( ContactEntryList *cel, QWidget *parent, const char *name = 0L );
  virtual ~KAddressBookView();
  virtual ContactEntryList* contactEntryList();
  virtual ContactListView* listView();
  virtual QStringList *fields();
  virtual QString selectedEmails();
  virtual void updateContact( QString addr, QString name );
  virtual void addEmail(const QString& aStr);

public slots:
  virtual void showSelectNameDialog();
  virtual void defaultSettings(); 
  virtual void cut();
  virtual void copy();
  virtual void paste();
  virtual void clear();
  virtual void properties();
  virtual void sendMail();
  virtual void selectAll();
  virtual void saveConfig();
  virtual void readConfig();
  virtual void reconstructListView();
  void change( QString entryKey, ContactEntry *ce );
  ContactListViewItem* addEntry( QString EntryKey );
  void addNewEntry( ContactEntry *ce );

protected slots:
  void itemSelected( QListViewItem* );
  void selectionChanged();
  void repopulate();
  void viewOptions();
 
protected:
  virtual void selectNames( QStringList fields );
  void setupListView();

  ContactEntryList *cel;
  QStringList field;
  QValueList<int> fieldWidth;
  QIconSet *myIcon;
  QBoxLayout *mainLayout;
  ContactListView *mListView;
  QLineEdit *iSearch;
  QComboBox *cbField;
};

#endif
