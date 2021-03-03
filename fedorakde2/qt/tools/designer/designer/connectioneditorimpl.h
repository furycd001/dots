/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef CONNECTIONEDITORIMPL_H
#define CONNECTIONEDITORIMPL_H

#include "connectioneditor.h"
#include "metadatabase.h"

#include <qmap.h>

class QListViewItem;
class FormWindow;

class ConnectionEditor : public ConnectionEditorBase
{
    Q_OBJECT

public:
    ConnectionEditor( QWidget *parent, QObject* sender, QObject* receiver, FormWindow *fw );
    ~ConnectionEditor();


protected:
    void signalChanged();
    void connectClicked();
    void disconnectClicked();
    void okClicked();
    void cancelClicked();
    void slotsChanged();
    void connectionsChanged();
    void addSlotClicked();
    
private:
    bool ignoreSlot( const char* ) const;
    struct Connection
    {
	QCString signal, slot;
    };

    QMap<QListViewItem*, Connection> connections;
    QValueList<MetaDataBase::Connection> oldConnections;
    QObject* sender;
    QObject* receiver;
    FormWindow *formWindow;

};

#endif
