#ifndef ABBROWSER_CONTACTENTRY_H
#define ABBROWSER_CONTACTENTRY_H
/* This file is part of KDE PIM
   Copyright (C) 1999 Don Sanders <sanders@kde.org>
   Ammendments: Gregory Stern <gstern@slac.com> 1/2/2001
   
   License: BSD
*/

#include <qobject.h>
#include <qdict.h>
#include <qstring.h>

class QDataStream;
class QTextStream;


/**
 * The ContactEntry class is used to store the current state of an address
 * book entry being edited. It is used in conjunction with ContactDialog,
 * and in doing so it plays the Document part of the View/Document pattern.
 * 
 * An instance of this call is also an Observer emitting the changed() signal 
 * to inform other objects that it has been edited. This allows different
 * widgets showing the same entry field in the corresponding ContactDialog 
 * object to be kept synchronized.
 *
 * This class is just a placeholder until it can be replaced with something
 * more suitable. It is incomplete, for instance no complementary 
 * ContactEntryIterator and ContactEntryList classes has been defined, 
 * spurious load and save methods exist that will have to be removed.
 *
 * Maybe this class is general enough to store PIM entries of all types
 * and not just address book entries. Perhaps this class could be augmented
 * by adding methods for encoding and decoding an object of this type to 
 * vCard or some other format.
 *
 * A couple of conventions are used
 * Fields beginning with the prefix "." won't be saved.
 * Fields beginning with the prefix "X-CUSTOM-" are custom fields.
 *
 * The underlying implementation is based on a QDict<QString> object.
 *
 * GDS: Note that is class has been seriously revamped to allow for easier
 * syncing for the Palm Pilot.  Many methods have been added for setting
 * and getting fields.  An inner class is used to represent address fields.
 * Serialization of the ContactEntry over QDataStream was added for
 * passing over DCOP.
 **/
class ContactEntry : public QObject
{
    Q_OBJECT

public:
/**
 * Creates a new ContactEntry object.
 */
  ContactEntry();
  ContactEntry( const ContactEntry &r );
  ContactEntry& operator=( const ContactEntry &r );

/**
 * Creates a ContactEntry object from data stored in a textstream.
 * 
 * Arguments:
 *
 * @param the name of the textstream.
 */
  ContactEntry( QTextStream &t );
  
/**
 * Returns a list of all custom fields, that is those beginning with
 * the prefix "X-CUSTOM-"
 */
  QStringList custom() const;

/**
 * Saves the entry to a text stream with the given filename.
 */
  void save( QTextStream &t );

/**
 * Loads the entry from a text stream with the given filename
 */
  void load( QTextStream &t );

  /** If loading contact entry from Kab Address::Entry, set this to true.
   *  This allows the ContactEntry class to know whether this is being
   *  modified by the user or is just loading up on creation.
   */
  void setLoading(bool v) { fLoading = v; }

  bool isLoading() const { return fLoading; }
  /**
   * Needed for DCOP serialization.
 */
  void save( QDataStream &d ) const;

/**
 * Needed for DCOP serialization.
 */
  void load( QDataStream &t );  
  
/**
 * Inserts a new key/value pair 
 */
  void insert( const QString &key, const QString *value);

/**
 * Updates the value associated with a key. The old value
 * will be deleted.
 */
  void replace( const QString &key, const QString *value);

  /** Same as replace() above except the arguments that it excepts
   */
  void replaceValue( const QString &key, const QString &value);
  
/**
 * Remove a key and deletes its associated value.
 */ 
  bool remove ( const QString &key );

/**
 * Returns a const pointer to the value associated with a key.
 */
  const QString *find ( const QString & key ) const ;

  /**
   * @return QString::null if key was not found; return's the key's value
   * otherwise; Same as find() but differs only in return type
   */
  const QString &findRef( const QString &key ) const ;
/**
 * Returns a const pointer to the value associated with a key.
 */
  const QString *operator[] ( const QString & key ) const;

/**
 * Cause the changed signal to be emitted.
 */
  void touch();

/**
 * Remove all key/value pairs stored.
 */
  void clear();

  /**
   * For debugging purposes only, print out the values stored in the
   * this class using qDebug
   */
  void debug();

  /** Set modified flag; to be used with KPilot during syncing; get's
   *  called by replaceValue() method; uses X-CUSTOM KPilot field
   */
  void setModified(bool v);
  /** @return true if modified sync last sync */
  bool isModified() const;
  /** if no modified field is found, then assume it is new */
  bool isNew() const;
  /* The following functions are convenience methods that return
   * values stored in this class by using the findRef() function
   * and by using their associated keys
   */
  void setFirstName(const QString &v)
	    { replaceValue("firstname", v); replaceValue("X-FirstName",v);}
  void setLastName(const QString &v)
	    { replaceValue("lastname", v); replaceValue("X-LastName",v); }
  void setMiddleName(const QString &v)
	    { replaceValue("middlename", v); replaceValue("X-MiddleName",v); }
  void setNamePrefix(const QString &v) { replaceValue("X-Title", v); }
  /** Use Prefix, First, Last, Middle to set the name */
  void setName();
  void setJobTitle(const QString &v)
	    { replaceValue("title", v); replaceValue("ROLE", v); }
  void setCompany(const QString &v) { replaceValue("ORG", v); }
  void setEmail(const QString &v)
	    { replaceValue("emails", v + "\\e"); replaceValue("EMAIL", v); }
  void setNickname(const QString &v) { replaceValue("NICKNAME", v); }
  void setNote(const QString &v) { replaceValue("X-Notes", v); }
  void setBusinessPhone(const QString &v)
	    { replaceValue("X-BusinessPhone", v); }
  void setHomePhone(const QString &v)
	    { replaceValue("X-HomePhone", v); }
  void setMobilePhone(const QString &v)
	    { replaceValue("X-MobilePhone", v); }
  void setHomeFax(const QString &v)
	    { replaceValue("X-HomeFax", v); }
  void setBusinessFax(const QString &v)
	    { replaceValue("X-BusinessFax", v); }
  void setPager(const QString &v)
	    { replaceValue("X-Pager", v); }
  void setCustomField(const QString &customName, const QString &v)
	    { replaceValue("X-CUSTOM-" + customName, v); }

