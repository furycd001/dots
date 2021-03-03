/* vi: ts=8 sts=4 sw=4
 *
 * $Id: bgdefaults.h,v 1.8 2001/02/18 05:34:37 rahn Exp $
 *
 * This file is part of the KDE project, module kdesktop.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 * 
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */
#ifndef __BGDefaults_h_Included__
#define __BGDefaults_h_Included__


// Globals
#define _defCommon true
#define _defDock true
#define _defExport false
#define _defLimitCache true
#define _defCacheSize 2048

// Per desktop defaults
// Before you change this get in touch with me (torsten@kde.org)
// Thanks!!
#define _defColorA  QColor("#1E72A0")
#define _defColorB  QColor("#C0C0C0")
#define _defBackgroundMode KBackgroundSettings::VerticalGradient
#define _defWallpaperMode KBackgroundSettings::NoWallpaper
#define _defMultiMode KBackgroundSettings::NoMulti
#define _defBlendMode KBackgroundSettings::NoBlending
#define _defBlendBalance 100
#define _defReverseBlending false

#endif // __BGDefaults_h_Included__
