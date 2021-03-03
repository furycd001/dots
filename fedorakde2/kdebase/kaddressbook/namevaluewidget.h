/* This file is part of KDE PIM
    Copyright (C) 1999 Don Sanders <sanders@kde.org>

    License: BSD
*/

#ifndef NAMEVALUE_H 
#define NAMEVALUE_H 

#include <qframe.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qsize.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qcombobox.h>

#include "contactentry.h"

/**
 * A table with two columns and a variable number of rows. The columns are 
 * field name and field value. The field value column is editable.
 *
 * A ContactEntry object is updated as values are changed.
 */
class NameValueSheet : public QFrame
{
    Q_OBJECT

public:
/**
 * Constructs a name value sheet.
 *
 * Arguments:
 *
 * @param rows The number of rows.
 * @param name A list of entry field names.
 * @param entryField A list of entry field keys.
 * @param ce A ContactEntry object that will be updated as values are changed.
 */
    NameValueSheet( QWidget *parent, int rows, QStringList name, QStringList entryField, ContactEntry *ce );

/**
 * Destroys the name NameValueSheet object
 */
    virtual~NameValueSheet();

/**
 * Returns the size of a row in the NameValueSheet
 */
    QSize cellSize();
private:
    enum fudgeFactor { verticalTrim = 4 };
    QLabel *lCell;
    int rows;
    int minNameWidth;
    int minNameHeight;
    QLabel *temp;
};

/**
 * A possibly scrollable frame for placing a NameValueSheet in.
 */ 
class NameValueFrame : public QScrollView
{
    Q_OBJECT

public:
/**
 * Creates a NameValueFrame object that contains a NameValueSheet object.
 */
    NameValueFrame( QWidget *parent, NameValueSheet* vs );

/**
 * Updates the NameValueSheet contained.
 */
    virtual void setSheet( NameValueSheet* vs );
protected:
    virtual void resizeEvent(QResizeEvent*);
    NameValueSheet* vs;
    QLabel *lName;
    QLabel *lValue;
};

/**
 * A ContactLineEdit object is substitutable for a QLineEdit object.
 * It both automatically updates an associated ContactEntry object and 
 * is itself  updated if changes are made to the ContactEntry object.
 */
class ContactLineEdit : public QLineEdit
{
    Q_OBJECT

public:
/**
 * Create a ContactLineEdit object.
 *
 * Arguments:
 *
 * @param name Both the name of the widget and the name of the key used in the ContactEntry object.
 * @param ce The ContactEntry object associated with this LineEdit.
 */
    ContactLineEdit( QWidget *parent, const char *name, ContactEntry *ce );

/**
 * When the widget loses focus the associated ContactEntry object is updated.
 */
    virtual void focusOutEvent ( QFocusEvent * );

/**
 * Changes the ContactEntry key this widget is associated with, and updates
 * the text of this ContactLineEdit.
 */
    virtual void setName ( const char * name );

private:
    ContactEntry *ce;

private slots:
    void sync();
};

/*
 * Same idea as ContactLineEdit but for a MultiLineEdit instead of a
 * a LineEdit
 */
class ContactMultiLineEdit : public QMultiLineEdit
{
    Q_OBJECT

public:
    ContactMultiLineEdit( QWidget *parent, const char *name, ContactEntry *ce );
    virtual void focusOutEvent ( QFocusEvent * );
    virtual void setName ( const char * name );

private:
    ContactEntry *ce;

private slots:
    void sync();
};

/*
 * Same idea as ContactLineEdit but for a  read/write ComboBox
 * instead of a LineEdit
 */
class FileAsComboBox : public QComboBox
{
    Q_OBJECT

public:
    FileAsComboBox( QWidget *parent, const char *name, ContactEntry *ce );
    virtual void setName ( const char * name );

private:
    ContactEntry *ce;

public slots:
    virtual void updateContact();

private slots:
    void sync();
};

/*
  A ContactComboBox object is a non-editable QComboBox like widget. Each 
  ContactComboBox has a buddy widget, normally a ContactLineEdit associated
  with it.

  Each item in ContactComboBox object has a ContactEntry key associated
  with it. When a new item is selected the buddy widget is renamed to
  the value of the key associated with the selected item. This will result
  in the buddy widget being associated with the new key.

  A quick and dirty derivation. A ContactComboBox object isn't
  substitutable for a QComboBox object.
*/
class ContactComboBox : public QComboBox
{
    Q_OBJECT

public:
    ContactComboBox( QWidget *parent );
    virtual void setBuddy( QWidget *buddy );
    virtual void insertItem ( const QString & text, const QString & vText );
    QString currentEntryField();

public slots:
    void updateBuddy( int index );

private:
    QWidget *buddy;
    QStringList vlEntryField;
};
#endif
