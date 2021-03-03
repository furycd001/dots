/*  This file is part of the KDE libraries
    Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>
                  2000 Malte Starostik <malte@kde.org> 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// $Id: textcreator.cpp,v 1.5 2001/07/27 22:45:50 malte Exp $

#include <qfile.h>
#include <qpixmap.h>
#include <qimage.h>

#include <kstddirs.h>
#include <kpixmapsplitter.h>

#include "textcreator.h"

extern "C"
{
    ThumbCreator *new_creator()
    {
        return new TextCreator;
    }
};

TextCreator::TextCreator()
    : m_splitter(0)
{
}

TextCreator::~TextCreator()
{
    delete m_splitter;
}

bool TextCreator::create(const QString &path, int width, int height, QImage &img)
{
    if ( !m_splitter )
    {
        m_splitter = new KPixmapSplitter;
        QString pixmap = locate( "data", "konqueror/pics/thumbnailfont_7x4.png" );
        if ( !pixmap.isEmpty() )
        {
            // FIXME: make configurable...
            // DF: What, the size ? You can determine it from m_size.
            // CP: no, I meant the font-pixmap and the glyph-size
            m_splitter->setPixmap( QPixmap( pixmap ));
            m_splitter->setItemSize( QSize( 4, 7 ));
        }
    }

    bool ok = false;
    // create text-preview
    const int bytesToRead = 1024; // FIXME, make configurable
    QFile file( path );
    if ( file.open( IO_ReadOnly ))
    {
        char data[bytesToRead+1];
        int read = file.readBlock( data, bytesToRead );
        if ( read > 0 )
        {
            ok = true;
            data[read] = '\0';
            QString text = QString::fromLocal8Bit( data );
            // FIXME: maybe strip whitespace and read more?

            QRect rect;

            // example: width: 60, height: 64
            QPixmap pix;
            if (height * 3 > width * 4)
                pix.resize(width, width * 4 / 3);
            else
                pix.resize(height * 3 / 4, height);
            pix.fill( QColor( 245, 245, 245 ) ); // light-grey background

            QSize chSize = m_splitter->itemSize(); // the size of one char
            int xOffset = chSize.width();
            int yOffset = chSize.height();

            // one pixel for the rectangle, the rest. whitespace
            int xborder = 1 + pix.width()/16;    // minimum x-border
            int yborder = 1 + pix.height()/16; // minimum y-border

            // calculate a better border so that the text is centered
            int canvasWidth = pix.width() - 2*xborder;
            int canvasHeight = pix.height() -  2*yborder;
            int numCharsPerLine = (int) (canvasWidth / chSize.width());
            int numLines = (int) (canvasHeight / chSize.height());

            int rest = pix.width() - (numCharsPerLine * chSize.width());
            xborder = QMAX( xborder, rest/2); // center horizontally
            rest = pix.height() - (numLines * chSize.height());
            yborder = QMAX( yborder, rest/2); // center vertically
            // end centering

            int x = xborder, y = yborder; // where to paint the characters
            int posNewLine  = pix.width() - (chSize.width() + xborder);
            int posLastLine = pix.height() - (chSize.height() + yborder);
            bool newLine = false;
            ASSERT( posNewLine > 0 );
            const QPixmap *fontPixmap = &(m_splitter->pixmap());

            for ( uint i = 0; i < text.length(); i++ )
            {
                if ( x > posNewLine || newLine ) // start a new line?
                {
                    x = xborder;
                    y += yOffset;

                    if ( y > posLastLine ) // more text than space
                        break;

                    // after starting a new line, we also jump to the next
                    // physical newline in the file if we don't come from one
                    if ( !newLine )
                    {
                        int pos = text.find( '\n', i );
                        if ( pos > (int) i )
                        i = pos +1;
                    }

                    newLine = false;
                }

                // check for newlines in the text (unix,dos)
                QChar ch = text.at( i );
                if ( ch == '\n' )
                {
                    newLine = true;
                    continue;
                }
                else if ( ch == '\r' && text.at(i+1) == '\n' )
                {
                    newLine = true;
                    i++; // skip the next character (\n) as well
                    continue;
                }
                
                rect = m_splitter->coordinates( ch );
                if ( !rect.isEmpty() )
                {
                    bitBlt( &pix, QPoint(x,y), fontPixmap, rect, Qt::CopyROP );
                }

                x += xOffset; // next character
            }
            if (ok)
                img = pix.convertToImage();
        }
        file.close();
    }
    return ok;
}

ThumbCreator::Flags TextCreator::flags() const
{
    return (Flags)(DrawFrame | BlendIcon);
}

