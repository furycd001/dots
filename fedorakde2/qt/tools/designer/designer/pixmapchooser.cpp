/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qvariant.h> // HP-UX compiler needs this here

#include "pixmapchooser.h"
#include "formwindow.h"
#include "pixmapfunction.h"
#include "metadatabase.h"
#include "mainwindow.h"

#include <qapplication.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "../pics/images.h"

static const char *logo_xpm[] = {
/* width height num_colors chars_per_pixel */
"21 16 213 2",
"  	c white",
". 	c #A3C511",
"+ 	c #A2C511",
"@ 	c #A2C611",
"# 	c #A2C510",
"$ 	c #A2C513",
"% 	c #A2C412",
"& 	c #A2C413",
"* 	c #A2C414",
"= 	c #A2C515",
"- 	c #A2C50F",
"; 	c #A3C510",
"> 	c #A2C410",
", 	c #A2C411",
"' 	c #A2C314",
") 	c #A2C316",
"! 	c #A2C416",
"~ 	c #A0C315",
"{ 	c #A1C313",
"] 	c #A1C412",
"^ 	c #A2C40F",
"/ 	c #A1C410",
"( 	c #A0C510",
"_ 	c #A0C511",
": 	c #A1C414",
"< 	c #9FC30E",
"[ 	c #98B51B",
"} 	c #5F7609",
"| 	c #5C6E0E",
"1 	c #5B6E10",
"2 	c #5C6C14",
"3 	c #5A6E0A",
"4 	c #839E16",
"5 	c #A0C515",
"6 	c #A0C513",
"7 	c #A2C512",
"8 	c #A1C512",
"9 	c #A1C511",
"0 	c #A1C50F",
"a 	c #91AE12",
"b 	c #505E11",
"c 	c #1F2213",
"d 	c #070606",
"e 	c #040204",
"f 	c #040306",
"g 	c #15160F",
"h 	c #2F3A0D",
"i 	c #859F1B",
"j 	c #A1C215",
"k 	c #A0C50F",
"l 	c #A1C510",
"m 	c #A0C110",
"n 	c #839C1B",
"o 	c #1E240A",
"p 	c #050205",
"q 	c #030304",
"r 	c #323917",
"s 	c #556313",
"t 	c #56680B",
"u 	c #536609",
"v 	c #4A561B",
"w 	c #0B0D04",
"x 	c #030208",
"y 	c #090A05",
"z 	c #5F6F18",
"A 	c #A0C117",
"B 	c #91AF10",
"C 	c #1E2209",
"D 	c #030205",
"E 	c #17190D",
"F 	c #7D981C",
"G 	c #9ABA12",
"H 	c #A3C411",
"I 	c #A3C713",
"J 	c #95B717",
"K 	c #7F9A18",
"L 	c #8FAE1B",
"M 	c #394413",
"N 	c #040305",
"O 	c #090807",
"P 	c #6C7E19",
"Q 	c #A6C614",
"R 	c #A1C411",
"S 	c #64761F",
"T 	c #030105",
"U 	c #070707",
"V 	c #728513",
"W 	c #A2C40C",
"X 	c #A2C70B",
"Y 	c #89A519",
"Z 	c #313B11",
"` 	c #101409",
" .	c #586A19",
"..	c #97B620",
"+.	c #1B2207",
"@.	c #282D11",
"#.	c #A6C41B",
"$.	c #A1C413",
"%.	c #A3C512",
"&.	c #2E370B",
"*.	c #030108",
"=.	c #21260F",
"-.	c #A5C21A",
";.	c #A0C60D",
">.	c #6D841A",
",.	c #0F1007",
"'.	c #040207",
").	c #0E1009",
"!.	c #515F14",
"~.	c #A2C41B",
"{.	c #5E701B",
"].	c #030203",
"^.	c #0B0B04",
"/.	c #87A111",
"(.	c #A0C411",
"_.	c #A0C316",
":.	c #212907",
"<.	c #222C0B",
"[.	c #A3C516",
"}.	c #9CBE1A",
"|.	c #5E6F1B",
"1.	c #0E0F0B",
"2.	c #040205",
"3.	c #181B0D",
"4.	c #93AE25",
"5.	c #A0C610",
"6.	c #617715",
"7.	c #030306",
"8.	c #070704",
"9.	c #809818",
"0.	c #A1C415",
"a.	c #475416",
"b.	c #030309",
"c.	c #12170B",
"d.	c #91B01E",
"e.	c #5C721F",
"f.	c #05050B",
"g.	c #33371D",
"h.	c #0E0F08",
"i.	c #040405",
"j.	c #758921",
"k.	c #46511B",
"l.	c #030207",
"m.	c #131409",
"n.	c #9FB921",
"o.	c #859D21",
"p.	c #080809",
"q.	c #030305",
"r.	c #46521C",
"s.	c #8EB017",
"t.	c #627713",
"u.	c #4D5F17",
"v.	c #97B71D",
"w.	c #77901D",
"x.	c #151708",
"y.	c #0D0D0B",
"z.	c #0C0B08",
"A.	c #455216",
"B.	c #A5C616",
"C.	c #A0C114",
"D.	c #556118",
"E.	c #050307",
"F.	c #050407",
"G.	c #363E17",
"H.	c #5D7309",
"I.	c #A2BF28",
"J.	c #A2C417",
"K.	c #A4C620",
"L.	c #60701D",
"M.	c #030103",
"N.	c #030303",
"O.	c #809A1B",
"P.	c #A0C310",
"Q.	c #A0C410",
"R.	c #A3C415",
"S.	c #9CB913",
"T.	c #6F801F",
"U.	c #1A210A",
"V.	c #1D1E0D",
"W.	c #1D220F",
"X.	c #1E210F",
"Y.	c #0F0F07",
"Z.	c #0E1007",
"`.	c #090906",
" +	c #2B360E",
".+	c #97B813",
"++	c #A2C50E",
"@+	c #A5C517",
"#+	c #90AD20",
"$+	c #5D6C1A",
"%+	c #394115",
"&+	c #050704",
"*+	c #040304",
"=+	c #202807",
"-+	c #5E6B21",
";+	c #728D0C",
">+	c #65791D",
",+	c #29330F",
"'+	c #7A911D",
")+	c #A2C614",
"!+	c #A1C513",
"~+	c #A3C50E",
"{+	c #A3C414",
"]+	c #9CBD11",
"^+	c #95B40C",
"/+	c #94B50F",
"(+	c #95B510",
"_+	c #99B913",
":+	c #A0C414",
"<+	c #9ABC11",
"[+	c #A0C314",
"}+	c #A1C40F",
"|+	c #A3C513",
". + + @ + # # $ % & * = & - + + + + + # # ",
"; > , > # > > $ ' ) ! ~ { ] ^ , - > , > # ",
"+ + / ( _ : < [ } | 1 2 3 4 5 6 : 7 8 # # ",
"+ 9 # ( 0 a b c d e e e f g h i j 9 k l + ",
"+ + > m n o p q r s t u v w x y z A & # # ",
"# % k B C D E F G H I J K L M N O P Q ] , ",
"$ R > S T U V W , X Y Z `  ...+.T @.#.$.] ",
"% %.* &.*.=.-.;.> >.,.'.).!.~.{.].^./.R 7 ",
"7 (._.:.D <.[.}.|.1.2.2.3.4.5.6.7.8.9._ 8 ",
". % 0.a.b.c.d.e.f.N g.h.2.i.j.k.l.m.n.$ # ",
"; + ; o.p.q.r.s.t.u.v.w.x.2.y.z.].A.B.l : ",
"# # R C.D.E.F.G.H.I.J.K.L.2.M.M.N.O.P.; l ",
"# / Q.R.S.T.U.].8.V.W.X.Y.e Z.`.]. +.+++7 ",
"+ + 9 / ; @+#+$+%+&+e *+=+-+;+>+,+'+)+, # ",
"# + > % & !+~+{+]+^+/+(+_+) Q.:+<+[+$ R # ",
"7 + > }+# % k |+8 + > + * $ _ / , 7 8 ] - "};

