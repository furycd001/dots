// This library is distributed under the conditions of the GNU LGPL.

#include "config.h"

#ifdef HAVE_LIBTIFF

#include <tiffio.h>

#include <qimage.h>
#include <qfile.h>

#include "tiffr.h"

void kimgio_tiff_read( QImageIO *io )
{
	TIFF *tiff;
	uint32 width, height;
	uint32 *data;

	// FIXME: use qdatastream

	// open file
	tiff = TIFFOpen( QFile::encodeName(io->fileName()), "r" );

	if( tiff == 0 ) {
		return;
	}

	// create image with loaded dimensions
	TIFFGetField( tiff, TIFFTAG_IMAGEWIDTH,	&width );
	TIFFGetField( tiff, TIFFTAG_IMAGELENGTH, &height );

	QImage image( width, height, 32 );
	data = (uint32 *)image.bits();

	//Sven: changed to %ld for 64bit machines
	//debug( "unsigned size: %ld, uint32 size: %ld",
	//	(long)sizeof(unsigned), (long)sizeof(uint32) );

	// read data
	bool stat =TIFFReadRGBAImage( tiff, width, height, data );

	if( stat == 0 ) {
		TIFFClose( tiff );
		return;
	}

	// reverse red and blue
	for( unsigned i = 0; i < width * height; ++i )
	{
		uint32 red = ( 0x00FF0000 & data[i] ) >> 16;
		uint32 blue = ( 0x000000FF & data[i] ) << 16;
		data[i] &= 0xFF00FF00;
		data[i] += red + blue;
	}

	// reverse image (it's upside down)
	for( unsigned ctr = 0; ctr < (height>>1); ) {
		unsigned *line1 = (unsigned *)image.scanLine( ctr );
		unsigned *line2 = (unsigned *)image.scanLine( height 
			- ( ++ctr ) );

		for( unsigned x = 0; x < width; x++ ) {
			int temp = *line1;
			*line1 = *line2;
			*line2 = temp;
			line1++;
			line2++;
		}

		// swap rows
	}

	// set channel order to Qt order
	// FIXME: Right now they are the same, but will it change?

//	for( int ctr = (image.numBytes() / sizeof(uint32))+1; ctr ; ctr-- ) {
//		// TODO: manage alpha with TIFFGetA
//		*data = qRgb( TIFFGetR( *data ), 
//			TIFFGetG( *data ), TIFFGetB( *data ) );
//		data++;
//	}
	TIFFClose( tiff );

	io->setImage( image );
	io->setStatus ( 0 );
}

void kimgio_tiff_write( QImageIO * )
{
	// TODO: stub
}

#endif
