/***************************************************************************
                          katesyntaxdocument.h  -  description
                             -------------------
    begin                : Sat 31 March 2001
    copyright            : (C) 2001 by Joseph Wenninger
    email                : jowenn@bigfoot.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNTAXDOCUMENT_H
#define SYNTAXDOCUMENT_H

#include <qdom.h>
#include <qlist.h>

class QStringList;

class syntaxModeListItem
{
  public:
    QString name;
    QString section;
    QString mimetype;
    QString extension;
    QString identifier;
};

class syntaxContextData
{
  public:
    QDomElement parent;
    QDomElement currentGroup;
    QDomElement item;
};

typedef QList<syntaxModeListItem> SyntaxModeList;

class SyntaxDocument : public QDomDocument
{
  public:
    SyntaxDocument();
    ~SyntaxDocument();

    QStringList& finddata(const QString& mainGroup,const QString& type,bool clearList=true);
    SyntaxModeList modeList();

    syntaxContextData* getGroupInfo(const QString& langName, const QString &group);
    void freeGroupInfo(syntaxContextData* data);
    syntaxContextData* getConfig(const QString& mainGroupName, const QString &Config);       
    bool nextItem(syntaxContextData* data);
    bool nextGroup(syntaxContextData* data);
    syntaxContextData* getSubItems(syntaxContextData* data);
    QString groupItemData(syntaxContextData* data,QString name);
    QString groupData(syntaxContextData* data,QString name);
    void setIdentifier(const QString& identifier);

  private:
     void setupModeList(bool force=false);
     QString currentFile;
    SyntaxModeList myModeList;
    QStringList m_data;
};

#endif
