/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

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

#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>

#include <kglobal.h>
#include <klocale.h>
#include <kicondialog.h>
#include <kfiledialog.h>
#include <klineedit.h>

#include "browser_dlg.h"
#include "browser_dlg.moc"

PanelBrowserDialog::PanelBrowserDialog( const QString& path, const QString &icon, QWidget *parent, const char *name )
    :  KDialogBase( parent, name, true, i18n( "Quick Browser Configuration" ), Ok|Cancel, Ok, true )
{
    setMinimumWidth( 300 );

    QVBox *page = makeVBoxMainWidget();

    QHBox *hbox2 = new QHBox( page );
    hbox2->setSpacing( KDialog::spacingHint() );
    QLabel *label1 = new QLabel( i18n( "Button Icon:" ), hbox2 );

    iconBtn = new KIconButton( hbox2 );
    iconBtn->setFixedSize( 50, 50 );
    iconBtn->setIconType( KIcon::Panel, KIcon::Application );
    label1->setBuddy( iconBtn );

    QHBox *hbox1 = new QHBox( page );
    hbox1->setSpacing( KDialog::spacingHint() );
    QLabel *label2 = new QLabel( i18n ( "Path:" ), hbox1 );
    pathInput = new KLineEdit( hbox1 );
    pathInput->setText( path );
    pathInput->setFocus();
    label2->setBuddy( pathInput );
    browseBtn = new QPushButton( i18n( "&Browse" ), hbox1 );
    if ( icon.isEmpty() ) {
        KURL u;
        u.setPath( path );
        iconBtn->setIcon( KMimeType::iconForURL( u ) );
    }
    else
        iconBtn->setIcon( icon );

    connect( browseBtn, SIGNAL( clicked() ), this, SLOT( browse() ) );
}

PanelBrowserDialog::~PanelBrowserDialog()
{

}

void PanelBrowserDialog::browse()
{
    QString dir = KFileDialog::getExistingDirectory( pathInput->text(), 0, i18n( "Select a directory" ) );
    if ( !dir.isEmpty() ) {
        pathInput->setText( dir );
        KURL u;
        u.setPath( dir );
        iconBtn->setIcon( KMimeType::iconForURL( u ) );
    }
}

const QString PanelBrowserDialog::icon()
{
    return iconBtn->icon();
}

QString PanelBrowserDialog::path()
{
    return pathInput->text();
}
