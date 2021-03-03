#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>

#include <kio/ioslave_defaults.h>
#include <kprotocolmanager.h>
#include <dcopclient.h>
#include <kdebug.h>
#include <klocale.h>
#include <kdialog.h>
#include <kapp.h>

#include "netpref.h"

#define MAX_TIMEOUT_VALUE  3600

KIOPreferences::KIOPreferences( QWidget* parent,  const char* name )
               :KCModule( parent, name )
{
    QVBoxLayout* mainLayout = new QVBoxLayout( this, KDialog::marginHint(),
                                                                                    KDialog::spacingHint() );
    d_timeout = new QGroupBox( i18n("Timeout Values"), this, "d_timeout" );
    d_timeout->setColumnLayout(0, Qt::Vertical );
    d_timeout->layout()->setSpacing( 0 );
    d_timeout->layout()->setMargin( 0 );
    QWhatsThis::add( d_timeout, i18n( "Here you can set timeout values. "
                                                                "You might want to tweak them if "
                                                                "your connection is very slow." ) );

    QVBoxLayout* d_timeoutLayout = new QVBoxLayout( d_timeout->layout() );
    d_timeoutLayout->setAlignment( Qt::AlignTop );
    d_timeoutLayout->setSpacing( 6 );
    d_timeoutLayout->setMargin( 11 );

    QGridLayout* grid_topLevel = new QGridLayout;
    grid_topLevel->setSpacing( KDialog::spacingHint() );
    grid_topLevel->setMargin( KDialog::marginHint() );

    QGridLayout* grid_firstColumn = new QGridLayout;
    grid_firstColumn->setSpacing( KDialog::spacingHint() );
    grid_firstColumn->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    grid_firstColumn->addItem( spacer, 0, 1 );

    QVBoxLayout* vlay_firstColumnLabels = new QVBoxLayout;
    vlay_firstColumnLabels->setSpacing( KDialog::spacingHint() );
    vlay_firstColumnLabels->setMargin( 0 );

    QLabel* lbl_socket = new QLabel( i18n( "Soc&ket Read" ), d_timeout,
                                                            "lbl_socket" );
    vlay_firstColumnLabels->addWidget( lbl_socket );
    QLabel* lbl_proxy = new QLabel( i18n( "Pro&xy Connect" ), d_timeout,
                                                          "lbl_proxy" );
    vlay_firstColumnLabels->addWidget( lbl_proxy );

    grid_firstColumn->addLayout( vlay_firstColumnLabels, 0, 0 );

    QVBoxLayout* vlay_firstColumnSpinBox = new QVBoxLayout;
    vlay_firstColumnSpinBox->setSpacing( KDialog::spacingHint() );
    vlay_firstColumnSpinBox->setMargin( 0 );

    d_socketRead = new QSpinBox( d_timeout, "d_socketRead" );
    d_socketRead->setSuffix( i18n( "    sec" ) );
    vlay_firstColumnSpinBox->addWidget( d_socketRead );
    connect ( d_socketRead, SIGNAL( valueChanged(int) ),
                     SLOT( readTimeoutChanged( int ) ) );

    d_proxyConnect = new QSpinBox( d_timeout, "d_proxyConnect" );
    d_proxyConnect->setSuffix( i18n( "    sec" ) );
    vlay_firstColumnSpinBox->addWidget( d_proxyConnect );
    connect ( d_proxyConnect, SIGNAL( valueChanged(int) ),
                     SLOT( proxyConnectTimeoutChanged( int ) ) );

    grid_firstColumn->addLayout( vlay_firstColumnSpinBox, 0, 2 );

    grid_topLevel->addLayout( grid_firstColumn, 0, 0 );
    spacer = new QSpacerItem( 20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum );
    grid_topLevel->addItem( spacer, 0, 3 );

    QGridLayout* grid_secondColumn = new QGridLayout;
    grid_secondColumn->setSpacing( KDialog::spacingHint() );
    grid_secondColumn->setMargin( 0 );

    QVBoxLayout* vlay_secondColumnLabel = new QVBoxLayout;
    vlay_secondColumnLabel->setSpacing( KDialog::spacingHint() );
    vlay_secondColumnLabel->setMargin( 0 );
    QLabel* lbl_serverConnect = new QLabel( i18n("Server Co&nnect"), d_timeout,
                                                                         "lbl_serverConnect" );
    vlay_secondColumnLabel->addWidget( lbl_serverConnect );
    QLabel* lbl_serverResponse = new QLabel( i18n("Server &Response"),
                                                                            d_timeout,
                                                                            "lbl_serverResponse" );
    vlay_secondColumnLabel->addWidget( lbl_serverResponse );
    grid_secondColumn->addLayout( vlay_secondColumnLabel, 0, 0 );

    QVBoxLayout* vlay_secondColumnSpinBox = new QVBoxLayout;
    vlay_secondColumnSpinBox->setSpacing( KDialog::spacingHint() );
    vlay_secondColumnSpinBox->setMargin( 0 );

    d_serverConnect = new QSpinBox( d_timeout, "d_serverConnect" );
    d_serverConnect->setSuffix( i18n( "    secs" ) );
    vlay_secondColumnSpinBox->addWidget( d_serverConnect );
    connect ( d_serverConnect, SIGNAL( valueChanged(int) ),
                     SLOT( connectTimeoutChanged( int ) ) );

    d_serverResponse = new QSpinBox( d_timeout, "d_serverResponse" );
    d_serverResponse->setSuffix( i18n( "    secs" ) );
    vlay_secondColumnSpinBox->addWidget( d_serverResponse );
    connect ( d_serverResponse, SIGNAL( valueChanged(int) ),
                     SLOT( responseTimeoutChanged( int ) ) );

    grid_secondColumn->addLayout( vlay_secondColumnSpinBox, 0, 2 );

    spacer = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    grid_secondColumn->addItem( spacer, 0, 1 );

    grid_topLevel->addLayout( grid_secondColumn, 0, 2 );
    spacer = new QSpacerItem( 20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum );
    grid_topLevel->addItem( spacer, 0, 1 );
    d_timeoutLayout->addLayout( grid_topLevel );
    mainLayout->addWidget( d_timeout );

    mainLayout->addStretch();

    lbl_socket->setBuddy( d_socketRead );
    lbl_proxy->setBuddy( d_proxyConnect );
    lbl_serverConnect->setBuddy( d_serverConnect );
    lbl_serverResponse->setBuddy( d_serverResponse );

    d_socketRead->setRange( MIN_TIMEOUT_VALUE,   MAX_TIMEOUT_VALUE );
    d_serverResponse->setRange( MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE );
    d_serverConnect->setRange( MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE );
    d_proxyConnect->setRange( MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE );

    load();
}

