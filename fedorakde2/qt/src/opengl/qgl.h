/****************************************************************************
** $Id: qt/src/opengl/qgl.h   2.3.2   edited 2001-01-26 $
**
** Definition of OpenGL classes for Qt
**
** Created : 970112
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the opengl module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QGL_H
#define QGL_H


#ifndef QT_H
#include <qwidget.h>
#endif // QT_H

#define QGL_VERSION	450
#define QGL_VERSION_STR	"4.5"

Q_EXPORT const char *qGLVersion();

#if !(defined(Q_WGL) || defined(Q_GLX))
#if defined(_OS_WIN32_)
#define Q_WGL
#else
#define Q_GLX
#endif
#endif

#if defined(Q_WGL)
#include <qt_windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

class QGLColorMap;
class QPixmap;
#if defined(Q_GLX)
class QGLOverlayWidget;
#endif

// Namespace class:
class Q_EXPORT QGL
{
public:
    enum FormatOption {
	DoubleBuffer		= 0x0001,
	DepthBuffer		= 0x0002,
	Rgba			= 0x0004,
	AlphaChannel		= 0x0008,
	AccumBuffer		= 0x0010,
	StencilBuffer		= 0x0020,
	StereoBuffers		= 0x0040,
	DirectRendering		= 0x0080,
	HasOverlay		= 0x0100,
	SingleBuffer            = DoubleBuffer  << 16,
	NoDepthBuffer           = DepthBuffer   << 16,
	ColorIndex              = Rgba          << 16,
	NoAlphaChannel          = AlphaChannel  << 16,
	NoAccumBuffer           = AccumBuffer   << 16,
	NoStencilBuffer         = StencilBuffer << 16,
	NoStereoBuffers         = StereoBuffers << 16,
	IndirectRendering       = DirectRendering << 16,
	NoOverlay       	= HasOverlay << 16
    };
};



class Q_EXPORT QGLFormat : public QGL
{
public:
    QGLFormat();
    QGLFormat( int options, int plane = 0 );

    bool    		doubleBuffer() const;
    void    		setDoubleBuffer( bool enable );
    bool    		depth() const;
    void    		setDepth( bool enable );
    bool    		rgba() const;
    void    		setRgba( bool enable );
    bool    		alpha() const;
    void    		setAlpha( bool enable );
    bool    		accum() const;
    void    		setAccum( bool enable );
    bool    		stencil() const;
    void    		setStencil( bool enable );
    bool    		stereo() const;
    void    		setStereo( bool enable );
    bool    		directRendering() const;
    void    		setDirectRendering( bool enable );
    bool    		hasOverlay() const;
    void    		setOverlay( bool enable );

    int			plane() const;
    void		setPlane( int plane );

    void		setOption( FormatOption opt );
    bool		testOption( FormatOption opt ) const;
    
    static QGLFormat	defaultFormat();
    static void		setDefaultFormat( const QGLFormat& f );

    static QGLFormat	defaultOverlayFormat();
    static void		setDefaultOverlayFormat( const QGLFormat& f );

    static bool		hasOpenGL();
    static bool		hasOpenGLOverlays();

    friend Q_EXPORT bool operator==( const QGLFormat&, const QGLFormat& );
    friend Q_EXPORT bool operator!=( const QGLFormat&, const QGLFormat& );
    
private:
    uint opts;
    int pln;
};


Q_EXPORT bool operator==( const QGLFormat&, const QGLFormat& );
Q_EXPORT bool operator!=( const QGLFormat&, const QGLFormat& );


class Q_EXPORT QGLContext : public QGL
{
public:
    QGLContext( const QGLFormat& format, QPaintDevice* device );
    virtual ~QGLContext();

    virtual bool	create( const QGLContext* shareContext = 0 );
    bool		isValid() const;
    bool		isSharing() const;
    virtual void	reset();

    QGLFormat		format() const;
    virtual void	setFormat( const QGLFormat& format );

    virtual void	makeCurrent();
    virtual void	swapBuffers() const;

    QPaintDevice*	device() const;

    QColor		overlayTransparentColor() const;

    static const QGLContext*	currentContext();

protected:
    virtual bool	chooseContext( const QGLContext* shareContext = 0 );
    virtual void	doneCurrent();
    
#if defined(Q_WGL)
    virtual int		choosePixelFormat( void* pfd, HDC pdc );
#elif defined(Q_GLX)
    virtual void*	tryVisual( const QGLFormat& f, int bufDepth = 1 );
    virtual void*	chooseVisual();
#endif

    bool		deviceIsPixmap() const;
    bool		windowCreated() const;
    void		setWindowCreated( bool on );
    bool		initialized() const;
    void		setInitialized( bool on );

    uint		colorIndex( const QColor& c ) const;

protected:
#if defined(Q_WGL)
    HGLRC		rc;
    HDC			dc;
    WId			win;
    int			pixelFormatId;
    QGLColorMap*		cmap;
#elif defined(Q_GLX)
    void*		vi;
    void*		cx;
    Q_UINT32		gpm;
#endif

    QGLFormat		glFormat;

private:
    bool		valid;
    bool		sharing;
    bool		initDone;
    bool		crWin;
    QPaintDevice*	paintDevice;
    QColor		transpColor;
    static QGLContext*	currentCtx;

    friend class QGLWidget;
    
private:	// Disabled copy constructor and operator=
    QGLContext() {}
    QGLContext( const QGLContext& ) {}
    QGLContext&		operator=( const QGLContext& ) { return *this; }
};




class Q_EXPORT QGLWidget : public QWidget, public QGL
{
    Q_OBJECT
public:
    QGLWidget( QWidget* parent=0, const char* name=0,
	       const QGLWidget* shareWidget = 0, WFlags f=0 );
    QGLWidget( const QGLFormat& format, QWidget* parent=0, const char* name=0,
	       const QGLWidget* shareWidget = 0, WFlags f=0 );
   ~QGLWidget();

    void		qglColor( const QColor& c ) const;
    void		qglClearColor( const QColor& c ) const;

    bool		isValid() const;
    bool		isSharing() const;
    virtual void	makeCurrent();

    bool		doubleBuffer() const;
    virtual void	swapBuffers();

    QGLFormat		format() const;
#if 1
    virtual void	setFormat( const QGLFormat& format ); // OBSOLETE
#endif

    const QGLContext*	context() const;
#if 1
    virtual void	setContext( QGLContext* context,
				    const QGLContext* shareContext = 0,
				    bool deleteOldContext = TRUE ); // OBSOLETE
#endif

    virtual QPixmap	renderPixmap( int w = 0, int h = 0,
				      bool useContext = FALSE );

    virtual void	makeOverlayCurrent();
    const QGLContext*	overlayContext() const;

    void		setMouseTracking( bool enable );

    static QImage	convertToGLFormat( const QImage& img );

public slots:
    virtual void	updateGL();
    virtual void	updateOverlayGL();

protected:
    virtual void	initializeGL();
    virtual void	resizeGL( int w, int h );
    virtual void	paintGL();

    virtual void	initializeOverlayGL();
    virtual void	resizeOverlayGL( int w, int h );
    virtual void	paintOverlayGL();

    void		setAutoBufferSwap( bool on );
    bool		autoBufferSwap() const;

    void		paintEvent( QPaintEvent* );
    void		resizeEvent( QResizeEvent* );

    virtual void	glInit();
    virtual void	glDraw();
    
private:
    void		init( const QGLFormat& fmt,
			      const QGLWidget* shareWidget );
    bool		renderCxPm( QPixmap* pm );
    QGLContext*		glcx;
    bool		autoSwap;

#if defined(Q_WGL)
    QGLContext*		olcx;
#elif defined(Q_GLX)
    QGLOverlayWidget*	olw;
    friend class QGLOverlayWidget;
#endif

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QGLWidget( const QGLWidget& );
    QGLWidget&		operator=( const QGLWidget& );
#endif
};


//
// QGLFormat inline functions
//

inline bool QGLFormat::doubleBuffer() const
{
    return testOption( DoubleBuffer );
}

inline bool QGLFormat::depth() const
{
    return testOption( DepthBuffer );
}

inline bool QGLFormat::rgba() const
{
    return testOption( Rgba );
}

inline bool QGLFormat::alpha() const
{
    return testOption( AlphaChannel );
}

inline bool QGLFormat::accum() const
{
    return testOption( AccumBuffer );
}

inline bool QGLFormat::stencil() const
{
    return testOption( StencilBuffer );
}

inline bool QGLFormat::stereo() const
{
    return testOption( StereoBuffers );
}

inline bool QGLFormat::directRendering() const
{
    return testOption( DirectRendering );
}

inline bool QGLFormat::hasOverlay() const
{
    return testOption( HasOverlay );
}

//
// QGLContext inline functions
//

inline bool QGLContext::isValid() const
{
    return valid;
}
inline bool QGLContext::isSharing() const
{
    return sharing;
}
inline QGLFormat QGLContext::format() const
{
    return glFormat;
}

inline QPaintDevice* QGLContext::device() const
{
    return paintDevice;
}

inline bool QGLContext::deviceIsPixmap() const
{
    return paintDevice->devType() == QInternal::Pixmap;
}


inline bool QGLContext::windowCreated() const
{
    return crWin;
}


inline void QGLContext::setWindowCreated( bool on )
{
    crWin = on;
}

inline bool QGLContext::initialized() const
{
    return initDone;
}

inline void QGLContext::setInitialized( bool on )
{
    initDone = on;
}

inline const QGLContext* QGLContext::currentContext()
{
    return currentCtx;
}

//
// QGLWidget inline functions
//

inline QGLFormat QGLWidget::format() const
{
    return glcx->format();
}

inline const QGLContext *QGLWidget::context() const
{
    return glcx;
}

inline bool QGLWidget::doubleBuffer() const
{
    return glcx->format().doubleBuffer();
}

inline void QGLWidget::setAutoBufferSwap( bool on )
{
    autoSwap = on;
}

inline bool QGLWidget::autoBufferSwap() const
{
    return autoSwap;
}


#endif // QGL_H
