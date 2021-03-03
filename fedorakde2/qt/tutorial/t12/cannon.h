/****************************************************************
**
** Definition of CannonField class, Qt tutorial 12
**
****************************************************************/

#ifndef CANNON_H
#define CANNON_H

class QTimer;


#include <qwidget.h>


class CannonField : public QWidget
{
    Q_OBJECT
public:
    CannonField( QWidget *parent=0, const char *name=0 );

    int   angle() const { return ang; }
    int   force() const { return f; }
    QSizePolicy sizePolicy() const;

public slots:
    void  setAngle( int degrees );
    void  setForce( int newton );
    void  shoot();
    void  newTarget();

private slots:
    void  moveShot();

signals:
    void  hit();
    void  missed();
    void  angleChanged( int );
    void  forceChanged( int );

protected:
    void  paintEvent( QPaintEvent * );

private:
    void  paintShot( QPainter * );
    void  paintTarget( QPainter * );
    void  paintCannon( QPainter * );
    QRect cannonRect() const;
    QRect shotRect() const;
    QRect targetRect() const;

    int ang;
    int f;

    int timerCount;
    QTimer * autoShootTimer;
    float shoot_ang;
    float shoot_f;

    QPoint target;
};


#endif // CANNON_H
