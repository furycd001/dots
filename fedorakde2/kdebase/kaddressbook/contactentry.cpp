/* This file is part of KDE PIM
    Copyright (C) 1999 Don Sanders <sanders@kde.org>

    License: BSD
*/

#include "contactentry.h"
#include <qdict.h>
#include <qfile.h>
#include <qregexp.h>

#include <klocale.h>
//#include <kabapi.h>
#include <kdebug.h>
#include <qtextstream.h>

////////////////////////
// ContactEntry methods

ContactEntry::ContactEntry() : QObject(), dict(), fLoading(false)
{
  dict.setAutoDelete( true );
}

ContactEntry::ContactEntry( const ContactEntry &r )
      : QObject(), fLoading(false)
    {
  QDictIterator<QString> it( r.dict );

  while ( it.current() ) {
    dict.replace( it.currentKey(), new QString( *it.current() ));
    ++it;
  }
}

ContactEntry& ContactEntry::operator=( const ContactEntry &r )
{
  if (this != &r) {
    dict.clear();
    QDictIterator<QString> it( r.dict );

    while ( it.current() ) {
      dict.replace( it.currentKey(), new QString( *it.current() ));
      ++it;
    }
  }
  return *this;
}

/*
ContactEntry::ContactEntry( const QString &filename )
{
  dict.setAutoDelete( true );
  load( filename );
}
*/

ContactEntry::ContactEntry( QTextStream &t )
{
  dict.setAutoDelete( true );
  load( t );
}

QStringList ContactEntry::custom() const
{
  QStringList result;
  QDictIterator<QString> it( dict );

  while ( it.current() ) {
    if (it.currentKey().find( "X-CUSTOM-", 0 ) == 0)
      result << it.currentKey();
    ++it;
  }
  return result;
}

/*
void ContactEntry::save( const QString &filename )
{
  QFile f( filename );
  if ( !f.open( IO_WriteOnly ) )
    return;

  QTextStream t( &f );
  QDictIterator<QString> it( dict );

  while ( it.current() ) {
    if (it.currentKey().find( ".", 0 ) != 0) {
      t << it.currentKey() << "\n";
      t << *it.current() << "\n[EOR]\n";
    }
    ++it;
  }
  t << "[EOS]\n";
  f.close();
}
*/

void ContactEntry::save( QTextStream &t )
{
  QDictIterator<QString> it( dict );
  QRegExp reg("\n");

  while ( it.current() ) {
    if ((it.currentKey().find( ".", 0 ) != 0) &&
	(!(*it.current()).isEmpty())) {
      t << " " << it.currentKey() << "\n";
      QString tmp = *it.current();
      tmp.replace( reg, "\n " );
      t << " " << tmp << "\n[EOR]\n";
    }
    ++it;
  }
  t << "[EOS]\n";
}

void ContactEntry::save( QDataStream &out ) const
    {
    out << dict.count();
    for (QDictIterator<QString> iter( dict );
	 iter.current();++iter)
	{
	out << iter.currentKey() << *(iter.current());
	}
    }

/*
void ContactEntry::load( const QString &filename )
{
  dict.clear();

  QFile f( filename );
  if ( !f.open( IO_ReadOnly ) )
    return;

  QTextStream t( &f );

  while ( !t.eof() ) {
    QString name = t.readLine();
    if (name == "[EOS]")
      break;
    QString tmp = t.readLine();
    QString value = "";
    while (tmp != QString( "\n[EOR]" )) {
      value += tmp;
      tmp = "\n" + t.readLine();
    }
    if ((name != "") && (value != ""))
      dict.replace( name, new QString( value ));
  }
  f.close();
  emit changed();
}
*/

