//
// KDE Shortcut config module
//
// Copyright (c)  Mark Donohoe 1998
// Copyright (c)  Matthias Ettrich 1998
// Converted to generic key configuration module, Duncan Haldane 1998.
// Layout fixes copyright (c) 2000 Preston Brown <pbrown@kde.org>

#include <config.h>
#include <stdlib.h>

#include <unistd.h>

#include <qlabel.h>
#include <qdir.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>
#include <kaccel.h>
#include <kwin.h>
#include <kdialog.h>
#include <kseparator.h>
#include <dcopclient.h>
#include <kapp.h>
#include <kaccel.h>	// Used in KKeyModule::init()

#include "keyconfig.h"
#include "keyconfig.moc"

#define KICKER_ALL_BINDINGS

//----------------------------------------------------------------------------

KKeyModule::KKeyModule( QWidget *parent, bool isGlobal, bool bSeriesOnly, bool bSeriesNone, const char *name )
  : KCModule( parent, name )
{
	init( isGlobal, bSeriesOnly, bSeriesNone );
}

KKeyModule::KKeyModule( QWidget *parent, bool isGlobal, const char *name )
  : KCModule( parent, name )
{
	init( isGlobal, false, false );
}

void KKeyModule::init( bool isGlobal, bool _bSeriesOnly, bool bSeriesNone )
{
  QString wtstr;

  KeyType = isGlobal ? "global" : "standard";

  keys = new KAccel( this );

  bSeriesOnly = _bSeriesOnly;

  if ( KeyType == "global" ) {
// see also KKeyModule::init() below !!!
#define WITH_LABELS
#include "../../kwin/kwinbindings.cpp"
#include "../../kicker/core/kickerbindings.cpp"
#include "../../kdesktop/kdesktopbindings.cpp"
#include "../../klipper/klipperbindings.cpp"
#include "../../kxkb/kxkbbindings.cpp"
#undef WITH_LABELS
    KeyScheme = "Global Key Scheme " ;
    KeySet    = "Global Keys" ;
    // Sorting Hack: I'll re-write the module once feature-adding begins again.
    if( bSeriesOnly || bSeriesNone ) {
	KKeyMapOrder *pMapOrder = &keys->keyInsertOrder();
	int j = 0;
	for( int i = 0; i < (int)pMapOrder->count(); i++ ) {
		QString sConfigKey = (*pMapOrder)[i];
		kdDebug(125) << "sConfigKey: " << sConfigKey << endl;
		int iLastSpace = sConfigKey.findRev( ' ' );
		bool bIsNum = false;
		if( iLastSpace >= 0 )
			sConfigKey.mid( iLastSpace+1 ).toInt( &bIsNum );

		if( (bSeriesOnly && bIsNum) || (bSeriesNone && !bIsNum) || sConfigKey.contains( ':' ) )
			mapOrder[j++] = sConfigKey;
	}
    }
  }

  if ( KeyType == "standard" ) {
    for(uint i=0; i<KStdAccel::NB_STD_ACCELS; i++) {
      KStdAccel::StdAccel id = (KStdAccel::StdAccel)i;
      keys->insertItem( KStdAccel::description(id),
                        KStdAccel::action(id),
                        KStdAccel::defaultKey3(id),
                        KStdAccel::defaultKey4(id),
                        true );
    }

    KeyScheme = "Standard Key Scheme " ;
    KeySet    = "Keys" ;
  }

  keys->setConfigGlobal( true );
  keys->setConfigGroup( KeySet );
  keys->readSettings();

  sFileList = new QStringList();
  sList = new QListBox( this );

  readSchemeNames();
  sList->setCurrentItem( 0 );
  connect( sList, SIGNAL( highlighted( int ) ),
           SLOT( slotPreviewScheme( int ) ) );

  QLabel *label = new QLabel( sList, i18n("&Key Scheme"), this );

  wtstr = i18n("Here you can see a list of the existing key binding schemes with 'Current scheme'"
    " referring to the settings you are using right now. Select a scheme to use, remove or"
    " change it.");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( sList, wtstr );

  addBt = new QPushButton(  i18n("&Save Scheme..."), this );
  connect( addBt, SIGNAL( clicked() ), SLOT( slotAdd() ) );
  QWhatsThis::add(addBt, i18n("Click here to add a new key bindings scheme. You will be prompted for a name."));

  removeBt = new QPushButton(  i18n("&Remove Scheme"), this );
  removeBt->setEnabled(FALSE);
  connect( removeBt, SIGNAL( clicked() ), SLOT( slotRemove() ) );
  QWhatsThis::add( removeBt, i18n("Click here to remove the selected key bindings scheme. You can not"
    " remove the standard system wide schemes, 'Current scheme' and 'KDE default'.") );

  // Hack to get this setting only displayed once.  It belongs in main.cpp instead.
  // That move will take a lot of UI redesigning, though, so i'll do it once CVS
  //  opens up for feature commits again. -- ellis
  /* Needed to remove because this depended upon non-BC changes in KeyEntry.
  / If this is the "Global Keys" section of the KDE Control Center:
  if( isGlobal && !bSeriesOnly ) {
	preferMetaBt = new QCheckBox( i18n("Prefer 4-Modifier Defaults"), this );
	if( !KAccel::keyboardHasMetaKey() )
		preferMetaBt->setEnabled( false );
	preferMetaBt->setChecked( KAccel::useFourModifierKeys() );
	connect( preferMetaBt, SIGNAL(clicked()), SLOT(slotPreferMeta()) );
	QWhatsThis::add( preferMetaBt, i18n("If your keyboard has a Meta key, but you would "
		"like KDE to prefer the 3-modifier configuration defaults, then this option "
		"should be unchecked.") );
  } else*/
	preferMetaBt = 0;

  KSeparator* line = new KSeparator( KSeparator::HLine, this );

  dict = keys->keyDict();

  if ( KeyType == "global" )
    kc = new KeyChooserSpec( &dict, mapOrder.count() ? &mapOrder : &keys->keyInsertOrder(), this, isGlobal );
  else
    kc =  new KeyChooserSpec( &dict, this, isGlobal );
  connect( kc, SIGNAL( keyChange() ), this, SLOT( slotChanged() ) );

  readScheme();

  QGridLayout *topLayout = new QGridLayout( this, 6, 2,
                                            KDialog::marginHint(),
                                            KDialog::spacingHint());
  topLayout->addWidget(label, 0, 0);
  topLayout->addMultiCellWidget(sList, 1, 2, 0, 0);
  topLayout->addWidget(addBt, 1, 1);
  topLayout->addWidget(removeBt, 2, 1);
  if( preferMetaBt )
    topLayout->addWidget(preferMetaBt, 3, 0);
  topLayout->addMultiCellWidget(line, 4, 4, 0, 1);
  topLayout->addRowSpacing(3, 15);
  topLayout->addMultiCellWidget(kc, 5, 5, 0, 1);

  setMinimumSize(topLayout->sizeHint());
}

