/****************************************************************************
** $Id: qt/src/dialogs/qfiledialog.cpp   2.3.2   edited 2001-09-13 $
**
** Implementation of QFileDialog class
**
** Created : 950429
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

#include "qfiledialog.h"

#ifndef QT_NO_FILEDIALOG

#include "qlineedit.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qtoolbutton.h"
#include "qmessagebox.h"
#include "qapplication.h"
#include "qlayout.h"
#include "qbitmap.h"
#include "qpopupmenu.h"
#include "qwidgetstack.h"
#include "qbuttongroup.h"
#include "qvector.h"
#include "qregexp.h"
#include "qstrlist.h"
#include "qtimer.h"
#include "qvbox.h"
#include "qhbox.h"
#include "qtooltip.h"
#include "qheader.h"
#include "qdragobject.h"
#include "qmime.h"
#include "qprogressbar.h"
#include "qfile.h"
#include "qcstring.h"
#include "qobjectlist.h"
#include "qcheckbox.h"
#include "qsplitter.h"
#include "qmap.h"
#include "qnetworkprotocol.h"
#include "qsemimodal.h"

#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#if defined(_OS_UNIX_)
// getlogin()
# include <unistd.h>
// getpwnam()
# include <sys/types.h>
# include <pwd.h>
#endif

// see comment near use of this variable
static const char * egcsWorkaround = "%x  %X";

static QFileIconProvider * fileIconProvider = 0;


/* XPM */
static const char * const start_xpm[]={
    "16 15 8 1",
    "a c #cec6bd",
    "# c #000000",
    "e c #ffff00",
    "b c #999999",
    "f c #cccccc",
    "d c #dcdcdc",
    "c c #ffffff",
    ". c None",
    ".....######aaaaa",
    "...bb#cccc##aaaa",
    "..bcc#cccc#d#aaa",
    ".bcef#cccc#dd#aa",
    ".bcfe#cccc#####a",
    ".bcef#ccccccccc#",
    "bbbbbbbbbbbbccc#",
    "bccccccccccbbcc#",
    "bcefefefefee#bc#",
    ".bcefefefefef#c#",
    ".bcfefefefefe#c#",
    "..bcfefefefeeb##",
    "..bbbbbbbbbbbbb#",
    "...#############",
    "................"};

/* XPM */
static const char * const end_xpm[]={
    "16 15 9 1",
    "d c #a0a0a0",
    "c c #c3c3c3",
    "# c #cec6bd",
    ". c #000000",
    "f c #ffff00",
    "e c #999999",
    "g c #cccccc",
    "b c #ffffff",
    "a c None",
    "......####aaaaaa",
    ".bbbb..###aaaaaa",
    ".bbbb.c.##aaaaaa",
    ".bbbb....ddeeeea",
    ".bbbbbbb.bbbbbe.",
    ".bbbbbbb.bcfgfe.",
    "eeeeeeeeeeeeefe.",
    "ebbbbbbbbbbeege.",
    "ebfgfgfgfgff.ee.",
    "aebfgfgfgfgfg.e.",
    "aebgfgfgfgfgf.e.",
    "aaebgfgfgfgffe..",
    "aaeeeeeeeeeeeee.",
    "aaa.............",
    "aaaaaaaaaaaaaaaa"};

/* XPM */
static const char* const open_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "c c #cccccc",
    "a c #ffffff",
    "................",
    "................",
    "...*****........",
    "..*aaaaa*.......",
    ".*abcbcba******.",
    ".*acbcbcaaaaaa*d",
    ".*abcbcbcbcbcb*d",
    "*************b*d",
    "*aaaaaaaaaa**c*d",
    "*abcbcbcbcbbd**d",
    ".*abcbcbcbcbcd*d",
    ".*acbcbcbcbcbd*d",
    "..*acbcbcbcbb*dd",
    "..*************d",
    "...ddddddddddddd",
    "................"};

/* XPM */
static const char * const link_dir_xpm[]={
    "16 16 10 1",
    "h c #808080",
    "g c #a0a0a0",
    "d c #000000",
    "b c #ffff00",
    "f c #303030",
    "# c #999999",
    "a c #cccccc",
    "e c #585858",
    "c c #ffffff",
    ". c None",
    "................",
    "................",
    "..#####.........",
    ".#ababa#........",
    "#abababa######..",
    "#cccccccccccc#d.",
    "#cbababababab#d.",
    "#cabababababa#d.",
    "#cbababdddddddd.",
    "#cababadccccccd.",
    "#cbababdcececcd.",
    "#cababadcefdfcd.",
    "#cbababdccgdhcd.",
    "#######dccchccd.",
    ".dddddddddddddd.",
    "................"};

/* XPM */
static const char * const link_file_xpm[]={
    "16 16 10 1",
    "h c #808080",
    "g c #a0a0a0",
    "d c #c3c3c3",
    ". c #7f7f7f",
    "c c #000000",
    "b c #bfbfbf",
    "f c #303030",
    "e c #585858",
    "a c #ffffff",
    "# c None",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaadc###",
    ".aaaaaaaaaadc###",
    ".aaaacccccccc###",
    ".aaaacaaaaaac###",
    ".aaaacaeaeaac###",
    ".aaaacaefcfac###",
    ".aaaacaagchac###",
    ".ddddcaaahaac###",
    "ccccccccccccc###"};

/* XPM */
static const char* const file_xpm[]={
    "16 16 5 1",
    ". c #7f7f7f",
    "# c None",
    "c c #000000",
    "b c #bfbfbf",
    "a c #ffffff",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".bbbbbbbbbbbc###",
    "ccccccccccccc###"};

/* XPM */
static const char * const closed_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "a c #cccccc",
    "c c #ffffff",
    "................",
    "................",
    "..*****.........",
    ".*ababa*........",
    "*abababa******..",
    "*cccccccccccc*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "**************d.",
    ".dddddddddddddd.",
    "................"};


/* XPM */
static const char* const cdtoparent_xpm[]={
    "15 13 3 1",
    ". c None",
    "* c #000000",
    "a c #ffff99",
    "..*****........",
    ".*aaaaa*.......",
    "***************",
    "*aaaaaaaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaa***aaaaaaa*",
    "*aa*****aaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa******aaa*",
    "*aaaaaaaaaaaaa*",
    "*aaaaaaaaaaaaa*",
    "***************"};


/* XPM */
static const char* const newfolder_xpm[] = {
    "15 14 4 1",
    " 	c None",
    ".	c #000000",
    "+	c #FFFF00",
    "@	c #FFFFFF",
    "          .    ",
    "               ",
    "          .    ",
    "       .     . ",
    "  ....  . . .  ",
    " .+@+@.  . .   ",
    "..........  . .",
    ".@+@+@+@+@..   ",
    ".+@+@+@+@+. .  ",
    ".@+@+@+@+@.  . ",
    ".+@+@+@+@+.    ",
    ".@+@+@+@+@.    ",
    ".+@+@+@+@+.    ",
    "...........    "};