void ContactEntry::load( QTextStream &t )
{
  while (!t.eof()) {
    QString name = t.readLine();
    if (name == "[EOS]")
      break;
    name = name.mid(1);
    QString tmp = t.readLine();
    QString value = "";
    while (tmp != QString( "[EOR]" )) {
      if (!value.isEmpty())
	value += "\n";
      value += tmp.mid(1);
      tmp = t.readLine();
    }
    if ((name != "") && (value != ""))
      dict.replace( name, new QString( value ));
  }
  emit changed();
}

void ContactEntry::load( QDataStream &in )
    {
    uint numItems = 0;
    in >> numItems;
    QString key;
    QString item;
    for (uint i=0;i < numItems;i++)
	{
	in >> key;
	//kdDebug()<<" ContactEntry::load key = "<< key.latin1()<<endl;
	in >> item;
	//kdDebug()<<" ContactEntry::load item = "<< item.latin1()<<endl;
	dict.replace(key, new QString(item));
	}
    }

bool ContactEntry::isNew() const
    {
    const QString *kpilotSyncDone = find("X-CUSTOM-KPILOT_ID");
    return (kpilotSyncDone == NULL);
    }

void ContactEntry::_setModified()
    {
    if (!isNew() && !isLoading())
	setModified(true);
    }

void ContactEntry::insert( const QString &key, const QString *item)

{
 bool internal = false;
 if (key[0] == '.')
     internal = true;

  if (item && (*item == ""))
    return;
  dict.insert( key, item );
  if (!internal && !fLoading)
      {
      //qDebug("ContactEntry::insert %s=%s int=%d", key.latin1(), item->latin1(),
      //     (int) internal);
      _setModified();
      }
  emit changed();
}

void ContactEntry::replace( const QString &key, const QString *item)
    {
    bool internal = false;
    if (key[0] == '.')
	internal = true;
    _replace(key, item, internal);
    }

void ContactEntry::_replace(const QString &key, const QString *item,
			    bool internal)
    {
    QString *current = dict.find( key );
    if (item) {
     if (current) {
     if (*item != *current) {

      if (*item == "")
	  dict.remove( key ); // temporary?
      else
	  dict.replace( key, item );
      if (!internal && !fLoading)
	  {
	  //qDebug("ContactEntry::replace %s=%s int=%d", key.latin1(),
	  // item->latin1(), (int) internal);
	  _setModified();
	  }
      emit changed();
      }
    }
    else { // (item && !current)

    dict.replace( key, item );
    if (!internal && !fLoading)
	{
	_setModified();
	//qDebug("ContactEntry::replace %s=%s int=%d", key.latin1(),
	//      item->latin1(), (int) internal);
	}
    emit changed();
    }
  }
  //else
  //qDebug("ContactEntry::replace( const QString, const QString* ) passed null item");
  /*
  if (item && (*item == ""))
    dict.remove( key );
  else
    dict.replace( key, item );
  emit changed();
  */
}

void ContactEntry::replaceValue(const QString &key, const QString &value)
    {
    QString *newValue = new QString(value);
    replace(key, newValue);
    }

bool ContactEntry::remove( const QString &key )
{
  if (dict.remove( key ))
      {
      _setModified();
      emit changed();
      return true;
      }
  return false;
}

void ContactEntry::touch()
{
  emit changed();
}

void ContactEntry::setName()
    {
    QString title = getNamePrefix().simplifyWhiteSpace();
    QString first = getFirstName().simplifyWhiteSpace();
    QString middle = getMiddleName().simplifyWhiteSpace();
    QString last = getLastName().simplifyWhiteSpace();

    QString name = title;
    if (!title.isEmpty())
	name += " ";
    name += first;
    if (!first.isEmpty())
	name += " ";
    name += middle;
    if (!middle.isEmpty())
	name += " ";
    name += last;
    replaceValue("N", name);
    replaceValue("X-FileAs", name);
    replaceValue("fn", name);
    }

const QString &ContactEntry::getFolder() const
    {
    return findRef("X-Folder");
    }
