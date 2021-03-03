/****************************************************************************
** $Id: qt/extensions/nsplugin/examples/movies/main.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qnp.h>
#include <qpainter.h>
#include <qmovie.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qimage.h>
#include <qapplication.h>
#include <qclipboard.h>


class MovieView : public QNPWidget {
    Q_OBJECT
    QMovie& movie;
    bool multiframe;
    QString url;

public:
    MovieView(QMovie& m) :
        QNPWidget(),
	movie(m)
    {
        // Get the movie to tell use when interesting things happen.
        movie.connectUpdate(this, SLOT(movieUpdated(const QRect&)));
        movie.connectResize(this, SLOT(movieResized(const QSize&)));
        movie.connectStatus(this, SLOT(movieStatus(int)));

	const char* bgcolor = instance()->arg("BGCOLOR");
	if ( bgcolor ) {
	    movie.setBackgroundColor(QColor(bgcolor));
	} else {
	    movie.setBackgroundColor(backgroundColor());
	}
        setBackgroundMode(NoBackground);

	multiframe = FALSE;
    }

    void setUrl(const QString& u)
    {
	url = u;
    }

protected:
    void mousePressEvent(QMouseEvent* me)
    {
	if ( me->button() == RightButton ) {
	    enum { SaveFrame, SaveAnimation, OpenLink, CopyLink, CopyFrame,
		    CopyURL, Pause, Resume, About };

	    bool paused = movie.paused();
	    movie.pause(); // so the user can grab a chosen frame
	    QPopupMenu popup;
	    QString href = instance()->arg("HREF");
	    if ( !href.isEmpty() ) {
		popup.insertItem("Open Link in New Window", OpenLink);
		popup.insertSeparator();
	    }
	    if ( multiframe ) {
		// Need Qt movie-save-in-format function
		//popup.insertItem("Save Animation As...", SaveAnimation);
		popup.insertItem("Save Frame As...", SaveFrame);
		popup.insertSeparator();
		// Some weird crash bug with clipboard
		//popup.insertItem("Copy Animation Location", CopyURL);
	    } else {
		popup.insertItem("Save Image As...", SaveFrame);
		popup.insertSeparator();
		//popup.insertItem("Copy Image Location", CopyURL);
	    }
	    //if ( !href.isEmpty() )
		//popup.insertItem("Copy Link Location", CopyLink);
	    //popup.insertItem("Copy Frame", CopyFrame);
	    popup.insertSeparator();
	    if ( paused )
		popup.insertItem("Resume", Resume);
	    else
		popup.insertItem("Pause", Pause);
	    popup.insertSeparator();
	    popup.insertItem("About Plugin...", About);
	    int r = popup.exec(me->globalPos());
	    QString filename;
	    switch ( r ) {
		case SaveFrame:
		    filename = QFileDialog::getSaveFileName("","*.png",this);
		    if ( !filename.isEmpty() ) {
			QFileInfo fi(filename);
			QString fmt = fi.extension();
			if ( fmt.isEmpty() ) {
			    fmt = "PNG";
			    filename += ".png";
			}
			if ( !movie.frameImage().save(filename,fmt.upper()) ) {
			    QMessageBox::warning( this, "Save failed",
				"<h2>Error</h2>Could not write to "
				    +filename+" in "+fmt.upper()+" format. "
				"<p>The following file types are supported: "+
				    QImage::outputFormatList().join(", ")+
				", but note that PNG will not work unless "
				"your Netscape uses the same libpng as this "
				"plugin was built with."
			    );
			}
		    }
		    break;
		case SaveAnimation:
		    // Needs some Qt support for getting access to the MNG
		    break;
		case CopyFrame:
		    QApplication::clipboard()->setImage(movie.frameImage());
		    break;
		case OpenLink:
		    instance()->getURL(href,"_blank");
		    break;
		case CopyLink:
		    QApplication::clipboard()->setText(href);
		    break;
		case CopyURL:
		    QApplication::clipboard()->setText(url);
		    break;
		case Pause:
		    paused = TRUE;
		    break;
		case Resume:
		    paused = FALSE;
		    break;
		case About: {
			int i = QMessageBox::information(this, "About Plugin",
			    "<h2>MNG Plugin - libmng + Qt</h2> "
			    "This plugin views MNG images. It is built "
			    "using libmng and Qt.",
			    "libmng?",
			    "Qt?",
			    "OK", 2);
			if ( i == 0 )
			    instance()->getURL("http://www.libmng.com/","_blank");
			else if ( i == 1 )
			    instance()->getURL("http://www.trolltech.com/products/qt/","_blank");
		    }
		    break;
	    }
	    if ( !paused )
		movie.unpause();
	}
    }

    void mouseReleaseEvent(QMouseEvent* me)
    {
	if ( me->button() != RightButton ) {
	    QString href = instance()->arg("HREF");
	    QString target = instance()->arg("TARGET");
	    if ( me->button() == MidButton || (me->state()&ShiftButton) )
		target == "_blank";
	    if ( !href.isNull() )
		instance()->getURL(href,target);
	}
    }

    // Draw the contents of the QFrame - the movie and on-screen-display
    void paintEvent(QPaintEvent*)
    {
	QPainter p(this);

	if ( movie.isNull() )
	    return;

	if ( movie.frameNumber() > 1 )
	    multiframe = TRUE;

        // Get the current movie frame.
        QPixmap pm = movie.framePixmap();

	if ( pm.isNull() )
	    return;

        // Get the area we have to draw in.
        QRect r = rect() & movie.getValidRect();
	if ( r != rect() )
	    p.eraseRect(rect());

        // Only rescale is we need to - it can take CPU!
        if ( r.size() != pm.size() ) {
            QWMatrix m;
            m.scale((double)r.width()/pm.width(),
                    (double)r.height()/pm.height());
            pm = pm.xForm(m);
        }

        // Draw the [possibly scaled] frame.  movieUpdated() below calls
        // repaint with only the changed area, so clipping will ensure we
        // only do the minimum amount of rendering.
        //
        p.drawPixmap(r.x(), r.y(), pm);


#if 0 // dumb

        // The on-screen display

        const char* message = 0;

        if (movie.paused()) {
            message = "PAUSED";
        } else if (movie.finished()) {
            message = "THE END";
        } else if (movie.steps() > 0) {
            message = "FF >>";
        }

        if (message) {
            // Find a good font size...
            p.setFont(QFont("Helvetica", 24));

            QFontMetrics fm = p.fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p.setFont(QFont("Helvetica", 18));

            fm = p.fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p.setFont(QFont("Helvetica", 14));

            fm = p.fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p.setFont(QFont("Helvetica", 12));

            fm = p.fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p.setFont(QFont("Helvetica", 10));

            // "Shadow" effect.
            p.setPen(black);
            p.drawText(1, 1, width()-1, height()-1, AlignCenter, message);
            p.setPen(white);
            p.drawText(0, 0, width()-1, height()-1, AlignCenter, message);
        }
#endif
    }

private slots:
    void movieUpdated(const QRect& area)
    {
        if (!isVisible())
            show();

        // The given area of the movie has changed.

        QRect r = rect();

        if ( r.size() != movie.framePixmap().size() ) {
            // Need to scale - redraw whole frame.
            repaint( r );
        } else {
            // Only redraw the changed area of the frame
            repaint( area.x()+r.x(), area.y()+r.x(),
                     area.width(), area.height() );
        }
    }

    void movieResized(const QSize& size)
    {
        // The movie changed size, probably from its initial zero size.

        resize( size.width(), size.height() );
    }

    void movieStatus(int status)
    {
        // The movie has sent us a status message.

        if (status < 0) {
	    // #### Give message?
        } else if (status == QMovie::Paused || status == QMovie::EndOfMovie) {
            repaint(); // Ensure status text is displayed
        }
    }
};


class MovieLoader : public QNPInstance {
    MovieView* v;
    QMovie* movie;

public:
    MovieLoader() : v(0), movie(0)
    {
    }

    QNPWidget* newWindow()
    {
	if ( !movie ) movie = new QMovie(4096);
	v = new MovieView(*movie);
	return v;
    }

    int writeReady(QNPStream*)
    {
	return movie ? movie->pushSpace() : 0;
    }

    int write(QNPStream* str, int /*offset*/, int len, void* buffer)
    {
	if ( !movie ) return 0;
	int l = movie->pushSpace();
	if ( l > len ) l = len;
	if ( v ) v->setUrl(str->url()); // ##### too many times
	movie->pushData((const uchar*)buffer,l);
	return l;
    }
};

class MoviePlugin : public QNPlugin {

public:
    MoviePlugin()
    {
    }

    QNPInstance* newInstance()
    {
	return new MovieLoader;
    }

    const char* getMIMEDescription() const
    {
	return "video/x-mng:mng:MNG animation;"
	       "video/mng::MNG animation;"
	       "image/x-jng:jng:MNG animation;"
	       "image/jng::JNG animation";
    }

    const char * getPluginNameString() const
    {
	return "MNG plugin (libmng + Qt)";
    }

    const char * getPluginDescriptionString() const
    {
	return "Supports all movie formats supported by Qt";
    }
};

QNPlugin* QNPlugin::create()
{
    return new MoviePlugin;
}

#include "main.moc"
