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

#ifndef TOPICCHOOSERIMPL_H
#define TOPICCHOOSERIMPL_H

#include "topicchooser.h"

class TopicChooser : public TopicChooserBase
{
    Q_OBJECT
    
public:
    TopicChooser( QWidget *parent, const QStringList &lnkNames,
		  const QStringList &lnks, const QString &title );
    
    QString link() const;
    
    static QString getLink( QWidget *parent, const QStringList &lnkNames,
			    const QStringList &lnks, const QString &title );
    
private:
    QString theLink;
    QStringList links, linkNames;

};

#endif
