/***************************************************************************
                          katefilelist.h  -  description
                             -------------------
    begin                : Mon Feb 5 2001
    copyright            : (C) 2001 by Christoph Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KANTFILELIST_H
#define KANTFILELIST_H

#include "../main/katemain.h"

#include <klistbox.h>
#include <qtooltip.h>


class KateFileListItem : public QListBoxItem
{
  public:
    KateFileListItem( uint docID, const QPixmap &pix, const QString& text);
    ~KateFileListItem();

    uint docID ();

    const QPixmap *pixmap() const { return &pm; };

    void setText(const QString &text);
    void setPixmap(const QPixmap &pixmap);

    void setBold(bool bold);

    int height( const QListBox* lb ) const;

    int width( const QListBox* lb ) const;

  protected:
    void paint( QPainter *painter );

  private:
    uint myDocID;
    QPixmap pm;
    bool _bold;
};

class KateFileList : public KListBox
{
  Q_OBJECT

  public:
    KateFileList (KateDocManager *_docManager, KateViewManager *_viewManager, QWidget * parent = 0, const char * name = 0 );
    ~KateFileList ();

    /** called by KFLToolTip::maybeTip() to get a string
     * and a rect based on the point.
     * Returns the URL for the doc which item is under p
     * if any.
     */
    void tip( const QPoint &p, QRect &r, QString &str );

  private:
    KateDocManager *docManager;
    KateViewManager *viewManager;

  private slots:
    void slotDocumentCreated (KateDocument *doc);
    void slotDocumentDeleted (uint docID);
    void slotActivateView( QListBoxItem *item );
    void slotModChanged (KateDocument *doc);
    void slotNameChanged (KateDocument *doc);
    void slotViewChanged ();
    void slotMenu ( QListBoxItem *item, const QPoint &p );

  private:
    /////////////////////////////////////////////////////////////////////
    // A private tooltip class to display the URL of a document in the
    // tooltip.
    // Thanks to KDevelop team for the code:)
    /////////////////////////////////////////////////////////////////////
    class KFLToolTip : public QToolTip
    {
      public:
        KFLToolTip(QWidget *parent);
      protected:
        void maybeTip( const QPoint & );
    };
    KFLToolTip* tooltip;
};

#endif
