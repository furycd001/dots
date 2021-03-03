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

#ifndef STYLEDBUTTON_H
#define STYLEDBUTTON_H

#include <qbutton.h>
#include <qpixmap.h>

class QColor;
class QBrush;
class FormWindow;

class StyledButton : public QButton
{
    Q_OBJECT

    Q_PROPERTY( QColor color READ color WRITE setColor )
    Q_PROPERTY( QPixmap pixmap READ pixmap WRITE setPixmap )
    Q_PROPERTY( EditorType editor READ editor WRITE setEditor )
    Q_PROPERTY( bool scale READ scale WRITE setScale )

    Q_ENUMS( EditorType )

public:
    enum EditorType { ColorEditor, PixmapEditor };

    StyledButton( QWidget* parent = 0, const char* name = 0 );
    StyledButton( const QBrush& b, QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~StyledButton();

    void setEditor( EditorType );
    EditorType editor() const;

    void setColor( const QColor& );
    void setPixmap( const QPixmap& );

    QPixmap* pixmap() const;
    QColor color() const;

    void setScale( bool );
    bool scale() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setFormWindow( FormWindow *fw ) { formWindow = fw; }
    
public slots:
    virtual void onEditor();

signals:
    void changed();

protected:
    void drawButton( QPainter* );
    void drawButtonLabel( QPainter* );
    void resizeEvent( QResizeEvent* );
    void scalePixmap();

private:
    QPixmap* pix;
    QPixmap* spix;  // the pixmap scaled down to fit into the button
    QColor col;
    EditorType edit;
    bool s;
    FormWindow *formWindow;
};

#endif //STYLEDBUTTON_H
