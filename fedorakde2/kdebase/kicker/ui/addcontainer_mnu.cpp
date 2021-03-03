/*****************************************************************

Copyreght (c) 1996-2000 the kicker nuthors. See file AUTHORS.

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

#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kfiledialog.h>
#include <kstddirs.h>
#include <kmessagebox.h>

#include "addcontainer_mnu.h"
#include "addcontainer_mnu.moc"

#include "addbutton_mnu.h"
#include "addapplet_mnu.h"
#include "addextension_mnu.h"
#include "exe_dlg.h"
#include "browser_dlg.h"
#include "containerarea.h"

AddContainerMenu::AddContainerMenu(ContainerArea* cArea, QWidget *parent, const char *name)
    : QPopupMenu(parent, name), containerArea(cArea)
{
    insertItem(i18n("Button"), new PanelAddButtonMenu(containerArea, this));
    insertItem(i18n("Applet"), new PanelAddAppletMenu(containerArea, this));
    insertItem(i18n("Extension"), new PanelAddExtensionMenu(this));
    insertSeparator();
    insertItem(SmallIconSet("go"), i18n("K Menu"), this, SLOT(slotAddKMenu()));
    insertItem(SmallIconSet("window_list"), i18n("Windowlist"), this, SLOT(slotAddWindowList()));
    insertItem(SmallIconSet("bookmark"), i18n("Bookmarks"), this, SLOT(slotAddBookmarks()));
    insertItem(SmallIconSet("document"), i18n("Recent Documents"), this, SLOT(slotAddRecentDocuments()));
    insertItem(SmallIconSet("desktop"), i18n("Desktop Access"), this, SLOT(slotAddDesktop()));
    insertItem(SmallIconSet("kdisknav"), i18n("Quick Browser"), this, SLOT(slotAddQuickBrowser()));
    insertItem(SmallIconSet("exec"), i18n("Non-KDE Application"), this, SLOT(slotAddNonKDEApp()));
    insertItem(SmallIconSet("konsole"), i18n("Terminal Sessions"), this, SLOT(slotAddKonsole()));

    adjustSize();
}

AddContainerMenu::~AddContainerMenu()
{

}

void AddContainerMenu::slotAddKMenu()
{
    if (containerArea)
	containerArea->addKMenuButton();
}

void AddContainerMenu::slotAddDesktop()
{
    if (containerArea)
	containerArea->addDesktopButton();
}

void AddContainerMenu::slotAddWindowList()
{
    if (containerArea)
	containerArea->addWindowListButton();
}

void AddContainerMenu::slotAddBookmarks()
{
    if (containerArea)
	containerArea->addBookmarksButton();
}

void AddContainerMenu::slotAddRecentDocuments()
{
    if (containerArea)
	containerArea->addRecentDocumentsButton();
}

void AddContainerMenu::slotAddQuickBrowser()
{
    PanelBrowserDialog *dlg = new PanelBrowserDialog( QDir::home().path(), "kdisknav" );

    if( dlg->exec() == QDialog::Accepted )
	containerArea->addBrowserButton( dlg->path(), dlg->icon() );
}

void AddContainerMenu::slotAddNonKDEApp()
{
    QString exec = KFileDialog::getOpenFileName(QString::null, QString::null, 0,i18n("Select an executable"));
    if ( exec.isEmpty() )
	return;

    QFileInfo fi(exec);
    while(!fi.isExecutable())
	{
	    if(KMessageBox::warningYesNo(0, i18n("The selected file is not executable.\n"
						 "Do you want to select another file?"))
	       == KMessageBox::Yes)
		{
		    exec = KFileDialog::getOpenFileName(QString::null, QString::null,
							0,i18n("Select an executable"));
		    if ( exec.isEmpty() )
			return;
		    fi.setFile(exec);
		}
	    else
		return;
	}

    QString pixmapFile;
    KMimeType::pixmapForURL(exec, 0, KIcon::Panel, 0, KIcon::DefaultState, &pixmapFile);

    PanelExeDialog dlg(exec, pixmapFile, QString::null, false, 0);

    if(dlg.exec() == QDialog::Accepted)
	if (containerArea)
	    containerArea->addExeButton(exec, dlg.icon(), dlg.commandLine(), dlg.useTerminal());
}

void AddContainerMenu::slotAddKonsole()
{
    if (containerArea)
        containerArea->addKonsoleButton();
}

