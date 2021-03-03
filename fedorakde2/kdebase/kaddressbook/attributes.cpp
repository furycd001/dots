/* This file is part of KDE PIM
    Copyright (C) 1999 Don Sanders <sanders@kde.org>

    License: BSD
*/

#include "attributes.h"
#include <klocale.h>

Attributes* Attributes::attributes_ = 0;

Attributes* Attributes::instance()
{
  if (!attributes_) {
    attributes_ = new Attributes();
  }
  return attributes_;
}

QString Attributes::fieldToName( const QString &field )
{
  // just temporary
  if (field.find( "X-CUSTOM-", 0 ) == 0)
    return( i18n(field.mid( 9 ).utf8()) );

  if (fieldToName_.contains( field ))
    return fieldToName_[field];
  else
    return "";
}

// Doesn't handle custom names
QString Attributes::nameToField( const QString &name )
{
  if (nameToField_.contains( name ))
    return nameToField_[name];
  else
    return "";
}

bool Attributes::nameFieldList( int index, 
				QStringList *pnames, 
				QStringList *pfields )
{
  if ((index >= 0) && (index < (int)nameList_.count())) {
    QStringList newpfields;
    QStringList::Iterator it;

    *pnames = nameList_[index];
    for ( it = pnames->begin(); it != pnames->end(); ++it )
      newpfields += nameToField( *it );
    *pfields = newpfields;

    return true;
  }
  return false;
}

QStringList Attributes::QStringPtrToQStringList( const QString *qs )
{
  QStringList tmpList;
  QString tmp;
  for (int i = 0; tmp = qs[i], tmp != ""; ++i )
    tmpList += fieldToName_[tmp];

  tmpList.sort();
  return tmpList;
}

