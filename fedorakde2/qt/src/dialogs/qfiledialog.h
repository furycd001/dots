/****************************************************************************
** $Id: qt/src/dialogs/qfiledialog.h   2.3.2   edited 2001-01-26 $
**
** Definition of QFileDialog class
**
** Created : 950428
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QFILEDIALOG_H
#define QFILEDIALOG_H

struct QFileDialogPrivate;
class QPushButton;
class QLabel;
class QWidget;
class QFileDialog;
class QTimer;
class QNetworkOperation;
class QFileListView;

#ifndef QT_H
#include "qdir.h"
#include "qdialog.h"
#include "qlistbox.h"
#include "qlineedit.h"
#include "qlistview.h"
#include "qurloperator.h"
#include "qurlinfo.h"
#endif // QT_H

#ifndef QT_NO_FILEDIALOG

class Q_EXPORT QFileIconProvider : public QObject
{
    Q_OBJECT
public:
    QFileIconProvider( QObject * parent = 0, const char* name = 0 );
    virtual const QPixmap * pixmap( const QFileInfo & );

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QFileIconProvider( const QFileIconProvider & );
    QFileIconProvider& operator=( const QFileIconProvider & );
#endif
};

class Q_EXPORT QFilePreview
{
public:
    QFilePreview();
    virtual void previewUrl( const QUrl &url ) = 0;

};

class Q_EXPORT QFileDialog : public QDialog
{
    friend class QFileListBox;
    friend class QFileListView;

    Q_OBJECT
    Q_ENUMS( Mode ViewMode PreviewMode )
    // ##### Why are this read-only properties ?
    Q_PROPERTY( QString selectedFile READ selectedFile )
    Q_PROPERTY( QString selectedFilter READ selectedFilter )
    Q_PROPERTY( QStringList selectedFiles READ selectedFiles )
    // #### Should not we be able to set the path ?
    Q_PROPERTY( QString dirPath READ dirPath )
    Q_PROPERTY( bool showHiddenFiles READ showHiddenFiles WRITE setShowHiddenFiles )
    Q_PROPERTY( Mode mode READ mode WRITE setMode )
    Q_PROPERTY( ViewMode viewMode READ viewMode WRITE setViewMode )
    Q_PROPERTY( PreviewMode previewMode READ previewMode WRITE setPreviewMode )
    Q_PROPERTY( bool infoPreview READ isInfoPreviewEnabled WRITE setInfoPreviewEnabled )
    Q_PROPERTY( bool contentsPreview READ isContentsPreviewEnabled WRITE setContentsPreviewEnabled )

public:
    QFileDialog( const QString& dirName, const QString& filter = QString::null,
                 QWidget *parent=0, const char *name = 0, bool modal = FALSE );
    QFileDialog( QWidget *parent=0, const char *name = 0, bool modal = FALSE );
    ~QFileDialog();

    // recommended static functions

    static QString getOpenFileName( const QString &initially = QString::null,
				    const QString &filter = QString::null,
				    QWidget *parent = 0, const char* name = 0 ); // ## merge 3.0
    static QString getOpenFileName( const QString &initially,
				    const QString &filter,
				    QWidget *parent, const char* name, const QString& caption );
    static QString getSaveFileName( const QString &initially = QString::null,
				    const QString &filter = QString::null,
				    QWidget *parent = 0, const char* name = 0);// ## merge 3.0
    static QString getSaveFileName( const QString &initially,
				    const QString &filter,
				    QWidget *parent, const char* name,
				    const QString& caption);
    static QString getExistingDirectory( const QString &dir = QString::null,
					 QWidget *parent = 0,
					 const char* name = 0 );// ## merge 3.0
    static QString getExistingDirectory( const QString &dir,
					 QWidget *parent,
					 const char* name,
					 const QString& caption );
    static QString getExistingDirectory( const QString &dir,
					 QWidget *parent,
					 const char* name,
					 const QString& caption,
					 bool dirOnly );
    static QStringList getOpenFileNames( const QString &filter= QString::null,
					 const QString &dir = QString::null,
					 QWidget *parent = 0,
					 const char* name = 0);// ## merge 3.0
    static QStringList getOpenFileNames( const QString &filter,
					 const QString &dir,
					 QWidget *parent,
					 const char* name,
					 const QString& caption);


    // other static functions

    static void setIconProvider( QFileIconProvider * );
    static QFileIconProvider * iconProvider();

    // non-static function for special needs

    QString selectedFile() const;
    QString selectedFilter() const;
    void setSelection( const QString &);

    void selectAll( bool b );

    QStringList selectedFiles() const;

    QString dirPath() const;

    void setDir( const QDir & );
    const QDir *dir() const;

    void setShowHiddenFiles( bool s );
    bool showHiddenFiles() const;

    void rereadDir();
    void resortDir();

    enum Mode { AnyFile, ExistingFile, Directory, ExistingFiles, DirectoryOnly };
    void setMode( Mode );
    Mode mode() const;

    enum ViewMode { Detail, List };
    enum PreviewMode { NoPreview, Contents, Info };
    void setViewMode( ViewMode m );
    ViewMode viewMode() const;
    void setPreviewMode( PreviewMode m );
    PreviewMode previewMode() const;

    bool eventFilter( QObject *, QEvent * );

    bool isInfoPreviewEnabled() const;
    bool isContentsPreviewEnabled() const;
    void setInfoPreviewEnabled( bool );
    void setContentsPreviewEnabled( bool );

    void setInfoPreview( QWidget *w, QFilePreview *preview );
    void setContentsPreview( QWidget *w, QFilePreview *preview );

    QUrl url() const;

public slots:
    void done( int );
    void setDir( const QString& );
    void setUrl( const QUrlOperator &url );
    void setFilter( const QString& );
    void setFilters( const QString& );
    void setFilters( const char ** );
    void setFilters( const QStringList& );

protected:
    void resizeEvent( QResizeEvent * );
    void keyPressEvent( QKeyEvent * );

    void addWidgets( QLabel *, QWidget *, QPushButton * );
    void addToolButton( QButton *b, bool separator = FALSE );
    void addLeftWidget( QWidget *w );
    void addRightWidget( QWidget *w );
    void addFilter( const QString &filter );

signals:
    void fileHighlighted( const QString& );
    void fileSelected( const QString& );
    void dirEntered( const QString& );

private slots:
    void detailViewSelectionChanged();
    void listBoxSelectionChanged();
    void changeMode( int );
    void fileNameEditReturnPressed();
    void stopCopy();
    void removeProgressDia();

    void fileSelected( int );
    void fileHighlighted( int );
    void dirSelected( int );
    void pathSelected( int );

    void updateFileNameEdit( QListViewItem *);
    void selectDirectoryOrFile( QListViewItem * );
    void popupContextMenu( QListViewItem *, const QPoint &, int );
    void popupContextMenu( QListBoxItem *, const QPoint & );
    void updateFileNameEdit( QListBoxItem *);
    void selectDirectoryOrFile( QListBoxItem * );
    void fileNameEditDone();

    void okClicked();
    void filterClicked(); // not used
    void cancelClicked();

    void cdUpClicked();
    void newFolderClicked();

    void fixupNameEdit();

    void doMimeTypeLookup();

    void updateGeometries();
    void modeButtonsDestroyed();
    void urlStart( QNetworkOperation *op );
    void urlFinished( QNetworkOperation *op );
    void dataTransferProgress( int bytesDone, int bytesTotal, QNetworkOperation * );
    void insertEntry( const QValueList<QUrlInfo> &fi, QNetworkOperation *op );
    void removeEntry( QNetworkOperation * );
    void createdDirectory( const QUrlInfo &info, QNetworkOperation * );
    void itemChanged( QNetworkOperation * );
    void goBack();

private:
    enum PopupAction {
        PA_Open = 0,
        PA_Delete,
        PA_Rename,
        PA_SortName,
        PA_SortSize,
        PA_SortType,
        PA_SortDate,
        PA_SortUnsorted,
        PA_Cancel,
        PA_Reload,
        PA_Hidden
    };

    void init();
    bool trySetSelection( bool isDir, const QUrlOperator &, bool );
    void deleteFile( const QString &filename );
    void popupContextMenu( const QString &filename, bool withSort,
                           PopupAction &action, const QPoint &p );

    QDir reserved; // was cwd
    QString fileName;

    QFileDialogPrivate *d;
    QFileListView  *files;

    QLineEdit  *nameEdit; // also filter
    QPushButton *okB;
    QPushButton *cancelB;

#if defined(_WS_WIN_)
    static QString winGetOpenFileName( const QString &initialSelection,
				       const QString &filter,
				       QString* workingDirectory,
				       QWidget *parent = 0,
				       const char* name = 0,
				       const QString& caption = QString::null);
    static QString winGetSaveFileName( const QString &initialSelection,
				       const QString &filter,
				       QString* workingDirectory,
				       QWidget *parent = 0,
				       const char* name = 0,
				       const QString& caption = QString::null);
    static QStringList winGetOpenFileNames( const QString &filter,
					    QString* workingDirectory,
					    QWidget *parent = 0,
					    const char* name = 0,
					    const QString& caption = QString::null);
#endif

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QFileDialog( const QFileDialog & );
    QFileDialog &operator=( const QFileDialog & );
#endif
};

#endif

#endif // QFILEDIALOG_H
