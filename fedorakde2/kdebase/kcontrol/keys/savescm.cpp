#include <stdlib.h>
#include <stdio.h>

#include "savescm.h"
#include "savescm.moc"

#include <kapp.h>
#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>

SaveScm::SaveScm( QWidget *parent, const char *name, const QString &def )
    : KDialogBase( parent, name, true, i18n("Save key scheme"), Ok|Cancel, Ok, true )
{
    QWidget *page = new QWidget(this);
    setMainWidget(page);
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

    nameLine = new KLineEdit( page );
    nameLine->setFocus();
    nameLine->setMaxLength(18);
    nameLine->setFixedHeight( nameLine->sizeHint().height() );
    nameLine->setText(def);
    nameLine->selectAll();

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( nameLine,
     i18n( "Enter a name for the key scheme:\n"), page);

    tmpQLabel->setAlignment( AlignLeft | AlignBottom | ShowPrefix );
    tmpQLabel->setFixedHeight( tmpQLabel->sizeHint().height() );
    tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() );

    topLayout->addWidget( tmpQLabel );
    topLayout->addWidget( nameLine );
    topLayout->addStretch( 10 );
}
