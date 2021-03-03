#ifndef KADDRESSBOOK_H 
#define KADDRESSBOOK_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <kmainwindow.h>

#include "kaddressbookiface.h"

class ContactEntry;
class ContactEntryList;
class KAddressBookView;

/**
 * This class serves as the main window for KAddressBook.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Don Sanders <dsanders@kde.org>
 * @version 0.1
 */
class KAddressBook : public KMainWindow, virtual public KAddressBookIface
{
    Q_OBJECT
  public:
    KAddressBook();
    virtual ~KAddressBook();
    
    ContactEntry *ce;

  public slots:
    void addEmail( QString addr );
    void newContact();
    /* CS: kaddressbook crashes, when newContact is called as a slot. The
    indirection by slotNewContact prevents this crash. The problem seems to
    be caused by the fact that newContact is defined in AbBrowserIface.
    Don't know, what is the real problem and don't have the time to
    investigate further right now. */
    void slotNewContact() { newContact(); }
    QStringList getKeys() const;
    QDict<ContactEntry> getEntryDict() const;
    void changeEntry( QString key, ContactEntry changeEntry);
    void removeEntry( QString key );
    void addEntry( ContactEntry newEntry );
                  
    void saveCe();
    void save();
    void slotSave() { save(); }
    void readConfig();
    void saveConfig();
    void undo();
    void redo();
    void updateEditMenu();
    /** Import comma-seperated list of all addressbook entries. */
    void importCSV();
    /** Export comma-seperated list of all addressbook entries. */
    void exportCSV();
    void exit();

  protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfig *);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(KConfig *);

    int undoId;
    int redoId;

  private:
    void initActions();
  
    KAddressBookView *mView;
    ContactEntryList *mDocument;
};

#endif // PAB_H 
