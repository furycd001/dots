/****************************************************************************
** $Id: qt/examples/movies/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qmovie.h>


class MovieScreen : public QFrame {
    Q_OBJECT
    QMovie movie;
    QString filename;

public:
    MovieScreen(const char* fname, QMovie m, QWidget* p=0, const char* name=0, WFlags f=0) :
        QFrame(p, name, f)
    {
        setCaption(fname);
        filename = fname;
        movie = m;

        // Set a frame around the movie.
        setFrameStyle(QFrame::WinPanel|QFrame::Sunken);

        // No background needed, since we draw on the whole widget.
        movie.setBackgroundColor(backgroundColor());
        setBackgroundMode(NoBackground);

        // Get the movie to tell use when interesting things happen.
        movie.connectUpdate(this, SLOT(movieUpdated(const QRect&)));
        movie.connectResize(this, SLOT(movieResized(const QSize&)));
        movie.connectStatus(this, SLOT(movieStatus(int)));
    }


protected:

    // Draw the contents of the QFrame - the movie and on-screen-display
    void drawContents(QPainter* p)
    {
        // Get the current movie frame.
        QPixmap pm = movie.framePixmap();

        // Get the area we have to draw in.
        QRect r = contentsRect();

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
        p->drawPixmap(r.x(), r.y(), pm);


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
            p->setFont(QFont("Helvetica", 24));

            QFontMetrics fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(QFont("Helvetica", 18));

            fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(QFont("Helvetica", 14));

            fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(QFont("Helvetica", 12));

            fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(QFont("Helvetica", 10));

            // "Shadow" effect.
            p->setPen(black);
            p->drawText(1, 1, width()-1, height()-1, AlignCenter, message);
            p->setPen(white);
            p->drawText(0, 0, width()-1, height()-1, AlignCenter, message);
        }
    }

    void mouseReleaseEvent(QMouseEvent* event)
    {
        // Do what the Help says...

        if (event->state() & ShiftButton) {
            movie.restart();
        } else if (!movie.paused()) {
            movie.pause();
        } else {
            if (event->button() & LeftButton)
                movie.step((event->state() & ControlButton) ? 10 : 1);
            else if (event->button() & (MidButton|RightButton))
                movie.unpause();
        }

        repaint(); // To hide/show "PAUSED".
    }

private slots:
    void movieUpdated(const QRect& area)
    {
        if (!isVisible())
            show();

        // The given area of the movie has changed.

        QRect r = contentsRect();

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

        int fw = frameWidth();
        resize( size.width() + fw*2, size.height() + fw*2 );
        qApp->setMainWidget(this); // Just geometry, etc.
        qApp->setMainWidget(0); // Not Close==Quit
    }

    void movieStatus(int status)
    {
        // The movie has sent us a status message.

        if (status < 0) {
            QString msg;
            msg.sprintf("Could not play movie \"%s\"", (const char*)filename);
            QMessageBox::message("Error", msg);
        } else if (status == QMovie::Paused || status == QMovie::EndOfMovie) {
            repaint(); // Ensure status text is displayed
        }
    }
};


// A QFileDialog that chooses movies.
//
class MovieStarter : public QFileDialog {
    Q_OBJECT
public:
    MovieStarter(const char *dir) :
        QFileDialog(dir, "*.gif")
    {
        //behave as in getOpenFilename
        setMode( ExistingFile );
        // When a file is selected, show it as a movie.
        connect(this, SIGNAL(fileSelected(const QString&)),
                this, SLOT(startMovie(const QString&)));
    }

public slots:
    void startMovie(const QString& filename)
    {
        if ( filename ) // Start a new movie - have it delete when closed.
            (void)new MovieScreen(filename, QMovie(filename), 0, 0, WDestructiveClose);
    }

    // QDialog's method - normally closes the file dialog.
    // We want it left open, and we want Cancel to quit everything.
    void done( int r )
    {
        if (r != Accepted) qApp->quit(); // end on Cancel
        setResult( r );

        // And don't hide.
    }
};


int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    if (argc > 1) {
        // Commandline mode - show movies given on the command line
        //
        for (int arg=1; arg<argc; arg++)
            (void)new MovieScreen(argv[arg], QMovie(argv[arg]), 0, 0,
                                  Qt::WDestructiveClose);
        QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));
    } else {
        // "GUI" mode - open a chooser for movies
        //
        MovieStarter* fd = new MovieStarter(".");
        fd->show();

        // Some help text to explain the `hidden' features.
        QLabel* help = new QLabel(
            "Choose some movies.\n\n"
            "Shift-click to Restart.\n"
            "Click to Pause,\n"
            "then left-click to Step,\n"
            "control-left-click to Step 10,\n"
            "right-click to Unpause\n\n"
            "Windows may be resized to enlarge movie.", 0 );
        help->setCaption( "Qt Examples - Movies" );
        help->setIndent( 10 );
        help->show();
    }

    // Go!
    return a.exec();
}

#include "main.moc"