KKeyModule::~KKeyModule (){
  //kdDebug() << "KKeyModule destructor" << endl;
  delete keys;
}

void KKeyModule::load()
{
  for (KKeyEntryMap::Iterator it = dict.begin(); it != dict.end(); ++it)
  {
      (*it).aConfigKeyCode = (*it).aCurrentKeyCode;
  }
  kc->listSync();
}

void KKeyModule::save()
{
  if( preferMetaBt )
    KAccel::useFourModifierKeys( preferMetaBt->isChecked() );

  keys->setKeyDict( dict );
  keys->writeSettings();
  if ( KeyType == "global" ) {
    if ( !kapp->dcopClient()->isAttached() )
      kapp->dcopClient()->attach();
    kapp->dcopClient()->send("kwin", "", "reconfigure()", "");
    kapp->dcopClient()->send("kdesktop", "", "configure()", "");
    kapp->dcopClient()->send("kicker", "Panel", "configure()", "");
  }
}

void KKeyModule::defaults()
{
  if( preferMetaBt )
    preferMetaBt->setChecked( false );
  KAccel::useFourModifierKeys( false );
  kc->allDefault();
}

void KKeyModule::slotRemove()
{
  QString kksPath =
        KGlobal::dirs()->saveLocation("data", "kcmkeys/" + KeyType);

  QDir d( kksPath );
  if (!d.exists()) // what can we do?
    return;

  d.setFilter( QDir::Files );
  d.setSorting( QDir::Name );
  d.setNameFilter("*.kksrc");

  uint ind = sList->currentItem();

  if ( !d.remove( *sFileList->at( ind ) ) ) {
    KMessageBox::sorry( 0,
                        i18n("This key scheme could not be removed.\n"
                             "Perhaps you do not have permission to alter the file\n"
                             "system where the key scheme is stored." ));
    return;
  }

  sList->removeItem( ind );
  sFileList->remove( sFileList->at(ind) );
}

void KKeyModule::slotChanged( )
{
  emit changed(true);
  emit keysChanged( &dict );
}

void KKeyModule::slotSave( )
{
    KSimpleConfig config(*sFileList->at( sList->currentItem() ) );
    //  global=true is necessary in order to
    //  let both 'Global Shortcuts' and 'Shortcut Sequences' be
    //  written to the same scheme file.
    KAccel::writeKeyMap( dict, KeyScheme, &config, KeyType == "global" );
}

