/****************************************************************
**
** Definition of LCDRange class, Qt tutorial 12
**
****************************************************************/

#ifndef LCDRANGE_H
#define LCDRANGE_H

#include <qwidget.h>

class QSlider;
class QLabel;


class LCDRange : public QWidget
{
    Q_OBJECT
public:
    LCDRange( QWidget *parent=0, const char *name=0 );
    LCDRange( const char *s, QWidget *parent=0, const char *name=0 );

    int         value() const;
    const char *text()  const;

public slots:
    void setValue( int );
    void setRange( int minVal, int maxVal );
    void setText( const char * );

signals:
    void valueChanged( int );

private:
    void init();

    QSlider     *slider;
    QLabel      *label;
};


#endif // LCDRANGE_H
