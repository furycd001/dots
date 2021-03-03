/*****************************************************************

Copyright (c) 2000 Matthias Elter

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

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>

#include "containerarea.h"
#include "containerareabox.h"
#include "addcontainer_mnu.h"

#include "childpanelextension.h"
#include "childpanelextension.moc"

Position p;

extern "C"
{
    KPanelExtension* init(QWidget *parent, const QString& configFile)
    {
      return new ChildPanelExtension(configFile, KPanelExtension::Stretch,
				     0, parent, "childpanelextension");
    }
}

ChildPanelExtension::ChildPanelExtension(const QString& configFile, Type type,
					 int actions, QWidget *parent, const char *name)
    : KPanelExtension(configFile, type, actions, parent, name)
, DCOPObject(QCString("ChildPanel_") + QString::number( (ulong) this ).latin1() )
,_size(1)
{

    // container area box
    _containerAreaBox = new ContainerAreaBox(this);
    _containerAreaBox->setFrameStyle(QFrame::StyledPanel  | QFrame::Raised);
    _containerAreaBox->setLineWidth(2);
    _containerAreaBox->installEventFilter( this );

    // container area
    _containerArea = new ContainerArea( orientation(), false, config(), _containerAreaBox );
    _containerArea->setFrameStyle( QFrame::NoFrame );
    _containerArea->viewport()->installEventFilter( this );
    _containerArea->init();
    _containerArea->show();

    // setup addmenu
    AddContainerMenu* addMnu = new AddContainerMenu(_containerArea, this);

    // setup size menu
    _sizeMnu = new QPopupMenu(this);
    _sizeMnu->setCheckable(true);
    _sizeMnu->insertItem(i18n("Tiny"), 0);
    _sizeMnu->insertItem(i18n("Small"), 1);
    _sizeMnu->insertItem(i18n("Normal"), 2);
    _sizeMnu->insertItem(i18n("Large"), 3);
    connect(_sizeMnu, SIGNAL(aboutToShow()), SLOT(slotSetupSizeMnu()));
    connect(_sizeMnu, SIGNAL(activated(int)), SLOT(slotSetSize(int)));

    // build op menu
    _opMnu = new QPopupMenu(this);
    _opMnu->insertItem(i18n("Add"), addMnu);
    _opMnu->insertItem(i18n("Size"), _sizeMnu);
    _opMnu->adjustSize();

    // restore size
    KConfig* c = config();
    c->setGroup("General");
    _size = c->readNumEntry("Size", 1);
}

ChildPanelExtension::~ChildPanelExtension()
{
}

void ChildPanelExtension::resizeEvent(QResizeEvent*)
{
    switch(position()) {
    case Left:
	p = ::Left; // This is from the global enum 'Position' in global.h
	break;
    case Right:
	p = ::Right;
	break;
    case Top:
	p = ::Top;
	break;
    case Bottom:
	p = ::Bottom;
	break;
    }
    _containerArea->setPosition(p);
    _containerArea->setOrientation(orientation());
    _containerAreaBox->setGeometry(0 ,0 , width(), height() );
}

QSize ChildPanelExtension::sizeHint(Position p, QSize maxSize) const
{
    int size = PGlobal::sizeValue((Size)_size);

    if (p == Left || p == Right)
	return QSize(size, maxSize.height());
    else
	return QSize(maxSize.width(), size);
}

bool ChildPanelExtension::eventFilter( QObject*, QEvent * e)
{
    switch ( e->type() ) {
    case QEvent::MouseButtonPress:
	{
	    QMouseEvent* me = (QMouseEvent*) e;
	    if ( me->button() == RightButton ) {
		if(_opMnu)
		    _opMnu->exec(me->globalPos());
	    }
	}
	break;
    default:
	break;
    }
    return false;
}

void ChildPanelExtension::slotSetupSizeMnu()
{
    _sizeMnu->setItemChecked(Tiny, false);
    _sizeMnu->setItemChecked(Normal, false);
    _sizeMnu->setItemChecked(Large, false);
    _sizeMnu->setItemChecked(_size, true);
}

void ChildPanelExtension::slotSetSize(int size)
{
    if (_size < 0 ) return;
    else if (_size > 3) return;
    _sizeMnu->setItemChecked(size, true);
    _sizeMnu->setItemChecked(_size, false);
    _size = size;

    // save size
    KConfig* c = config();
    c->setGroup("General");
    c->writeEntry("Size", _size);

    emit updateLayout();
}


void ChildPanelExtension::addKMenuButton()
{
    _containerArea->addKMenuButton();
}

void ChildPanelExtension::addDesktopButton()
{
    _containerArea->addDesktopButton();
}

void ChildPanelExtension::addWindowListButton()
{
    _containerArea->addWindowListButton();
}

void ChildPanelExtension::addURLButton(const QString &url)
{
    _containerArea->addURLButton(url);
}

void ChildPanelExtension::addBrowserButton(const QString &startDir)
{
    _containerArea->addBrowserButton(startDir);
}

void ChildPanelExtension::addServiceMenuButton(const QString &name, const QString& relPath)
{
    _containerArea->addServiceMenuButton(name, relPath);
}

void ChildPanelExtension::addExeButton(const QString &filePath, const QString &icon, const QString &cmdLine, bool inTerm)
{
    _containerArea->addExeButton(filePath, icon, cmdLine, inTerm);
}

void ChildPanelExtension::addApplet(const QString &desktopFile, bool internal)
{
    _containerArea->addApplet(desktopFile, internal);
}

void ChildPanelExtension::addAppletContainer(const QString &desktopFile, bool internal) // bad name, can this go away?####
{
    addAppletContainer( desktopFile, internal );
}

