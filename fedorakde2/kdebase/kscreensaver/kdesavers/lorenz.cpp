//-----------------------------------------------------------------------------
//
// Lorenz - Lorenz Attractor screen saver
//   Nicolas Brodu, brodu@kde.org, 2000
//
// Portions of code from kblankscrn and khop.
//   See authors there.
//
// I release my code as GPL, but see the other headers and the README

#include <math.h>
#include <stdlib.h>

#include <qpainter.h>
#include <qslider.h>
#include <qlayout.h>
#include <qcolor.h>
#include <qlabel.h>

#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>

#include "lorenz.h"
#include "lorenz.moc"

// libkscreensaver interface
extern "C"
{
    const char *kss_applicationName = "klorenz.kss";
    const char *kss_description = I18N_NOOP( "KLorenz" );
    const char *kss_version = "2.2.0";

    KScreenSaver *kss_create( WId id )
    {
        return new KLorenzSaver( id );
    }

    QDialog *kss_setup()
    {
        return new KLorenzSetup();
    }
}

#define MINSPEED 1
#define MAXSPEED 1500
#define DEFSPEED 150
#define MINZROT -180
#define MAXZROT 180
#define DEFZROT 104 //100
#define MINYROT -180
#define MAXYROT 180
#define DEFYROT -19 //80
#define MINXROT -180
#define MAXXROT 180
#define DEFXROT 25 //20
#define MINEPOCH 1
#define MAXEPOCH 30000
#define DEFEPOCH 5800
#define MINCOLOR 1
#define MAXCOLOR 100
#define DEFCOLOR 20

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
KLorenzSetup::KLorenzSetup( QWidget *parent, const char *name )
    : QDialog( parent, name, TRUE )
{
    readSettings();

    setCaption( i18n("Setup Lorenz Attractor Screen Saver") );

    QLabel *label;
    QPushButton *button;
    QSlider *slider;

    QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
    QHBoxLayout *tl1 = new QHBoxLayout;
    tl->addLayout(tl1);
    QVBoxLayout *tl11 = new QVBoxLayout(5);
    tl1->addLayout(tl11);

    label = new QLabel( i18n("Speed:"), this );
    tl11->addWidget(label);

    sps = slider = new QSlider(MINSPEED, MAXSPEED, 10, speed, QSlider::Horizontal, this);
    slider->setMinimumSize( 90, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(100);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotSpeed( int ) ) );
    tl11->addWidget(slider);
    tl11->addSpacing(5);

    label = new QLabel( i18n("Epoch:"), this );
    tl11->addWidget(label);

    eps = slider = new QSlider(MINEPOCH, MAXEPOCH, 100, epoch, QSlider::Horizontal, this);
    slider->setMinimumSize( 90, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(3000);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotEpoch( int ) ) );
    tl11->addWidget(slider);
    tl11->addSpacing(5);

    label = new QLabel( i18n("Color rate:"), this );
    tl11->addWidget(label);

    crs = slider = new QSlider(MINCOLOR, MAXCOLOR, 10, crate, QSlider::Horizontal, this);
    slider->setMinimumSize( 90, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(10);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotCRate( int ) ) );
    tl11->addWidget(slider);
    tl11->addSpacing(5);

    label = new QLabel( i18n("Rotation Z:"), this );
    tl11->addWidget(label);

    zrs = slider = new QSlider(MINZROT, MAXZROT, 10, zrot, QSlider::Horizontal, this);
    slider->setMinimumSize( 90, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(20);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotZRot( int ) ) );
    tl11->addWidget(slider);
    tl11->addSpacing(5);

    label = new QLabel( i18n("Rotation Y:"), this );
    tl11->addWidget(label);

    yrs = slider = new QSlider(MINYROT, MAXYROT, 10, yrot, QSlider::Horizontal, this);
    slider->setMinimumSize( 90, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(20);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotYRot( int ) ) );
    tl11->addWidget(slider);
    tl11->addSpacing(5);

    label = new QLabel( i18n("Rotation X:"), this );
    tl11->addWidget(label);

    xrs = slider = new QSlider(MINXROT, MAXXROT, 10, xrot, QSlider::Horizontal, this);
    slider->setMinimumSize( 90, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(20);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotXRot( int ) ) );
    tl11->addWidget(slider);
    tl11->addStretch(1);

    preview = new QWidget( this );
    preview->setFixedSize( 400, 300 );
    preview->setBackgroundColor( black );
    preview->show();    // otherwise saver does not get correct size
    saver = new KLorenzSaver( preview->winId() );
    tl1->addWidget(preview);

    KButtonBox *bbox = new KButtonBox(this);
    button = bbox->addButton( i18n("About"));
    connect( button, SIGNAL( clicked() ), SLOT(slotAbout() ) );
    bbox->addStretch(1);

    button = bbox->addButton( i18n("OK"));
    connect( button, SIGNAL( clicked() ), SLOT( slotOkPressed() ) );

    button = bbox->addButton( i18n("Default"));
    connect( button, SIGNAL( clicked() ), SLOT( slotDefaultPressed() ) );

    button = bbox->addButton(i18n("Cancel"));
    connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
    bbox->layout();
    tl->addWidget(bbox);

    tl->freeze();
}

