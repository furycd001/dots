/****************************************************************************

 KHotKeys -  (C) 1999 Lubos Lunak <l.lunak@email.cz>

 khkglobalaccel.h  - Slightly modified KGlobalAccel from kdelibs

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.

 $Id: khkglobalaccel.h,v 1.1.1.1 2000/07/19 16:50:09 elter Exp $

****************************************************************************/

#ifndef KHKGLOBALACCEL_H 
#define KHKGLOBALACCEL_H 

#include <kglobalaccel.h>
#include <X11/Xlib.h>
#undef Bool   /* stupid X11 */

/**
 * Since KGlobalAccel requires different slots for each accelerator, 
 * this class overrides its x11EventFilter() to make varying number
 * of accelerators possible. Now the slots can have 2 arguments,
 * see @ref #activated () .
 * 
 *
 * @short KHKGlobalAccel class - slight improvement of KGlobalAccel
 * @author Lubos Lunak <l.lunak@email.cz>
 * @version 0.2
 * @see KGlobalAccel
 */
 
class KHKGlobalAccel
    : public KGlobalAccel
    {
    Q_OBJECT
    public:
	KHKGlobalAccel();
    	bool x11EventFilter(const XEvent *);
    signals:
	/**
	* "Better" activated() signal
	* @param action is the accelerator item action name
	* @param descr is the description 
	* @param keyCode is the accelerator keycode, use @ref keyToString()
	*   to get its name
	* @see KKeyEntry
	*/
    	void activated( const QString& action, const QString& descr,
    	    int keyCode );
    };

#endif // KHKGLOBALACCEL_H