void KKeyModule::slotPreferMeta()
{
	kc->setPreferFourModifierKeys( preferMetaBt->isChecked() );
}

void KKeyModule::readScheme( int index )
{
  kdDebug(125) << "readScheme( " << index << " )\n";
  if( index == 1 )
    kc->allDefault( false );
  //else if( index == 2 )
  //  kc->allDefault( true );
  else {
    KConfigBase* config;
    if( index == 0 )	config = new KConfig( "kdeglobals" );
    else		config = new KSimpleConfig( *sFileList->at( index ), true );

    KAccel::readKeyMap( dict, index == 0 ? KeySet : KeyScheme, config );
    delete config;
  }
  kc->listSync();
}

void KKeyModule::slotAdd()
{
  QString sName;

  if ( sList->currentItem() >= nSysSchemes )
     sName = sList->currentText();
  SaveScm ss( 0,  "save scheme", sName );

  bool nameValid;
  QString sFile;
  int exists = -1;

  do {

    nameValid = TRUE;

    if ( ss.exec() ) {
      sName = ss.nameLine->text();
      if ( sName.stripWhiteSpace().isEmpty() )
        return;

      sName = sName.simplifyWhiteSpace();
      sFile = sName;

      int ind = 0;
      while ( ind < (int) sFile.length() ) {

        // parse the string for first white space

        ind = sFile.find(" ");
        if (ind == -1) {
          ind = sFile.length();
          break;
        }

        // remove from string

        sFile.remove( ind, 1);

        // Make the next letter upper case

        QString s = sFile.mid( ind, 1 );
        s = s.upper();
        sFile.replace( ind, 1, s );

      }

      exists = -1;
      for ( int i = 0; i < (int) sList->count(); i++ ) {
        if ( sName.lower() == (sList->text(i)).lower() ) {
          exists = i;

          int result = KMessageBox::warningContinueCancel( 0,
               i18n("A key scheme with the name '%1' already exists.\n"
                    "Do you want to overwrite it?\n").arg(sName),
		   i18n("Save key scheme"),
                   i18n("Overwrite"));
          if (result == KMessageBox::Continue)
             nameValid = true;
          else
             nameValid = false;
        }
      }
    } else return;

  } while ( nameValid == FALSE );

  disconnect( sList, SIGNAL( highlighted( int ) ), this,
              SLOT( slotPreviewScheme( int ) ) );


  QString kksPath = KGlobal::dirs()->saveLocation("data", "kcmkeys/");

  QDir d( kksPath );
  if ( !d.exists() )
    if ( !d.mkdir( kksPath ) ) {
      qWarning("KKeyModule: Could not make directory to store user info.");
      return;
    }

  kksPath +=  KeyType ;
  kksPath += "/";

  d.setPath( kksPath );
  if ( !d.exists() )
    if ( !d.mkdir( kksPath ) ) {
      qWarning("KKeyModule: Could not make directory to store user info.");
      return;
    }

  sFile.prepend( kksPath );
  sFile += ".kksrc";
  if (exists == -1)
  {
     sList->insertItem( sName );
     sList->setFocus();
     sList->setCurrentItem( sList->count()-1 );
     sFileList->append( sFile );
  }
  else
  {
     sList->setFocus();
     sList->setCurrentItem( exists );
  }

  KSimpleConfig *config =
    new KSimpleConfig( sFile );

  config->setGroup( KeyScheme );
  config->writeEntry( "Name", sName );
  delete config;

  slotSave();

  connect( sList, SIGNAL( highlighted( int ) ), this,
           SLOT( slotPreviewScheme( int ) ) );

  slotPreviewScheme( sList->currentItem() );
}

void KKeyModule::slotPreviewScheme( int indx )
{
  readScheme( indx );

  // Set various appropriate for the scheme

  if ( indx < nSysSchemes ||
       (*sFileList->at(indx)).contains( "/global-" ) ||
       (*sFileList->at(indx)).contains( "/app-" ) ) {
    removeBt->setEnabled( FALSE );
  } else {
    removeBt->setEnabled( TRUE );
  }
}

