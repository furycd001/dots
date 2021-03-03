#ifndef KADDRESSBOOKIFACE_H
#define KADDRESSBOOKIFACE_H
 
#include <dcopobject.h>
#include <contactentry.h>

class KAddressBookIface : virtual public DCOPObject
{
    K_DCOP
  k_dcop:
    virtual void addEmail( QString addr ) = 0;

  /** Show's dialog for creation of a new contact.  Returns once a contact
   *  is created (or canceled).
   */
    virtual void newContact() = 0;
    virtual QStringList getKeys() const = 0;
  /** @return QDict of kab id strings (kab database numbers) to ContactEntry
   *  returns the entire database for the user, so this could be a timely
   *  operation and a large QDict
   */
    virtual QDict<ContactEntry> getEntryDict() const = 0;
  /** Add the newEntry and return it's key */
    virtual void addEntry( ContactEntry newEntry) = 0;
    virtual void changeEntry( QString key, ContactEntry changeEntry) = 0;
    virtual void removeEntry( QString key ) = 0;
  /** Save changes to the address book files */
    virtual void save() = 0;
    virtual void exit() = 0;
};

#endif