// read settings from config file
void KLorenzSetup::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

    speed = config->readNumEntry( "Speed", DEFSPEED );
    epoch = config->readNumEntry( "Epoch", DEFEPOCH );
    crate = config->readNumEntry( "Color Rate", DEFCOLOR );
    zrot = config->readNumEntry( "ZRot", DEFZROT );
    yrot = config->readNumEntry( "YRot", DEFZROT );
    xrot = config->readNumEntry( "XRot", DEFZROT );
}


void KLorenzSetup::slotSpeed(int num)
{
    speed = num;
    if (saver) saver->setSpeed(speed);
}

void KLorenzSetup::slotEpoch(int num)
{
    epoch = num;
    if (saver) saver->setEpoch(epoch);
}

void KLorenzSetup::slotCRate(int num)
{
    crate = num;
    if (saver) saver->setCRate(crate);
}

void KLorenzSetup::slotZRot(int num)
{
    zrot = num;
    if (saver) {
        saver->setZRot(zrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
}

void KLorenzSetup::slotYRot(int num)
{
    yrot = num;
    if (saver) {
        saver->setYRot(yrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
}

void KLorenzSetup::slotXRot(int num)
{
    xrot = num;
    if (saver) {
        saver->setXRot(xrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
}

void KLorenzSetup::slotAbout()
{
    KMessageBox::about(this,i18n("Lorenz Attractor screen saver for KDE\n\nCopyright (c) 2000 Nicolas Brodu"));
}

// Ok pressed - save settings and exit
void KLorenzSetup::slotOkPressed()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

    config->writeEntry( "Speed", speed );
    config->writeEntry( "Epoch", epoch );
    config->writeEntry( "Color Rate", crate );
    config->writeEntry( "ZRot", zrot );
    config->writeEntry( "YRot", yrot );
    config->writeEntry( "XRot", xrot );

    config->sync();

    accept();
}

void KLorenzSetup::slotDefaultPressed()
{
    speed = DEFSPEED;
    epoch = DEFEPOCH;
    crate = DEFCOLOR;
    zrot = DEFZROT;
    yrot = DEFYROT;
    xrot = DEFXROT;
    if (saver) {
        saver->setSpeed(speed);
        saver->setEpoch(epoch);
        saver->setCRate(crate);
        saver->setZRot(zrot);
        saver->setYRot(yrot);
        saver->setXRot(xrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
    sps->setValue(speed);
    eps->setValue(epoch);
    crs->setValue(crate);
    zrs->setValue(zrot);
    yrs->setValue(yrot);
    xrs->setValue(xrot);

/*  // User can cancel, or save defaults?

    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

    config->writeEntry( "Speed", speed );
    config->writeEntry( "Epoch", epoch );
    config->writeEntry( "Color Rate", crate );
    config->writeEntry( "ZRot", zrot );
    config->writeEntry( "YRot", yrot );
    config->writeEntry( "XRot", xrot );

    config->sync();
*/
}

//-----------------------------------------------------------------------------


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const double pi = M_PI;

// Homogeneous coordinate transform matrix
// I initially wrote it for a Java applet, it is inspired from a
// Matrix class in the JDK.
// Nicolas Brodu, 1998-2000
class Matrix3D
{
    // All coefficients
    double xx, xy, xz, xo;
    double yx, yy, yz, yo;
    double zx, zy, zz, zo;
    // 0, 0, 0, 1 are implicit
public:

    void unit()
    {
        xx=1.0; xy=0.0; xz=0.0; xo=0.0;
        yx=0.0; yy=1.0; yz=0.0; yo=0.0;
        zx=0.0; zy=0.0; zz=1.0; zo=0.0;
    }

    Matrix3D ()
    {
        unit();
    }

    // Translation
    void translate(double x, double y, double z)
    {
        xo += x;
        yo += y;
        zo += z;
    }

    // Rotation, in degrees, around the Y axis
    void rotY(double theta)
    {
        theta *= pi / 180;
        double ct = cos(theta);
        double st = sin(theta);

        double Nxx = xx * ct + zx * st;
        double Nxy = xy * ct + zy * st;
        double Nxz = xz * ct + zz * st;
        double Nxo = xo * ct + zo * st;

        double Nzx = zx * ct - xx * st;
        double Nzy = zy * ct - xy * st;
        double Nzz = zz * ct - xz * st;
        double Nzo = zo * ct - xo * st;

        xo = Nxo;
        xx = Nxx;
        xy = Nxy;
        xz = Nxz;
        zo = Nzo;
        zx = Nzx;
        zy = Nzy;
        zz = Nzz;
    }


    // Rotation, in degrees, around the X axis
    void rotX(double theta)
    {
        theta *= pi / 180;
        double ct = cos(theta);
        double st = sin(theta);

        double Nyx = yx * ct + zx * st;
        double Nyy = yy * ct + zy * st;
        double Nyz = yz * ct + zz * st;
        double Nyo = yo * ct + zo * st;

        double Nzx = zx * ct - yx * st;
        double Nzy = zy * ct - yy * st;
        double Nzz = zz * ct - yz * st;
        double Nzo = zo * ct - yo * st;

        yo = Nyo;
        yx = Nyx;
        yy = Nyy;
        yz = Nyz;
        zo = Nzo;
        zx = Nzx;
        zy = Nzy;
        zz = Nzz;
    }


    // Rotation, in degrees, around the Z axis
    void rotZ(double theta)
    {
        theta *= pi / 180;
        double ct = cos(theta);
        double st = sin(theta);

        double Nyx = yx * ct + xx * st;
        double Nyy = yy * ct + xy * st;
        double Nyz = yz * ct + xz * st;
        double Nyo = yo * ct + xo * st;

        double Nxx = xx * ct - yx * st;
        double Nxy = xy * ct - yy * st;
        double Nxz = xz * ct - yz * st;
        double Nxo = xo * ct - yo * st;

        yo = Nyo;
        yx = Nyx;
        yy = Nyy;
        yz = Nyz;
        xo = Nxo;
        xx = Nxx;
        xy = Nxy;
        xz = Nxz;
    }

    // Multiply by a projection matrix, with camera f
    // f 0 0 0   x   f*x
    // 0 f 0 0 * y = f*y
    // 0 0 1 f   z   z+f
    // 0 0 0 1   1   1
    // So, it it easy to find the 2D coordinates after the transform
    //  u = f*x / (z+f)
    //  v = f*y / (z+f)
    void proj(double f)
    {
        xx*=f;
        xy*=f;
        xz*=f;
        xo*=f;
        yx*=f;
        yy*=f;
        yz*=f;
        yo*=f;
        zo+=f;
    }

    // Apply the transformation 3D => 2D
    void transform(double x, double y, double z, double &u, double& v, double& w)
    {
        u = x * xx + y * xy + z * xz + xo;
        v = x * yx + y * yy + z * yz + yo;
        w = x * zx + y * zy + z * zz + zo;
    }
};

KLorenzSaver::KLorenzSaver( WId id ) : KScreenSaver( id )
{
    readSettings();

    // Create a transform matrix with the parameters
    mat = new Matrix3D();
    updateMatrix();

    colorContext = QColor::enterAllocContext();
    setBackgroundColor( black );
    newEpoch();

    timer.start( 10 );
    connect( &timer, SIGNAL( timeout() ), SLOT( drawOnce() ) );
}

KLorenzSaver::~KLorenzSaver()
{
    if (mat) {delete mat; mat=0;} // Overly cautious?
    timer.stop();
    QColor::leaveAllocContext();
    QColor::destroyAllocContext( colorContext );
}

// read configuration settings from config file
void KLorenzSaver::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

    speed = config->readNumEntry( "Speed", DEFSPEED );
    epoch = config->readNumEntry( "Epoch", DEFEPOCH );
    zrot = config->readNumEntry( "ZRot", DEFZROT );
    yrot = config->readNumEntry( "YRot", DEFZROT );
    xrot = config->readNumEntry( "XRot", DEFZROT );

    int crate_num = config->readNumEntry( "Color Rate", DEFCOLOR );
    crate = (double)crate_num / (double)MAXCOLOR;
}

void KLorenzSaver::setSpeed(int num)
{
    speed = num;
}

void KLorenzSaver::setEpoch(int num)
{
    epoch = num;
}

void KLorenzSaver::setZRot(int num)
{
    zrot = num;
}

void KLorenzSaver::setYRot(int num)
{
    yrot = num;
}

void KLorenzSaver::setXRot(int num)
{
    xrot = num;
}

void KLorenzSaver::setCRate(int num)
{
    crate = (double)num / (double)MAXCOLOR;
}

void KLorenzSaver::updateMatrix()
{
    // reset matrix
    mat->unit();
    // Remove the mean before the rotations...
    mat->translate(-0.95413, -0.96740, -23.60065);
    mat->rotZ(zrot);
    mat->rotY(yrot);
    mat->rotX(xrot);
    mat->translate(0, 0, 100);
    mat->proj(1);
}

void KLorenzSaver::newEpoch()
{
    // Start at a random position, somewhere around the mean
    x = 0.95-25.0+50.0*kapp->random() / (RAND_MAX+1.0);
    y = 0.97-25.0+50.0*kapp->random() / (RAND_MAX+1.0);
    z = 23.6-25.0+50.0*kapp->random() / (RAND_MAX+1.0);
    // start at some random 'time' as well to have different colors
    t = 10000.0*kapp->random() / (RAND_MAX+1.0);
    erase();
    e=0; // reset epoch counter
}

// Computes the derivatives using Lorenz equations
static void lorenz(double x, double y, double z, double& dx, double& dy, double& dz)
{
    dx = 10*(y-x);
    dy = 28*x - y - x*z;
    dz = x*y - z*8.0/3.0;
}

// Use a simple Runge-Kutta formula to draw a few points
// No need to go beyond 2nd order for a screensaver!
void KLorenzSaver::drawOnce()
{
    double kx, ky, kz, dx, dy, dz;
    const double h = 0.0001;
    const double tqh = h * 3.0 / 4.0;
    QPainter p(this);

    for (int i=0; i<speed; i++) {
        // Runge-Kutta formula
        lorenz(x,y,z,dx,dy,dz);
        lorenz(x + tqh*dx, y + tqh*dy, z + tqh*dz, kx, ky, kz);
        x += h*(dx/3.0+2*kx/3.0);
        y += h*(dy/3.0+2*ky/3.0);
        z += h*(dz/3.0+2*kz/3.0);
        // Apply transform
        mat->transform(x,y,z,kx,ky,kz);
        // Choose a color
        p.setPen( 
            QColor((int)(sin(t*crate/pi)*127+128),
                   (int)(sin(t*crate/(pi-1))*127+128),
                   (int)(sin(t*crate/(pi-2))*127+128)).pixel() );
        // Draw a point
        p.drawPoint( (int)(kx*width()*1.5/kz)+(int)(width()/2),
                     (int)(ky*height()*1.5/kz)+(int)(height()/2));
        t+=h;
    }
    if (++e>=epoch) newEpoch();
}