void ContactEntry::setFolder(const QString &category)
    {
    replaceValue("X-Folder", category);
    }

const QString *ContactEntry::find ( const QString & key ) const
{
  return dict.find( key );
}

const QString &ContactEntry::findRef( const QString &key) const
    {
    const QString *valueStr = find(key);
    if (valueStr)
	return *valueStr;
    return QString::null;
    }

const QString *ContactEntry::operator[] ( const QString & key ) const
{
  return dict[key];
}

void ContactEntry::clear ()
{
  dict.clear();
  emit changed();
}

void ContactEntry::debug()
    {
    for (QDictIterator<QString> iter(dict);iter.current();++iter)
	{
	qDebug("\t'%s' : '%s'", iter.currentKey().latin1(),
	       iter.current()->latin1());
	}
    }

void ContactEntry::setModified(bool v)
    {
    int val = 0;
    if (v)
	val = 1;
    QString * valStr = new QString(QString::number(val));
    _replace("X-CUSTOM-KPILOT-MODIFIED", valStr, true);
    // qDebug("\tContactEntry::setModified %d", val);
    }

bool ContactEntry::isModified() const
    {
    const QString *kpilotModified = find("X-CUSTOM-KPILOT-MODIFIED");
    // assume that if there is no modified field, then must have not
    // synced yet (new to pilot) so it is as good as modified
    if (!kpilotModified)
	return true;
    int val = kpilotModified->toInt();
    return (val != 0);
    }

ContactEntry::Address *ContactEntry::getHomeAddress()
    {
    Address *a = new Address(this, "Home");
    return a;
    }

ContactEntry::Address *ContactEntry::getBusinessAddress()
    {
    Address *a = new Address(this, "Business");
    return a;
    }

QDataStream &operator<<(QDataStream &out, const ContactEntry &entry)
    {
    entry.save(out);
    return out;
    }

QDataStream &operator>>(QDataStream &in, ContactEntry &entry)
    {
    entry.load(in);
    return in;
    }


QDataStream &operator<<(QDataStream &out, const QDict<ContactEntry> &entries)
    {
    //kdDebug()<<"  Writing out QDict<ContactEntry>\n";
    out << entries.count();
    for (QDictIterator<ContactEntry> iter(entries);iter.current();++iter)
	{
	out << iter.currentKey();
	iter.current()->save(out);
	}
    return out;
    }
QDataStream &operator>>(QDataStream &in, QDict<ContactEntry> &entries)
    {
    //kdDebug()<<"  Reading in QDict<ContactEntry>\n";
    entries.setAutoDelete(true);
    uint numEntries = 0;
    in >> numEntries;
    //qDebug("  reading %d entries", numEntries);
    QString key;
    for (uint i=0;i < numEntries;++i)
	{
	in >> key;
	//kdDebug()<<" read key = "<< key.latin1()<<endl;
	ContactEntry *entry = new ContactEntry();
	entry->load(in);
	entries.insert(key, entry);
	}
    return in;
    }

bool ContactEntry::Address::isEmpty() const
    {
    return (getStreet() == QString::null &&
	    getCity() == QString::null &&
	    getState() == QString::null &&
	    getZip() == QString::null &&
	    getCountry() == QString::null);
    }

void ContactEntry::Address::setStreet(const QString &v)
    {
    QString key = fPre + "Street";
    fParent->replaceValue(key, v);
    }
void ContactEntry::Address::setCity(const QString &v)
    {
    fParent->replaceValue(fPre + "City", v);
    }
void ContactEntry::Address::setState(const QString &v)
    {
    fParent->replaceValue(fPre + "State", v);
    }
void ContactEntry::Address::setZip(const QString &v)
    {
    fParent->replaceValue(fPre + "Zip", v);
    }
void ContactEntry::Address::setCountry(const QString &v)
    {
    fParent->replaceValue(fPre + "Country", v);
    }
#include "contactentry.moc"