/* XPM */
static const char* const detailedview_xpm[]={
    "14 11 3 1",
    ". c None",
    "* c #000000",
    "a c #000099",
    ".****.***.***.",
    "..............",
    "aaaaaaaaaaaaaa",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***."};

/* XPM */
static const char* const previewinfoview_xpm[]={
    "13 13 4 1",
    ". c #00007f",
    "a c black",
    "# c #cec6bd",
    "b c #000000",
    "..#####aaaaaa",
    ".#.#bb#a#####",
    "...####a#bbb#",
    "#######a#####",
    "#######a#bb##",
    "..#####a#####",
    ".#.#bb#a#bbb#",
    "...####a#####",
    "#######a#bb##",
    "#######a#####",
    "..#####a#bbb#",
    ".#.#bb#a#####",
    "...####aaaaaa"};

/* XPM */
static const char* const previewcontentsview_xpm[]={
    "14 13 5 1",
    ". c #00007f",
    "a c black",
    "c c #7f007f",
    "# c #cec6bd",
    "b c #000000",
    "..#####aaaaaaa",
    ".#.#bb#a#####a",
    "...####a#ccc#a",
    "#######a#ccc#a",
    "#######a#####a",
    "..#####a#bbb#a",
    ".#.#bb#a#####a",
    "...####a#bbb#a",
    "#######a#####a",
    "#######a#bbb#a",
    "..#####a#####a",
    ".#.#bb#a#####a",
    "...####aaaaaaa"};

/* XPM */
static const char* const mclistview_xpm[]={
    "15 11 4 1",
    "* c None",
    "b c #000000",
    ". c #000099",
    "a c #ffffff",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****",
    "***************",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****",
    "***************",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****"};

/* XPM */
static const char * const back_xpm [] = {
    "13 11 3 1",
    "a c #00ffff",
    "# c #000000",
    ". c None",
    ".....#.......",
    "....##.......",
    "...#a#.......",
    "..#aa########",
    ".#aaaaaaaaaa#",
    "#aaaaaaaaaaa#",
    ".#aaaaaaaaaa#",
    "..#aa########",
    "...#a#.......",
    "....##.......",
    ".....#......."};

static QPixmap * openFolderIcon = 0;
static QPixmap * closedFolderIcon = 0;
static QPixmap * detailViewIcon = 0;
static QPixmap * multiColumnListViewIcon = 0;
static QPixmap * cdToParentIcon = 0;
static QPixmap * newFolderIcon = 0;
static QPixmap * fifteenTransparentPixels = 0;
static QPixmap * symLinkDirIcon = 0;
static QPixmap * symLinkFileIcon = 0;
static QPixmap * fileIcon = 0;
static QPixmap * startCopyIcon = 0;
static QPixmap * endCopyIcon = 0;
static QString * workingDirectory = 0;
static bool bShowHiddenFiles = FALSE;
static int sortFilesBy = (int)QDir::Name;
static bool sortAscending = TRUE;
static bool detailViewMode = FALSE;
static QPixmap * previewContentsViewIcon = 0;
static QPixmap * previewInfoViewIcon = 0;
static QPixmap *goBackIcon = 0;
static QSize *lastSize = 0;

static bool isDirectoryMode( int m )
{
    return m == QFileDialog::Directory || m == QFileDialog::DirectoryOnly;
}

static void cleanup() {
    delete openFolderIcon;
    openFolderIcon = 0;
    delete closedFolderIcon;
    closedFolderIcon = 0;
    delete detailViewIcon;
    detailViewIcon = 0;
    delete multiColumnListViewIcon;
    multiColumnListViewIcon = 0;
    delete cdToParentIcon;
    cdToParentIcon = 0;
    delete newFolderIcon;
    newFolderIcon = 0;
    delete fifteenTransparentPixels;
    fifteenTransparentPixels = 0;
    delete workingDirectory;
    workingDirectory = 0;
    delete previewContentsViewIcon;
    previewContentsViewIcon = 0;
    delete previewInfoViewIcon;
    previewInfoViewIcon = 0;
    delete symLinkDirIcon;
    symLinkDirIcon = 0;
    delete symLinkFileIcon;
    symLinkFileIcon = 0;
    delete fileIcon;
    fileIcon = 0;
    delete startCopyIcon;
    startCopyIcon = 0;
    delete endCopyIcon;
    endCopyIcon = 0;
    delete goBackIcon;
    goBackIcon = 0;
    delete lastSize;
    lastSize = 0;
#if defined (_WS_WIN_)
    delete fileIconProvider;
    fileIconProvider = 0;
#endif
}

#if defined(_WS_WIN_)

class QWindowsIconProvider : public QFileIconProvider
{
public:
    QWindowsIconProvider( QWidget *parent=0, const char *name=0 );
    ~QWindowsIconProvider();

    const QPixmap * pixmap( const QFileInfo &fi );

private:
    QPixmap defaultFolder;
    QPixmap defaultFile;
    QPixmap defaultExe;
    QPixmap pix;
    int pixw, pixh;
    QMap< QString, QPixmap > cache;

};
#endif

static void makeVariables() {
    if ( !openFolderIcon ) {
	qAddPostRoutine( cleanup );
	workingDirectory = new QString( QDir::currentDirPath() );
	openFolderIcon = new QPixmap( (const char **)open_xpm);
	symLinkDirIcon = new QPixmap( (const char **)link_dir_xpm);
	symLinkFileIcon = new QPixmap( (const char **)link_file_xpm);
	fileIcon = new QPixmap( (const char **)file_xpm);
	closedFolderIcon = new QPixmap( (const char **)closed_xpm);
	detailViewIcon = new QPixmap( (const char **)detailedview_xpm);
	multiColumnListViewIcon = new QPixmap( (const char **)mclistview_xpm);
	cdToParentIcon = new QPixmap( (const char **)cdtoparent_xpm);
	newFolderIcon = new QPixmap( (const char **)newfolder_xpm);
	previewInfoViewIcon
	    = new QPixmap( (const char **)previewinfoview_xpm );
	previewContentsViewIcon
	    = new QPixmap( (const char **)previewcontentsview_xpm );
	startCopyIcon = new QPixmap( (const char **)start_xpm );
	endCopyIcon = new QPixmap( (const char **)end_xpm );
	goBackIcon = new QPixmap( (const char **)back_xpm );
	fifteenTransparentPixels = new QPixmap( closedFolderIcon->width(), 1 );
	QBitmap m( fifteenTransparentPixels->width(), 1 );
	m.fill( Qt::color0 );
	fifteenTransparentPixels->setMask( m );
	bShowHiddenFiles = FALSE;
	sortFilesBy = (int)QDir::Name;
	detailViewMode = FALSE;
#if defined(_WS_WIN_)
	fileIconProvider = new QWindowsIconProvider();
#endif
    }
}

/******************************************************************
 *
 * Definitions of view classes
 *
 ******************************************************************/

class QRenameEdit : public QLineEdit
{
    Q_OBJECT

public:
    QRenameEdit( QWidget *parent )
        : QLineEdit( parent )
    {}

protected:
    void keyPressEvent( QKeyEvent *e );
    void focusOutEvent( QFocusEvent *e );

signals:
    void escapePressed();

};

class QFileListBox : public QListBox
{
    friend class QFileDialog;

    Q_OBJECT

private:
    QFileListBox( QWidget *parent, QFileDialog *d );

    void clear();
    void show();
    void startRename( bool check = TRUE );
    void viewportMousePressEvent( QMouseEvent *e );
    void viewportMouseReleaseEvent( QMouseEvent *e );
    void viewportMouseDoubleClickEvent( QMouseEvent *e );
    void viewportMouseMoveEvent( QMouseEvent *e );
#ifndef QT_NO_DRAGANDDROP
    void viewportDragEnterEvent( QDragEnterEvent *e );
    void viewportDragMoveEvent( QDragMoveEvent *e );
    void viewportDragLeaveEvent( QDragLeaveEvent *e );
    void viewportDropEvent( QDropEvent *e );
    bool acceptDrop( const QPoint &pnt, QWidget *source );
    void setCurrentDropItem( const QPoint &pnt );
#endif
    void keyPressEvent( QKeyEvent *e );

private slots:
    void rename();
    void cancelRename();
    void doubleClickTimeout();
    void changeDirDuringDrag();
    void dragObjDestroyed();
    void contentsMoved( int, int );

private:
    QRenameEdit *lined;
    QFileDialog *filedialog;
    bool renaming;
    QTimer* renameTimer;
    QListBoxItem *renameItem, *dragItem;
    QPoint pressPos, oldDragPos;
    bool mousePressed;
    int urls;
    QString startDragDir;
    QListBoxItem *currDropItem;
    QTimer *changeDirTimer;
    bool firstMousePressEvent;
    QUrlOperator startDragUrl;

};


class QFileListView : public QListView
{
    friend class QFileDialog;

    Q_OBJECT

private:
    QFileListView( QWidget *parent, QFileDialog *d );

    void clear();
    void startRename( bool check = TRUE );
    void setSorting( int column, bool increasing = TRUE );

private:
    void viewportMousePressEvent( QMouseEvent *e );
    void viewportMouseDoubleClickEvent( QMouseEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void viewportMouseReleaseEvent( QMouseEvent *e );
    void viewportMouseMoveEvent( QMouseEvent *e );
#ifndef QT_NO_DRAGANDDROP
    void viewportDragEnterEvent( QDragEnterEvent *e );
    void viewportDragMoveEvent( QDragMoveEvent *e );
    void viewportDragLeaveEvent( QDragLeaveEvent *e );
    void viewportDropEvent( QDropEvent *e );
    bool acceptDrop( const QPoint &pnt, QWidget *source );
    void setCurrentDropItem( const QPoint &pnt );
#endif

private slots:
    void rename();
    void cancelRename();
    void changeSortColumn2( int column );
    void doubleClickTimeout();
    void changeDirDuringDrag();
    void dragObjDestroyed();
    void contentsMoved( int, int );

private:
    QRenameEdit *lined;
    QFileDialog *filedialog;
    bool renaming;
    QTimer* renameTimer;
    QListViewItem *renameItem;
    QPoint pressPos, oldDragPos;
    bool mousePressed;
    int urls;
    QString startDragDir;
    QListViewItem *currDropItem, *dragItem;
    QTimer *changeDirTimer;
    bool firstMousePressEvent;
    bool ascending;
    int sortcolumn;
    QUrlOperator startDragUrl;

};

/****************************************************************************
 *
 * Classes for copy progress dialog
 *
 ****************************************************************************/

class QFDProgressAnimation : public QWidget
{
    Q_OBJECT

public:
    QFDProgressAnimation( QWidget *parent );
    void start();

private slots:
    void next();

protected:
    void paintEvent( QPaintEvent *e );

private:
    int step;
    QTimer *timer;

};

QFDProgressAnimation::QFDProgressAnimation( QWidget *parent )
    : QWidget( parent )
{
    setFixedSize( 300, 50 );
    step = -1;
    next();
    timer = new QTimer( this );
    connect( timer, SIGNAL( timeout() ),
	     this, SLOT( next() ) );
}

void QFDProgressAnimation::start()
{
    timer->start( 150, FALSE );
}

void QFDProgressAnimation::next()
{
    ++step;
    if ( step > 10 )
	step = 0;
    repaint();
}

void QFDProgressAnimation::paintEvent( QPaintEvent * )
{
    erase();

    QPainter p;
    p.begin( this );
    if ( step == 0 ) {
	p.drawPixmap( 5, ( height() - startCopyIcon->height() ) / 2,
		      *startCopyIcon );
	p.drawPixmap( width() - 5 - openFolderIcon->width(),
		      ( height() - openFolderIcon->height() ) / 2 , *openFolderIcon );
    } else if ( step == 10 ) {
	p.drawPixmap( 5, ( height() - openFolderIcon->height() ) / 2,
		      *openFolderIcon );
	p.drawPixmap( width() - 5 - endCopyIcon->width(),
		      ( height() - endCopyIcon->height() ) / 2 , *endCopyIcon );
    } else {
	p.drawPixmap( 5, ( height() - openFolderIcon->height() ) / 2,
		      *openFolderIcon );
	p.drawPixmap( width() - 5 - openFolderIcon->width(),
		      ( height() - openFolderIcon->height() ) / 2 , *openFolderIcon );
	int x = 10 + openFolderIcon->width();
	int w = width() - 2 * x;
	int s = w / 9;
	p.drawPixmap( x + s * step, ( height() - fileIcon->height() ) / 2 - fileIcon->height(),
		      *fileIcon );
    }
}


class QFDProgressDialog : public QSemiModal
{
    Q_OBJECT

public:
    QFDProgressDialog( QWidget *parent, const QString &fn, int steps );

    void setReadProgress( int p );
    void setWriteProgress( int p );
    void setWriteLabel( const QString &s );

signals:
    void cancelled();

private:
    QProgressBar *readBar;
    QProgressBar *writeBar;
    QLabel *writeLabel;
    QFDProgressAnimation *animation;

};

QFDProgressDialog::QFDProgressDialog( QWidget *parent, const QString &fn, int steps )
    : QSemiModal( parent, "", TRUE )
{
    setCaption( tr( "Copy or Move a File" ) );
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setSpacing( 5 );
    layout->setMargin( 5 );

    animation = new QFDProgressAnimation( this );
    layout->addWidget( animation );

    layout->addWidget( new QLabel( tr( "Read: %1" ).arg( fn ), this ) );
    readBar = new QProgressBar( steps, this );
    readBar->reset();
    readBar->setProgress( 0 );
    layout->addWidget( readBar );
    writeLabel = new QLabel( tr( "Write: %1" ).arg( QString::null ), this );
    layout->addWidget( writeLabel );
    writeBar = new QProgressBar( steps, this );
    writeBar->reset();
    writeBar->setProgress( 0 );
    layout->addWidget( writeBar );

    QPushButton *b = new QPushButton( tr( "&Cancel" ), this );
    b->setFixedSize( b->sizeHint() );
    layout->addWidget( b );
    connect( b, SIGNAL( clicked() ),
	     this, SIGNAL( cancelled() ) );

    animation->start();
}

void QFDProgressDialog::setReadProgress( int p )
{
    readBar->setProgress( p );
}

void QFDProgressDialog::setWriteProgress( int p )
{
    writeBar->setProgress( p );
}

void QFDProgressDialog::setWriteLabel( const QString &s )
{
    writeLabel->setText( tr( "Write: %1" ).arg( s ) );
}

/************************************************************************
 *
 * Private QFileDialog members
 *
 ************************************************************************/

#if !defined(_CC_SUN_) // Work around bug 4328291/4325168 (Sun WorkShop 5.0)
inline
#endif
int sun4325168Workaround( QCollection::Item n1, QCollection::Item n2 ) {
    if ( !n1 || !n2 )
	return 0;

    QUrlInfo *i1 = ( QUrlInfo *)n1;
    QUrlInfo *i2 = ( QUrlInfo *)n2;

    if ( i1->isDir() && !i2->isDir() )
	return -1;
    if ( !i1->isDir() && i2->isDir() )
	return 1;

    if ( i1->name() == ".." )
	return -1;
    if ( i2->name() == ".." )
	return 1;

#if defined(_OS_WIN32_)
    if ( sortFilesBy == QDir::Name ) {
	QString name1 = i1->name().lower();
	QString name2 = i2->name().lower();
	return name1.compare( name2 );
    }
#endif
    if ( QUrlInfo::equal( *i1, *i2, sortFilesBy ) )
	return 0;
    else if ( QUrlInfo::greaterThan( *i1, *i2, sortFilesBy ) )
	return 1;
    else if ( QUrlInfo::lessThan( *i1, *i2, sortFilesBy ) )
	return -1;
    // can't happen...
    return 0;
}

struct QFileDialogPrivate {
    ~QFileDialogPrivate();

    QStringList history;

    bool geometryDirty;
    QComboBox * paths;
    QComboBox * types;
    QLabel * pathL;
    QLabel * fileL;
    QLabel * typeL;

    QVBoxLayout * topLevelLayout;
    QHBoxLayout *buttonLayout, *leftLayout, *rightLayout;
    QList<QHBoxLayout> extraWidgetsLayouts;
    QList<QLabel> extraLabels;
    QList<QWidget> extraWidgets;
    QList<QWidget> extraButtons;
    QList<QButton> toolButtons;

    QWidgetStack * stack;

    QToolButton * cdToParent, *newFolder, * detailView, * mcView,
	*previewInfo, *previewContents, *goBack;
    QButtonGroup * modeButtons;

    QString currentFileName;
    QListViewItem *last;

    struct File: public QListViewItem {
	File( QFileDialogPrivate * dlgp,
	      const QUrlInfo * fi, QListViewItem * parent )
	    : QListViewItem( parent, dlgp->last ), info( *fi ), d(dlgp), i( 0 ), hasMimePixmap( FALSE )
	{ setup(); dlgp->last = this; }
	File( QFileDialogPrivate * dlgp,
	      const QUrlInfo * fi, QListView * parent )
	    : QListViewItem( parent, dlgp->last ), info( *fi ), d(dlgp), i( 0 ), hasMimePixmap( FALSE )
	{ setup(); dlgp->last = this; }
	File( QFileDialogPrivate * dlgp,
	      const QUrlInfo * fi, QListView * parent, QListViewItem * after )
	    : QListViewItem( parent, after ), info( *fi ), d(dlgp), i( 0 ), hasMimePixmap( FALSE )
	{ setup(); if ( !nextSibling() ) dlgp->last = this; }
	~File();

	QString text( int column ) const;
	const QPixmap * pixmap( int ) const;

	QUrlInfo info;
	QFileDialogPrivate * d;
	QListBoxItem *i;
	bool hasMimePixmap;
    };

    class MCItem: public QListBoxItem {
    public:
	MCItem( QListBox *, QListViewItem * item );
	MCItem( QListBox *, QListViewItem * item, QListBoxItem *after );
	QString text() const;
	const QPixmap *pixmap() const;
	int height( const QListBox * ) const;
	int width( const QListBox * ) const;
	void paint( QPainter * );
	QListViewItem * i;
    };

    class UrlInfoList : public QList<QUrlInfo> {
    public:
	UrlInfoList() { setAutoDelete( TRUE ); }
	int compareItems( QCollection::Item n1, QCollection::Item n2 ) {
	    return sun4325168Workaround( n1, n2 );
	}
	QUrlInfo *operator[]( int i ) {
	    return at( i );
	}
    };

    UrlInfoList sortedList;
    QList<File> pendingItems;

    QFileListBox * moreFiles;

    QFileDialog::Mode mode;

    QString rw;
    QString ro;
    QString wo;
    QString inaccessible;

    QString symLinkToFile;
    QString file;
    QString symLinkToDir;
    QString dir;
    QString symLinkToSpecial;
    QString special;
    QWidgetStack *preview;
    bool infoPreview, contentsPreview;
    QSplitter *splitter;
    QUrlOperator url, oldUrl;
    QWidget *infoPreviewWidget, *contentsPreviewWidget;
    QFilePreview *infoPreviewer, *contentsPreviewer;
    bool hadDotDot;

    bool ignoreNextKeyPress;
    QFDProgressDialog *progressDia;
    bool checkForFilter;
    bool ignoreReturn;
    bool ignoreStop;

    QTimer *mimeTypeTimer;
    const QNetworkOperation *currListChildren;

};

QFileDialogPrivate::~QFileDialogPrivate()
{
    delete modeButtons;
}



/************************************************************************
 *
 * Internal class QRenameEdit
 *
 ************************************************************************/

void QRenameEdit::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Escape )
	emit escapePressed();
    else
	QLineEdit::keyPressEvent( e );
    e->accept();
}

void QRenameEdit::focusOutEvent( QFocusEvent * )
{
    emit escapePressed();
}

/************************************************************************
 *
 * Internal class QFileListBox
 *
 ************************************************************************/

QFileListBox::QFileListBox( QWidget *parent, QFileDialog *dlg )
    : QListBox( parent, "filelistbox" ), filedialog( dlg ),
      renaming( FALSE ), renameItem( 0 ), mousePressed( FALSE ),
      firstMousePressEvent( TRUE )
{
    changeDirTimer = new QTimer( this );
    QVBox *box = new QVBox( viewport() );
    box->setFrameStyle( QFrame::Box | QFrame::Plain );
    lined = new QRenameEdit( box );
    lined->setFixedHeight( lined->sizeHint().height() );
    box->hide();
    box->setBackgroundMode( PaletteBase );
    renameTimer = new QTimer( this );
    connect( lined, SIGNAL( returnPressed() ),
	     this, SLOT (rename() ) );
    connect( lined, SIGNAL( escapePressed() ),
	     this, SLOT( cancelRename() ) );
    connect( renameTimer, SIGNAL( timeout() ),
	     this, SLOT( doubleClickTimeout() ) );
    connect( changeDirTimer, SIGNAL( timeout() ),
	     this, SLOT( changeDirDuringDrag() ) );
    connect( this, SIGNAL( contentsMoving( int, int ) ),
	     this, SLOT( contentsMoved( int, int ) ) );
    viewport()->setAcceptDrops( TRUE );
    dragItem = 0;
}

void QFileListBox::show()
{
    setBackgroundMode( PaletteBase );
    viewport()->setBackgroundMode( PaletteBase );
    QListBox::show();
}

void QFileListBox::keyPressEvent( QKeyEvent *e )
{
    if ( ( e->key() == Key_Enter ||
	   e->key() == Key_Return ) &&
	 renaming )
	return;

    cancelRename();
    QListBox::keyPressEvent( e );
}

void QFileListBox::viewportMousePressEvent( QMouseEvent *e )
{
    pressPos = e->pos();
    mousePressed = FALSE;

    bool didRename = renaming;

    cancelRename();
    if ( !hasFocus() && !viewport()->hasFocus() )
	setFocus();

    if ( e->button() != LeftButton ) {
	QListBox::viewportMousePressEvent( e );
	firstMousePressEvent = FALSE;
	return;
    }

    int i = currentItem();
    bool wasSelected = FALSE;
    if ( i != -1 )
	wasSelected = item( i )->selected();
    QListBox::viewportMousePressEvent( e );

    QFileDialogPrivate::MCItem *i1 = (QFileDialogPrivate::MCItem*)item( currentItem() );
    if ( i1 )
	mousePressed = !( (QFileDialogPrivate::File*)i1->i )->info.isDir();

    if ( itemAt( e->pos() ) != item( i ) ) {
	firstMousePressEvent = FALSE;
	return;
    }

     if ( !firstMousePressEvent && !didRename && i == currentItem() && currentItem() != -1 &&
	 wasSelected && filedialog->mode() != QFileDialog::ExistingFiles &&
	 QUrlInfo( filedialog->d->url, "." ).isWritable() && item( currentItem() )->text() != ".." ) {
	renameTimer->start( QApplication::doubleClickInterval(), TRUE );
	renameItem = item( i );
    }

    firstMousePressEvent = FALSE;
}

void QFileListBox::viewportMouseReleaseEvent( QMouseEvent *e )
{
    dragItem = 0;
    QListBox::viewportMouseReleaseEvent( e );
    mousePressed = FALSE;
}

void QFileListBox::viewportMouseDoubleClickEvent( QMouseEvent *e )
{
    renameTimer->stop();
    QListBox::viewportMouseDoubleClickEvent( e );
}

void QFileListBox::viewportMouseMoveEvent( QMouseEvent *e )
{
    if ( !dragItem )
	dragItem = itemAt( e->pos() );
    renameTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    if (  ( pressPos - e->pos() ).manhattanLength() > QApplication::startDragDistance() && mousePressed ) {
	QListBoxItem *item = dragItem;
	dragItem = 0;
	if ( item ) {
	    if ( !itemRect( item ).contains( e->pos() ) )
		return;
	    QUriDrag* drag = new QUriDrag( viewport() );
	    drag->setUnicodeUris( filedialog->selectedFiles() );

	    if ( lined->parentWidget()->isVisible() )
		cancelRename();

	    connect( drag, SIGNAL( destroyed() ),
		     this, SLOT( dragObjDestroyed() ) );
	    drag->drag();

	    mousePressed = FALSE;
	}
    } else
#endif
    {
	QListBox::viewportMouseMoveEvent( e );
    }

}

void QFileListBox::dragObjDestroyed()
{
#ifndef QT_NO_DRAGANDDROP
    //#######
    //filedialog->rereadDir();
#endif
}

#ifndef QT_NO_DRAGANDDROP
void QFileListBox::viewportDragEnterEvent( QDragEnterEvent *e )
{
    startDragUrl = filedialog->d->url;
    startDragDir = filedialog->dirPath();
    currDropItem = 0;

    if ( !QUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    QStringList l;
    QUriDrag::decodeLocalFiles( e, l );
    urls = l.count();

    if ( acceptDrop( e->pos(), e->source() ) ) {
	e->accept();
	setCurrentDropItem( e->pos() );
    } else {
	e->ignore();
	setCurrentDropItem( QPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void QFileListBox::viewportDragMoveEvent( QDragMoveEvent *e )
{
    if ( acceptDrop( e->pos(), e->source() ) ) {
	switch ( e->action() ) {
	case QDropEvent::Copy:
	    e->acceptAction();
	    break;
	case QDropEvent::Move:
	    e->acceptAction();
	    break;
	case QDropEvent::Link:
	    break;
	default:
	    break;
	}
	if ( oldDragPos != e->pos() )
	    setCurrentDropItem( e->pos() );
    } else {
	changeDirTimer->stop();
	e->ignore();
	setCurrentDropItem( QPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void QFileListBox::viewportDragLeaveEvent( QDragLeaveEvent * )
{
    changeDirTimer->stop();
    setCurrentDropItem( QPoint( -1, -1 ) );
//########
//     if ( startDragDir != filedialog->d->url )
// 	filedialog->setUrl( startDragUrl );
}

void QFileListBox::viewportDropEvent( QDropEvent *e )
{
    changeDirTimer->stop();

    if ( !QUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    QStrList l;
    QUrlDrag::decode( e, l );

    bool move = e->action() == QDropEvent::Move;
//     bool supportAction = move || e->action() == QDropEvent::Copy;

    QUrlOperator dest;
    if ( currDropItem )
	dest = QUrlOperator( filedialog->d->url, currDropItem->text() );
    else
	dest = filedialog->d->url;
    QStringList lst;
    for ( uint i = 0; i < l.count(); ++i ) {
	lst << l.at( i );
    }

    filedialog->d->url.copy( lst, dest, move );

    // ##### what is supportAction for?
    e->acceptAction();
    currDropItem = 0;
}

bool QFileListBox::acceptDrop( const QPoint &pnt, QWidget *source )
{
    QListBoxItem *item = itemAt( pnt );
    if ( !item || item && !itemRect( item ).contains( pnt ) ) {
	if ( source == viewport() && startDragDir == filedialog->dirPath() )
	    return FALSE;
	return TRUE;
    }

    QUrlInfo fi( filedialog->d->url, item->text() );

    if ( fi.isDir() && itemRect( item ).contains( pnt ) )
	return TRUE;
    return FALSE;
}

void QFileListBox::setCurrentDropItem( const QPoint &pnt )
{
    changeDirTimer->stop();

    QListBoxItem *item = itemAt( pnt );
    if ( pnt == QPoint( -1, -1 ) )
	item = 0;
    if ( item && !QUrlInfo( filedialog->d->url, item->text() ).isDir() )
	item = 0;

    if ( item && !itemRect( item ).contains( pnt ) )
	item = 0;

    currDropItem = item;
    if ( currDropItem )
	setCurrentItem( currDropItem );
    changeDirTimer->start( 750 );
}
#endif // QT_NO_DRAGANDDROP

void QFileListBox::changeDirDuringDrag()
{
#ifndef QT_NO_DRAGANDDROP
    if ( !currDropItem )
	return;
    changeDirTimer->stop();
    QUrl u( filedialog->d->url, currDropItem->text() );
    filedialog->setDir( u );
    currDropItem = 0;
#endif
}

void QFileListBox::doubleClickTimeout()
{
    startRename();
    renameTimer->stop();
}

void QFileListBox::startRename( bool check )
{
    if ( check && ( !renameItem || renameItem != item( currentItem() ) ) )
	return;

    int i = currentItem();
    setSelected( i, TRUE );
    QRect r = itemRect( item( i ) );
    int bdr = item( i )->pixmap() ?
	      item( i )->pixmap()->width() : 16;
    int x = r.x() + bdr;
    int y = r.y();
    int w = item( i )->width( this ) - bdr;
    int h = QMAX( lined->height() + 2, r.height() );
    y = y + r.height() / 2 - h / 2;

    lined->parentWidget()->setGeometry( x, y, w + 6, h );
    lined->setFocus();
    lined->setText( item( i )->text() );
    lined->selectAll();
    lined->setFrame( FALSE );
    lined->parentWidget()->show();
    viewport()->setFocusProxy( lined );
    renaming = TRUE;
}

void QFileListBox::clear()
{
    cancelRename();
    QListBox::clear();
}

void QFileListBox::rename()
{
    if ( !lined->text().isEmpty() ) {
	QString file = currentText();

	if ( lined->text() != file )
	    filedialog->d->url.rename( file, lined->text() );
    }
    cancelRename();
}

void QFileListBox::cancelRename()
{
    renameItem = 0;
    lined->parentWidget()->hide();
    viewport()->setFocusProxy( this );
    renaming = FALSE;
    updateItem( currentItem() );
    if ( lined->hasFocus() )
	viewport()->setFocus();
}

void QFileListBox::contentsMoved( int, int )
{
    changeDirTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    setCurrentDropItem( QPoint( -1, -1 ) );
#endif
}

/************************************************************************
 *
 * Internal class QFileListView
 *
 ************************************************************************/

QFileListView::QFileListView( QWidget *parent, QFileDialog *dlg )
    : QListView( parent ), filedialog( dlg ), renaming( FALSE ),
      renameItem( 0 ), mousePressed( FALSE ),
      firstMousePressEvent( TRUE )
{
    changeDirTimer = new QTimer( this );
    QVBox *box = new QVBox( viewport() );
    box->setFrameStyle( QFrame::Box | QFrame::Plain );
    lined = new QRenameEdit( box );
    lined->setFixedHeight( lined->sizeHint().height() );
    box->hide();
    box->setBackgroundMode( PaletteBase );
    renameTimer = new QTimer( this );
    connect( lined, SIGNAL( returnPressed() ),
	     this, SLOT (rename() ) );
    connect( lined, SIGNAL( escapePressed() ),
	     this, SLOT( cancelRename() ) );
    header()->setMovingEnabled( FALSE );
    connect( renameTimer, SIGNAL( timeout() ),
	     this, SLOT( doubleClickTimeout() ) );
    connect( changeDirTimer, SIGNAL( timeout() ),
	     this, SLOT( changeDirDuringDrag() ) );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
		this, SLOT( changeSortColumn( int ) ) );
    connect( header(), SIGNAL( sectionClicked( int ) ),
	     this, SLOT( changeSortColumn2( int ) ) );
    connect( this, SIGNAL( contentsMoving( int, int ) ),
	     this, SLOT( contentsMoved( int, int ) ) );

    viewport()->setAcceptDrops( TRUE );
    sortcolumn = 0;
    ascending = TRUE;
    dragItem = 0;
}

void QFileListView::setSorting( int column, bool increasing )
{
    if ( column == -1 ) {
	QListView::setSorting( column, increasing );
	return;
    }

    sortAscending = ascending = increasing;
    sortcolumn = column;
    switch ( column ) {
    case 0:
	sortFilesBy = QDir::Name;
	break;
    case 1:
	sortFilesBy = QDir::Size;
	break;
    case 3:
	sortFilesBy = QDir::Time;
	break;
    default:
	sortFilesBy = QDir::Name; // #### ???
	break;
    }

    filedialog->resortDir();
}

void QFileListView::changeSortColumn2( int column )
{
    int lcol = header()->mapToLogical( column );
    setSorting( lcol, sortcolumn == lcol ? !ascending : TRUE );
}

void QFileListView::keyPressEvent( QKeyEvent *e )
{
    if ( ( e->key() == Key_Enter ||
	   e->key() == Key_Return ) &&
	 renaming )
	return;

    cancelRename();
    QListView::keyPressEvent( e );
}

void QFileListView::viewportMousePressEvent( QMouseEvent *e )
{
    pressPos = e->pos();
    mousePressed = FALSE;

    bool didRename = renaming;
    cancelRename();
    if ( !hasFocus() && !viewport()->hasFocus() )
	setFocus();

    if ( e->button() != LeftButton ) {
	QListView::viewportMousePressEvent( e );
	firstMousePressEvent = FALSE;
	return;
    }

    QListViewItem *i = currentItem();
    QListView::viewportMousePressEvent( e );

    QFileDialogPrivate::File *i1 = (QFileDialogPrivate::File*)currentItem();
    if ( i1 )
	mousePressed = !i1->info.isDir();

    if ( itemAt( e->pos() ) != i ||
	 e->x() + contentsX() > columnWidth( 0 ) ) {
	firstMousePressEvent = FALSE;
	return;
    }

    if ( !firstMousePressEvent && !didRename && i == currentItem() && currentItem() &&
 	 filedialog->mode() != QFileDialog::ExistingFiles &&
	 QUrlInfo( filedialog->d->url, "." ).isWritable() && currentItem()->text( 0 ) != ".." ) {
 	renameTimer->start( QApplication::doubleClickInterval(), TRUE );
 	renameItem = currentItem();
    }

    firstMousePressEvent = FALSE;
}

void QFileListView::viewportMouseDoubleClickEvent( QMouseEvent *e )
{
    renameTimer->stop();
    QListView::viewportMouseDoubleClickEvent( e );
}

void QFileListView::viewportMouseReleaseEvent( QMouseEvent *e )
{
    QListView::viewportMouseReleaseEvent( e );
    mousePressed = FALSE;
    dragItem = 0;
}

void QFileListView::viewportMouseMoveEvent( QMouseEvent *e )
{
    renameTimer->stop();
    if ( !dragItem )
	dragItem = itemAt( e->pos() );
#ifndef QT_NO_DRAGANDDROP
    if (  ( pressPos - e->pos() ).manhattanLength() > QApplication::startDragDistance() && mousePressed ) {
	QListViewItem *item = dragItem;
	dragItem = 0;
	if ( item ) {
	    QUriDrag* drag = new QUriDrag( viewport() );
	    drag->setUnicodeUris( filedialog->selectedFiles() );

	    if ( lined->isVisible() )
		cancelRename();

	    connect( drag, SIGNAL( destroyed() ),
		     this, SLOT( dragObjDestroyed() ) );
	    drag->drag();

	    mousePressed = FALSE;
	}
    }
#endif
}

void QFileListView::dragObjDestroyed()
{
#ifndef QT_NO_DRAGANDDROP
    //######
    //filedialog->rereadDir();
#endif
}

#ifndef QT_NO_DRAGANDDROP
void QFileListView::viewportDragEnterEvent( QDragEnterEvent *e )
{
    startDragUrl = filedialog->d->url;
    startDragDir = filedialog->dirPath();
    currDropItem = 0;

    if ( !QUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    QStringList l;
    QUriDrag::decodeLocalFiles( e, l );
    urls = l.count();

    if ( acceptDrop( e->pos(), e->source() ) ) {
	e->accept();
	setCurrentDropItem( e->pos() );
    } else {
	e->ignore();
	setCurrentDropItem( QPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void QFileListView::viewportDragMoveEvent( QDragMoveEvent *e )
{
    if ( acceptDrop( e->pos(), e->source() ) ) {
	if ( oldDragPos != e->pos() )
	    setCurrentDropItem( e->pos() );
	switch ( e->action() ) {
	case QDropEvent::Copy:
	    e->acceptAction();
	    break;
	case QDropEvent::Move:
	    e->acceptAction();
	    break;
	case QDropEvent::Link:
	    break;
	default:
	    break;
	}
    } else {
	changeDirTimer->stop();
	e->ignore();
	setCurrentDropItem( QPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void QFileListView::viewportDragLeaveEvent( QDragLeaveEvent * )
{
    changeDirTimer->stop();
    setCurrentDropItem( QPoint( -1, -1 ) );
//########
//     if ( startDragDir != filedialog->d->url )
// 	filedialog->setUrl( startDragUrl );
}

void QFileListView::viewportDropEvent( QDropEvent *e )
{
    changeDirTimer->stop();

    if ( !QUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    QStringList l;
    QUrlDrag::decodeToUnicodeUris( e, l );

    bool move = e->action() == QDropEvent::Move;
//     bool supportAction = move || e->action() == QDropEvent::Copy;

    QUrlOperator dest;
    if ( currDropItem )
	dest = QUrlOperator( filedialog->d->url, currDropItem->text( 0 ) );
    else
	dest = filedialog->d->url;
    filedialog->d->url.copy( l, dest, move );

    // ##### what is supportAction for?
    e->acceptAction();
    currDropItem = 0;
}

bool QFileListView::acceptDrop( const QPoint &pnt, QWidget *source )
{
    QListViewItem *item = itemAt( pnt );
    if ( !item || item && !itemRect( item ).contains( pnt ) ) {
	if ( source == viewport() && startDragDir == filedialog->dirPath() )
	    return FALSE;
	return TRUE;
    }

    QUrlInfo fi( filedialog->d->url, item->text( 0 ) );

    if ( fi.isDir() && itemRect( item ).contains( pnt ) )
	return TRUE;
    return FALSE;
}

void QFileListView::setCurrentDropItem( const QPoint &pnt )
{
    changeDirTimer->stop();

    QListViewItem *item = itemAt( pnt );
    if ( pnt == QPoint( -1, -1 ) )
	item = 0;
    if ( item && !QUrlInfo( filedialog->d->url, item->text( 0 ) ).isDir() )
	item = 0;

    if ( item && !itemRect( item ).contains( pnt ) )
	item = 0;

    currDropItem = item;

    if ( currDropItem )
	setCurrentItem( currDropItem );

    changeDirTimer->start( 750 );
}
#endif // QT_NO_DRAGANDDROP

void QFileListView::changeDirDuringDrag()
{
#ifndef QT_NO_DRAGANDDROP
    if ( !currDropItem )
	return;
    changeDirTimer->stop();
    QUrl u( filedialog->d->url, currDropItem->text( 0 ) );
    filedialog->setDir( u );
    currDropItem = 0;
#endif // QT_NO_DRAGANDDROP
}


void QFileListView::doubleClickTimeout()
{
    startRename();
    renameTimer->stop();
}

void QFileListView::startRename( bool check )
{
    if ( check && ( !renameItem || renameItem != currentItem() ) )
	return;

    QListViewItem *i = currentItem();
    setSelected( i, TRUE );

    QRect r = itemRect( i );
    int bdr = i->pixmap( 0 ) ?
	      i->pixmap( 0 )->width() : 16;
    int x = r.x() + bdr;
    int y = r.y();
    int w = columnWidth( 0 ) - bdr;
    int h = QMAX( lined->height() + 2, r.height() );
    y = y + r.height() / 2 - h / 2;

    lined->parentWidget()->setGeometry( x, y, w + 6, h );
    lined->setFocus();
    lined->setText( i->text( 0 ) );
    lined->selectAll();
    lined->setFrame( FALSE );
    lined->parentWidget()->show();
    viewport()->setFocusProxy( lined );
    renaming = TRUE;
}

void QFileListView::clear()
{
    cancelRename();
    QListView::clear();
}

void QFileListView::rename()
{
    if ( !lined->text().isEmpty() ) {
	QString file = currentItem()->text( 0 );

	if ( lined->text() != file )
	    filedialog->d->url.rename( file, lined->text() );
    }
    cancelRename();
}

void QFileListView::cancelRename()
{
    renameItem = 0;
    lined->parentWidget()->hide();
    viewport()->setFocusProxy( this );
    renaming = FALSE;
    if ( currentItem() )
	currentItem()->repaint();
    if ( lined->hasFocus() )
	viewport()->setFocus();
}

void QFileListView::contentsMoved( int, int )
{
    changeDirTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    setCurrentDropItem( QPoint( -1, -1 ) );
#endif
}


QFileDialogPrivate::File::~File()
{
    if ( d->pendingItems.findRef( this ) )
	d->pendingItems.removeRef( this );
}

QString QFileDialogPrivate::File::text( int column ) const
{
    makeVariables();

    switch( column ) {
    case 0:
	return info.name();
    case 1:
	if ( info.isFile() )
	    return QString::number(info.size());
	else
	    return QString::fromLatin1("");
    case 2:
	if ( info.isFile() && info.isSymLink() ) {
	    return d->symLinkToFile;
	} else if ( info.isFile() ) {
	    return d->file;
	} else if ( info.isDir() && info.isSymLink() ) {
	    return d->symLinkToDir;
	} else if ( info.isDir() ) {
	    return d->dir;
	} else if ( info.isSymLink() ) {
	    return d->symLinkToSpecial;
	} else {
	    return d->special;
	}
    case 3: {
	QDateTime epoch;
	epoch.setTime_t( 0 );
	char a[256];
	time_t t1 = epoch.secsTo( info.lastModified() );
	struct tm * t2 = ::localtime( &t1 );
	// looks wrong for the last hour of the day...
	if ( t2 && t2->tm_hour != info.lastModified().time().hour() )
	    t2->tm_hour = info.lastModified().time().hour();
	// use a static const char here, so that egcs will not see
	// the formatting string and give an incorrect warning.
	if ( t2 && strftime( a, 255, egcsWorkaround, t2 ) > 0 )
	    return QString::fromLocal8Bit(a);
	else
	    return QString::fromLatin1("????");
    }
    case 4:
	if ( info.isReadable() )
	    return info.isWritable() ? d->rw : d->ro;
	else
	    return info.isWritable() ? d->wo : d->inaccessible;
    }

    return QString::fromLatin1("<--->");
}

const QPixmap * QFileDialogPrivate::File::pixmap( int column ) const
{
    if ( column ) {
	return 0;
    } else if ( QListViewItem::pixmap( column ) ) {
	return QListViewItem::pixmap( column );
    } else if ( info.isSymLink() ) {
	if ( info.isFile() )
	    return symLinkFileIcon;
	else
	    return symLinkDirIcon;
    } else if ( info.isDir() ) {
	return closedFolderIcon;
    } else if ( info.isFile() ) {
	return fileIcon;
    } else {
	return fifteenTransparentPixels;
    }
}

QFileDialogPrivate::MCItem::MCItem( QListBox * lb, QListViewItem * item )
    : QListBoxItem()
{
    i = item;
    if ( lb )
	lb->insertItem( this );
}

QFileDialogPrivate::MCItem::MCItem( QListBox * lb, QListViewItem * item, QListBoxItem *after )
    : QListBoxItem()
{
    i = item;
    if ( lb )
	lb->insertItem( this, after );
}

QString QFileDialogPrivate::MCItem::text() const
{
    return i->text( 0 );
}


const QPixmap *QFileDialogPrivate::MCItem::pixmap() const
{
    return i->pixmap( 0 );
}


int QFileDialogPrivate::MCItem::height( const QListBox * lb ) const
{
    if ( pixmap() )
	return QMAX( lb->fontMetrics().height(), pixmap()->height()) + 2;

    return lb->fontMetrics().height() + 2;
}


int QFileDialogPrivate::MCItem::width( const QListBox * lb ) const
{
    QFontMetrics fm = lb->fontMetrics();
    int w = 2;
    if ( pixmap() )
	w += pixmap()->width() + 4;
    else
	w += 18;
    w += fm.width( text() );
    w += -fm.minLeftBearing();
    w += -fm.minRightBearing();
    w += 6;
    return w;
}


void QFileDialogPrivate::MCItem::paint( QPainter * ptr )
{
    QFontMetrics fm = ptr->fontMetrics();

    int h;

    if ( pixmap() )
	h = QMAX( fm.height(), pixmap()->height()) + 2;
    else
	h = fm.height() + 2;

    const QPixmap * pm = pixmap();
    if ( pm )
	ptr->drawPixmap( 2, 1, *pm );

    ptr->drawText( pm ? pm->width() + 4 : 22, h - fm.descent() - 2,
		   text() );
}

static QStringList makeFiltersList( const QString &filter )
{
    if ( filter.isEmpty() )
	return QStringList();

    int i = filter.find( ";;", 0 );
    QString sep( ";;" );
    if ( i == -1 ) {
	if ( filter.find( "\n", 0 ) != -1 ) {
	    sep = "\n";
	    i = filter.find( sep, 0 );
	}
    }

    return QStringList::split( sep, filter );
}

// NOT REVISED
/*!
  \class QFileDialog qfiledialog.h
  \brief The QFileDialog class provides a dialog widget for inputting file names.
  \ingroup dialogs

  This class implements a dialog which can be used if the user should select
  a file or a directory.

  Example (e.g. to get a filename for saving a file):

  \code
    QString fileName = QFileDialog::getSaveFileName( "newfile.txt", "Textfiles (*.txt)", this );
    if ( !fileName.isNull() ) {			// got a file name
	...
    }
  \endcode

  To let the user specify a filename for e.g. opening a file, you could use following
  code:

  \code
    QString s( QFileDialog::getOpenFileName( QString::null, "Images (*.png *.xpm *.jpg)", this ) );
    if ( s.isEmpty() )
	return;

    open( s ); // open() being your function to read the file
  \endcode

  Other convenient static methods are QFileDialog::getExistingDirectory() to let the user
  choose a directory or QFileDialog::getOpenFileNames() to let the user select multiple
  files.

  Additionally to these convenient static methods you can use one of QFileDialog's
  constructors, set a mode (see setMode()) and do more things, like adding a preview
  widget which will preview the current file or information of the current file while
  the user does the selection (see setInfoPreview(), setContentsPreview(), setInfoPreviewEnabled() and
  setContentsPreviewEnabled()) or add additional widgets to the filedialog then
  (see addWidgets(), addToolButton(), addLeftWidget() and addRightWidget()).

  To get the selection the user did then, see selectedFile(), selectedFiles(), selectedFilter()
  and url(). To set these things see setUrl() and setSelection().

  For an example about how to use this customization of the QFileDialog, take a look
  at the qdir example (qt/examples/qdir/qdir.cpp)

  <img src=qfiledlg-m.png> <img src=qfiledlg-w.png>

  \sa QPrintDialog
*/


/*! \enum QFileDialog::Mode

  This enum type is used to set and read QFileDialog's operating mode.
  The defined values are:
  <ul>
  <li> \c AnyFile - Return the name of any file, whether existing or not.
  <li> \c ExistingFile - Return the name of a single, existing, file.
  <li> \c Directory - Return the name of a directory.
  <li> \c DirectoryOnly - Return the name of a directory and display no files in the file views of the filedialog.
  <li> \c ExistingFiles - Return the names of zero or more existing files.
  </ul>

  Using setMode() you can set this mode to the file dialog.
*/

/*!
  \enum QFileDialog::ViewMode

  This enum type describes the view mode of the filedialog.
  <ul>
  <li> \c Detail - View which shows except the filename also
  size, date, etc. of a file in columns
  <li> \c List - Simple view which shows only all filenames plus icons
  </ul>

  Using setViewMode() you can set this mode to the file dialog.
*/

/*!
  \enum QFileDialog::PreviewMode

  This enum type describes the preview mode of the filedialog.
  <ul>
  <li> \c NoPreview - No preview is shown at all
  <li> \c Contents - Besides the view with the files a preview
  widget is shown which shows the contents of the currently selected file
  <li> \c Info - Besides the view with the files a preview
  widget is shown which shows infos of the currently selected file
  </ul>

  Using setPreviewMode() this mode can be set to the file dialog.
*/

/*!
  \fn void QFileDialog::detailViewSelectionChanged()
  \internal
*/

/*!
  \fn void QFileDialog::listBoxSelectionChanged()
  \internal
*/

/*!
  Constructs a file dialog with a \e parent, \e name and \e modal flag.

  The dialog becomes modal if \e modal is TRUE, otherwise modeless.
*/

QFileDialog::QFileDialog( QWidget *parent, const char *name, bool modal )
    : QDialog( parent, name, modal )
{
    init();
    d->mode = ExistingFile;
    d->types->insertItem( QFileDialog::tr( "All files (*)" ) );
    emit dirEntered( d->url.dirPath() );
    rereadDir();
}


/*!
  Constructs a file dialog with a \e parent, \e name and \e modal flag.

  The dialog becomes modal if \e modal is TRUE, otherwise modeless.
*/

QFileDialog::QFileDialog( const QString& dirName, const QString & filter,
			  QWidget *parent, const char *name, bool modal )
    : QDialog( parent, name, modal )
{
    init();
    d->mode = ExistingFile;
    rereadDir();
    if ( !dirName.isEmpty() )
	setSelection( dirName );
    else if ( workingDirectory && !workingDirectory->isEmpty() )
	setDir( *workingDirectory );


    if ( !filter.isEmpty() ) {
	setFilters( filter );
    } else {
	d->types->insertItem( QFileDialog::tr( "All files (*)" ) );
    }
}


/*!
  \internal
  Initializes the file dialog.
*/

void QFileDialog::init()
{
    setSizeGripEnabled( TRUE );
    d = new QFileDialogPrivate();
    d->mode = AnyFile;
    d->last = 0;
    d->moreFiles = 0;
    d->infoPreview = FALSE;
    d->contentsPreview = FALSE;
    d->hadDotDot = FALSE;
    d->ignoreNextKeyPress = FALSE;
    d->progressDia = 0;
    d->checkForFilter = FALSE;
    d->ignoreReturn = FALSE;
    d->ignoreStop = FALSE;
    d->pendingItems.setAutoDelete( FALSE );
    d->mimeTypeTimer = new QTimer( this );
    connect( d->mimeTypeTimer, SIGNAL( timeout() ),
	     this, SLOT( doMimeTypeLookup() ) );

    d->url = QUrlOperator( QDir::currentDirPath() );
    d->oldUrl = d->url;
    d->currListChildren = 0;

    connect( &d->url, SIGNAL( start( QNetworkOperation * ) ),
             this, SLOT( urlStart( QNetworkOperation * ) ) );
    connect( &d->url, SIGNAL( finished( QNetworkOperation * ) ),
             this, SLOT( urlFinished( QNetworkOperation * ) ) );
    connect( &d->url, SIGNAL( newChildren( const QValueList<QUrlInfo> &, QNetworkOperation * ) ),
             this, SLOT( insertEntry( const QValueList<QUrlInfo> &, QNetworkOperation * ) ) );
    connect( &d->url, SIGNAL( removed( QNetworkOperation * ) ),
             this, SLOT( removeEntry( QNetworkOperation * ) ) );
    connect( &d->url, SIGNAL( createdDirectory( const QUrlInfo &, QNetworkOperation * ) ),
             this, SLOT( createdDirectory( const QUrlInfo &, QNetworkOperation * ) ) );
    connect( &d->url, SIGNAL( itemChanged( QNetworkOperation * ) ),
             this, SLOT( itemChanged( QNetworkOperation * ) ) );
    connect( &d->url, SIGNAL( dataTransferProgress( int, int, QNetworkOperation * ) ),
             this, SLOT( dataTransferProgress( int, int, QNetworkOperation * ) ) );

    nameEdit = new QLineEdit( this, "name/filter editor" );
    nameEdit->setMaxLength( 255 ); //_POSIX_MAX_PATH
    connect( nameEdit, SIGNAL(textChanged(const QString&)),
	     this,  SLOT(fileNameEditDone()) );
    nameEdit->installEventFilter( this );

    d->splitter = new QSplitter( this );

    d->stack = new QWidgetStack( d->splitter, "files and more files" );

    files = new QFileListView( d->stack, this );
    QFontMetrics fm = fontMetrics();
    files->addColumn( tr("Name") );
    files->addColumn( tr("Size") );
    files->setColumnAlignment( 1, AlignRight );
    files->addColumn( tr("Type") );
    files->addColumn( tr("Date") );
    files->addColumn( tr("Attributes") );

    files->setMinimumSize( 50, 25 + 2*fm.lineSpacing() );

    connect( files, SIGNAL( selectionChanged() ),
	     this, SLOT( detailViewSelectionChanged() ) );
    connect( files, SIGNAL(currentChanged(QListViewItem *)),
	     this, SLOT(updateFileNameEdit(QListViewItem *)) );
    connect( files, SIGNAL(doubleClicked(QListViewItem *)),
	     this, SLOT(selectDirectoryOrFile(QListViewItem *)) );
    connect( files, SIGNAL(returnPressed(QListViewItem *)),
	     this, SLOT(selectDirectoryOrFile(QListViewItem *)) );
    connect( files, SIGNAL(rightButtonPressed(QListViewItem *,
					      const QPoint &, int)),
	     this, SLOT(popupContextMenu(QListViewItem *,
					 const QPoint &, int)) );

    files->installEventFilter( this );
    files->viewport()->installEventFilter( this );

    d->moreFiles = new QFileListBox( d->stack, this );
    d->moreFiles->setRowMode( QListBox::FitToHeight );
    d->moreFiles->setVariableWidth( TRUE );

    connect( d->moreFiles, SIGNAL(selected(QListBoxItem *)),
	     this, SLOT(selectDirectoryOrFile(QListBoxItem *)) );
    connect( d->moreFiles, SIGNAL( selectionChanged() ),
	     this, SLOT( listBoxSelectionChanged() ) );
    connect( d->moreFiles, SIGNAL(highlighted(QListBoxItem *)),
	     this, SLOT(updateFileNameEdit(QListBoxItem *)) );
    connect( d->moreFiles, SIGNAL( rightButtonPressed( QListBoxItem *, const QPoint & ) ),
	     this, SLOT( popupContextMenu( QListBoxItem *, const QPoint & ) ) );

    d->moreFiles->installEventFilter( this );
    d->moreFiles->viewport()->installEventFilter( this );

    okB = new QPushButton( tr("OK"), this, "OK" ); //### Or "Save (see other "OK")
    okB->setDefault( TRUE );
    okB->setEnabled( FALSE );
    connect( okB, SIGNAL(clicked()), this, SLOT(okClicked()) );
    cancelB = new QPushButton( tr("Cancel") , this, "Cancel" );
    connect( cancelB, SIGNAL(clicked()), this, SLOT(cancelClicked()) );

    d->paths = new QComboBox( TRUE, this, "directory history/editor" );
    d->paths->setDuplicatesEnabled( FALSE );
    d->paths->setInsertionPolicy( QComboBox::NoInsertion );
    const QFileInfoList * rootDrives = QDir::drives();
    QFileInfoListIterator it( *rootDrives );
    QFileInfo *fi;
    makeVariables();

    while ( (fi = it.current()) != 0 ) {
	++it;
	d->paths->insertItem( *openFolderIcon, fi->absFilePath() );
    }

    if ( !!QDir::homeDirPath() ) {
	if ( !d->paths->listBox()->findItem( QDir::homeDirPath() ) )
	    d->paths->insertItem( *openFolderIcon, QDir::homeDirPath() );
    }

    connect( d->paths, SIGNAL(activated(const QString&)),
	     this, SLOT(setDir(const QString&)) );

    d->paths->installEventFilter( this );
    QObjectList *ol = d->paths->queryList( "QLineEdit" );
    if ( ol && ol->first() )
	( (QLineEdit*)ol->first() )->installEventFilter( this );
    delete ol;

    d->geometryDirty = TRUE;
    d->types = new QComboBox( TRUE, this, "file types" );
    d->types->setDuplicatesEnabled( FALSE );
    d->types->setEditable( FALSE );
    connect( d->types, SIGNAL(activated(const QString&)),
	     this, SLOT(setFilter(const QString&)) );

    d->pathL = new QLabel( d->paths, tr("Look &in:"), this );
    d->fileL = new QLabel( nameEdit, tr("File &name:"), this );
    d->typeL = new QLabel( d->types, tr("File &type:"), this );

#if defined(_WS_WIN_)
    if ( QApplication::winVersion() == Qt::WV_2000 ) {
	d->goBack = new QToolButton( this, "go back" );
	d->goBack->setAutoRaise( TRUE );
	d->goBack->setEnabled( FALSE );
	d->goBack->setFocusPolicy( TabFocus );
	connect( d->goBack, SIGNAL( clicked() ),
		 this, SLOT( goBack() ) );
	QToolTip::add( d->goBack, tr( "Back" ) );
	d->goBack->setIconSet( *goBackIcon );
    } else {
	d->goBack = 0;
    }
#else
    d->goBack = 0;
#endif

    d->cdToParent = new QToolButton( this, "cd to parent" );
#if defined(_WS_WIN_)
    if ( QApplication::winVersion() == Qt::WV_2000 )
	d->cdToParent->setAutoRaise( TRUE );
#endif
    d->cdToParent->setFocusPolicy( TabFocus );
    QToolTip::add( d->cdToParent, tr( "One directory up" ) );
    d->cdToParent->setIconSet( *cdToParentIcon );
    connect( d->cdToParent, SIGNAL(clicked()),
	     this, SLOT(cdUpClicked()) );

    d->newFolder = new QToolButton( this, "new folder" );
#if defined(_WS_WIN_)
    if ( QApplication::winVersion() == Qt::WV_2000 )
	d->newFolder->setAutoRaise( TRUE );
#endif
    d->newFolder->setFocusPolicy( TabFocus );
    QToolTip::add( d->newFolder, tr( "Create New Folder" ) );
    d->newFolder->setIconSet( *newFolderIcon );
    connect( d->newFolder, SIGNAL(clicked()),
	     this, SLOT(newFolderClicked()) );

    d->modeButtons = new QButtonGroup( 0, "invisible group" );
    connect( d->modeButtons, SIGNAL(destroyed()),
	     this, SLOT(modeButtonsDestroyed()) );
    d->modeButtons->setExclusive( TRUE );
    connect( d->modeButtons, SIGNAL(clicked(int)),
	     d->stack, SLOT(raiseWidget(int)) );
    connect( d->modeButtons, SIGNAL(clicked(int)),
	     this, SLOT(changeMode(int)) );

    d->mcView = new QToolButton( this, "mclistbox view" );
#if defined(_WS_WIN_)
    if ( QApplication::winVersion() == Qt::WV_2000 )
	d->mcView->setAutoRaise( TRUE );
#endif
    d->mcView->setFocusPolicy( TabFocus );
    QToolTip::add( d->mcView, tr( "List View" ) );
    d->mcView->setIconSet( *multiColumnListViewIcon );
    d->mcView->setToggleButton( TRUE );
    d->stack->addWidget( d->moreFiles, d->modeButtons->insert( d->mcView ) );
    d->detailView = new QToolButton( this, "list view" );
#if defined(_WS_WIN_)
    if ( QApplication::winVersion() == Qt::WV_2000 )
	d->detailView->setAutoRaise( TRUE );
#endif
    d->detailView->setFocusPolicy( TabFocus );
    QToolTip::add( d->detailView, tr( "Detail View" ) );
    d->detailView->setIconSet( *detailViewIcon );
    d->detailView->setToggleButton( TRUE );
    d->stack->addWidget( files, d->modeButtons->insert( d->detailView ) );

    d->previewInfo = new QToolButton( this, "preview info view" );
#if defined(_WS_WIN_)
    if ( QApplication::winVersion() == Qt::WV_2000 )
	d->previewInfo->setAutoRaise( TRUE );
#endif
    d->previewInfo->setFocusPolicy( TabFocus );
    QToolTip::add( d->previewInfo, tr( "Preview File Info" ) );
    d->previewInfo->setIconSet( *previewInfoViewIcon );
    d->previewInfo->setToggleButton( TRUE );
    d->modeButtons->insert( d->previewInfo );

    d->previewContents = new QToolButton( this, "preview info view" );
#if defined(_WS_WIN_)
    if ( QApplication::winVersion() == Qt::WV_2000 )
	d->previewContents->setAutoRaise( TRUE );
#endif
    d->previewContents->setFocusPolicy( TabFocus );
    QToolTip::add( d->previewContents, tr( "Preview File Contents" ) );
    d->previewContents->setIconSet( *previewContentsViewIcon );
    d->previewContents->setToggleButton( TRUE );
    d->modeButtons->insert( d->previewContents );

    connect( d->detailView, SIGNAL( clicked() ),
	     d->moreFiles, SLOT( cancelRename() ) );
    connect( d->detailView, SIGNAL( clicked() ),
	     files, SLOT( cancelRename() ) );
    connect( d->mcView, SIGNAL( clicked() ),
	     d->moreFiles, SLOT( cancelRename() ) );
    connect( d->mcView, SIGNAL( clicked() ),
	     files, SLOT( cancelRename() ) );

    d->stack->raiseWidget( d->moreFiles );
    d->mcView->setOn( TRUE );

    QHBoxLayout *lay = new QHBoxLayout( this );
    lay->setMargin( 6 );
    d->leftLayout = new QHBoxLayout( lay, 5 );
    d->topLevelLayout = new QVBoxLayout( (QWidget*)0, 5 );
    lay->addLayout( d->topLevelLayout, 1 );
    d->extraWidgetsLayouts.setAutoDelete( FALSE );
    d->extraLabels.setAutoDelete( FALSE );
    d->extraWidgets.setAutoDelete( FALSE );
    d->extraButtons.setAutoDelete( FALSE );
    d->toolButtons.setAutoDelete( FALSE );

    QHBoxLayout * h;

    d->preview = new QWidgetStack( d->splitter );

    d->infoPreviewWidget = new QWidget( d->preview );
    d->contentsPreviewWidget = new QWidget( d->preview );
    d->infoPreviewer = d->contentsPreviewer = 0;

    h = new QHBoxLayout( 0 );
    d->buttonLayout = h;
    d->topLevelLayout->addLayout( h );
    h->addWidget( d->pathL );
    h->addSpacing( 8 );
    h->addWidget( d->paths );
    h->addSpacing( 8 );
    if ( d->goBack )
	h->addWidget( d->goBack );
    h->addWidget( d->cdToParent );
    h->addSpacing( 2 );
    h->addWidget( d->newFolder );
    h->addSpacing( 4 );
    h->addWidget( d->mcView );
    h->addWidget( d->detailView );
    h->addWidget( d->previewInfo );
    h->addWidget( d->previewContents );

    d->topLevelLayout->addWidget( d->splitter );

    h = new QHBoxLayout();
    d->topLevelLayout->addLayout( h );
    h->addWidget( d->fileL );
    h->addWidget( nameEdit );
    h->addSpacing( 15 );
    h->addWidget( okB );

    h = new QHBoxLayout();
    d->topLevelLayout->addLayout( h );
    h->addWidget( d->typeL );
    h->addWidget( d->types );
    h->addSpacing( 15 );
    h->addWidget( cancelB );

    d->rightLayout = new QHBoxLayout( lay, 5 );
    d->topLevelLayout->setStretchFactor( d->mcView, 1 );
    d->topLevelLayout->setStretchFactor( files, 1 );

    updateGeometries();

    if ( d->goBack ) {
	setTabOrder( d->paths, d->goBack );
	setTabOrder( d->goBack, d->cdToParent );
    } else {
	setTabOrder( d->paths, d->cdToParent );
    }
    setTabOrder( d->cdToParent, d->newFolder );
    setTabOrder( d->newFolder, d->mcView );
    setTabOrder( d->mcView, d->detailView );
    setTabOrder( d->detailView, d->moreFiles );
    setTabOrder( d->moreFiles, files );
    setTabOrder( files, nameEdit );
    setTabOrder( nameEdit, d->types );
    setTabOrder( d->types, okB );
    setTabOrder( okB, cancelB );

    setFontPropagation( SameFont );
    setPalettePropagation( SamePalette );

    d->rw = tr( "Read-write" );
    d->ro = tr( "Read-only" );
    d->wo = tr( "Write-only" );
    d->inaccessible = tr( "Inaccessible" );

    d->symLinkToFile = tr( "Symlink to File" );
    d->symLinkToDir = tr( "Symlink to Directory" );
    d->symLinkToSpecial = tr( "Symlink to Special" );
    d->file = tr( "File" );
    d->dir = tr( "Dir" );
    d->special = tr( "Special" );

    if ( !lastSize ) {
	if ( QApplication::desktop()->width() < 1024 ||
	     QApplication::desktop()->height() < 768 ) {
	    resize( QMIN(QApplication::desktop()->width(),420),
		    QMIN(QApplication::desktop()->height(),236) );
	} else {
	    QSize s( files->sizeHint() );
	    s = QSize( s.width() + 300, s.height() + 82 );

	    if ( s.width() * 3 > QApplication::desktop()->width() * 2 )
		s.setWidth( QApplication::desktop()->width() * 2 / 3 );

	    if ( s.height() * 3 > QApplication::desktop()->height() * 2 )
		s.setHeight( QApplication::desktop()->height() * 2 / 3 );
	    else if ( s.height() * 3 < QApplication::desktop()->height() )
		s.setHeight( QApplication::desktop()->height() / 3 );

	    resize( s );
	}
	lastSize = new QSize;
	*lastSize = size();
    } else
	resize( *lastSize );

    if ( detailViewMode ) {
	d->stack->raiseWidget( files );
	d->mcView->setOn( FALSE );
	d->detailView->setOn( TRUE );
    }

    d->preview->hide();
    nameEdit->setFocus();

    connect( nameEdit, SIGNAL( returnPressed() ),
	     this, SLOT( fileNameEditReturnPressed() ) );
}

/*!
  \internal
*/

void QFileDialog::fileNameEditReturnPressed()
{
    d->oldUrl = d->url;
    if ( !isDirectoryMode( d->mode ) ) {
	okClicked();
    } else {
	d->currentFileName = QString::null;
	if ( nameEdit->text().isEmpty() ) {
	    emit fileSelected( selectedFile() );
	    accept();
	} else {
	    QUrlInfo f;
	    QFileDialogPrivate::File * c
		= (QFileDialogPrivate::File *)files->currentItem();
	    if ( c && files->isSelected(c) )
		f = c->info;
	    else
		f = QUrlInfo( d->url, nameEdit->text() );
	    if ( f.isDir() ) {
		setUrl( QUrlOperator( d->url, nameEdit->text() + "/" ) );
		d->checkForFilter = TRUE;
		trySetSelection( TRUE, d->url, TRUE );
		d->checkForFilter = FALSE;
	    }
	}
	nameEdit->setText( QString::null );
	d->ignoreReturn = TRUE;
    }
}

/*!
  \internal
  Changes the preview mode.
*/

void QFileDialog::changeMode( int id )
{
    if ( !d->infoPreview && !d->contentsPreview )
	return;

    QButton *btn = (QButton*)d->modeButtons->find( id );
    if ( !btn )
	return;

    if ( btn == d->previewContents && !d->contentsPreview )
	return;
    if ( btn == d->previewInfo && !d->infoPreview )
	return;

    if ( btn != d->previewContents && btn != d->previewInfo ) {
	d->preview->hide();
    } else {
	if ( files->currentItem() ) {
	    if ( d->infoPreviewer )
		d->infoPreviewer->previewUrl( QUrl( d->url, files->currentItem()->text( 0 ) ) );
	    if ( d->contentsPreviewer )
		d->contentsPreviewer->previewUrl( QUrl( d->url, files->currentItem()->text( 0 ) ) );
	}
	if ( btn == d->previewInfo )
	    d->preview->raiseWidget( d->infoPreviewWidget );
	else
	    d->preview->raiseWidget( d->contentsPreviewWidget );
	d->preview->show();
    }
}

/*!
  Destructs the file dialog.
*/

QFileDialog::~QFileDialog()
{
    // since clear might call setContentsPos which would emit 
    // a signal and thus cause a recompute of sizes...
    files->blockSignals( TRUE );
    d->moreFiles->blockSignals( TRUE );
    files->clear();
    d->moreFiles->clear();
    d->moreFiles->blockSignals( FALSE );
    files->blockSignals( FALSE );
    delete d;
    d = 0;
}


/*!
  Returns the selected file name.

  If a file name was selected, the returned string contains the
  absolute path name.  The returned string is an empty string if no file
  name was selected.

  \sa QString::isNull(), QFileDialog::selectedFiles(), QFileDialog::selectedFilter()
*/

QString QFileDialog::selectedFile() const
{
    QString res;
    QUrl u( d->currentFileName );
    if ( u.isLocalFile() ) {
	QString s = u.toString();
	if ( s.left( 5 ) == "file:" )
	    s.remove( 0, 5 );
	return s;
    }
    return d->currentFileName;
}

/*!
  Returns the filter which the user has chosen in
  the file dialog.

  \sa QString::isNull(), QFileDialog::selectedFiles()
*/

QString QFileDialog::selectedFilter() const
{
    return d->types->currentText();
}

/*!
  Returns a list of selected files. This is only useful,
  if the mode of the filedialog is ExistingFiles. Else
  the list will only contain one entry, which is the
  the selectedFile. If no files were selected, this list
  is empty.

  \sa QFileDialog::selectedFile(), QValueList::isEmpty()
*/

QStringList QFileDialog::selectedFiles() const
{
    QStringList lst;

    if ( mode() == ExistingFiles ) {
	QListViewItem * i = files->firstChild();
	while( i ) {
	    if ( i->isSelected() ) {
		QUrl u = QUrl( d->url, ((QFileDialogPrivate::File*)i)->info.name() );
		if ( u.isLocalFile() ) {
		    QString s = u.toString();
		    if ( s.left( 5 ) == "file:" )
			s.remove( 0, 5 );
		    lst << s;
		} else {
		    lst << u.toString();
		}
	    }
	    i = i->nextSibling();
	}
    } else {
	lst << selectedFile();
    }

    return lst;
}

/*!
  Sets the default selection to \a filename.  If \a filename is
  absolute, setDir() is also called.

  \internal
  Only for external use.  Not useful inside QFileDialog.
*/

void QFileDialog::setSelection( const QString & filename )
{
    d->oldUrl = d->url;
    QString nf = d->url.nameFilter();
    if ( QUrl::isRelativeUrl( filename ) )
	d->url = QUrlOperator( d->url, filename );
    else
	d->url = QUrlOperator( filename );
    d->url.setNameFilter( nf );
    d->checkForFilter = TRUE;
    bool isDirOk;
    bool isDir = d->url.isDir( &isDirOk );
    if ( !isDirOk )
	isDir = d->url.path().right( 1 ) == "/";
    if ( !isDir ) {
	QUrlOperator u( d->url );
	d->url.setPath( d->url.dirPath() );
	trySetSelection( FALSE, d->url, FALSE );
	rereadDir();
	emit dirEntered( d->url.dirPath() );
	nameEdit->setText( u.fileName() );
    } else {
	if ( !d->url.path().isEmpty() &&
	     d->url.path().right( 1 ) != "/" ) {
	    QString p = d->url.path();
	    p += "/";
	    d->url.setPath( p );
	}
	trySetSelection( TRUE, d->url, FALSE );
	rereadDir();
	emit dirEntered( d->url.dirPath() );
	nameEdit->setText( QString::fromLatin1("") );
    }
    d->checkForFilter = FALSE;
}

/*!
  Returns the active directory path string in the file dialog.
  \sa dir(), setDir()
*/

QString QFileDialog::dirPath() const
{
    return d->url.dirPath();
}


/*!  Sets the filter spec in use to \a newFilter.

  If \a newFilter matches the regular expression
  <tt>([a-zA-Z0-9\.\*\?\ \+\;]*)$</tt> (ie. it ends with a normal wildcard
  expression enclosed in parentheses), only the parenthesized is used.
  This means that these calls are all equivalent:

  \code
     fd->setFilter( "All C++ files (*.cpp *.cc *.C *.cxx *.c++)" );
     fd->setFilter( "*.cpp *.cc *.C *.cxx *.c++" )
     fd->setFilter( "All C++ files (*.cpp;*.cc;*.C;*.cxx;*.c++)" );
     fd->setFilter( "*.cpp;*.cc;*.C;*.cxx;*.c++" )
  \endcode
*/

void QFileDialog::setFilter( const QString & newFilter )
{
    if ( !newFilter )
	return;
    QString f = newFilter;
    QRegExp r( QString::fromLatin1("([a-zA-Z0-9.*? +;#]*)$") );
    int len;
    int index = r.match( f, 0, &len );
    if ( index >= 0 )
	f = f.mid( index+1, len-2 );
    d->url.setNameFilter( f );
	if ( d->types->count() == 1 )  {
		d->types->clear();
		d->types->insertItem( QFileDialog::tr( QString::fromLatin1( newFilter ) ) );
	}
    rereadDir();
}


/*!
  Sets a directory path string for the file dialog.
  \sa dir()
*/

void QFileDialog::setDir( const QString & pathstr )
{
    QString dr = pathstr;
    if ( dr.isEmpty() )
	return;

#if defined(_OS_UNIX_)
    if ( dr.length() && dr[0] == '~' ) {
	struct passwd *pw;
	int i;

	i = 0;
	while( i < (int)dr.length() && dr[i] != '/' )
	    i++;
	QCString user;
	if ( i == 1 ) {
	    user = ::getlogin();
	    if( !user )
		user = getenv( "LOGNAME" );
	} else
	    user = dr.mid( 1, i-1 ).local8Bit();
	dr = dr.mid( i, dr.length() );
	pw = ::getpwnam( user );
	if ( pw )
	    dr.prepend( QString::fromLocal8Bit(pw->pw_dir) );
    }
#endif

    setUrl( dr );
}

/*!
  Returns the active directory in the file dialog.
  \sa setDir()
*/

const QDir *QFileDialog::dir() const
{
    if ( d->url.isLocalFile() )
	return  new QDir( d->url.path() );
    else
	return 0;
}

/*!
  Sets a directory path for the file dialog.
  \sa dir()
*/

void QFileDialog::setDir( const QDir &dir )
{
    d->oldUrl = d->url;
    QString nf( d->url.nameFilter() );
    d->url = dir.canonicalPath();
    d->url.setNameFilter( nf );
    QUrlInfo i( d->url, nameEdit->text() );
    d->checkForFilter = TRUE;
    trySetSelection( i.isDir(), QUrlOperator( d->url, nameEdit->text() ), FALSE );
    d->checkForFilter = FALSE;
    rereadDir();
    emit dirEntered( d->url.path() );
}

/*!
  Sets the \a url which should be used as working directory
*/

void QFileDialog::setUrl( const QUrlOperator &url )
{
    QString nf = d->url.nameFilter();
    d->url = QUrl( d->url, url.toString( FALSE, FALSE ) );
    d->url.setNameFilter( nf );

    d->checkForFilter = TRUE;
    if ( !d->url.isDir() ) {
	QUrlOperator u = d->url;
	d->url.setPath( d->url.dirPath() );
	trySetSelection( FALSE, u, FALSE );
	rereadDir();
	emit dirEntered( d->url.dirPath() );
	nameEdit->setText( u.fileName() );
    } else {
	trySetSelection( TRUE, d->url, FALSE );
	rereadDir();
	emit dirEntered( d->url.dirPath() );
    }
    d->checkForFilter = FALSE;
}

/*!
  If \a s is TRUE, hidden files are shown in the filedialog, else
  no hidden files are shown.
*/

void QFileDialog::setShowHiddenFiles( bool s )
{
    if ( s == bShowHiddenFiles )
	return;

    bShowHiddenFiles = s;
    rereadDir();
}

/*!
  Returns TRUE if hidden files are shown in the filedialog, else FALSE.
*/

bool QFileDialog::showHiddenFiles() const
{
    return bShowHiddenFiles;
}

/*!
  Re-reads the active directory in the file dialog.

  It is seldom necessary to call this function.	 It is provided in
  case the directory contents change and you want to refresh the
  directory list box.
*/

void QFileDialog::rereadDir()
{
    d->pendingItems.clear();
    if ( d->mimeTypeTimer->isActive() )
	d->mimeTypeTimer->stop();
    d->currListChildren = d->url.listChildren();
}


/*!
  \fn void QFileDialog::fileHighlighted( const QString& )

  This signal is emitted when the user highlights a file.
*/

/*!
  \fn void QFileDialog::fileSelected( const QString& )

  This signal is emitted when the user selects a file.
*/

/*!
  \fn void QFileDialog::dirEntered( const QString& )

  This signal is emitted when the user has selected a new directory.
*/

// Defined in qapplication.cpp:
void qt_enter_modal( QWidget* );
void qt_leave_modal( QWidget* );

/*!
  Opens a modal file dialog and returns the name of the file to be
  opened.

  If \a startWith is the name of a directory, the dialog starts off in
  that directory.  If \a startWith is the name of an existing file,
  the dialogs starts in that directory, and with \a startWith
  selected.

  Only files matching \a filter are selectable.	 If \a filter is QString::null,
  all files are selectable. In the filter string multiple filters can be specified
  separated by either two semicolons next to each other or separated by newlines. To add
  two filters, one to show all C++ files and one to show all header files, the filter
  string could look like "C++ Files (*.cpp *.cc *.C *.cxx *.c++);;Header Files (*.h *.hxx *.h++)"

  If \a widget and/or \a name is provided, the dialog will be centered
  over \a widget and \link QObject::name() named \endlink \a name.

  getOpenFileName() returns a \link QString::isNull() null string
  \endlink if the user cancelled the dialog.

  This static function is less capable than the full QFileDialog object,
  but is convenient and easy to use.

  Example:
  \code
    // start at the current working directory and with *.cpp as filter
    QString f = QFileDialog::getOpenFileName( QString::null, "*.cpp", this );
    if ( !f.isEmpty() ) {
	// the user selected a valid existing file
    } else {
	// the user cancelled the dialog
    }
  \endcode

  getSaveFileName() is another convenience function, equal to this one
  except that it allows the user to specify the name of a nonexistent file
  name.

  NOTE: In the windows version of Qt this static method uses the native
  windows file dialog, and not the QFileDialog.

  \sa getSaveFileName()
*/

QString QFileDialog::getOpenFileName( const QString & startWith,
				      const QString& filter,
				      QWidget *parent, const char* name,
				      const QString& caption )
{
    QStringList filters;
    if ( !filter.isEmpty() )
	filters = makeFiltersList( filter );

    makeVariables();
    QString initialSelection;
    //### Problem with the logic here: If a startWith is given, and a file
    // with that name exists in D->URL, the box will be opened at D->URL instead of
    // the last directory used ('workingDirectory').
    if ( !startWith.isEmpty() ) {
	QUrlOperator u( startWith );
	if ( u.isLocalFile() && QFileInfo( u.path() ).isDir() ) {
	    *workingDirectory = startWith;
	} else {
	    *workingDirectory = u.toString();
	    initialSelection = QString::null;//u.fileName();
	}
    }

    if ( workingDirectory->isNull() )
	*workingDirectory = QDir::currentDirPath();

#if defined(_WS_WIN_)
    if ( qApp->style() == WindowsStyle )
	return winGetOpenFileName( initialSelection, filter, workingDirectory,
				   parent, name, caption );
#endif

    QFileDialog *dlg = new QFileDialog( *workingDirectory, QString::null,
					parent, name, TRUE );

    if ( parent && parent->icon() && !parent->icon()->isNull() )
	dlg->setIcon( *parent->icon() );
    else if ( qApp->mainWidget() && qApp->mainWidget()->icon() && !qApp->mainWidget()->icon()->isNull() )
	dlg->setIcon( *qApp->mainWidget()->icon() );

    CHECK_PTR( dlg );
    if ( !caption.isNull() )
	dlg->setCaption( caption );
    else
	dlg->setCaption( QFileDialog::tr( "Open" ) );

    dlg->setFilters( filters );
    dlg->setMode( QFileDialog::ExistingFile );
    QString result;
    if ( !initialSelection.isEmpty() )
	dlg->setSelection( initialSelection );
    if ( dlg->exec() == QDialog::Accepted ) {
	result = dlg->selectedFile();
	*workingDirectory = dlg->d->url;
    }
    delete dlg;

    return result;
}


/*!\overload
 */
QString QFileDialog::getOpenFileName( const QString & startWith,
				      const QString& filter,
				      QWidget *parent, const char* name )
{
    return getOpenFileName( startWith, filter, parent, name, QString::null  );
}

/*!
  Opens a modal file dialog and returns the name of the file to be
  saved.

  If \a startWith is the name of a directory, the dialog starts off in
  that directory.  If \a startWith is the name of an existing file,
  the dialogs starts in that directory, and with \a startWith
  selected.

  Only files matching \a filter are selectable.	 If \a filter is QString::null,
  all files are selectable. In the filter string multiple filters can be specified
  separated by either two semicolons next to each other or separated by newlines. To add
  two filters, one to show all C++ files and one to show all header files, the filter
  string could look like "C++ Files (*.cpp *.cc *.C *.cxx *.c++);;Header Files (*.h *.hxx *.h++)"

  If \a widget and/or \a name is provided, the dialog will be centered
  over \a widget and \link QObject::name() named \endlink \a name.

  Returns a \link QString::isNull() null string\endlink if the user
  cancelled the dialog.

  This static function is less capable than the full QFileDialog object,
  but is convenient and easy to use.

  Example:
  \code
    // start at the current working directory and with *.cpp as filter
    QString f = QFileDialog::getSaveFileName( QString::null, "*.cpp", this );
    if ( !f.isEmpty() ) {
	// the user gave a file name
    } else {
	// the user cancelled the dialog
    }
  \endcode

  getOpenFileName() is another convenience function, equal to this one
  except that it does not allow the user to specify the name of a
  nonexistent file name.

  NOTE: In the windows version of Qt this static method uses the native
  windows file dialog, and not the QFileDialog.

  \sa getOpenFileName()
*/

QString QFileDialog::getSaveFileName( const QString & startWith,
				      const QString& filter,
				      QWidget *parent, const char* name,
				      const QString& caption )
{
    QStringList filters;
    if ( !filter.isEmpty() )
	filters = makeFiltersList( filter );

    makeVariables();
    QString initialSelection;
    if ( !startWith.isEmpty() ) {
	QUrlOperator u( startWith );
	if ( u.isLocalFile() && QFileInfo( u.path() ).isDir() ) {
	    *workingDirectory = startWith;
	} else {
	    *workingDirectory = u.toString();
	    initialSelection = QString::null;//u.fileName();
	}
    }

    if ( workingDirectory->isNull() )
	*workingDirectory = QDir::currentDirPath();

#if defined(_WS_WIN_)
    if ( qApp->style() == WindowsStyle )
	return winGetSaveFileName( initialSelection, filter, workingDirectory,
				   parent, name, caption );
#endif

    QFileDialog *dlg = new QFileDialog( *workingDirectory, QString::null, parent, name, TRUE );
    CHECK_PTR( dlg );
    if ( parent && parent->icon() && !parent->icon()->isNull() )
	dlg->setIcon( *parent->icon() );
    else if ( qApp->mainWidget() && qApp->mainWidget()->icon() && !qApp->mainWidget()->icon()->isNull() )
	dlg->setIcon( *qApp->mainWidget()->icon() );

    if ( !caption.isNull() )
	dlg->setCaption( caption );
    else
	dlg->setCaption( QFileDialog::tr( "Save as" ) );
    QString result;
    dlg->setFilters( filters );
    dlg->setMode( QFileDialog::AnyFile );
    if ( !initialSelection.isEmpty() )
	dlg->setSelection( initialSelection );
    if ( dlg->exec() == QDialog::Accepted ) {
	result = dlg->selectedFile();
	*workingDirectory = dlg->d->url;
    }
    delete dlg;
    return result;
}

/*!\overload
 */
QString QFileDialog::getSaveFileName( const QString & startWith,
				      const QString& filter,
				      QWidget *parent, const char* name )
{
    return getSaveFileName( startWith, filter, parent, name, QString::null );
}

/*!
  \internal
  Activated when the "OK" button is clicked.
*/

void QFileDialog::okClicked()
{
    QString fn( nameEdit->text() );
    if ( fn.contains( "*") ) {
	addFilter( fn );
	nameEdit->blockSignals( TRUE );
	nameEdit->setText( QString::fromLatin1("") );
	nameEdit->blockSignals( FALSE );
	return;
    }

    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    *lastSize = size();

    if ( isDirectoryMode( d->mode ) ) {
	if ( d->ignoreReturn ) {
	    d->ignoreReturn = FALSE;
	    return;
	}
	QUrlInfo f( d->url, nameEdit->text() );
	if ( f.isDir() ) {
	    d->currentFileName = d->url;
	    if ( d->currentFileName.right(1) != "/" )
		d->currentFileName += '/';
	    d->currentFileName += f.name();
	    accept();
	    return;
	}
    }

    // if we're in multi-selection mode and something is selected,
    // accept it and be done.
    if ( mode() == ExistingFiles ) {
	QListViewItem * i = files->firstChild();
	while ( i ) {
	    if ( i->isSelected() ) {
		accept();
		return;
	    }
	    i = i->nextSibling();
	}
	for ( uint j = 0; j < d->moreFiles->count(); ++j ) {
	    if ( d->moreFiles->isSelected( j ) ) {
		accept();
		return;
	    }
	}
    }

    if ( mode() == AnyFile ) {
	QUrlOperator u( d->url, nameEdit->text() );
	if ( !u.isDir() ) {
	    d->currentFileName = u;
	    emit fileSelected( selectedFile() );
	    accept();
	    return;
	}
    }

    // If selection is valid, return it, else try
    // using selection as a directory to change to.
    if ( !d->currentFileName.isNull() && !d->currentFileName.contains( "*" ) ) {
	emit fileSelected( selectedFile() );
	accept();
    } else {
	QUrlInfo f;
	QFileDialogPrivate::File * c
	    = (QFileDialogPrivate::File *)files->currentItem();
	QFileDialogPrivate::MCItem * m
	    = (QFileDialogPrivate::MCItem *)d->moreFiles->item( d->moreFiles->currentItem() );
	if ( c && files->isVisible() && files->hasFocus() ||
	     m && d->moreFiles->isVisible() && d->moreFiles->hasFocus() ) {
	    if ( c && files->isVisible() )
		f = c->info;
	    else
		f = ( (QFileDialogPrivate::File*)m->i )->info;
	} else {
	    f = QUrlInfo( d->url, nameEdit->text() );
	}
	if ( f.isDir() ) {
	    setUrl( QUrlOperator( d->url, f.name() + "/" ) );
	    d->checkForFilter = TRUE;
	    trySetSelection( TRUE, d->url, TRUE );
	    d->checkForFilter = FALSE;
	} else {
	    if ( !nameEdit->text().contains( "/" ) &&
		 !nameEdit->text().contains( "\\" )
#if defined(_OS_WIN32_)
		 && nameEdit->text()[ 1 ] != ':'
#endif
		 )
		addFilter( nameEdit->text() );
	    else if ( nameEdit->text()[ 0 ] == '/' ||
		      nameEdit->text()[ 0 ] == '\\'
#if defined(_OS_WIN32_)
		      || nameEdit->text()[ 1 ] == ':'
#endif
		      )
		setDir( nameEdit->text() );
	    else if ( nameEdit->text().left( 3 ) == "../" || nameEdit->text().left( 3 ) == "..\\" )
		setDir( QUrl( d->url.toString(), nameEdit->text() ).toString() );
	}
	nameEdit->setText( "" );
    }
}

/*!
  \internal
  Activated when the "Filter" button is clicked.
*/

void QFileDialog::filterClicked()
{
    // unused
}

/*!
  \internal
  Activated when the "Cancel" button is clicked.
*/

void QFileDialog::cancelClicked()
{
    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    *lastSize = size();
    reject();
}


/*!\reimp
*/

void QFileDialog::resizeEvent( QResizeEvent * e )
{
    QDialog::resizeEvent( e );
    updateGeometries();
}

/*
  \internal
  The only correct way to try to set currentFileName
*/
bool QFileDialog::trySetSelection( bool isDir, const QUrlOperator &u, bool updatelined )
{
    if ( !isDir && !u.path().isEmpty() && u.path().right( 1 ) == "/" )
	isDir = TRUE;
    if ( u.fileName().contains( "*") && d->checkForFilter ) {
	QString fn( u.fileName() );
	if ( fn.contains( "*" ) ) {
	    addFilter( fn );
	    d->currentFileName = QString::null;
	    d->url.setFileName( QString::null );
	    nameEdit->setText( QString::fromLatin1("") );
	    return FALSE;
	}
    }

    if ( isDir ) {
        if ( d->preview && d->preview->isVisible() ) {
            if ( d->infoPreviewer )
                d->infoPreviewer->previewUrl( u );
            if ( d->contentsPreviewer )
                d->contentsPreviewer->previewUrl( u );
        }
    }

    QString old = d->currentFileName;

    if ( isDirectoryMode( mode() ) ) {
	if ( isDir )
	    d->currentFileName = u;
	else
	    d->currentFileName = QString::null;
    } else if ( !isDir && mode() == ExistingFiles ) {
	d->currentFileName = u;
    } else if ( !isDir || ( mode() == AnyFile && !isDir ) ) {
	d->currentFileName = u;
    } else {
	d->currentFileName = QString::null;
    }
    if ( updatelined && !d->currentFileName.isEmpty() ) {
	// If the selection is valid, or if its a directory, allow OK.
	if ( !d->currentFileName.isNull() || isDir ) {
	    if ( u.fileName() != ".." )
		nameEdit->setText( u.fileName() );
	    else
		nameEdit->setText("");
	} else
	    nameEdit->setText( QString::fromLatin1("") );
    }

    if ( !d->currentFileName.isNull() || isDir ) {
	okB->setEnabled( TRUE );
    } else if ( !isDirectoryMode( d->mode ) ) {
	okB->setEnabled( FALSE );
    }

    if ( d->currentFileName.length() && old != d->currentFileName )
	emit fileHighlighted( selectedFile() );

    return !d->currentFileName.isNull();
}


/*!  Make sure the minimum and maximum sizes of everything are sane.
*/

void QFileDialog::updateGeometries()
{
    if ( !d || !d->geometryDirty )
	return;

    d->geometryDirty = FALSE;

    QSize r, t;

    // we really should have a QSize::unite()
#define RM r.setWidth( QMAX(r.width(),t.width()) ); \
r.setHeight( QMAX(r.height(),t.height()) )

    // labels first
    r = d->pathL->sizeHint();
    t = d->fileL->sizeHint();
    RM;
    t = d->typeL->sizeHint();
    RM;
    d->pathL->setFixedSize( d->pathL->sizeHint() );
    d->fileL->setFixedSize( r );
    d->typeL->setFixedSize( r );

    // single-line input areas
    r = d->paths->sizeHint();
    t = nameEdit->sizeHint();
    RM;
    t = d->types->sizeHint();
    RM;
    r.setWidth( t.width() * 2 / 3 );
    t.setWidth( QWIDGETSIZE_MAX );
    t.setHeight( r.height() );
    d->paths->setMinimumSize( r );
    d->paths->setMaximumSize( t );
    nameEdit->setMinimumSize( r );
    nameEdit->setMaximumSize( t );
    d->types->setMinimumSize( r );
    d->types->setMaximumSize( t );

    // buttons on top row
    r = QSize( 0, d->paths->minimumSize().height() );
    t = QSize( 21, 20 );
    RM;
    if ( r.height()+1 > r.width() )
	r.setWidth( r.height()+1 );
    if ( d->goBack )
	d->goBack->setFixedSize( r );
    d->cdToParent->setFixedSize( r );
    d->newFolder->setFixedSize( r );
    d->mcView->setFixedSize( r );
    d->detailView->setFixedSize( r );

    QButton *b = 0;
    if ( !d->toolButtons.isEmpty() ) {
	for ( b = d->toolButtons.first(); b; b = d->toolButtons.next() )
	    b->setFixedSize( b->sizeHint().width(), r.height() );
    }

    if ( d->infoPreview ) {
	d->previewInfo->show();
	d->previewInfo->setFixedSize( r );
    } else {
	d->previewInfo->hide();
	d->previewInfo->setFixedSize( QSize( 0, 0 ) );
    }

    if ( d->contentsPreview ) {
	d->previewContents->show();
	d->previewContents->setFixedSize( r );
    } else {
	d->previewContents->hide();
	d->previewContents->setFixedSize( QSize( 0, 0 ) );
    }

    // open/save, cancel
    r = QSize( 75, 20 );
    t = okB->sizeHint();
    RM;
    t = cancelB->sizeHint();
    RM;

    okB->setFixedSize( r );
    cancelB->setFixedSize( r );

    d->topLevelLayout->activate();

#undef RM
}


/*!  Updates the dialog when the cursor moves in the listview. */

void QFileDialog::updateFileNameEdit( QListViewItem * newItem )
{
    if ( !newItem )
	return;

    if ( mode() == ExistingFiles ) {
	detailViewSelectionChanged();
    } else if ( files->isSelected( newItem ) ) {
	QFileDialogPrivate::File * i = (QFileDialogPrivate::File *)newItem;
	if ( !i->i->selected() ) {
	    d->moreFiles->blockSignals( TRUE );
	    d->moreFiles->setSelected( i->i, TRUE );
	    d->moreFiles->blockSignals( FALSE );
	}
	trySetSelection( i->info.isDir(), QUrlOperator( d->url, newItem->text( 0 ) ), TRUE );
    }
}

void QFileDialog::detailViewSelectionChanged()
{
    if ( d->mode != ExistingFiles )
	return;

    nameEdit->clear();
    QString str;
    QListViewItem * i = files->firstChild();
    d->moreFiles->blockSignals( TRUE );
    while( i ) {
	if ( d->moreFiles && isVisible() ) {
	    if ( ( (QFileDialogPrivate::File *)i )->i->selected() != i->isSelected() )
		d->moreFiles->setSelected( ( (QFileDialogPrivate::File *)i )->i, i->isSelected() );
	}
	if ( i->isSelected() && !( (QFileDialogPrivate::File *)i )->info.isDir() )
	    str += QString( "\"%1\" " ).arg( i->text( 0 ) );
	i = i->nextSibling();
    }
    d->moreFiles->blockSignals( FALSE );
    nameEdit->setText( str );
    nameEdit->setCursorPosition( str.length() );
    okB->setEnabled( TRUE );
    if ( d->preview && d->preview->isVisible() && files->currentItem() ) {
	QUrl u = QUrl( d->url, ((QFileDialogPrivate::File*)files->currentItem())->info.name() );
	if ( d->infoPreviewer )
	    d->infoPreviewer->previewUrl( u );
	if ( d->contentsPreviewer )
	    d->contentsPreviewer->previewUrl( u );
    }
}

void QFileDialog::listBoxSelectionChanged()
{
    if ( d->mode != ExistingFiles )
	return;

    nameEdit->clear();
    QString str;
    QListBoxItem * j = 0;
    QListBoxItem * i = d->moreFiles->item( 0 );
    int index = 0;
    files->blockSignals( TRUE );
    while( i ) {
	if ( files && isVisible() ) {
	    if ( ( (QFileDialogPrivate::MCItem *)i )->i->isSelected() != i->selected() )
		files->setSelected( ( (QFileDialogPrivate::MCItem *)i )->i, i->selected() );
	}
	if ( d->moreFiles->isSelected( i )
        && !( (QFileDialogPrivate::File*)( (QFileDialogPrivate::MCItem *)i )->i )->info.isDir() ) {
	    str += QString( "\"%1\" " ).arg( i->text() );
        if ( j == 0 )
            j = i;
    }
	i = d->moreFiles->item( ++index );
    }
    files->blockSignals( FALSE );
    nameEdit->setText( str );
    nameEdit->setCursorPosition( str.length() );
    okB->setEnabled( TRUE );
	if ( d->preview && d->preview->isVisible() && j ) {
	QUrl u = QUrl( d->url,
		       ( (QFileDialogPrivate::File*)( (QFileDialogPrivate::MCItem*)j )->i )->info.name() );
	if ( d->infoPreviewer )
	    d->infoPreviewer->previewUrl( u );
	if ( d->contentsPreviewer )
	    d->contentsPreviewer->previewUrl( u );
    }
}

/*! \overload */

void QFileDialog::updateFileNameEdit( QListBoxItem * newItem )
{
    if ( !newItem )
	return;
    QFileDialogPrivate::MCItem * i = (QFileDialogPrivate::MCItem *)newItem;
    if ( d->mode != ExistingFiles ) {
	i->i->listView()->setSelected( i->i, i->selected() );
	updateFileNameEdit( i->i );
    }
}


/*!  Updates the dialog when the file name edit changes. */

void QFileDialog::fileNameEditDone()
{
    QUrlInfo f( d->url, nameEdit->text() );
    if ( mode() != ExistingFiles ) {
        QUrlOperator u( d->url, nameEdit->text() );
        trySetSelection( f.isDir(), u, FALSE );
        if ( d->preview && d->preview->isVisible() ) {
            if ( d->infoPreviewer )
                d->infoPreviewer->previewUrl( u );
            if ( d->contentsPreviewer )
                d->contentsPreviewer->previewUrl( u );
        }
    }
}



/*!  This private slot reacts to double-clicks in the list view. */

void QFileDialog::selectDirectoryOrFile( QListViewItem * newItem )
{
    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    *lastSize = size();

    if ( !newItem )
	return;

    QFileDialogPrivate::File * i = (QFileDialogPrivate::File *)newItem;

    QString oldName = nameEdit->text();
    if ( i->info.isDir() ) {
	setUrl( QUrlOperator( d->url, i->info.name() + "/" ) );
	if ( isDirectoryMode( mode() ) ) {
	    QUrlInfo f ( d->url, QString::fromLatin1( "." ) );
	    trySetSelection( f.isDir(), d->url, TRUE );
	}
    } else if ( newItem->isSelectable() &&
		trySetSelection( i->info.isDir(), QUrlOperator( d->url, i->info.name() ), TRUE ) ) {
	if ( !isDirectoryMode( mode() ) ) {
	    emit fileSelected( selectedFile() );
	    accept();
	}
    } else if ( isDirectoryMode( d->mode ) ) {
	d->currentFileName = d->url;
	accept();
    }
    if ( !oldName.isEmpty() && !isDirectoryMode( mode() ) )
	nameEdit->setText( oldName );
}


void QFileDialog::selectDirectoryOrFile( QListBoxItem * newItem )
{
    if ( !newItem )
	return;
    QFileDialogPrivate::MCItem * i = (QFileDialogPrivate::MCItem *)newItem;
    i->i->listView()->setSelected( i->i, i->selected() );
    selectDirectoryOrFile( i->i );
}


void QFileDialog::popupContextMenu( QListViewItem *item, const QPoint &p,
				    int )
{
    if ( d->mode == ExistingFiles )
	return;
    if ( item ) {
	files->setCurrentItem( item );
	files->setSelected( item, TRUE );
    }

    PopupAction action;
    popupContextMenu( item ? item->text( 0 ) : QString::null, TRUE, action, p );

    if ( action == PA_Open )
	selectDirectoryOrFile( item );
    else if ( action == PA_Rename )
	files->startRename( FALSE );
    else if ( action == PA_Delete )
	deleteFile( item ? item->text( 0 ) : QString::null );
    else if ( action == PA_Reload )
	rereadDir();
    else if ( action == PA_Hidden ) {
	bShowHiddenFiles = !bShowHiddenFiles;
	rereadDir();
    } else if ( action == PA_SortName ) {
	sortFilesBy = (int)QDir::Name;
	sortAscending = TRUE;
	resortDir();
    } else if ( action == PA_SortSize ) {
	sortFilesBy = (int)QDir::Size;
	sortAscending = TRUE;
	resortDir();
    } else if ( action == PA_SortDate ) {
	sortFilesBy = (int)QDir::Time;
	sortAscending = TRUE;
	resortDir();
    } else if ( action == PA_SortUnsorted ) {
	sortFilesBy = (int)QDir::Unsorted;
	sortAscending = TRUE;
	resortDir();
    }

}

void QFileDialog::popupContextMenu( QListBoxItem *item, const QPoint & p )
{
    if ( d->mode == ExistingFiles )
	return;

    PopupAction action;
    popupContextMenu( item ? item->text() : QString::null, FALSE, action, p );

    if ( action == PA_Open )
	selectDirectoryOrFile( item );
    else if ( action == PA_Rename )
	d->moreFiles->startRename( FALSE );
    else if ( action == PA_Delete )
	deleteFile( item->text() );
    else if ( action == PA_Reload )
	rereadDir();
    else if ( action == PA_Hidden ) {
	bShowHiddenFiles = !bShowHiddenFiles;
	rereadDir();
    } else if ( action == PA_SortName ) {
	sortFilesBy = (int)QDir::Name;
	sortAscending = TRUE;
	resortDir();
    } else if ( action == PA_SortSize ) {
	sortFilesBy = (int)QDir::Size;
	sortAscending = TRUE;
	resortDir();
    } else if ( action == PA_SortDate ) {
	sortFilesBy = (int)QDir::Time;
	sortAscending = TRUE;
	resortDir();
    } else if ( action == PA_SortUnsorted ) {
	sortFilesBy = (int)QDir::Unsorted;
	sortAscending = TRUE;
	resortDir();
    }
}

void QFileDialog::popupContextMenu( const QString &filename, bool,
				    PopupAction &action, const QPoint &p )
{
    action = PA_Cancel;

    bool glob = filename.isEmpty();

    QPopupMenu m( 0, "file dialog context menu" );
    m.setCheckable( TRUE );

    if ( !glob ) {
	QString okt =
		     QUrlInfo( d->url, filename ).isDir()
		     ? tr( "&Open" )
	 : ( mode() == AnyFile
	     ? tr( "&Save" )
	     : tr( "&Open" ) );
	int ok = m.insertItem( okt );

	m.insertSeparator();
	int rename = m.insertItem( tr( "&Rename" ) );
	int del = m.insertItem( tr( "&Delete" ) );

	if ( filename.isEmpty() || !QUrlInfo( d->url, "." ).isWritable() ||
	     filename == ".." ) {
	    if ( filename.isEmpty() || !QUrlInfo( d->url, filename ).isReadable() )
		m.setItemEnabled( ok, FALSE );
	    m.setItemEnabled( rename, FALSE );
	    m.setItemEnabled( del, FALSE );
	} 

	if ( mode() == QFileDialog::ExistingFiles )
	    m.setItemEnabled( rename, FALSE );

	m.move( p );
	int res = m.exec();

	if ( res == ok )
	    action = PA_Open;
	else if ( res == rename )
	    action = PA_Rename;
	else if ( res == del )
	    action = PA_Delete;
    } else {
	int reload = m.insertItem( tr( "R&eload" ) );

	QPopupMenu m2( 0, "sort menu" );

	int sname = m2.insertItem( tr( "Sort by &Name" ) );
	//int stype = m2.insertItem( tr( "Sort by &Type" ) );
	int ssize = m2.insertItem( tr( "Sort by &Size" ) );
	int sdate = m2.insertItem( tr( "Sort by &Date" ) );
	m2.insertSeparator();
	int sunsorted = m2.insertItem( tr( "&Unsorted" ) );

	//m2.setItemEnabled( stype, FALSE );

	if ( sortFilesBy == (int)QDir::Name )
	    m2.setItemChecked( sname, TRUE );
	else if ( sortFilesBy == (int)QDir::Size )
	    m2.setItemChecked( ssize, TRUE );
// 	else if ( sortFilesBy == 0x16 )
// 	    m2.setItemChecked( stype, TRUE );
	else if ( sortFilesBy == (int)QDir::Time )
	    m2.setItemChecked( sdate, TRUE );
	else if ( sortFilesBy == (int)QDir::Unsorted )
	    m2.setItemChecked( sunsorted, TRUE );

	m.insertItem( tr( "Sort" ), &m2 );

	m.insertSeparator();

	int hidden = m.insertItem( tr( "Show &hidden files" ) );
	m.setItemChecked( hidden, bShowHiddenFiles );

	m.move( p );
	int res = m.exec();

	if ( res == reload )
	    action = PA_Reload;
	else if ( res == hidden )
	    action = PA_Hidden;
	else if ( res == sname )
	    action = PA_SortName;
// 	else if ( res == stype )
// 	    action = PA_SortType;
	else if ( res == sdate )
	    action = PA_SortDate;
	else if ( res == ssize )
	    action = PA_SortSize;
	else if ( res == sunsorted )
	    action = PA_SortUnsorted;
    }

}

void QFileDialog::deleteFile( const QString &filename )
{
    if ( filename.isEmpty() )
	return;

    QUrlInfo fi( d->url, filename );
    QString t = tr( "the file" );
    if ( fi.isDir() )
	t = tr( "the directory" );
    if ( fi.isSymLink() )
	t = tr( "the symlink" );

    if ( QMessageBox::warning( this,
			       tr( "Delete %1" ).arg( t ),
			       tr( "<qt>Do you really want to delete %1 \"%2\"?</qt>" )
			       .arg( t ).arg(filename),
			       tr( "&Yes" ), tr( "&No" ), QString::null, 1 ) == 0 )
	d->url.remove( filename );

}

void QFileDialog::fileSelected( int  )
{
    // unused
}

void QFileDialog::fileHighlighted( int )
{
    // unused
}

void QFileDialog::dirSelected( int )
{
    // unused
}

void QFileDialog::pathSelected( int )
{
    // unused
}


void QFileDialog::cdUpClicked()
{
    QString oldName = nameEdit->text();
    setUrl( QUrlOperator( d->url, ".." ) );
    if ( !oldName.isEmpty() )
	nameEdit->setText( oldName );
}

void QFileDialog::newFolderClicked()
{
    QString foldername( tr( "New Folder 1" ) );
    int i = 0;
    QStringList lst;
    QListViewItemIterator it( files );
    for ( ; it.current(); ++it )
	if ( it.current()->text( 0 ).contains( tr( "New Folder" ) ) )
	    lst.append( it.current()->text( 0 ) );

    if ( !lst.count() == 0 )
	while ( lst.contains( foldername ) )
	    foldername = tr( "New Folder %1" ).arg( ++i );

    d->url.mkdir( foldername );
}

void QFileDialog::createdDirectory( const QUrlInfo &info, QNetworkOperation * )
{
    resortDir();
    if ( d->moreFiles->isVisible() ) {
	for ( uint i = 0; i < d->moreFiles->count(); ++i ) {
	    if ( d->moreFiles->text( i ) == info.name() ) {
		d->moreFiles->setCurrentItem( i );
		d->moreFiles->startRename( FALSE );
		break;
	    }
	}
    } else {
	QListViewItem *item = files->firstChild();
	while ( item ) {
	    if ( item->text( 0 ) == info.name() ) {
		files->setSelected( item, TRUE );
		files->setCurrentItem( item );
		files->startRename( FALSE );
		break;
	    }
	    item = item->nextSibling();
	}
    }
}


/*!
  Ask the user for the name of an existing directory, starting at
  \a dir.  Returns the name of the directory the user selected.

  If \a dir is null, getExistingDirectory() starts wherever the
  previous file dialog left off.

  \a caption specifies the caption of the dialog, if this is empty a
  default caption will be used. If \a dirOnly if TRUE no files will be
  displayed in the file view widgets.
*/

QString QFileDialog::getExistingDirectory( const QString & dir,
					   QWidget *parent,
					   const char* name,
					   const QString& caption,
					   bool dirOnly )
{
    makeVariables();
    QString wd;
    if ( workingDirectory )
	wd = *workingDirectory;
    QFileDialog *dialog = new QFileDialog( parent, name, TRUE );
    if ( !caption.isNull() )
	dialog->setCaption( caption );
    else
	dialog->setCaption( QFileDialog::tr("Find Directory") );

    dialog->setMode( dirOnly ? DirectoryOnly : Directory );

    dialog->d->types->clear();
    dialog->d->types->insertItem( QFileDialog::tr("Directories") );
    dialog->d->types->setEnabled( FALSE );

    QString dir_( dir );
    dir_ = dir_.simplifyWhiteSpace();
    if ( dir_.isEmpty() && !wd.isEmpty() )
	dir_ = wd;
    QUrlOperator u( dir_ );
    if ( u.isLocalFile() ) {
	if ( !dir_.isEmpty() ) {
	    QFileInfo f( u.path() );
        if ( f.exists() )
        if ( f.isDir() ) {
		dialog->setDir( dir_ );
		wd = dir_;
	    }
	} else if ( !wd.isEmpty() ) {
	    QUrl tempUrl( wd );
	    QFileInfo f( tempUrl.path() );
	    if ( f.isDir() ) {
		dialog->setDir( wd );
	    }
	} else {
	    QString theDir = dir_;
	    if ( theDir.isEmpty() ) {
		theDir = QDir::currentDirPath();
	    } if ( !theDir.isEmpty() ) {
		QUrl tempUrl( theDir );
		QFileInfo f( tempUrl.path() );
		if ( f.isDir() ) {
		    wd = theDir;
		    dialog->setDir( theDir );
		}
	    }
	}
    } else {
	dialog->setUrl( dir_ );
    }

    QString result;
    dialog->setSelection( dialog->d->url.toString() );

    if ( dialog->exec() == QDialog::Accepted ) {
	result = dialog->selectedFile();
	wd = result;
    }
    delete dialog;

    if ( !result.isEmpty() && result.right( 1 ) != "/" )
	result += "/";

    return result;
}

/*!\overload
 */
QString QFileDialog::getExistingDirectory( const QString & dir,
					   QWidget *parent,
					   const char* name )
{
    return getExistingDirectory( dir, parent, name, QString::null );
}

/*!\overload
 */
QString QFileDialog::getExistingDirectory( const QString & dir,
					   QWidget *parent,
					   const char* name,
					   const QString &caption )
{
    return getExistingDirectory( dir, parent, name, caption, FALSE );
}

/*!  Sets this file dialog to \a newMode, which can be one of \c
  Directory (directories are accepted), \c ExistingFile (existing
  files are accepted), \c AnyFile (any valid file name is accepted)
  or \c ExistingFiles (like \c ExistingFile, but multiple files may be
  selected)

  \sa mode()
*/

void QFileDialog::setMode( Mode newMode )
{
    if ( d->mode != newMode ) {
	d->mode = newMode;
	QString sel = d->currentFileName;
	if ( isDirectoryMode( newMode ) ) {
	    files->setMultiSelection( FALSE );
	    d->moreFiles->setMultiSelection( FALSE );
	    if ( sel.isNull() )
		sel = QString::fromLatin1(".");
	    d->types->setEnabled( FALSE );
	} else if ( newMode == ExistingFiles ) {
	    files->setSelectionMode( QListView::Extended );
	    d->moreFiles->setSelectionMode( QListBox::Extended );
	} else {
	    files->setMultiSelection( FALSE );
	    d->moreFiles->setMultiSelection( FALSE );
	}
	rereadDir();
	QUrlInfo f( d->url, "." );
	trySetSelection( f.isDir(), d->url, TRUE );
    }

    QString okt;
    if ( mode() == AnyFile )
        okt = tr("Save");
    else if ( mode() == Directory || mode() == DirectoryOnly )
        okt = tr("OK");
    else
        okt = tr("Open");

    okB->setText( okt );
}


/*!  Returns the file mode of this dialog.

  \sa setMode()
*/

QFileDialog::Mode QFileDialog::mode() const
{
    return d->mode;
}

/*! \reimp
*/

void QFileDialog::done( int i )
{
    if ( i == QDialog::Accepted && (d->mode == ExistingFile || d->mode == ExistingFiles) ) {
	QStringList selection = selectedFiles();
	for ( uint f = 0; f < selection.count(); f++ ) {
	    QString file = selection[f];
	    if ( file.isNull() )
		continue;
#if 0 // #### we can't do that - people use getOpenFileName() instead of getSaveFileName() often, so this stuff below makes lots of apps useless
	    if ( d->url.isLocalFile() && !QFile::exists( file ) ) {
		QMessageBox::information( this, tr("Error"), tr("%1\nFile not found.\nCheck path and filename.").arg( file ) );
		return;
	    }
#endif
	}
    }
    QDialog::done( i );
}

/*!
  Sets the viewmode of the filedialog. You can choose between
  Detail, List.

  \sa setPreviewMode()
*/

void QFileDialog::setViewMode( ViewMode m )
{
    if ( m == Detail ) {
	d->stack->raiseWidget( files );
	d->detailView->setOn( TRUE );
	d->mcView->setOn( FALSE );
    } else if ( m == List ) {
	d->stack->raiseWidget( d->moreFiles );
	d->detailView->setOn( FALSE );
	d->mcView->setOn( TRUE );
    }
}

/*!
  Set the preview mode of the filedialog. You can choose between
  NoPreview, Info and Contents.

  To be able to set a preview mode other than NoPreview you need
  to set the preview widget, and enable this preview mode.

  \sa setInfoPreviewEnabled(), setContentsPreviewEnabled(),
  setInfoPreview(), setContentsPreview()
*/

void QFileDialog::setPreviewMode( PreviewMode m )
{
    if ( m == NoPreview ) {
	d->previewInfo->setOn( FALSE );
	d->previewContents->setOn( FALSE );
    } else if ( m == Info && d->infoPreview ) {
	d->previewInfo->setOn( TRUE );
	d->previewContents->setOn( FALSE );
	changeMode( d->modeButtons->id( d->previewInfo ) );
    } else if ( m == Contents && d->contentsPreview ) {
	d->previewInfo->setOn( FALSE );
	d->previewContents->setOn( TRUE );
	changeMode( d->modeButtons->id( d->previewContents ) );
    }
}

/*!
  Returns the viewmode of the filedialog.

  \sa setViewMode()
*/

QFileDialog::ViewMode QFileDialog::viewMode() const
{
    if ( d->moreFiles->isVisible() )
	return Detail;
    else
	return List;
}

/*!
  Returns the preview mode of the filedialog.

  \sa setPreviewMode()
*/

QFileDialog::PreviewMode QFileDialog::previewMode() const
{
    if ( d->infoPreview && d->previewInfo->isVisible() )
	return Info;
    else if ( d->contentsPreview && d->previewContents->isVisible() )
	return Contents;

    return NoPreview;
}

/*!  Adds 1-3 widgets to the bottom of the file dialog. \a l is the
  (optional) label, which is put beneath the "file name" and "file
  type" labels, \a w is a (optional) widget, which is put beneath the
  file type combo box, and \a b is the (you guessed it - optional)
  button, which is put beneath the cancel button.

  If you don't want to add something in one of the columns, pass 0.

  Each time calling this method adds a new row of widgets to the
  bottom of the filedialog.

  \sa addToolButton(), addLeftWidget(), addRightWidget()
*/

void QFileDialog::addWidgets( QLabel * l, QWidget * w, QPushButton * b )
{
    if ( !l && !w && !b )
	return;

    d->geometryDirty = TRUE;

    QHBoxLayout *lay = new QHBoxLayout();
    d->extraWidgetsLayouts.append( lay );
    d->topLevelLayout->addLayout( lay );

    if ( !l )
	l = new QLabel( this );
    d->extraLabels.append( l );
    lay->addWidget( l );

    if ( !w )
	w = new QWidget( this );
    d->extraWidgets.append( w );
    lay->addWidget( w );
    lay->addSpacing( 15 );

    if ( b ) {
	d->extraButtons.append( b );
	lay->addWidget( b );
    } else {
	QWidget *wid = new QWidget( this );
	d->extraButtons.append( wid );
	lay->addWidget( wid );
    }

    updateGeometries();
}

/*!
  Adds a the button \a b to the row of tool buttons on the top of the
  filedialog. The button is appended at the end (right) of
  this row. If \a separator is TRUE, a small space is inserted between the
  last button of the row and the new button \a b.

  \sa addWidgets(), addLeftWidget(), addRightWidget()
*/

void QFileDialog::addToolButton( QButton *b, bool separator )
{
    if ( !b || !d->buttonLayout )
	return;

    d->geometryDirty = TRUE;

    d->toolButtons.append( b );
    if ( separator )
	d->buttonLayout->addSpacing( 8 );
    d->buttonLayout->addWidget( b );

    updateGeometries();
}

/*!
  Adds the widget \a w to the left of the filedialog.

  \sa addRightWidget(), addWidgets(), addToolButton()
*/

void QFileDialog::addLeftWidget( QWidget *w )
{
    if ( !w )
	return;
    d->geometryDirty = TRUE;

    d->leftLayout->addWidget( w );
    d->leftLayout->addSpacing( 5 );

    updateGeometries();
}

/*!
  Adds the widget \a w to the right of the filedialog.

  \sa addLeftWidget(), addWidgets(), addToolButton()
*/

void QFileDialog::addRightWidget( QWidget *w )
{
    if ( !w )
	return;
    d->geometryDirty = TRUE;

    d->rightLayout->addSpacing( 5 );
    d->rightLayout->addWidget( w );

    updateGeometries();
}

/*! \reimp */

void QFileDialog::keyPressEvent( QKeyEvent * ke )
{
    if ( !d->ignoreNextKeyPress &&
	 ke && ( ke->key() == Key_Enter ||
		 ke->key() == Key_Return ) ) {
	ke->ignore();
	if ( d->paths->hasFocus() ) {
	    ke->accept();
	    if ( d->url == QUrl(d->paths->currentText()) )
		nameEdit->setFocus();
	} else if ( d->types->hasFocus() ) {
	    ke->accept();
	    // ### is there a suitable condition for this?  only valid
	    // wildcards?
	    nameEdit->setFocus();
	} else if ( nameEdit->hasFocus() ) {
	    if ( d->currentFileName.isNull() ) {
		// maybe change directory
		QUrlInfo i( d->url, nameEdit->text() );
		if ( i.isDir() ) {
		    nameEdit->setText( QString::fromLatin1("") );
		    setDir( QUrlOperator( d->url, i.name() ) );
		}
		ke->accept();
	    } else if ( mode() == ExistingFiles ) {
		QUrlInfo i( d->url, nameEdit->text() );
		if ( i.isFile() ) {
		    QListViewItem * i = files->firstChild();
		    while ( i && nameEdit->text() != i->text( 0 ) )
			i = i->nextSibling();
		    if ( i )
			files->setSelected( i, TRUE );
		    else
			ke->accept(); // strangely, means to ignore that event
		}
	    }
	} else if ( files->hasFocus() || d->moreFiles->hasFocus() ) {
	    ke->accept();
	}
    } else if ( ke->key() == Key_Escape ) {
	ke->ignore();
    }

    d->ignoreNextKeyPress = FALSE;

    if ( !ke->isAccepted() ) {
	QDialog::keyPressEvent( ke );
    }
}


/*! \class QFileIconProvider qfiledialog.h

  \brief The QFileIconProvider class provides icons for QFileDialog to
  use.

  \ingroup misc

  By default, QFileIconProvider is not used, but any application or
  library can subclass it, reimplement pixmap() to return a suitable
  icon, and make all QFileDialog objects use it by calling the static
  function QFileDialog::setIconProvider().

  It's advisable to make all the icons QFileIconProvider returns be of
  the same size, or at least the same width.  This makes the list view
  look much better.

  \sa QFileDialog
*/


/*!  Constructs an empty file icon provider. */

QFileIconProvider::QFileIconProvider( QObject * parent, const char* name )
    : QObject( parent, name )
{
    // nothing necessary
}


/*!
  Returns a pointer to a pixmap which should be used for
  visualizing the file with the information \a info.

  If pixmap() returns 0, QFileDialog draws the default pixmap.

  The default implementation returns particular icons for files, directories,
  link-files, link-directories, and blank for other types.

  If you return a pixmap here, it should be of the size 16x16.
*/

const QPixmap * QFileIconProvider::pixmap( const QFileInfo & info )
{
    if ( info.isSymLink() ) {
	if ( info.isFile() )
	    return symLinkFileIcon;
	else
	    return symLinkDirIcon;
    } else if ( info.isDir() ) {
	return closedFolderIcon;
    } else if ( info.isFile() ) {
	return fileIcon;
    } else {
	return fifteenTransparentPixels;
    }
}

/*!  Sets all file dialogs to use \a provider to select icons to draw
  for each file.  By default there is no icon provider, and
  QFileDialog simply draws a "folder" icon next to each directory and
  nothing next to the files.

  \sa QFileIconProvider iconProvider()
*/

void QFileDialog::setIconProvider( QFileIconProvider * provider )
{
    fileIconProvider = provider;
}


/*!  Returns the icon provider currently in use.  By default there is
  no icon provider and this function returns 0.

  \sa setIconProvider() QFileIconProvider
*/

QFileIconProvider * QFileDialog::iconProvider()
{
    return fileIconProvider;
}


#if defined(_WS_WIN_)
#include <windows.h>

static QString getWindowsRegString( HKEY key, const char *subKey )
{
    QString s;
    char  buf[512];
    DWORD bsz = sizeof(buf);
    int r = RegQueryValueExA( key, subKey, 0, 0, (LPBYTE)buf, &bsz );
    if ( r == ERROR_SUCCESS ) {
	s = buf;
    } else if ( r == ERROR_MORE_DATA ) {
	char *ptr = new char[bsz+1];
	r = RegQueryValueExA( key, subKey, 0, 0, (LPBYTE)ptr, &bsz );
	if ( r == ERROR_SUCCESS )
	    s = ptr;
	delete [] ptr;
    }
    return s;
}

static void initPixmap( QPixmap &pm )
{
    pm.fill( Qt::white );
}

QWindowsIconProvider::QWindowsIconProvider( QWidget *parent, const char *name )
    : QFileIconProvider( parent, name )
{
    pixw = GetSystemMetrics( SM_CXSMICON );
    pixh = GetSystemMetrics( SM_CYSMICON );

    HKEY k;
    HICON si;
    int r;
    QString s;
    UINT res;

    // ---------- get default folder pixmap
    r = RegOpenKeyExA( HKEY_CLASSES_ROOT,
		       "folder\\DefaultIcon",
		       0, KEY_READ, &k );
    if ( r == ERROR_SUCCESS ) {
	s = getWindowsRegString( k, 0 );
	RegCloseKey( k );

	QStringList lst = QStringList::split( ",", s );

	res = ExtractIconExA( (const char*)lst[ 0 ].simplifyWhiteSpace().latin1(),
			      lst[ 1 ].simplifyWhiteSpace().toInt(),
			      0, &si, 1 );

	if ( res != -1 ) {
	    defaultFolder.resize( pixw, pixh );
	    initPixmap( defaultFolder );
	    QPainter p( &defaultFolder );
	    DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, NULL,  DI_NORMAL );
	    p.end();
	    defaultFolder.setMask( defaultFolder.createHeuristicMask() );
	    *closedFolderIcon = defaultFolder;
	    DestroyIcon( si );
	} else {
	    defaultFolder = *closedFolderIcon;
	}
    } else {
	RegCloseKey( k );
    }

    //------------------------------- get default file pixmap
    res = ExtractIconExA( (char*)"shell32.dll",
			     0, 0, &si, 1 );

    if ( res != -1 ) {
	defaultFile.resize( pixw, pixh );
	initPixmap( defaultFile );
	QPainter p( &defaultFile );
	DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, NULL,  DI_NORMAL );
	p.end();
	defaultFile.setMask( defaultFile.createHeuristicMask() );
	*fileIcon = defaultFile;
	DestroyIcon( si );
    } else {
	defaultFile = *fileIcon;
    }

    //------------------------------- get default exe pixmap
    res = ExtractIconExA( (char*) "shell32.dll",
			  2, 0, &si, 1 );

    if ( res != -1 ) {
	defaultExe.resize( pixw, pixh );
	initPixmap( defaultExe );
	QPainter p( &defaultExe );
	DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, NULL,  DI_NORMAL );
	p.end();
	defaultExe.setMask( defaultExe.createHeuristicMask() );
	DestroyIcon( si );
    } else {
	defaultExe = *fileIcon;
    }
}

QWindowsIconProvider::~QWindowsIconProvider()
{
}

const QPixmap * QWindowsIconProvider::pixmap( const QFileInfo &fi )
{
    QString ext = fi.extension().upper();
    QString key = ext;
    ext.prepend( "." );
    QMap< QString, QPixmap >::Iterator it;

    if ( fi.isDir() ) {
	return &defaultFolder;
    } else if ( ext.lower() != ".exe" ) {
	it = cache.find( key );
	if ( it != cache.end() )
	    return &( *it );

	HKEY k, k2;
	int r = RegOpenKeyExA( HKEY_CLASSES_ROOT,
			       ext.latin1(),
			       0, KEY_READ, &k );
	QString s;
	if ( r == ERROR_SUCCESS ) {
	    s = getWindowsRegString( k, 0 );
	} else {
	    cache[ key ] = defaultFile;
	    RegCloseKey( k );
	    return &defaultFile;
	}
	RegCloseKey( k );

	r = RegOpenKeyExA( HKEY_CLASSES_ROOT,
			   QString( s + "\\DefaultIcon" ).latin1() ,
			   0, KEY_READ, &k2 );
	if ( r == ERROR_SUCCESS ) {
	    s = getWindowsRegString( k2, 0 );
	} else {
	    cache[ key ] = defaultFile;
	    RegCloseKey( k2 );
	    return &defaultFile;
	}
	RegCloseKey( k2 );

	QStringList lst = QStringList::split( ",", s );

	HICON si;
	UINT res;
	QString filepath = lst[ 0 ].stripWhiteSpace();
	if ( filepath.find("%1") != -1 ) {
	    filepath = filepath.arg( fi.filePath() );
	    if ( ext.lower() == ".dll" ) {
		pix = defaultFile;
		return &pix;
	    }
	}

	if ( filepath.isNull() ) 
	    return &defaultFile;

	res = ExtractIconExA( filepath.latin1(),
			      lst[ 1 ].stripWhiteSpace().toInt(),
			      NULL, &si, 1 );

	if ( res != -1 ) {
	    pix.resize( pixw, pixh );
	    initPixmap( pix );
	    QPainter p( &pix );
	    DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, NULL,  DI_NORMAL );
	    p.end();
	    pix.setMask( pix.createHeuristicMask() );
	    DestroyIcon( si );
	} else {
	    pix = defaultFile;
	}

	cache[ key ] = pix;
	return &pix;
    } else {
	HICON si;
	UINT res;
	res = ExtractIconExA( (const char*)fi.absFilePath().latin1(),
			      -1,
			      0, 0, 1 );

	if ( res == 0 ) {
	    return &defaultExe;
	} else {
	    res = ExtractIconExA( (char*)fi.absFilePath().latin1(),
				  res - 1,
				  0, &si, 1 );
	}

	if ( res != -1 ) {
	    pix.resize( pixw, pixh );
	    initPixmap( pix );
	    QPainter p( &pix );
	    DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, NULL,  DI_NORMAL );
	    p.end();
	    pix.setMask( pix.createHeuristicMask() );
	    DestroyIcon( si );
	} else {
	    pix = defaultExe;
	}

	return &pix;
    }

    // can't happen!
    return 0;
}
#endif



/*!
  \reimp
*/
bool QFileDialog::eventFilter( QObject * o, QEvent * e )
{
    if ( !o || !e )
	return TRUE;

    if ( e->type() == QEvent::KeyPress && ( (QKeyEvent*)e )->key() == Key_F5 ) {
	rereadDir();
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( e->type() == QEvent::KeyPress && ( (QKeyEvent*)e )->key() == Key_F2 &&
		( o == files || o == files->viewport() ) ) {
	if ( files->isVisible() && files->currentItem() ) {
	    if ( mode() != QFileDialog::ExistingFiles &&
		 QUrlInfo( d->url, "." ).isWritable() && files->currentItem()->text( 0 ) != ".." ) {
		files->renameItem = files->currentItem();
		files->startRename( TRUE );
	    }
	}
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( e->type() == QEvent::KeyPress && ( (QKeyEvent*)e )->key() == Key_F2 &&
		( o == d->moreFiles || o == d->moreFiles->viewport() ) ) {
	if ( d->moreFiles->isVisible() && d->moreFiles->currentItem() != -1 ) {
	    if ( mode() != QFileDialog::ExistingFiles &&
		 QUrlInfo( d->url, "." ).isWritable() &&
		 d->moreFiles->item( d->moreFiles->currentItem() )->text() != ".." ) {
		d->moreFiles->renameItem = d->moreFiles->item( d->moreFiles->currentItem() );
		d->moreFiles->startRename( TRUE );
	    }
	}
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( e->type() == QEvent::KeyPress && d->moreFiles->renaming ) {
	d->moreFiles->lined->setFocus();
	QApplication::sendEvent( d->moreFiles->lined, e );
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( e->type() == QEvent::KeyPress && files->renaming ) {
	files->lined->setFocus();
	QApplication::sendEvent( files->lined, e );
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( e->type() == QEvent::KeyPress &&
		((QKeyEvent *)e)->key() == Key_Backspace &&
		( o == files ||
		  o == d->moreFiles ||
		  o == files->viewport() ||
		  o == d->moreFiles->viewport() ) ) {
	cdUpClicked();
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( e->type() == QEvent::KeyPress &&
		((QKeyEvent *)e)->key() == Key_Delete &&
		( o == files ||
		  o == files->viewport() ) ) {
	if ( files->currentItem() )
	    deleteFile( files->currentItem()->text( 0 ) );
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( e->type() == QEvent::KeyPress &&
		((QKeyEvent *)e)->key() == Key_Delete &&
		( o == d->moreFiles ||
		  o == d->moreFiles->viewport() ) ) {
	int c = d->moreFiles->currentItem();
	if ( c >= 0 )
	    deleteFile( d->moreFiles->item( c )->text() );
	((QKeyEvent *)e)->accept();
	return TRUE;
    } else if ( o == files && e->type() == QEvent::FocusOut &&
		files->currentItem() && mode() != ExistingFiles ) {
    } else if ( o == files && e->type() == QEvent::KeyPress ) {
	QTimer::singleShot( 0, this, SLOT(fixupNameEdit()) );
    } else if ( o == nameEdit && e->type() == QEvent::KeyPress ) {
	if ( ( nameEdit->cursorPosition() == (int)nameEdit->text().length() || nameEdit->hasMarkedText() ) &&
	     isprint(((QKeyEvent *)e)->ascii()) ) {
#if defined(_WS_WIN_)
	    QString nt( nameEdit->text().lower() );
#else
	    QString nt( nameEdit->text() );
#endif
	    nt.truncate( nameEdit->cursorPosition() );
	    nt += (char)(((QKeyEvent *)e)->ascii());
	    QListViewItem * i = files->firstChild();
#if defined(_WS_WIN_)
	    while( i && i->text( 0 ).left(nt.length()).lower() != nt )
#else
	    while( i && i->text( 0 ).left(nt.length()) != nt )
#endif
		i = i->nextSibling();
	    if ( i ) {
		nt = i->text( 0 );
		int cp = nameEdit->cursorPosition()+1;
		nameEdit->validateAndSet( nt, cp, cp, nt.length() );
		return TRUE;
	    }
	}
    } else if ( o == nameEdit && e->type() == QEvent::FocusIn ) {
	fileNameEditDone();
    } else if ( d->moreFiles->renaming && o != d->moreFiles->lined && e->type() == QEvent::FocusIn ) {
	d->moreFiles->lined->setFocus();
	return TRUE;
    } else if ( files->renaming && o != files->lined && e->type() == QEvent::FocusIn ) {
	files->lined->setFocus();
	return TRUE;
    } else if ( ( o == d->moreFiles || o == d->moreFiles->viewport() ) &&
		e->type() == QEvent::FocusIn ) {
	if ( o == d->moreFiles->viewport() && !d->moreFiles->viewport()->hasFocus() ||
	     o == d->moreFiles && !d->moreFiles->hasFocus() )
	    ((QWidget*)o)->setFocus();
	return FALSE;
    }

    return QDialog::eventFilter( o, e );
}

/*!  Sets this file dialog to offer \a types in the File Type combo
  box.	\a types must be a null-terminated list of strings; each
  string must be in the format described in the documentation for
  setFilter().

  \sa setFilter()
*/

void QFileDialog::setFilters( const char ** types )
{
    if ( !types || !*types )
	return;

    d->types->clear();
    while( types && *types ) {
	d->types->insertItem( QString::fromLatin1(*types) );
	types++;
    }
    d->types->setCurrentItem( 0 );
    setFilter( d->types->text( 0 ) );
}


/*! \overload void QFileDialog::setFilters( const QStringList & )
*/

void QFileDialog::setFilters( const QStringList & types )
{
    if ( types.count() < 1 )
	return;

    d->types->clear();
    for ( QStringList::ConstIterator it = types.begin(); it != types.end(); ++it )
	d->types->insertItem( *it );
    d->types->setCurrentItem( 0 );
    setFilter( d->types->text( 0 ) );
}

/*!
  \overload void QFileDialog::setFilters( const QString & )
*/

void QFileDialog::setFilters( const QString &filters )
{
    QStringList lst = makeFiltersList( filters );
    setFilters( lst );
}

/*!
  Adds \a filter to the filter list and makes it the current one.
*/

void QFileDialog::addFilter( const QString &filter )
{
    if ( filter.isEmpty() )
	return;

    QString f = filter;
    QRegExp r( QString::fromLatin1("([a-zA-Z0-9.*? +;#]*)$") );
    int len;
    int index = r.match( f, 0, &len );
    if ( index >= 0 )
	f = f.mid( index + 1, len - 2 );
    for ( int i = 0; i < d->types->count(); ++i ) {
	QString f2( d->types->text( i ) );
	int len;
	int index = r.match( f2, 0, &len );
	if ( index >= 0 )
	    f2 = f2.mid( index + 1, len - 2 );
	if ( f2 == f ) {
	    d->types->setCurrentItem( i );
	    setFilter( f2 );
	    return;
	}
    }

    d->types->insertItem( filter );
    d->types->setCurrentItem( d->types->count() - 1 );
    setFilter( d->types->text( d->types->count() - 1 ) );
}

/*!
  Since modeButtons is a top-level widget, it may be destroyed by the
  kernel at application exit time. Notice if this happens to
  avoid double deletion.
*/

void QFileDialog::modeButtonsDestroyed()
{
    if ( d )
	d->modeButtons = 0;
}


/*!  Lets the user select N files from a single directory, and returns
  a list of the selected files.	 The list may be empty, and the file
  names are fully qualified (i.e. "/usr/games/quake" or
  "c:\\quake\\quake").

  \a filter is the default glob pattern (which the user can change).
  The default is all files. In the filter string multiple filters can be specified
  separated by either two semicolons next to each other or separated by newlines. To add
  two filters, one to show all C++ files and one to show all header files, the filter
  string could look like "C++ Files (*.cpp *.cc *.C *.cxx *.c++);;Header Files (*.h *.hxx *.h++)"

  \a dir is the starting directory.  If \a
  dir is not supplied, QFileDialog picks something presumably useful
  (such as the directory where the user selected something last, or
  the current working directory).

  \a parent is a widget over which the dialog should be positioned and
  \a name is the object name of the temporary QFileDialog object.

  Example:

  \code
    QStringList s( QFileDialog::getOpenFileNames() );
    // do something with the files in s.
  \endcode

  NOTE: In the windows version of Qt this static method uses the native
  windows file dialog, and not the QFileDialog.
*/

QStringList QFileDialog::getOpenFileNames( const QString & filter,
					   const QString& dir,
					   QWidget *parent,
					   const char* name,
					   const QString& caption )
{
    QStringList filters;
    if ( !filter.isEmpty() )
	filters = makeFiltersList( filter );

    makeVariables();

    if ( workingDirectory->isNull() )
	*workingDirectory = QDir::currentDirPath();

    if ( !dir.isEmpty() ) {
	// #### works only correct for local files
	QUrlOperator u( dir );
	if ( u.isLocalFile() && QFileInfo( u ).isDir() ) {
	    *workingDirectory = dir;
	} else {
	    *workingDirectory = u.toString();
	}
    }

#if defined(_WS_WIN_)
    if ( qApp->style() == WindowsStyle )
	return winGetOpenFileNames( filter, workingDirectory, parent, name, caption );
#endif

    QFileDialog *dlg = new QFileDialog( *workingDirectory, QString::null,
					parent, name, TRUE );
    CHECK_PTR( dlg );
    if ( parent && parent->icon() && !parent->icon()->isNull() )
	dlg->setIcon( *parent->icon() );
    else if ( qApp->mainWidget() && qApp->mainWidget()->icon() && !qApp->mainWidget()->icon()->isNull() )
	dlg->setIcon( *qApp->mainWidget()->icon() );

    dlg->setFilters( filters );
    if ( !caption.isNull() )
	dlg->setCaption( caption );
    else
	dlg->setCaption( QFileDialog::tr("Open") );
    dlg->setMode( QFileDialog::ExistingFiles );
    QString result;
    QStringList lst;
    if ( dlg->exec() == QDialog::Accepted ) {
	lst = dlg->selectedFiles();
	*workingDirectory = dlg->d->url;
    }
    delete dlg;
    return lst;
}


/*!\overload
 */
QStringList QFileDialog::getOpenFileNames( const QString & filter,
					   const QString& dir,
					   QWidget *parent,
					   const char* name )
{
    return getOpenFileNames( filter, dir, parent, name, QString::null );
}



/*!  Updates the line edit to match the speed-key usage in QListView. */

void QFileDialog::fixupNameEdit()
{
    if ( files->currentItem() && d->mode != ExistingFiles ) {
	if ( ( (QFileDialogPrivate::File*)files->currentItem() )->info.isFile() )
	    nameEdit->setText( files->currentItem()->text( 0 ) );
    }
}

/*!
  Returns the URL of the current working directory.
*/

QUrl QFileDialog::url() const
{
    return d->url;
}

static bool isRoot( const QUrl &u )
{
#if defined(_OS_UNIX_)
    if ( u.path() == "/" )
	return TRUE;
#elif defined(_OS_WIN32_)
    QString p = u.path();
    if ( p.length() == 3 &&
	 p.right( 2 ) == ":/" )
	return TRUE;
    if ( p[ 0 ] == '/' && p[ 1 ] == '/' ) {
	int slashes = p.contains( '/' );
	if ( slashes <= 3 )
	    return TRUE;
	if ( slashes == 4 && p[ (int)p.length() - 1 ] == '/' )
	    return TRUE;
    }
#endif

    if ( !u.isLocalFile() && u.path() == "/" )
	return TRUE;

    return FALSE;
}

void QFileDialog::urlStart( QNetworkOperation *op )
{
    if ( !op )
	return;

    if ( op->operation() == QNetworkProtocol::OpListChildren ) {
	if ( isRoot( d->url ) )
	    d->cdToParent->setEnabled( FALSE );
	else
	    d->cdToParent->setEnabled( TRUE );
	d->mimeTypeTimer->stop();
	d->sortedList.clear();
	d->pendingItems.clear();
	d->moreFiles->clearSelection();
	files->clearSelection();
	d->moreFiles->clear();
	files->clear();
	files->setSorting( -1 );

	QString s = d->url.toString( FALSE, FALSE );
	bool found = FALSE;
	for ( int i = 0; i < d->paths->count(); ++i ) {
	    if ( d->paths->text( i ) == s ) {
		found = TRUE;
		d->paths->setCurrentItem( i );
		break;
	    }
	}
	if ( !found ) {
	    d->paths->insertItem( *openFolderIcon, s, -1 );
	    d->paths->setCurrentItem( d->paths->count() - 1 );
	}
	d->last = 0;
	d->hadDotDot = FALSE;

	if ( d->goBack && d->history.last() != d->url.toString() ) {
	    d->history.append( d->url.toString() );
	    if ( d->history.count() > 1 )
		d->goBack->setEnabled( TRUE );
	}
    }
}

void QFileDialog::urlFinished( QNetworkOperation *op )
{
    if ( !op )
	return;

    if ( op->state() == QNetworkProtocol::StFailed ) {
	if ( d->paths->hasFocus() )
	    d->ignoreNextKeyPress = TRUE;

	if ( d->progressDia ) {
	    d->ignoreStop = TRUE;
	    d->progressDia->close();
	    delete d->progressDia;
	    d->progressDia = 0;
	}

	if ( isVisible() )
	    QMessageBox::critical( this, tr( "ERROR" ), op->protocolDetail() );

	int ecode = op->errorCode();
	if ( ecode == QNetworkProtocol::ErrListChlidren || ecode == QNetworkProtocol::ErrParse ||
	     ecode == QNetworkProtocol::ErrUnknownProtocol || ecode == QNetworkProtocol::ErrLoginIncorrect ||
	     ecode == QNetworkProtocol::ErrValid || ecode == QNetworkProtocol::ErrHostNotFound ||
	     ecode == QNetworkProtocol::ErrFileNotExisting ) {
	    d->url = d->oldUrl;
	    rereadDir();
	} else {
	    // another error happened, no need to go back to last dir
	}
    } else if ( op->operation() == QNetworkProtocol::OpListChildren &&
		op == d->currListChildren ) {
	if ( !d->hadDotDot && !isRoot( d->url ) ) {
	    bool ok = TRUE;
#if defined(_WS_WIN_)
	    if ( d->url.path().left( 2 ) == "//" )
		ok = FALSE;
#endif
	    if ( ok ) {
		QUrlInfo ui( d->url, ".." );
		ui.setName( ".." );
		ui.setDir( TRUE );
		ui.setFile( FALSE );
		ui.setSymLink( FALSE );
		ui.setSize( 0 );
		QValueList<QUrlInfo> lst;
		lst << ui;
		insertEntry( lst, 0 );
	    }
	}
	resortDir();
    } else if ( op->operation() == QNetworkProtocol::OpGet ) {
    } else if ( op->operation() == QNetworkProtocol::OpPut ) {
 	rereadDir();
	if ( d->progressDia ) {
	    d->ignoreStop = TRUE;
	    d->progressDia->close();
	}
	delete d->progressDia;
	d->progressDia = 0;
    }
}

void QFileDialog::dataTransferProgress( int bytesDone, int bytesTotal, QNetworkOperation *op )
{
    if ( !op )
	return;

    QString label;
    QUrl u( op->arg( 0 ) );
    if ( u.isLocalFile() ) {
	label = u.path();
    } else {
	label = QString( "%1 (on %2)" );
	label = label.arg( u.path() ).arg( u.host() );
    }

    if ( !d->progressDia ) {
	if ( bytesDone < bytesTotal) {
	    d->ignoreStop = FALSE;
	    d->progressDia = new QFDProgressDialog( this, label, bytesTotal );
	    connect( d->progressDia, SIGNAL( cancelled() ),
		     this, SLOT( stopCopy() ) );
	    d->progressDia->show();
	} else
	    return;
    }

    if ( d->progressDia ) {
	if ( op->operation() == QNetworkProtocol::OpGet ) {
	    if ( d->progressDia ) {
		d->progressDia->setReadProgress( bytesDone );
	    }
	} else if ( op->operation() == QNetworkProtocol::OpPut ) {
	    if ( d->progressDia ) {
		d->progressDia->setWriteLabel( label );
		d->progressDia->setWriteProgress( bytesDone );
	    }
	} else {
	    return;
	}
    }
}

void QFileDialog::insertEntry( const QValueList<QUrlInfo> &lst, QNetworkOperation *op )
{
    if ( op && op->operation() == QNetworkProtocol::OpListChildren &&
	 op != d->currListChildren )
	return;

    QValueList<QUrlInfo>::ConstIterator it = lst.begin();
    for ( ; it != lst.end(); ++it ) {
	const QUrlInfo &inf = *it;
	if ( d->mode == DirectoryOnly && !inf.isDir() )
	    continue;
	if ( inf.name() == ".." ) {
	    d->hadDotDot = TRUE;
	    if ( isRoot( d->url ) )
		continue;
#if defined(_WS_WIN_)
	    if ( d->url.path().left( 2 ) == "//" )
		continue;
#endif
	} else if ( inf.name() == "." )
	    continue;

	// check for hidden files
	// #### todo make this work on windows
	if ( !bShowHiddenFiles && inf.name() != ".." ) {
	    if ( inf.name()[ 0 ] == QChar( '.' ) )
		continue;
	}

	if ( !d->url.isLocalFile() ) {
	    QFileDialogPrivate::File * i = 0;
	    QFileDialogPrivate::MCItem *i2 = 0;
	    i = new QFileDialogPrivate::File( d, &inf, files );
	    i2 = new QFileDialogPrivate::MCItem( d->moreFiles, i );

	    if ( d->mode == ExistingFiles && inf.isDir() ||
                ( isDirectoryMode( d->mode ) && inf.isFile() ) ) {
		i->setSelectable( FALSE );
		i2->setSelectable( FALSE );
	    }

	    i->i = i2;
	}

	d->sortedList.append( new QUrlInfo( inf ) );
    }
}

void QFileDialog::removeEntry( QNetworkOperation *op )
{
    if ( !op )
	return;

    QUrlInfo *i = 0;
    QListViewItemIterator it( files );
    bool ok1 = FALSE, ok2 = FALSE;
    for ( i = d->sortedList.first(); it.current(); ++it, i = d->sortedList.next() ) {
	if ( ( (QFileDialogPrivate::File*)it.current() )->info.name() == op->arg( 0 ) ) {
	    d->pendingItems.removeRef( (QFileDialogPrivate::File*)it.current() );
	    delete ( (QFileDialogPrivate::File*)it.current() )->i;
	    delete it.current();
	    ok1 = TRUE;
	}
	if ( i && i->name() == op->arg( 0 ) ) {
	    d->sortedList.removeRef( i );
	    i = d->sortedList.prev();
	    ok2 = TRUE;
	}
	if ( ok1 && ok2 )
	    break;
    }
}

void QFileDialog::itemChanged( QNetworkOperation *op )
{
    if ( !op )
	return;

    QUrlInfo *i = 0;
    QListViewItemIterator it1( files );
    bool ok1 = FALSE, ok2 = FALSE;
    // first check whether the new file replaces an existing file.
    for ( i = d->sortedList.first(); it1.current(); ++it1, i = d->sortedList.next() ) {
	if ( ( (QFileDialogPrivate::File*)it1.current() )->info.name() == op->arg( 1 ) ) {
	    delete ( (QFileDialogPrivate::File*)it1.current() )->i;
	    delete it1.current();
	    ok1 = TRUE;
	}
	if ( i && i->name() == op->arg( 1 ) ) {
	    d->sortedList.removeRef( i );
	    i = d->sortedList.prev();
	    ok2 = TRUE;
	}
	if ( ok1 && ok2 )
	    break;
    }

    i = 0;
    QListViewItemIterator it( files );
    ok1 = FALSE;
    ok2 = FALSE;
    for ( i = d->sortedList.first(); it.current(); ++it, i = d->sortedList.next() ) {
	if ( ( (QFileDialogPrivate::File*)it.current() )->info.name() == op->arg( 0 ) ) {
	    ( (QFileDialogPrivate::File*)it.current() )->info.setName( op->arg( 1 ) );
	    ok1 = TRUE;
	}
	if ( i && i->name() == op->arg( 0 ) ) {
	    i->setName( op->arg( 1 ) );
	    ok2 = TRUE;
	}
	if ( ok1 && ok2 )
	    break;
    }

    resortDir();
}

/*!
  Returns TRUE if the file dialog offers the user
  the possibility to preview the information of
  the currently selected file.

  \sa setInfoPreviewEnabled()
*/
bool QFileDialog::isInfoPreviewEnabled() const
{
    return d->infoPreview;
}

/*!
  Returns TRUE if the file dialog offers the user
  the possibility to preview the contents of
  the currently selected file.

  \sa setContentsPreviewWidget()
*/

bool QFileDialog::isContentsPreviewEnabled() const
{
    return d->contentsPreview;
}

/*!
  Specifies if the filedialog should offer the possibility
  to preview the information of the currently selected
  file, if \a info is TRUE, else not.

  \sa setInfoPreview()
*/

void QFileDialog::setInfoPreviewEnabled( bool info )
{
    if ( info == d->infoPreview )
	return;
    d->geometryDirty = TRUE;
    d->infoPreview = info;
    updateGeometries();
}

/*!
  Specifies if the filedialog should offer the possibility
  to preview the contents of the currently selected
  file, if \a contents is TRUE, else not.

  \sa setInfoPreview()
*/

void QFileDialog::setContentsPreviewEnabled( bool contents )
{
    if ( contents == d->contentsPreview )
	return;
    d->geometryDirty = TRUE;
    d->contentsPreview = contents;
    updateGeometries();
}

/*!
  Sets the widget which should be used for displaying information
  of a file to \a w and the preview object of that to \a preview.

  Normally as preview widget you create a class which derives from
  a widget type class (which actually displays the preview) and
  from QFilePreview. So you will pass here two times the same pointer
  then.

  A implementation of a preview class could look like this:

  \code
  class MyPreview : public QWidget, public QFilePreview
  {
  public:
      MyPreview() : QWidget(), QFilePreview() {}
      // reimplementation from QFilePreview
      void previewUrl( const QUrl &url ) {
          QPainter p( this );
          p.drawThePreviewOfUrl();
          p.end();
      }
  }
  \endcode

  Later you would use this...

  \code
  MyPreview *preview = new MyPreview;
  fd.setInfoPreviewEnabled( TRUE );
  fd.setInfoPreview( preview, preview );
  \endcode
*/

void QFileDialog::setInfoPreview( QWidget *w, QFilePreview *preview )
{
    if ( !w || !preview )
	return;

    if ( d->infoPreviewWidget ) {
	d->preview->removeWidget( d->infoPreviewWidget );
	delete d->infoPreviewWidget;
    }
    if ( d->infoPreviewer )
	delete d->infoPreviewer;
    d->infoPreviewWidget = w;
    d->infoPreviewer = preview;
    w->recreate( d->preview, 0, QPoint( 0, 0 ) );
}

/*!
  Sets the widget which should be used for displaying the contents
  of a file to \a w and the preview object of that to \a preview.

  Normally as preview widget you create a class which derives from
  a widget type class (which actually displays the preview) and
  from QFilePreview. So you will pass here two times the same pointer
  then.

  A implementation of a preview class could look like this:

  \code
  class MyPreview : public QWidget, public QFilePreview
  {
  public:
      MyPreview() : QWidget(), QFilePreview() {}
      // reimplementation from QFilePreview
      void previewUrl( const QUrl &url ) {
          QPainter p( this );
          p.drawThePreviewOfUrl();
          p.end();
      }
  }
  \endcode

  Later you would use this...

  \code
  MyPreview *preview = new MyPreview;
  fd.setInfoPreviewEnabled( TRUE );
  fd.setInfoPreview( preview, preview );
  \endcode
*/

void QFileDialog::setContentsPreview( QWidget *w, QFilePreview *preview )
{
    if ( !w || !preview )
	return;

    if ( d->contentsPreviewWidget ) {
	d->preview->removeWidget( d->contentsPreviewWidget );
	delete d->contentsPreviewWidget;
    }
    if ( d->contentsPreviewer )
	delete d->contentsPreviewer;
    d->contentsPreviewWidget = w;
    d->contentsPreviewer = preview;
    w->recreate( d->preview, 0, QPoint( 0, 0 ) );
}

/*!
  Resorts the displayed directory
*/

void QFileDialog::resortDir()
{
    d->mimeTypeTimer->stop();
    d->pendingItems.clear();

    QFileDialogPrivate::File *item = 0;
    QFileDialogPrivate::MCItem *item2 = 0;

    d->sortedList.sort();

    if ( files->childCount() > 0 || d->moreFiles->count() > 0 ) {
	files->clear();
	d->last = 0;
	d->moreFiles->clear();
	files->setSorting( -1 );
    }

    QUrlInfo *i = sortAscending ? d->sortedList.first() : d->sortedList.last();
    for ( ; i; i = sortAscending ? d->sortedList.next() : d->sortedList.prev() ) {
	item = new QFileDialogPrivate::File( d, i, files );
	item2 = new QFileDialogPrivate::MCItem( d->moreFiles, item, item2 );
	item->i = item2;
	d->pendingItems.append( item );
	if ( d->mode == ExistingFiles && item->info.isDir() ||
            ( isDirectoryMode( d->mode ) && item->info.isFile() ) ) {
	    item->setSelectable( FALSE );
	    item2->setSelectable( FALSE );
	}
    }

    // ##### As the QFileIconProvider only support QFileInfo and no
    // QUrlInfo it can be only used for local files at the moment. In
    // 3.0 we have to change the API of QFileIconProvider to work on
    // QUrlInfo so that also remote filesystems can be show mime-type
    // specific icons.
    if ( d->url.isLocalFile() )
	d->mimeTypeTimer->start( 0 );
}

/*!
  Stops the current copy operation.
*/

void QFileDialog::stopCopy()
{
    if ( d->ignoreStop )
	return;

    d->url.blockSignals( TRUE );
    d->url.stop();
    if ( d->progressDia ) {
	d->ignoreStop = TRUE;
	QTimer::singleShot( 100, this, SLOT( removeProgressDia() ) );
    }
    d->url.blockSignals( FALSE );
}

/*!
  \internal
*/

void QFileDialog::removeProgressDia()
{
    if ( d->progressDia )
	delete d->progressDia;
    d->progressDia = 0;
}

/*!
  \internal
*/

void QFileDialog::doMimeTypeLookup()
{
    if ( !iconProvider() ) {
	d->pendingItems.clear();
	d->mimeTypeTimer->stop();
	return;
    }

    d->mimeTypeTimer->stop();
    if ( d->pendingItems.count() == 0 ) {
	return;
    }

    QRect r;
    QFileDialogPrivate::File *item = d->pendingItems.first();
    if ( item ) {
	QFileInfo fi;
	if ( d->url.isLocalFile() ) {
	    fi.setFile( QUrl( d->url.path(), item->info.name() ).path( FALSE ) );
	} else
	    fi.setFile( item->info.name() ); // #####
	const QPixmap *p = iconProvider()->pixmap( fi );
	if ( p && p != item->pixmap( 0 ) &&
	     ( !item->pixmap( 0 ) || p->serialNumber() != item->pixmap( 0 )->serialNumber() ) &&
	     p != fifteenTransparentPixels ) {
	    item->hasMimePixmap = TRUE;

	    // evil hack to avoid much too much repaints!
	    qApp->processEvents();
	    files->setUpdatesEnabled( FALSE );
	    files->viewport()->setUpdatesEnabled( FALSE );
	    if ( item != d->pendingItems.first() )
		return;
	    item->setPixmap( 0, *p );
	    qApp->processEvents();
	    files->setUpdatesEnabled( TRUE );
	    files->viewport()->setUpdatesEnabled( TRUE );

	    if ( files->isVisible() ) {
		QRect ir( files->itemRect( item ) );
		if ( ir != QRect( 0, 0, -1, -1 ) ) {
		    r = r.unite( ir );
		}
	    } else {
		QRect ir( d->moreFiles->itemRect( item->i ) );
		if ( ir != QRect( 0, 0, -1, -1 ) ) {
		    r = r.unite( ir );
		}
	    }
	}
	if ( d->pendingItems.count() )
	    d->pendingItems.removeFirst();
    }

    if ( d->moreFiles->isVisible() ) {
	d->moreFiles->viewport()->repaint( r, FALSE );
    } else {
	files->viewport()->repaint( r, FALSE );
    }

    if ( d->pendingItems.count() )
	d->mimeTypeTimer->start( 0 );
}

/*!
  If you pass TRUE for \a b all files are selected, otherwise they
  are de-selected. This only works in ExistingFiles mode.
*/

void QFileDialog::selectAll( bool b )
{
    if ( d->mode != ExistingFiles )
	return;
    d->moreFiles->selectAll( b );
    files->selectAll( b );
}

void QFileDialog::goBack()
{
    if ( !d->goBack || !d->goBack->isEnabled() )
	return;
    d->history.remove( d->history.last() );
    if ( d->history.count() < 2 )
	d->goBack->setEnabled( FALSE );
    setUrl( d->history.last() );
}

/*!
  \class QFilePreview qfiledialog.h
  \brief Abstract preview widget for the QFileDialog

  This class is an abstract base class which is used for implementing
  widgets which can display a preview of a file in the QFileDialog.

  If you want to do that you have to derive your preview widget
  from any QWidget and from this class. Then you have to reimplement
  the previewUrl() method of this class which is called by the filedialog
  if the preview of an URL should be shown.

  See also QFileDialog::setPreviewMode(), QFileDialog::setContentsPreview(),
  QFileDialog::setInfoPreview(), QFileDialog::setInfoPreviewEnabled(),
  QFileDialog::setContentsPreviewEnabled().

  For an example documentation of a preview widget look at the example
  qt/examples/qdir/qdir.cpp.
*/

/*!
  Constructor. Does nothing.
*/

QFilePreview::QFilePreview()
{
}

/*!
  \fn void QFilePreview::previewUrl( const QUrl &url )

  This method is called by QFileDialog if a preview
  for the \a url should be shown. Reimplement this
  method to do file/URL previews.
*/


#include "qfiledialog.moc"

#endif