void KKeyModule::readSchemeNames( )
{
  QStringList schemes = KGlobal::dirs()->findAllResources("data", "kcmkeys/" + KeyType + "/*.kksrc");
  //QRegExp r( "-kde[34].kksrc$" );
  QRegExp r( "-kde3.kksrc$" );

  sList->clear();
  sFileList->clear();
  sList->insertItem( i18n("Current Scheme"), 0 );
  sFileList->append( "Not a kcsrc file" );
  sList->insertItem( i18n("KDE Default for 3 Modifiers (Alt/Ctrl/Shift)"), 1 );
  sFileList->append( "Not a kcsrc file" );
  //sList->insertItem( i18n("KDE Default for 4 Modifiers (Meta/Alt/Ctrl/Shift)"), 2 );
  //sFileList->append( "Not a kcsrc file" );
  nSysSchemes = 2;

  // This for system files
  for ( QStringList::ConstIterator it = schemes.begin(); it != schemes.end(); it++) {
    // KPersonalizer relies on .kksrc files containing all the keyboard shortcut
    //  schemes for various setups.  It also requires the KDE defaults to be in
    //  a .kksrc file.  The KDE defaults shouldn't be listed here.
    if( r.match( *it ) != -1 )
       continue;

    KSimpleConfig config( *it, true );
    config.setGroup( KeyScheme );
    QString str = config.readEntry( "Name" );

    sList->insertItem( str );
    sFileList->append( *it );
  }
}

void KKeyModule::updateKeys( const KKeyEntryMap* map_P )
    {
    kc->updateKeys( map_P );
    }

// write all the global keys to kdeglobals
// this is needed to be able to check for conflicts with global keys in app's keyconfig
// dialogs, kdeglobals is empty as long as you don't apply any change in controlcenter/keys
void KKeyModule::init()
{
  kdDebug(125) << "KKeyModule::init()\n";

    {
    KSimpleConfig cfg( "kdeglobals" );
    cfg.deleteGroup( "Global Keys" );
    }

  /*kdDebug(125) << "KKeyModule::init() - Initialize # Modifier Keys Settings\n";
  KConfigGroupSaver cgs( KGlobal::config(), "Keyboard" );
  QString fourMods = KGlobal::config()->readEntry( "Use Four Modifier Keys", KAccel::keyboardHasMetaKey() ? "true" : "false" );
  KAccel::useFourModifierKeys( fourMods == "true" );
  bool bUseFourModifierKeys = KAccel::useFourModifierKeys();
  KGlobal::config()->writeEntry( "User Four Modifier Keys", bUseFourModifierKeys ? "true" : "false", true, true );
  */
  QWidget workaround;
  KAccel* keys = new KAccel( &workaround );

  kdDebug(125) << "KKeyModule::init() - Load Included Bindings\n";
// this should match the included files above
#include "../../klipper/klipperbindings.cpp"
#include "../../kwin/kwinbindings.cpp"
#include "../../kicker/core/kickerbindings.cpp"
#include "../../kdesktop/kdesktopbindings.cpp"
#include "../../kxkb/kxkbbindings.cpp"

  kdDebug(125) << "KKeyModule::init() - Read Modifier Mapping\n";
  KAccel::readModifierMapping();

  kdDebug(125) << "KKeyModule::init() - Read Config Bindings\n";
  keys->setConfigGlobal( true );
  keys->setConfigGroup( "Global Keys" );
  keys->readSettings();

  kdDebug(125) << "KKeyModule::init() - Write Config Bindings\n";
  keys->writeSettings();
}

//-----------------------------------------------------------------
// KeyChooserSpec
//-----------------------------------------------------------------

KeyChooserSpec::KeyChooserSpec( KKeyEntryMap *aKeyDict, KKeyMapOrder *pKeyOrder, QWidget* parent, bool global_P )
    : KKeyChooser( aKeyDict, pKeyOrder, parent, global_P, false, true ), global( global_P )
    {
    if( global )
        globalDict()->clear(); // don't check against global keys twice
    }

KeyChooserSpec::KeyChooserSpec( KKeyEntryMap *aKeyDict, QWidget* parent, bool global_P )
    : KKeyChooser( aKeyDict, parent, global_P, false, true ), global( global_P )
    {
    if( global )
        globalDict()->clear(); // don't check against global keys twice
    }

void KeyChooserSpec::updateKeys( const KKeyEntryMap* map_P )
    {
    if( global )
        {
        stdDict()->clear();
        for( KKeyEntryMap::ConstIterator gIt( map_P->begin());
             gIt != map_P->end();
             ++gIt )
            {
            int* keyCode = new int;
            *keyCode = ( *gIt ).aConfigKeyCode;
            stdDict()->insert( gIt.key(), keyCode);
            }
        }
    else
        {
        globalDict()->clear();
        for( KKeyEntryMap::ConstIterator gIt( map_P->begin());
             gIt != map_P->end();
             ++gIt )
            {
            int* keyCode = new int;
            *keyCode = ( *gIt ).aConfigKeyCode;
            globalDict()->insert( gIt.key(), keyCode);
            }
        }
    }