static ImageIconProvider *imageIconProvider = 0;
static PixmapChooser *pixmapChooser = 0;

PixmapView::PixmapView( QWidget *parent )
    : QScrollView( parent )
{
    viewport()->setBackgroundMode( PaletteBase );
}

void PixmapView::setPixmap( const QPixmap &pix )
{
    pixmap = pix;
    resizeContents( pixmap.size().width(), pixmap.size().height() );
    viewport()->repaint( FALSE );
}

void PixmapView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    p->fillRect( cx, cy, cw, ch, colorGroup().brush( QColorGroup::Base ) );
    p->drawPixmap( 0, 0, pixmap );
}

void PixmapView::previewUrl( const QUrl &u )
{
    if ( u.isLocalFile() ) {
	QString path = u.path();
	QPixmap pix( path );
	if ( !pix.isNull() )
	    setPixmap( pix );
    } else {
	qWarning( "Previewing remot files not supported" );
    }
}

QPixmap qChoosePixmap( QWidget *parent, FormWindow *fw, const QPixmap &old )
{
    if ( !fw || fw->savePixmapInline() ) {
	if ( !imageIconProvider && !QFileDialog::iconProvider() )
	    QFileDialog::setIconProvider( ( imageIconProvider = new ImageIconProvider ) );

	QString filter;
	QString all = qApp->translate( "qChoosePixmap", "All Pixmaps (" );;
	for ( uint i = 0; i < QImageIO::outputFormats().count(); i++ ) {
	    filter += qApp->translate( "qChoosePixmap", "%1-Pixmaps (%2)\n" ).
		     arg( QImageIO::outputFormats().at( i ) ).
		     arg( "*." + QString( QImageIO::outputFormats().at( i ) ).lower() );
	    all += "*." + QString( QImageIO::outputFormats().at( i ) ).lower() + ";";
	}
	filter.prepend( all + qApp->translate( "qChoosePixmap", ")\n" ) );
	filter += qApp->translate( "qChoosePixmap", "All Files (*)" );

	QFileDialog fd( QString::null, filter, parent, 0, TRUE );
	fd.setContentsPreviewEnabled( TRUE );
	PixmapView *pw = new PixmapView( &fd );
	fd.setContentsPreview( pw, pw );
	fd.setViewMode( QFileDialog::List );
	fd.setPreviewMode( QFileDialog::Contents );
	fd.setCaption( qApp->translate( "qChoosePixmap", "Choose a Pixmap..." ) );
	if ( fd.exec() == QDialog::Accepted ) {
	    QPixmap pix( fd.selectedFile() );
	    MetaDataBase::setPixmapArgument( fw, pix.serialNumber(), fd.selectedFile() );
	    return pix;
	}
    } else {
	PixmapFunction dia( parent, 0, TRUE );
	QObject::connect( dia.helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
	dia.labelFunction->setText( fw->pixmapLoaderFunction() + "(" );
	dia.editArguments->setText( MetaDataBase::pixmapArgument( fw, old.serialNumber() ) );
	dia.editArguments->setFocus();
	if ( dia.exec() == QDialog::Accepted ) {
	    QPixmap pix( PixmapChooser::loadPixmap( "image.xpm" ) );
	    MetaDataBase::setPixmapArgument( fw, pix.serialNumber(), dia.editArguments->text() );
	    return pix;
	}
    }
    return QPixmap();
}

ImageIconProvider::ImageIconProvider( QWidget *parent, const char *name )
    : QFileIconProvider( parent, name ), imagepm( PixmapChooser::loadPixmap( "image.xpm", PixmapChooser::Mini ) )
{
    fmts = QImage::inputFormats();
}

ImageIconProvider::~ImageIconProvider()
{
}

const QPixmap * ImageIconProvider::pixmap( const QFileInfo &fi )
{
    QString ext = fi.extension().upper();
    if ( fmts.contains( ext ) ) {
	return &imagepm;
    } else {
	return QFileIconProvider::pixmap( fi );
    }
}

PixmapChooser::PixmapChooser()
{
    // #### hardcoded at the moment
    miniPixDir = "../pics/mini/";
    noSizePixDir = "../pics/";
    smallPixDir = "../pics/small/";
    largePixDir = "../pics/large/";
}

QString PixmapChooser::pixmapPath( Size size ) const
{
    if ( size == Small )
	return smallPixDir;
    if ( size == Mini )
	return miniPixDir;
    if ( size == NoSize )
	return noSizePixDir;
    return largePixDir;
}

QPixmap PixmapChooser::loadPixmap( const QString &name, Size size )
{
    if ( !pixmapChooser )
	pixmapChooser = new PixmapChooser;

    if ( name == "logo" )
	return QPixmap( logo_xpm );

    if ( name[ 0 ] == '/' || name[ 0 ] == '\\' || name[ 1 ] == ':' )
	return QPixmap( name );

    QString lookup;
    switch ( size ) {
    case Small:
	lookup = "small/";
	break;
    case Disabled:
	lookup = "small/disabled/";
	break;
    case Large:
	lookup = "large/";
	break;
    case Mini:
	lookup = "mini/";
	break;
    default:
	break;
    }

    lookup += name;

    Embed *e = &embed_vec[ 0 ];
    while ( e->name ) {
	if ( QString( e->name ) == lookup ) {
	    QImage img;
	    img.loadFromData( (const uchar*)e->data, e->size );
	    QPixmap pix;
	    pix.convertFromImage( img );
	    return pix;
	}
	e++;
    }

    // fallback
    return QPixmap( pixmapChooser->pixmapPath( size ) + name );
}
