/****************************************************************
**
** Definition of CannonField class, Qt tutorial 13
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
    bool  gameOver() const { return gameEnded; }
    bool  isShooting() const;
    QSizePolicy sizePolicy() const;

public slots:
    void  setAngle( int degrees );
    void  setForce( int newton );
    void  shoot();
    void  newTarget();
    void  setGameOver();
    void  restartGame();

private slots:
    void  moveShot();

signals:
    void  hit();
    void  missed();
    void  angleChanged( int );
    void  forceChanged( int );
    void  canShoot( bool );

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

    bool gameEnded;
};


#endif // CANNON_H