  const QString &getCustomField(const QString &customName)
	    { return findRef("X-CUSTOM-" + customName); }
  const QString &getFirstName() const { return findRef("X-FirstName"); }
  const QString &getLastName() const { return findRef("X-LastName"); }
  const QString &getMiddleName() const { return findRef("X-MiddleName"); }
  const QString &getNamePrefix() const { return findRef("X-Title"); }
  const QString &getFullName() const { return findRef("fn"); }
  
  const QString &getJobTitle() const { return findRef("ROLE"); }
  const QString &getCompany() const { return findRef("ORG"); }
  const QString &getEmail() const { return findRef("EMAIL"); }
  const QString &getNickname() const { return findRef("NICKNAME"); }
  const QString &getNote() const { return findRef("X-Notes"); }
  const QString &getBusinessPhone() const { return findRef("X-BusinessPhone"); }
  const QString &getHomePhone() const { return findRef("X-HomePhone"); }
  const QString &getMobilePhone() const { return findRef("X-MobilePhone"); }
  const QString &getHomeFax() const { return findRef("X-HomeFax"); }
  const QString &getBusinessFax() const { return findRef("X-BusinessFax"); }
  const QString &getPager() const { return findRef("X-Pager"); }
 
  class Address
      {
      friend class ContactEntry;
      public :
	~Address() { }

	/** @return true if all address values are null, or not set */
	bool isEmpty() const;
	const QString &getStreet() const
		  { return fParent->findRef(fPre + "Street"); }
	const QString &getCity() const
		  { return fParent->findRef(fPre + "City"); }
	const QString &getState() const
		  { return fParent->findRef(fPre + "State"); }
	const QString &getZip() const
		  { return fParent->findRef(fPre + "PostalCode"); }
	const QString &getCountry() const
		  { return fParent->findRef(fPre + "Country"); }

	void setStreet(const QString &v);
	void setCity(const QString &v);
	void setState(const QString &v);
	void setZip(const QString &v);
	void setCountry(const QString &v);
	
      protected :
	Address(ContactEntry *parent, const QString &type) :
	      fParent(parent),
	      fPre("X-" + type + "Address")
		  { }
	    
      private :
	ContactEntry *fParent;
	QString fPre;
      };

  /** @return a newed Address.  The user must delete the Address when finished.
   */
  Address *getHomeAddress();
  /** @return a newed Address.  The user must delete the Address when finished.
   */
  Address *getBusinessAddress();

  /** GDS: Every entry will have a Category field for now.  It will be stored
   *  in the "X-Folder" field.  I am adding this for palm pilot syncing.
   *  @return QString::null if the ContactEntry is unfiled; returns
   *  the entry 
   *
   *  @note I would rather implement Folder as a parental tree so the
   *  folder  names could be changed easier and multiple levels can be easily
   *  added. Perhaps each entry could have a lookup
   *  index (integer) to the corresponding category; that map could handle
   *  the names.  Anyway, to get this working for now (and for kpilot) which
   *  doesn't have multiple levels, I am just adding a field.
   *
   *  It was discussed that folder (category) should be a custom
   *  field.  I think that a folder is not custom to palm apps but is
   *  good for general orgination of contacts so I am storing it in
   *  it's own field.
   */
  const QString &getFolder() const;
  void setFolder(const QString &category);

  /** Calls setFolder("KabTrash") */
  void moveToTrash() { setFolder("KabTrash"); }
  /** @return true if getFolder() == "KabTrash" */
  bool inTrash() const { return getFolder() == "KabTrash"; }

signals:
/**
 * Emitted when key/value pair is updated or inserted
 */
  void changed();

private:
  /** For KPilot: if this contact !isNew(), then setModified() to true
   */
  void _setModified();
  void _replace(const QString &key, const QString *item, bool internal);
  
  QDict<QString> dict; // This unfortunately doesn't make a good base class
  // It's not derived from QOBject and the majority of methods are not virtual

  bool fLoading;
};

QDataStream &operator<<(QDataStream &out, const QDict<ContactEntry> &entries);
QDataStream &operator<<(QDataStream &out, const ContactEntry &entry);

QDataStream &operator>>(QDataStream &in, QDict<ContactEntry> &entries);
QDataStream &operator>>(QDataStream &in, ContactEntry &entry);


#endif
