/****************************************************************
**
** Definition of CannonField class, Qt tutorial 10
**
****************************************************************/

#ifndef CANNON_H
#define CANNON_H

#include <qwidget.h>


class CannonField : public QWidget
{
    Q_OBJECT
public:
    CannonField( QWidget *parent=0, const char *name=0 );

    QSizePolicy sizePolicy() const;

    int   angle() const { return ang; }
    int   force() const { return f; }

public slots:
    void  setAngle( int degrees );
    void  setForce( int newton );

signals:
    void  angleChanged( int );
    void  forceChanged( int );

protected:
    void  paintEvent( QPaintEvent * );

private:
    QRect cannonRect() const;

    int ang;
    int f;
};


#endif // CANNON_H
