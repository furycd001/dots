/* This file is part of KDE PIM
   Copyright (C) 1999 Don Sanders <sanders@kde.org>

   License: BSD
*/

#ifndef CONTACT_H 
#define CONTACT_H 

#include <qstring.h>
#include <qdialog.h>
#include <qstringlist.h>

class NameValueSheet;
class NameValueFrame;
class ContactComboBox;
class ContactEntry;
class QMultiLineEdit;
class QLineEdit;
class QComboBox;
class QWidget;
class QTabWidget;
class QPushButton;
class FileAsComboBox;

/**
 * A tabbed dialog for entering/updating an address book entry.
 */
class ContactDialog : public QDialog
{
    Q_OBJECT

public:
/**
 * Creates a ContactDialog. If a pointer to a ContactEntry object is given 
 * then that ContactEntry object will be updated otherwise a new 
 * ContactEntry object will be created
 */
    ContactDialog( const QString & title, QWidget *parent, const char *name, ContactEntry* ce = 0, bool modal = false );

/**
 * Returns the ContactEntry associated with this dialog.
 */
    ContactEntry* entry();

protected:
    void setupTab1();
    void setupTab2();
    void setupTab3();
    void updateFileAs();
    QTabWidget* tabs;
    NameValueSheet *vs;
    NameValueFrame *vp;
    QStringList names;
    QStringList values;
    QStringList entryNames;
    ContactEntry* ce;
    QMultiLineEdit *mleAddress;
    ContactComboBox *cbAddress;
    FileAsComboBox *cbFileAs;
    QLineEdit *leFullName;
    QString curName;
    QString curCompany;
    QComboBox *cbSelectFrom;
    QPushButton *pbOk;

public slots:
    void pickBirthDate();
    void pickAnniversaryDate();
    void newAddressDialog();
    void newFieldDialog();
    void newNameDialog();
    void parseName();
    void monitorCompany();
    void setSheet( int );
    void ok();

signals:
    void accepted();
};


/**
 * A dialog for entering/updating address information
 */
class AddressDialog : public QDialog
{
    Q_OBJECT

public:
/**
 * Constructs an AddressDialog object.
 *
 * Arguments:
 *
 * @param entryField Specifies the category of address (Business, Home, Other) to update.
 * @param ce The ContactEntry to update.
 */
    AddressDialog( QWidget *parent, QString entryField, ContactEntry *ce, bool modal = false );

private:
    QString entryField;
    ContactEntry *ce;
    QMultiLineEdit *mleStreet;
    QLineEdit *leCity;
    QLineEdit *leState;
    QLineEdit *lePostal;
    QComboBox *cbCountry;

private slots:
    void AddressOk();
};

/**
 * A dialog for entering/updating a name.
 */
class NameDialog : public QDialog
{
    Q_OBJECT

public:
/**
 * Constructs an NameDialog object.
 *
 * Arguments:
 *
 * @param ce The ContactEntry to update.
 */
    NameDialog( QWidget *parent, ContactEntry *ce, bool modal = false );

private:
    ContactEntry *ce;
    QComboBox *cbTitle;
    QLineEdit *leFirst;
    QLineEdit *leMiddle;
    QLineEdit *leLast;
    QComboBox *cbSuffix;

public slots:
    virtual void polish();

private slots:
    void NameOk();
};

/*
 * A dialog for specifying a new name/value pair for insertion into
 * a ContactEntry object.
 */
class NewFieldDialog : public QDialog
{
    Q_OBJECT

public:
/*
 * Constructs a new NewFieldDialog dialog.
 */
    NewFieldDialog( QWidget *parent, bool modal = false );

/*
 * The name of the new name/value pair.
 */
    QString field() const;

/*
 * The value of the new name/value pair.
 */
    QString value() const;

private:
    QLineEdit *leField;
    QLineEdit *leValue;
};

#endif // CONTACT_H 
