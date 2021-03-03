// "$Id$"
/* This file is part of the KDE libraries
    Copyright (C) 2000 David Faure <faure@kde.org>

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
#ifndef __open_with_h__
#define __open_with_h__

#include <qwidget.h>
#include <qfileinfo.h>
#include <qdialog.h>

#include <kapp.h>
#include <kurl.h>
#include <klistview.h>
#include <krun.h>

#include <kservice.h>

class KApplicationTree;
class KURLRequester;

class QCheckBox;
class QPushButton;
class QLabel;
class QStringList;

/* ------------------------------------------------------------------------- */

/**
 * "Open with" dialog box.
 * Used automatically by KRun, and used by libkonq.
 *
 * @author David Faure <faure@kde.org>
 */
class KOpenWithDlg : public QDialog
{
    Q_OBJECT
public:

    /**
     * Create a dialog that asks for a application to open a given
     * URL(s) with.
     *
     * @param urls   the URLs that should be opened. The list can be empty,
     * if the dialog is used to choose an application but not for some particular URLs.
     * @param parent parent widget
     */
    KOpenWithDlg( const KURL::List& urls, QWidget *parent = 0L );

    /**
     * Create a dialog that asks for a application to open a given
     * URL(s) with.
     *
     * @param urls   is the URL that should be opened
     * @param text   appears as a label on top of the entry box.
     * @param value  is the initial value of the line
     * @param parent parent widget
     */
    KOpenWithDlg( const KURL::List& urls, const QString& text, const QString& value, QWidget *parent = 0L );

    /**
     * Create a dialog to select a service for a given service type.
     * Note that this dialog doesn't apply to URLs.
     *
     * @param serviceType the service type we want to choose an application for.
     * @param value  is the initial value of the line
     * @param parent parent widget
     */
    KOpenWithDlg( const QString& serviceType, const QString& value, QWidget *parent = 0L );

    /**
     * Destructor
     */
    ~KOpenWithDlg();

    /**
     * @return the text the user entered
     */
    QString text();
    /**
     * @return the chosen service in the application tree
     * Can be null, if the user typed some text and didn't select a service.
     */
    KService::Ptr service() { return m_pService; }

public slots:
    /**
    * The slot for clearing the edit widget
    */
    void slotClear();
    void slotSelected( const QString&_name, const QString& _exec );
    void slotHighlighted( const QString& _name, const QString& _exec );
    void slotTextChanged();
    void slotTerminalToggled(bool);
    void slotDbClick();
    void slotOK();

protected slots:
    /**
     * Reimplemented from QDialog::accept() to save history of the combobox
     */
    virtual void accept();
    
protected:

    /**
     * Determine service type from URLs
     */
    void setServiceType( const KURL::List& _urls );

    /**
     * Create a dialog that asks for a application to open a given
     * URL(s) with.
     *
     * @param text   appears as a label on top of the entry box.
     * @param value  is the initial value of the line
     */
    void init( const QString& text, const QString& value );

    KURLRequester * edit;
    QString m_command;

    KApplicationTree* m_pTree;
    QLabel *label;

    QString qName, qServiceType;
    bool m_terminaldirty;
    QCheckBox   *terminal, *remember;
    QPushButton *ok;
    QPushButton *clear;
    QPushButton *cancel;

    KService::Ptr m_pService;

    class KOpenWithDlgPrivate;
    KOpenWithDlgPrivate *d;
};

/* ------------------------------------------------------------------------- */

/**
 * This class handles the displayOpenWithDialog call, made by KRun
 * when it has no idea what to do with a URL.
 * It displays the open-with dialog box.
 *
 * If you use KRun you _need_ to create an instance of KFileOpenWithHandler
 * (except if you can make sure you only use it for executables or
 *  Type=Application desktop files)
 *
 *
 */
class KFileOpenWithHandler : public KOpenWithHandler
{
public:
  KFileOpenWithHandler() : KOpenWithHandler() {}
  virtual ~KFileOpenWithHandler() {}

  /**
   * Opens an open-with dialog box for @p urls
   * @returns true if the operation succeeded
   */
  virtual bool displayOpenWithDialog( const KURL::List& urls );
    // TODO (BCI) : add optionnal mimetype parameter
};



/* ------------------------------------------------------------------------- */

/**
 * @internal
 */
class KAppTreeListItem : public QListViewItem
{
    bool parsed;
    bool directory;
    QString path;
    QString exec;

protected:
    QString key(int column, bool ascending) const;

    void init(const QPixmap& pixmap, bool parse, bool dir, QString _path, QString exec);

public:
    KAppTreeListItem( KListView* parent, const QString & name, const QPixmap& pixmap,
                      bool parse, bool dir, QString p, QString c );
    KAppTreeListItem( QListViewItem* parent, const QString & name, const QPixmap& pixmap,
                      bool parse, bool dir, QString p, QString c );
    bool isDirectory();

protected:
    virtual void activate();
    virtual void setOpen( bool o );

    friend class KApplicationTree;
};

/* ------------------------------------------------------------------------- */

/**
 * @internal
 */
class KApplicationTree : public KListView
{
    Q_OBJECT
public:
    KApplicationTree( QWidget *parent );

    /**
     * Add a group of .desktop/.kdelnk entries
     */
    void addDesktopGroup( QString relPath, KAppTreeListItem *item = 0 );

    KAppTreeListItem *it;
    bool isDirSel();

protected:
    void resizeEvent( QResizeEvent *_ev );
    KAppTreeListItem* currentitem;

public slots:
    void slotItemHighlighted(QListViewItem* i);
    void slotSelectionChanged(QListViewItem* i);

signals:
    void selected( const QString& _name, const QString& _exec );
    void highlighted( const QString& _name, const  QString& _exec );
};

/* ------------------------------------------------------------------------- */

#endif