Attributes::Attributes()
{
  const QString completeAllNames[] = {
    i18n( "Account" ), i18n( "Anniversary" ), i18n( "Assistant's Name" ), 
    i18n( "Assistant's Phone" ), i18n( "Attachment" ), 
    i18n( "Billing Information" ), i18n( "Birthday" ), 
    i18n( "Business Address" ), i18n( "Business Address City" ), 
    i18n( "Business Address Country" ), i18n( "Business Address PO Box" ), 
    i18n( "Business Address Postal Code" ), i18n( "Business Address State" ),
    i18n( "Business Address Street" ), i18n( "Business Fax" ), 
    i18n( "Business Home Page" ), i18n( "Business Phone" ), 
    i18n( "Business Phone 2" ), i18n( "Callback" ), i18n( "Car Phone" ), 
    i18n( "Categories" ), i18n( "Children" ), i18n( "City" ), 
    i18n( "Company" ), i18n( "Company Main Phone" ), 
    i18n( "Computer Network Name" ), i18n( "Country" ), i18n( "Created" ),
    i18n( "Customer ID" ), i18n( "Department" ), i18n( "Email" ), 
    i18n( "Email 2" ), i18n( "Email 3" ), i18n( "Name in Order", "File As" ), 
    i18n( "First Name" ), i18n("Folder"), i18n( "FTP Site" ), 
    i18n( "Full Name" ), 
    i18n( "Gender" ), i18n( "Government ID Number" ), i18n( "Hobbies" ),
    i18n( "Home Address" ), i18n( "Home Address City" ), 
    i18n( "Home Address Country" ), i18n( "Home Address PO Box" ), 
    i18n( "Home Address Postal Code" ), i18n( "Home Address State" ), 
    i18n( "Home Address Street" ), i18n( "Home Fax" ), i18n( "Home Phone" ),
    i18n( "Home Phone 2" ), i18n( "Icon" ), i18n( "In Folder" ), 
    i18n( "Initials" ), i18n( "ISDN" ), i18n( "Job Title" ), i18n( "Journal" ),
    i18n( "Language" ), i18n( "Last Name" ), i18n( "Location" ), 
    i18n( "Mailing Address" ), i18n( "Manager's Name" ), 
    i18n( "Message Class" ), i18n( "Middle Name" ), i18n( "Mileage" ),
    i18n( "Mobile Phone" ), i18n( "Modified" ), i18n( "Nickname" ), 
    i18n( "Office Location" ), i18n( "Organizational ID Number" ),
    i18n( "Other Address" ), i18n( "Other Address City" ),
    i18n( "Other Address Country" ), i18n( "Other Address PO Box" ), 
    i18n( "Other Address Postal Code" ), i18n( "Other Address State" ), 
    i18n( "Other Address Street" ), i18n( "Other Fax" ), i18n( "Other Phone" ),
    i18n( "Pager" ), i18n( "Personal Home Page" ), i18n( "PO Box" ), 
    i18n( "Primary Phone" ), i18n( "Profession" ), i18n( "Radio Phone" ), 
    i18n( "Read" ), i18n( "Referred By" ), i18n( "Sensitivity" ), 
    i18n( "Size" ), i18n( "Spouse" ), i18n( "State" ), 
    i18n( "Street Address" ), i18n( "Subject" ), i18n( "Suffix" ), 
    i18n( "Telex" ), i18n( "Title" ), i18n( "TTY/TDD Phone" ), 
    i18n( "User Field 1" ), i18n( "User Field 2" ), i18n( "User Field 3" ), 
    i18n( "User Field 4" ), i18n( "Web Page" ), i18n( "ZIP/Postal Code" ),
    i18n( "Notes" ), i18n( "Business" ), i18n( "Home" ), i18n( "Other" ),
    ""
  };

  const QString completeAllFields[] = {
    "X-Account", "X-Anniversary", "X-AssistantsName", "X-AssistantsPhone",
    "X-Attachment", "X-BillingInformation", "BDAY", "X-BusinessAddress",
    "X-BusinessAddressCity", "X-BusinessAddressCountry", 
    "X-BusinessAddressPOBox", "X-BusinessAddressPostalCode",
    "X-BusinessAddressState", "X-BusinessAddressStreet", "X-BusinessFax", 
    "X-BusinessHomePage", "X-BusinessPhone", "X-BusinessPhone2", 
    "X-Callback", "X-CarPhone", "X-Categories", "X-Children", "X-City", "ORG",
    "X-CompanyMainPhone", "X-ComputerNetworkName", "X-Country", "X-Created",
    "X-CustomerID", "X-Department", "EMAIL", "X-E-mail2",
    "X-E-mail3", "X-FileAs", "X-FirstName", "X-Folder", "X-FTPSite",
    "N", "X-Gender", "X-GovernmentIDNumber", "X-Hobbies",
    "X-HomeAddress", "X-HomeAddressCity", "X-HomeAddressCountry", 
    "X-HomeAddressPOBox", "X-HomeAddressPostalCode", "X-HomeAddressState", 
    "X-HomeAddressStreet", "X-HomeFax", "X-HomePhone", "X-HomePhone2", 
    "X-Icon", "X-InFolder", "X-Initials", "X-ISDN", "ROLE", "X-Journal",
    "X-Language", "X-LastName", "X-Location", "X-MailingAddress",
    "X-ManagersName", "X-MessageClass", "X-MiddleName", "X-Mileage",
    "X-MobilePhone", "X-Modified", "X-Nickname", "X-OfficeLocation",
    "X-OrganizationalIDNumber", "X-OtherAddress", "X-OtherAddressCity", 
    "X-OtherAddressCountry", "X-OtherAddressPOBox", 
    "X-OtherAddressPostalCode", "X-OtherAddressState", 
    "X-OtherAddressStreet", "X-OtherFax", "X-OtherPhone",
    "X-Pager", "X-PersonalHomePage", "X-POBox", "X-PrimaryPhone",
    "X-Profession", "X-RadioPhone", "X-Read", "X-ReferredBy",
    "X-Sensitivity", "X-Size", "X-Spouse", "X-State",
    "X-StreetAddress", "X-Subject", "X-Suffix", "X-Telex",
    "X-Title", "X-TtyTddPhone", "X-UserField1", "X-UserField2",
    "X-UserField3", "X-UserField4", "WEBPAGE", "X-ZIPPostalCode",
    "X-Notes", "X-BusinessAddress", "X-HomeAddress", "X-OtherAddress",
    "" 
  };

  QString field;
  QString name;
  for (int i = 0; field = completeAllFields[i], field != ""; ++i ) {
    name = completeAllNames[i];
    nameToField_.insert( name, field );
    fieldToName_.insert( field, name );
  }

  const QString allFields[] = {
    "X-Account", "X-Anniversary", "X-AssistantsName", "X-AssistantsPhone",
    "X-Attachment", "X-BillingInformation", "BDAY", "X-BusinessAddress",
    "X-BusinessAddressCity", "X-BusinessAddressCountry", 
    "X-BusinessAddressPOBox", "X-BusinessAddressPostalCode",
    "X-BusinessAddressState", "X-BusinessAddressStreet", "X-BusinessFax", 
    "X-BusinessHomePage", "X-BusinessPhone", "X-BusinessPhone2", 
    "X-Callback", "X-CarPhone", "X-Categories", "X-Children", "X-City", "ORG",
    "X-CompanyMainPhone", "X-ComputerNetworkName", "X-Country", "X-Created",
    "X-CustomerID", "X-Department", "EMAIL", "X-E-mail2",
    "X-E-mail3", "X-FileAs", "X-FirstName", "X-Folder", "X-FTPSite",
    "N", "X-Gender", "X-GovernmentIDNumber", "X-Hobbies",
    "X-HomeAddress", "X-HomeAddressCity", "X-HomeAddressCountry", 
    "X-HomeAddressPOBox", "X-HomeAddressPostalCode", "X-HomeAddressState", 
    "X-HomeAddressStreet", "X-HomeFax", "X-HomePhone", "X-HomePhone2", 
    "X-Icon", "X-InFolder", "X-Initials", "X-ISDN", "ROLE", "X-Journal",
    "X-Language", "X-LastName", "X-Location", "X-MailingAddress",
    "X-ManagersName", "X-MessageClass", "X-MiddleName", "X-Mileage",
    "X-MobilePhone", "X-Modified", "X-Nickname", "X-OfficeLocation",
    "X-OrganizationalIDNumber", "X-OtherAddress", "X-OtherAddressCity", 
    "X-OtherAddressCountry", "X-OtherAddressPOBox", 
    "X-OtherAddressPostalCode", "X-OtherAddressState", 
    "X-OtherAddressStreet", "X-OtherFax", "X-OtherPhone",
    "X-Pager", "X-PersonalHomePage", "X-POBox", "X-PrimaryPhone",
    "X-Profession", "X-RadioPhone", "X-Read", "X-ReferredBy",
    "X-Sensitivity", "X-Size", "X-Spouse", "X-State",
    "X-StreetAddress", "X-Subject", "X-Suffix", "X-Telex",
    "X-Title", "X-TtyTddPhone", "X-UserField1", "X-UserField2",
    "X-UserField3", "X-UserField4", "WEBPAGE", "X-ZIPPostalCode",
    "" 
  };

  const QString frequentFields[] = {
    "X-AssistantsPhone", "X-Attachment", "X-BusinessAddress",
    "X-BusinessFax", "X-BusinessHomePage", "X-BusinessPhone", 
    "X-BusinessPhone2", "X-Callback", "X-CarPhone", "X-Categories",
    "ORG", "X-CompanyMainPhone", "X-Country",
    "X-Department", "EMAIL", "X-E-mail2", "X-E-mail3", 
    "X-FileAs", "X-FirstName", "X-Folder", "N", "X-HomeAddress", 
    "X-HomeFax", "X-HomePhone", "X-HomePhone2", 
    "X-Icon", "X-ISDN", "ROLE", "X-Journal",
    "X-LastName", "X-MailingAddress", "X-MobilePhone", 
    "X-Modified", "X-OfficeLocation", "X-OtherAddress", 
    "X-OtherAddressCity", "X-OtherFax", "X-OtherPhone",
    "X-Pager", "X-PersonalHomePage", "X-PrimaryPhone",
    "X-RadioPhone", "X-Sensitivity", "X-State", "X-Telex",
    "X-TtyTddPhone", "WEBPAGE",
    "" 
  };

  const QString addressFields[] = { 
    "X-City", "X-Country", "X-Department",
    "X-HomeAddress", "X-HomeAddressCity", "X-HomeAddressCountry",
    "X-HomeAddressPOBox", "X-HomeAddressPostalCode", "X-HomeAddressState",
    "X-HomeAddressStreet", "X-Location", "X-MailingAddress",
    "X-OfficeLocation", "X-OtherAddress", "X-OtherAddressCity",
    "X-OtherAddressCountry", "X-OtherAddressPOBox", "X-OtherAddressPostalCode",
    "X-OtherAddressState", "X-OtherAddressStreet", "X-POBox",
    "X-State", "X-StreetAddress", "X-ZIPPostalCode",
    ""
  };

  const QString emailFields[] = { "EMAIL", "X-E-mail2", "X-E-mail3", "" };

  const QString faxFields[] = {
    "X-BusinessFax", "X-ComputerNetworkName", "X-FTPSite",		
    "X-HomeFax", "X-ISDN", "X-OtherFax", "X-Telex",
    ""
  };

  const QString miscFields[] = { 
    "X-Account", "X-AssistantsName", "X-AssistantsPhone",
    "X-ComputerNetworkName", "X-CustomerID", "X-Department",
    "X-FTPSite", "X-GovernmentIDNumber", "X-OrganizationalIDNumber",
    "X-UserField1", "X-UserField2", "X-UserField3", "X-UserField4",
    ""
  };

  const QString nameFields[] = {
    "X-AssistantsName", "X-Children", "X-FileAs",
    "X-FirstName", "N", "X-Initials",	
    "ROLE", "X-LastName", "X-ManagersName",
    "X-MiddleName", "X-Nickname", "X-ReferredBy",	
    "X-Spouse", "X-Suffix", "X-Title",
    ""
  };

  const QString personalFields[] = {
    "X-Anniversary", "BDAY", "X-Children",
    "X-Gender", "X-Hobbies", "X-Language",
    "X-Profession", "X-ReferredBy", "X-Spouse",
    "WEBPAGE",
    ""
  };

  const QString phoneFields[] = {
    "X-AssistantsPhone", "X-BusinessPhone", "X-BusinessPhone2",
    "X-BusinessFax", "X-Callback", "X-CarPhone",
    "X-CompanyMainPhone", "X-HomePhone", "X-HomePhone2", "X-HomeFax",
    "X-ISDN", "X-MobilePhone", "X-OtherPhone", "X-OtherFax",
    "X-Pager", "X-PrimaryPhone", "X-RadioPhone", "X-Telex",
    "X-TtyTddPhone",
    ""
  };

  /* Outlook default doesn't include fax fields
  const QString phoneFields[] = {
    "X-AssistantsPhone", "X-BusinessPhone", "X-BusinessPhone2",
    "X-Callback", "X-CarPhone", "X-CompanyMainPhone",
    "X-HomePhone", "X-HomePhone2", "X-MobilePhone",
    "X-OtherPhone", "X-Pager", "X-PrimaryPhone",
    "X-RadioPhone", "X-TtyTddPhone",
    ""
  };
  */

  nameList_.append( QStringPtrToQStringList( allFields ));
  nameList_.append( QStringPtrToQStringList( frequentFields ));
  nameList_.append( QStringPtrToQStringList( addressFields ));
  nameList_.append( QStringPtrToQStringList( emailFields ));
  nameList_.append( QStringPtrToQStringList( faxFields  ));
  nameList_.append( QStringPtrToQStringList( miscFields  ));
  nameList_.append( QStringPtrToQStringList( nameFields ));
  nameList_.append( QStringPtrToQStringList( personalFields ));
  nameList_.append( QStringPtrToQStringList( phoneFields ));
}

QString Attributes::fieldListName( int index )
{
  const QString sFieldsList[] = {
    i18n( "All Contact fields" ), i18n( "Frequently-used fields" ),
    i18n( "Address fields" ), i18n( "Email fields" ),
    i18n( "Fax/Other number fields" ), i18n( "Miscellaneous fields" ),
    i18n( "Name fields" ), i18n( "Personal fields" ),
    i18n( "Phone number fields" ),
    ""
  };
  return sFieldsList[index];
}
