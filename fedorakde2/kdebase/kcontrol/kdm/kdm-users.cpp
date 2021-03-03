/* This file is part of the KDE Display Manager Configuration package
    Copyright (C) 1997 Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <unistd.h>
#include <sys/types.h>


#include <qdragobject.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qwhatsthis.h>
#include <qvgroupbox.h>
#include <qstringlist.h>

#include <kapp.h>
#include <ksimpleconfig.h>
#include <kicondialog.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kimageio.h>
#include <kmessagebox.h>
#include "kdm-users.moc"


extern KSimpleConfig *c;

KDMUsersWidget::KDMUsersWidget(QWidget *parent, const char *name, QStringList *show_users)
    : KCModule(parent, name)
{
    m_userPixDir = KGlobal::dirs()->resourceDirs("data").last() + "kdm/pics/users/";
    m_defaultText = i18n("<default>");

    QString wtstr;
    QHBoxLayout *main = new QHBoxLayout(this, 10);
    QGridLayout *rLayout = new QGridLayout(main, 4, 3, 10);

    QLabel *a_label = new QLabel(i18n("&Remaining users"), this);
    rLayout->addWidget(a_label, 0, 0);

    QLabel *s_label = new QLabel(i18n("S&elected users"), this);
    rLayout->addWidget(s_label, 0, 2);

    QLabel *n_label = new QLabel(i18n("No-sho&w users"), this);
    rLayout->addWidget(n_label, 4, 2);

    QPushButton *all_to_no, *all_to_usr, *no_to_all, *usr_to_all;

    QSize sz(40, 20);

    all_to_usr = new QPushButton( ">>", this );
    all_to_usr->setFixedSize( sz );
    rLayout->addWidget(all_to_usr, 1, 1);
    connect( all_to_usr, SIGNAL( clicked() ), SLOT( slotAllToUsr() ) );
    connect( all_to_usr, SIGNAL(clicked()), this, SLOT(slotChanged()));
    QWhatsThis::add( all_to_usr, i18n("Click here to add the highlighted user on the left to"
      " the list of selected users on the right, i.e. users that should in any case be shown in KDM's user list.") );

    usr_to_all = new QPushButton( "<<", this );
    rLayout->addWidget(usr_to_all, 2, 1);
    usr_to_all->setFixedSize( sz );
    connect( usr_to_all, SIGNAL( clicked() ), SLOT( slotUsrToAll() ) );
    connect( usr_to_all, SIGNAL(clicked()), this, SLOT(slotChanged()));
    QWhatsThis::add( usr_to_all, i18n("Click here to remove the highlighted user from the list"
      " of selected users."));

    rLayout->setRowStretch(3, 1);

    all_to_no  = new QPushButton( ">>", this );
    rLayout->addWidget(all_to_no, 5, 1);
    all_to_no->setFixedSize( sz );
    connect( all_to_no, SIGNAL( clicked() ), SLOT( slotAllToNo() ) );
    connect( all_to_no, SIGNAL(clicked()), this, SLOT(slotChanged()));
    QWhatsThis::add( all_to_no, i18n("Click here to add the highlighted user on the left to the list"
      " of users explicitly not shown by KDM.") );

    no_to_all = new QPushButton( "<<", this );
    rLayout->addWidget(no_to_all, 6, 1);
    no_to_all->setFixedSize( sz );
    connect( no_to_all, SIGNAL( clicked() ), SLOT( slotNoToAll() ) );
    connect( no_to_all, SIGNAL(clicked()), this, SLOT(slotChanged()));
    QWhatsThis::add( no_to_all, i18n("Click here to remove the highlighted user from the list"
      " of users explicitly not shown by KDM.") );

    rLayout->setRowStretch(7, 1);

    remuserlb = new KListBox(this);
    a_label->setBuddy(remuserlb);
    rLayout->addMultiCellWidget(remuserlb, 1, 7, 0, 0);
    wtstr = i18n("This is the list of users for which no explicit show policy has been set,"
      " i.e. they will only be shown by KDM if \"Show users\" is \"all but no-show\".");
    QWhatsThis::add( remuserlb, wtstr );
    QWhatsThis::add( a_label, wtstr );

    userlb = new KListBox(this);
    s_label->setBuddy(userlb);
    rLayout->addMultiCellWidget(userlb, 1, 3, 2, 2);
    wtstr = i18n("This is the list of users KDM will show in its login dialog in any case.");
    QWhatsThis::add( userlb, wtstr );
    QWhatsThis::add( s_label, wtstr );

    nouserlb = new KListBox(this);
    n_label->setBuddy(nouserlb);
    rLayout->addMultiCellWidget(nouserlb, 5, 7, 2, 2);
    wtstr = i18n("This is the list of users KDM will not show in its login dialog."
      " Users (except for root) with a user ID less than the one specified in the \"Hide UIDs below\" field will not be shown, too.");
    QWhatsThis::add( nouserlb, wtstr );
    QWhatsThis::add( n_label, wtstr );

    connect( userlb, SIGNAL( highlighted( const QString & ) ),
             SLOT( slotUserSelected( const QString & ) ) );
    connect( remuserlb, SIGNAL( highlighted( const QString & ) ),
             SLOT( slotUserSelected( const QString & ) ) );
    connect( nouserlb, SIGNAL( highlighted( const QString & ) ),
             SLOT( slotUserSelected( const QString & ) ) );

    QVBoxLayout *lLayout = new QVBoxLayout(main, 10);
    lLayout->addSpacing(20);

    userlabel = new QLabel(" ", this );
    userlabel->setAlignment(AlignCenter);
    lLayout->addWidget(userlabel);

    userbutton = new KIconButton(this);
    userbutton->setAcceptDrops(true);
    userbutton->installEventFilter(this); // for drag and drop
    userbutton->setFixedSize(80, 80);
    connect(userbutton, SIGNAL(iconChanged(QString)), SLOT(slotUserPixChanged(QString)));
    connect(userbutton, SIGNAL(iconChanged(QString)), this, SLOT(slotChanged()));

    QToolTip::add(userbutton, i18n("Click or drop an image here"));
    lLayout->addWidget(userbutton);
    wtstr = i18n("Here you can see the username of the currently selected user and the"
      " image assigned to this user. Click on the image button to select from a list"
      " of images or drag and drop your own image onto the button (e.g. from Konqueror).");
    QWhatsThis::add( userlabel, wtstr );
    QWhatsThis::add( userbutton, wtstr );

    usrGroup = new QButtonGroup(5, Qt::Vertical, i18n("Show users"),  this );
    connect(usrGroup, SIGNAL(clicked(int)), this, SLOT(slotShowUsers(int)));
    connect(usrGroup, SIGNAL(clicked(int)), this, SLOT(slotChanged()));
    rbnoneusr = new QRadioButton(i18n("&None"), usrGroup );
    QWhatsThis::add(rbnoneusr, i18n("If this option is selected, KDM will not show any users."
      " If one of the alternative radio buttons is selected, KDM will show a list of users in its"
      " login dialog, so users can click on their name and image rather than typing"
      " in their login."));
    rbselusr = new QRadioButton(i18n("Selected onl&y"), usrGroup );
    QWhatsThis::add(rbselusr, i18n("If this option is selected, KDM will only show the users listed"
      " in the \"selected users\" listbox in its login dialog."));
    rballusr = new QRadioButton( i18n("A&ll but no-show"), usrGroup );
    QWhatsThis::add( rballusr, i18n("If this option is selected, KDM will show all users but those"
      " who are listed in the \"no-show users\" listbox or have user ID less then the one specified"
      " in the \"Hide UIDs below\" field (except root)."));
    cbusrsrt = new QCheckBox(i18n("Sor&t users"), usrGroup);
    connect(cbusrsrt, SIGNAL(toggled(bool)), this, SLOT(slotChanged()));
    QWhatsThis::add(cbusrsrt, i18n("This option tells KDM to alphabetically sort the users"
      " shown in its login dialog.") );
    lLayout->addWidget( usrGroup );

    minGroup = new QVGroupBox( i18n("Hide U&IDs below"), this );
    QWhatsThis::add( minGroup, i18n("In \"All but no-show\"-mode all users with a user ID less than"
      " this number are hidden. Note, that the root user is not affected by this and must be"
      " explicitly listed in \"No-show users\" to be hidden."));
    leminuid = new QLineEdit(minGroup);
    leminuid->setSizePolicy(
	QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Preferred));
    QValidator *valid = new QIntValidator(0, 999999, minGroup);
    leminuid->setValidator(valid);
    lLayout->addWidget( minGroup );

//    shwGroup = new QButtonGroup( 1, Qt::Vertical, this );
//    lLayout->addWidget( shwGroup );

    lLayout->addStretch( 1 );

    load(show_users);

    // read only mode
    if (getuid() != 0)
      {
	rbnoneusr->setEnabled(false);
	rbselusr->setEnabled(false);
	rballusr->setEnabled(false);
	cbusrsrt->setEnabled(false);
	leminuid->setReadOnly(true);
	userbutton->setEnabled(false);
	remuserlb->setEnabled(false);
	nouserlb->setEnabled(false);
	userlb->setEnabled(false);
	all_to_usr->setEnabled(false);
	usr_to_all->setEnabled(false);
	no_to_all->setEnabled(false);
	all_to_no->setEnabled(false);
      }
}

void KDMUsersWidget::slotShowUsers(int bt)
{
    if (bt != 3) {
	cbusrsrt->setEnabled(bt != 0);
	minGroup->setEnabled(bt == 2);
    }
}

void KDMUsersWidget::slotUserPixChanged(QString)
{
    QString user(userlabel->text().stripWhiteSpace());
    if(user == m_defaultText)
    {
       user = "default";
       if (KMessageBox::questionYesNo(this, i18n("Save image as default image?"))
            != KMessageBox::Yes)
          return;
    }

    QString userpix = m_userPixDir + user + ".png";
    const QPixmap *p = userbutton->pixmap();
    if(!p)
        return;
    if(!p->save(userpix, "PNG")) {
        QString msg = i18n("There was an error saving the image:\n%1\n")
        .arg(userpix);
        KMessageBox::sorry(this, msg);
    }
    userbutton->adjustSize();
}


bool KDMUsersWidget::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::DragEnter) {
        userButtonDragEnterEvent((QDragEnterEvent *) e);
        return true;
    }

    if (e->type() == QEvent::Drop) {
        userButtonDropEvent((QDropEvent *) e);
        return true;
    }

    return false;
}


void KDMUsersWidget::userButtonDragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QUriDrag::canDecode(e));
}


KURL *decodeImgDrop(QDropEvent *e, QWidget *wdg);

void KDMUsersWidget::userButtonDropEvent(QDropEvent *e)
{
    KURL *url = decodeImgDrop(e, this);
    if (url) {
	QString pixpath;
	KIO::NetAccess::download(*url, pixpath);
	QPixmap p(pixpath);
	KIO::NetAccess::removeTempFile(pixpath);
	if(!p.isNull()) { // Save image as <userpixdir>/<user>.<ext>
	    userbutton->setPixmap(p);
	    slotUserPixChanged(userlabel->text());
	} else {
	    QString msg = i18n("There was an error loading the image:\n"
				"%1\n"
				"It will not be saved...")
				.arg(url->prettyURL());
	    KMessageBox::sorry(this, msg);
	}
	delete url;
    }
}


void KDMUsersWidget::slotAllToNo()
{
    int id = remuserlb->currentItem();
    if (id < 0)
       return;
    QString user = remuserlb->currentText();
    if (user == m_defaultText)
       return;
    nouserlb->insertItem(user);
    nouserlb->sort();
    remuserlb->removeItem(id);
    emit show_user_remove (user);
}


void KDMUsersWidget::slotNoToAll()
{
    int id = nouserlb->currentItem();
    if (id < 0)
       return;
    QString user = nouserlb->currentText();
    remuserlb->insertItem(user);
    remuserlb->sort();
    nouserlb->removeItem(id);
    emit show_user_add (user);
}


void KDMUsersWidget::slotAllToUsr()
{
    int id = remuserlb->currentItem();
    if (id < 0)
       return;
    QString user = remuserlb->currentText();
    if (user == m_defaultText)
       return;
    userlb->insertItem(user);
    userlb->sort();
    remuserlb->removeItem(id);
}


void KDMUsersWidget::slotUsrToAll()
{
    int id = userlb->currentItem();
    if (id < 0)
       return;
    QString user = userlb->currentText();
    remuserlb->insertItem(user);
    remuserlb->sort();
    userlb->removeItem(id);
}


void KDMUsersWidget::slotUserSelected(const QString &user)
{
   userlabel->setText(user);

   QString name;
   if (user == m_defaultText)
      name = m_userPixDir + "default.png";
   else
      name = m_userPixDir + user + ".png";
   QPixmap p( name );
   if(p.isNull())
      p = QPixmap(m_userPixDir + "default.png");
   userbutton->setPixmap(p);
   userbutton->adjustSize();
}


void KDMUsersWidget::save()
{
    c->setGroup("X-*-Greeter");

    c->writeEntry( "SortUsers", cbusrsrt->isChecked() );

    c->writeEntry( "ShowUsers", 
	rballusr->isChecked() ? "All" :
	rbselusr->isChecked() ? "Selected" : "None");

    QString nousrstr;
    for(uint i = 0, j = nouserlb->count(); i < j; i++) {
        nousrstr.append(nouserlb->text(i));
        nousrstr.append(",");
    }
    c->writeEntry( "NoUsers", nousrstr );

    QString usrstr;
    for(uint i = 0, j = userlb->count(); i < j; i++) {
        usrstr.append(userlb->text(i));
        usrstr.append(",");
    }
    c->writeEntry( "Users", usrstr );

    c->writeEntry( "MinShowUID", leminuid->text());
}

#define CHECK_STRING( x) (x != 0 && x[0] != 0)

void KDMUsersWidget::load(QStringList *show_users)
{
    iconloader = KGlobal::iconLoader();
    QString str;

    c->setGroup("X-*-Greeter");

    // Read users from kdmrc and /etc/passwd
    QStringList users = c->readListEntry( "Users");
    userlb->clear();
    userlb->insertStringList(users);

    QStringList no_users = c->readListEntry( "NoUsers");
    nouserlb->clear();
    nouserlb->insertStringList(no_users);

    QStringList rem_users;

    struct passwd *ps;
    for( setpwent(); (ps = getpwent()) != 0; ) {
        if( CHECK_STRING(ps->pw_dir) && CHECK_STRING(ps->pw_shell) &&
            ( no_users.contains( ps->pw_name) == 0)
	) {
	    // kapp->processEvents(50) makes layout calculation to fail
	    // do we really need them here?
            //  kapp->processEvents(50);

	    // "Convenience" tab -> Auto-login-able users
    	    show_users->append( QString::fromLocal8Bit(ps->pw_name));

            if ( users.contains( ps->pw_name) == 0)
        	rem_users.append( QString::fromLocal8Bit(ps->pw_name));
        }
    }
    endpwent();

    // "Users" tab
    rem_users.append(m_defaultText);
    rem_users.sort();
    remuserlb->clear();
    remuserlb->insertStringList(rem_users);

    cbusrsrt->setChecked(c->readBoolEntry("SortUsers", true));

    leminuid->setText(c->readEntry("MinShowUID", "0"));

    QString su = c->readEntry( "ShowUsers");
    if (su == QString::fromLatin1("None")) {
	rbnoneusr->setChecked(true);
	slotShowUsers(0);
    } else if (su == QString::fromLatin1("Selected")) {
	rbselusr->setChecked(true);
	slotShowUsers(1);
    } else {
	rballusr->setChecked(true);
	slotShowUsers(2);
    }

    remuserlb->setCurrentItem(0);
    slotUserSelected(remuserlb->currentText());
}

#undef CHECK_STRING

void KDMUsersWidget::defaults()
{
    cbusrsrt->setChecked(true);
    rballusr->setChecked(true);
    slotShowUsers(2);
    leminuid->setText("0");
}


void KDMUsersWidget::slotChanged()
{
  emit KCModule::changed(true);
}
