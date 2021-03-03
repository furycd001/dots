//-----------------------------------------------------------------------------
//
// Lorenz - Lorenz Attractor screen saver
//   Nicolas Brodu, brodu@kde.org, 2000
//
// Portions of code from kblankscrn and khop.
//   See authors there.
//
// I release my code as GPL, but see the other headers and the README

#ifndef __LORENZKSCRN_H__
#define __LORENZKSCRN_H__

#include <qtimer.h>
#include <qdialog.h>
#include <qcolor.h>
#include <kscreensaver.h>

// See lorenz.cpp for this private class
class Matrix3D;

class KLorenzSaver : public KScreenSaver
{
    Q_OBJECT
public:
    KLorenzSaver( WId id );
    virtual ~KLorenzSaver();
    void setSpeed(int num);
    void setEpoch(int num);
    void setCRate(int num);
    void setZRot(int num);
    void setYRot(int num);
    void setXRot(int num);
    void updateMatrix();
    void newEpoch();

protected slots:
    void drawOnce();

protected:
    QTimer timer;
    int colorContext;

private:
    void readSettings();

private:
    double x, y, z, t;
    double speed, epoch, zrot, yrot, xrot, crate;
    int e;
    Matrix3D *mat;
};

class QSlider;

class KLorenzSetup : public QDialog
{
    Q_OBJECT
public:
    KLorenzSetup(QWidget *parent = 0, const char *name = 0 );

protected:
    void readSettings();

private slots:
    void slotSpeed(int num);
    void slotEpoch(int num);
    void slotCRate(int num);
    void slotZRot(int num);
    void slotYRot(int num);
    void slotXRot(int num);
    void slotDefaultPressed();
    void slotOkPressed();
    void slotAbout();

private:
    QWidget *preview;
    QSlider *sps, *eps, *zrs, *yrs, *xrs, *crs;
    KLorenzSaver *saver;
    int speed, epoch, zrot, yrot, xrot, crate;
};

#endif

