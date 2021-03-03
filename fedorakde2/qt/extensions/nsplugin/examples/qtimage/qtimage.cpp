// Qt stuff
#include "qnp.h"
#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include "qpngio.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

class ImageView : public QNPWidget {
public:
    ImageView()
    {
	popup = new QPopupMenu;
	popup->insertItem("Left as");
	popup->insertItem("An exercise");
	popup->insertItem("For the");
	popup->insertItem("Reader!");
    }

    void paintEvent(QPaintEvent* event)
    {
	QPainter p(this);
	p.setClipRect(event->rect());

	if ( pm.size() == size() ) {
	    p.drawPixmap(0,0,pm);
	} else {
	    if ( pmScaled.size() != size() ) {
		QWMatrix m;
		m.scale((double)width()/pm.width(),
			(double)height()/pm.height());
		pmScaled = pm.xForm(m);
	    }
	    p.drawPixmap(0,0,pmScaled);
	}
    }

    void mousePressEvent(QMouseEvent* e)
    {
	popup->popup(mapToGlobal(e->pos()));
    }

    void showImage(const QImage& image)
    {
	pm.convertFromImage(image, QPixmap::Color);
	repaint( FALSE );
    }

private:
    QPixmap pm;
    QPixmap pmScaled;
    QPopupMenu* popup;
};

class ImageLoader : public QNPInstance {
    ImageView* iv;
    QImage image;

public:
    ImageLoader() : iv(0)
    {
    }

    QNPWidget* newWindow()
    {
	iv = new ImageView;
	imageToIV();
	return iv;
    }

    void imageToIV()
    {
	if (!iv || image.isNull()) return;

	iv->showImage(image);
	image.reset();
    }

    bool newStreamCreated(QNPStream*, StreamMode& smode)
    {
	smode = AsFileOnly;
	return TRUE;
    }

    void streamAsFile(QNPStream*, const char* fname)
    {
	qInitPngIO();

	image = QImage(fname);
	if ( image.isNull() )
	    fprintf(stderr, "Could not convert file: %s\n", fname);
	imageToIV();
    }
};

class ImagePlugin : public QNPlugin {

public:
    ImagePlugin()
    {
    }

    QNPInstance* newInstance()
    {
	return new ImageLoader;
    }

    const char* getMIMEDescription() const
    {
	return "image/x-png:png:PNG Image;"
	       "image/png:png:PNG Image;"
	       "image/x-portable-bitmap:pbm:PBM Image;"
	       "image/x-portable-graymap:pgm:PGM Image;"
	       "image/x-portable-pixmap:ppm:PPM Image;"
	       "image/bmp:bmp:BMP Image;"
	       "image/x-ms-bmp:bmp:BMP Image;"
	       "image/x-xpixmap:xpm:XPM Image;"
	       "image/xpm:xpm:XPM Image";
    }

    const char * getPluginNameString() const
    {
	return "Qt-based Image Plugin";
    }

    const char * getPluginDescriptionString() const
    {
	return "Supports all image formats supported by Qt";
    }
};

QNPlugin* QNPlugin::create()
{
    return new ImagePlugin;
}
