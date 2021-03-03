/****************************************************************************
** $Id: qt/src/kernel/qpaintdevicemetrics.h   2.3.2   edited 2001-01-26 $
**
** Definition of QPaintDeviceMetrics class
**
** Created : 941109
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
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
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
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

#ifndef QPAINTDEVICEMETRICS_H
#define QPAINTDEVICEMETRICS_H

#ifndef QT_H
#include "qwindowdefs.h"
#include "qpaintdevice.h"
#endif // QT_H


class Q_EXPORT QPaintDeviceMetrics			// paint device metrics
{
public:
    QPaintDeviceMetrics( const QPaintDevice * );

    enum {
	PdmWidth = 1,
	PdmHeight,
	PdmWidthMM,
	PdmHeightMM,
	PdmNumColors,
	PdmDepth,
	PdmDpiX,
	PdmDpiY
    };

    int	  width()	const	{ return (int)pdev->metric(PdmWidth); }
    int	  height()	const	{ return (int)pdev->metric(PdmHeight); }
    int	  widthMM()	const	{ return (int)pdev->metric(PdmWidthMM); }
    int	  heightMM()	const	{ return (int)pdev->metric(PdmHeightMM); }
    int	  logicalDpiX()	const	{ return (int)pdev->metric(PdmDpiX); }
    int	  logicalDpiY()	const	{ return (int)pdev->metric(PdmDpiY); }
    int	  numColors()	const	{ return (int)pdev->metric(PdmNumColors); }
    int	  depth()	const	{ return (int)pdev->metric(PdmDepth); }

private:
    QPaintDevice *pdev;
};


#endif // QPAINTDEVICEMETRICS_H
