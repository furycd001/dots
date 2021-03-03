// -*- c++ -*-

#ifndef KASBAREXTENSION_H
#define KASBAREXTENSION_H

#include <kpanelextension.h>

class KasTasker;

/**
 * @version $Id: kasbarextension.h,v 1.5 2001/05/13 23:40:15 rich Exp $
 */
class KasBarExtension : public KPanelExtension
{
    Q_OBJECT

public:
    KasBarExtension( const QString& configFile,
                     Type t = Normal,
                     int actions = 0,
                     QWidget *parent = 0, const char *name = 0 );

    virtual ~KasBarExtension();

    QSize sizeHint( Position, QSize maxSize ) const;
    Position preferedPosition() const { return Right; }

    virtual void positionChange( Position position );

protected slots:
    void updateConfig();

protected:
    void resizeEvent( QResizeEvent * );
    virtual void about();
    virtual void preferences();
    
private:
    KasTasker* kasbar;
};

#endif // KASBAREXTENSION_H