KIOPreferences::~KIOPreferences()
{
}

void KIOPreferences::load()
{
  d_proxyConnectTimeout = KProtocolManager::proxyConnectTimeout();
  d_connectTimeout =KProtocolManager::connectTimeout();
  d_responseTimeout = KProtocolManager::responseTimeout();
  d_readTimeout = KProtocolManager::readTimeout();

  d_socketRead->setValue( d_readTimeout );
  d_serverResponse->setValue( d_responseTimeout );
  d_serverConnect->setValue( d_connectTimeout );
  d_proxyConnect->setValue( d_proxyConnectTimeout );

  d_valueChanged = false;
}

void KIOPreferences::changed( bool changed )
{
  if ( d_valueChanged != changed )
    emit KCModule::changed( true );
}

void KIOPreferences::proxyConnectTimeoutChanged( int value )
{
  if (d_proxyConnectTimeout != value)
  {
      d_proxyConnectTimeout = value;
      changed( true );
  }
}

void KIOPreferences::connectTimeoutChanged( int value )
{
  if (d_connectTimeout != value)
  {
      kdDebug() << "Emitting changed! " << endl;
      d_connectTimeout = value;
      changed( true );
  }
}

void KIOPreferences::responseTimeoutChanged( int value )
{
  if (d_responseTimeout != value)
  {
    d_responseTimeout =value;
    changed( true );
  }
}

void KIOPreferences::readTimeoutChanged( int value )
{
  if (d_readTimeout != value)
  {
      d_readTimeout = value;
      changed( true );
  }
}

void KIOPreferences::save()
{
  kdDebug() << "Saving values: " << endl;
  kdDebug() << "     Read Timeout: " << d_readTimeout << endl;
  kdDebug() << "     Response Timeout: " << d_responseTimeout << endl;
  kdDebug() << "     Connect Timeout: " << d_connectTimeout << endl;
  kdDebug() << "     Proxy Timeout: " << d_proxyConnectTimeout << endl;

  KProtocolManager::setReadTimeout( d_readTimeout );
  KProtocolManager::setResponseTimeout( d_responseTimeout );
  KProtocolManager::setConnectTimeout( d_connectTimeout );
  KProtocolManager::setProxyConnectTimeout( d_proxyConnectTimeout );

  // Inform running io-slaves about change...
  QByteArray data;
  QDataStream stream( data, IO_WriteOnly );
  stream << QString::null;
  if ( !kapp->dcopClient()->isAttached() )
    kapp->dcopClient()->attach();
  kapp->dcopClient()->send( "*", "KIO::Scheduler", "reparseSlaveConfiguration(QString)", data );

  // Re-load the new values please!
  d_valueChanged = false;
}

void KIOPreferences::defaults()
{
  d_socketRead->setValue( DEFAULT_READ_TIMEOUT );
  d_serverResponse->setValue( DEFAULT_RESPONSE_TIMEOUT );
  d_serverConnect->setValue( DEFAULT_CONNECT_TIMEOUT );
  d_proxyConnect->setValue( DEFAULT_PROXY_CONNECT_TIMEOUT );
  emit changed(true);
}

QString KIOPreferences::quickHelp() const
{
  return i18n("<h1>Network Preferences</h1>Here you can define"
	      " the behavior of KDE programs when using Internet"
	      " and network connections. If you experience timeouts"
	      " and problems or sit behind a modem, you might want"
	      " to adjust these values." );
}

#include "netpref.moc"
