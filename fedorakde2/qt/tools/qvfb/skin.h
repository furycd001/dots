#include <qwidget.h>


class QVFb;
class QVFbView;


class Skin : public QWidget
{
public:
    Skin( QVFb *p, const QString &skinFile, int &viewW, int &viewH );
    ~Skin( );
    void setView( QVFbView *v );
protected:
    virtual void paintEvent( QPaintEvent * );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent * );
private:
    QVFb *parent;
    QVFbView *view;
    QPoint clickPos;
    bool buttonPressed;
    int buttonCode;
    int buttonIndex;

    QString skinImageUpFileName;
    QString skinImageDownFileName;
    QPixmap *skinImageUp;
    QPixmap *skinImageDown;
    int viewX1, viewY1;
    int numberOfAreas;

    typedef struct {
	QString	name;
        int	keyCode;
        int	x1, y1;
        int	x2, y2;
    } ButtonAreas;

    ButtonAreas *areas;
};


