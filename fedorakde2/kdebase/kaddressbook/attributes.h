/* This file is part of KDE PIM
   Copyright (C) 1999 Don Sanders <sanders@kde.org>

   License: BSD
*/

#ifndef ATTRIBUTES_H 
#define ATTRIBUTES_H 

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qmap.h>

typedef QValueList< QStringList > QStringListList;
typedef QMap< QString, QString > QStringMap;

class Attributes {

public:
  static Attributes *instance();
  QString fieldListName( int index );
  QString fieldToName( const QString &field );
  QString nameToField( const QString &name );
  bool nameFieldList( int index, QStringList *pnames, QStringList *pfields );

protected:
  QStringListList nameList_;
  QStringMap nameToField_;
  QStringMap fieldToName_;

  QStringList QStringPtrToQStringList( const QString *qs );
  Attributes();
  static Attributes *attributes_;
};

#endif
