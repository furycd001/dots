/*****************************************************************

Copyright (c) 1996-2001 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <stdlib.h>
#include <unistd.h>

#include <qfile.h>
#include <qtooltip.h>

#include <kglobal.h>
#include <kdebug.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <dcopclient.h>
#include <kwin.h>
#include <kstddirs.h>
#include <kicontheme.h>

#include "containerarea.h"
#include "extensionmanager.h"

#include "panel.h"
#include "panel.moc"

Panel::Panel() : PanelContainer(0, "Panel")
  , DCOPObject("Panel")
{
    // FIXME: panelop_mnu crashes on start if we don't do this.
    // We really should get rid of PGlobal::panel altogether.
    PGlobal::panel = this;

    setAcceptDrops( true );

    containerAreaBox()->setFrameStyle(QFrame::StyledPanel  | QFrame::Raised);
    containerAreaBox()->setLineWidth(2);

    // applet area
    _containerArea = new ContainerArea( orientation(), true, KGlobal::config(), containerAreaBox() );
    _containerArea->setFrameStyle( QFrame::NoFrame );
    _containerArea->viewport()->installEventFilter( this );
    _containerArea->init();
    connect(_containerArea, SIGNAL(needScrollButtons(bool)), SLOT(showScrollButtons(bool)));
    connect(_containerArea, SIGNAL(sizeHintChanged()), SLOT(updateLayout()));

    connect(this, SIGNAL(positionChange(Position)), SLOT(slotSetPosition(Position)));

    // initialise
    readConfig();
}

Panel::~Panel()
{
//    kdDebug(1210) << "Panel::~Panel()" << endl;

    if(_containerArea)
        _containerArea->slotSaveContainerConfig();

    // Have all panels save configuration
    PanelContainer::writeContainerConfig();
}

void Panel::configure()
{
//    kdDebug(1210) << "Panel::configure()" << endl;

    PanelContainer::readContainerConfig();
}

void Panel::readConfig()
{
//    kdDebug(1210) << "Panel::readConfig()" << endl;

    KConfig *config = KGlobal::config();
    config->reparseConfiguration();
    emit configurationChanged();
    _containerArea->configure();

    config->setGroup("General");

    _size = static_cast<Size>(config->readNumEntry("Size", Normal));

    _panelsize = PGlobal::sizeValue(_size);

    PanelContainer::readConfig( config );

}

void Panel::writeConfig()
{
//    kdDebug(1210) << "Panel::writeConfig()" << endl;

    KConfig *config = KGlobal::config();
    config->setGroup("General");

    config->writeEntry("Size", static_cast<int>(_size));

    KSimpleConfig kdeglobals("kdeglobals", false);
    kdeglobals.setGroup("PanelIcons");
    kdeglobals.writeEntry("Size", (_size==Large) ? KIcon::SizeLarge :
	   ((_size==Normal) ? KIcon::SizeMedium : KIcon::SizeSmall));

    PanelContainer::writeConfig( config );

    config->sync();
}

void Panel::setSize(Size s)
{
    _size = s;
    _panelsize = PGlobal::sizeValue(s);
    updateLayout();
    writeConfig();
}

void Panel::slotSetPosition( Position p )
{
    _containerArea->setOrientation(orientation());
    _containerArea->setPosition(position());
}

void Panel::autoHide(bool hide)
{
    if(_containerArea->inMoveOperation()) // is the user currently moving a container around?
	return;
    PanelContainer::autoHide(hide);
}

void Panel::closeEvent( QCloseEvent *e )
{
    e->ignore();
}

void Panel::slotRestart()
{
//    kdDebug(1210) << "Panel::slotRestart()" << endl;
    
    char ** o_argv = new char*[2];
    o_argv[0] = strdup("kicker");
    o_argv[1] = 0L;

    writeConfig();
    delete PGlobal::panel;
    execv(QFile::encodeName(locate("exe", "kdeinit_wrapper")), o_argv);
    exit(1);
}

void Panel::restart()
{
    // do this on a timer to give us time to return true
    QTimer::singleShot(1000, this, SLOT(slotRestart()));
}

void Panel::addKMenuButton()
{
    _containerArea->addKMenuButton();
}

void Panel::addDesktopButton()
{
    _containerArea->addDesktopButton();
}

void Panel::addWindowListButton()
{
    _containerArea->addWindowListButton();
}

void Panel::addURLButton(const QString &url)
{
    _containerArea->addURLButton(url);
}

void Panel::addBrowserButton(const QString &startDir)
{
    _containerArea->addBrowserButton(startDir);
}

void Panel::addServiceMenuButton(const QString &name, const QString& relPath)
{
    _containerArea->addServiceMenuButton(name, relPath);
}

void Panel::addExeButton(const QString &filePath, const QString &icon, const QString &cmdLine, bool inTerm)
{
    _containerArea->addExeButton(filePath, icon, cmdLine, inTerm);
}

void Panel::addKonsoleButton()
{
    _containerArea->addKonsoleButton();
}

void Panel::addApplet( const QString &desktopFile )
{
    _containerArea->addApplet( desktopFile );
}

void Panel::addExtension( const QString &desktopFile )
{
    PGlobal::extensionManager->addExtension( desktopFile );
}

QSize Panel::sizeHint( Position p, QSize maxSize )
{
    QSize size = PanelContainer::sizeHint( p, maxSize );

    QSize ourSize;
    int w = _panelsize - size.width();
    int h = _panelsize - size.height();
    if( p == Top || p == Bottom ) {
	ourSize = QSize( _containerArea->minimumUsedSpace( Horizontal, w, h ), h );
    } else {
	ourSize = QSize( w, _containerArea->minimumUsedSpace( Vertical, w, h ) );
    }

//    kdDebug(1210) << "     Panel requests " << ourSize.width() << " x " << ourSize.height() << endl;

    return (size + ourSize).boundedTo( maxSize );
}

void Panel::scrollLeftUp()
{
    _containerArea->scrollLeftUp();
}

void Panel::scrollRightDown()
{
    _containerArea->scrollRightDown();
}
