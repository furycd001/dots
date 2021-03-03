    /*

    Config option definitions private to KDM

    $Id: kdm_config.h,v 1.5 2001/07/03 10:20:13 ossi Exp $

    Copyright (C) 2001 Oswald Buddenhagen <ossi@kde.org>


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifndef _KDM_CONFIG_H_
#define _KDM_CONFIG_H_

#include <greet.h>

#define C_grabServer		(C_TYPE_INT | 0x1000)
#define C_grabTimeout		(C_TYPE_INT | 0x1001)	
#define C_authComplain		(C_TYPE_INT | 0x1002)

#define C_useLilo		(C_TYPE_INT | 0x1008)
#define C_liloCmd		(C_TYPE_STR | 0x1009)
#define C_liloMap		(C_TYPE_STR | 0x100a)

#define C_SessionTypes		(C_TYPE_ARGV | 0x1010)
#define C_GUIStyle		(C_TYPE_INT | 0x1011)
# define GUI_KDE	0
# define GUI_Windows	1
# define GUI_Platinum	2
# define GUI_Motif	3
# define GUI_MotifPlus	4
# define GUI_CDE	5
# define GUI_SGI	6
#define C_LogoArea		(C_TYPE_INT | 0x1012)	/* XXX to change */
# define LOGO_NONE	0
# define LOGO_LOGO	1
# define LOGO_CLOCK	2
#define C_LogoPixmap		(C_TYPE_STR | 0x1013)
#define C_GreeterPosFixed	(C_TYPE_INT | 0x1014)
#define C_GreeterPosX		(C_TYPE_INT | 0x1015)
#define C_GreeterPosY		(C_TYPE_INT | 0x1016)
#define C_StdFont		(C_TYPE_STR | 0x1017)
#define C_FailFont		(C_TYPE_STR | 0x1018)
#define C_GreetString		(C_TYPE_STR | 0x1019)
#define C_GreetFont		(C_TYPE_STR | 0x101a)
#define C_Language		(C_TYPE_STR | 0x101b)
#define C_ShowUsers		(C_TYPE_INT | 0x101c)
# define SHOW_ALL	0
# define SHOW_SEL	1
# define SHOW_NONE	2
#define C_Users			(C_TYPE_ARGV | 0x101d)
#define C_NoUsers		(C_TYPE_ARGV | 0x101e)
#define C_MinShowUID		(C_TYPE_INT | 0x101f)
#define C_MaxShowUID		(C_TYPE_INT | 0x1020)
#define C_SortUsers		(C_TYPE_INT | 0x1021)
#define C_PreselectUser		(C_TYPE_INT | 0x1022)
# define PRESEL_NONE	0
# define PRESEL_PREV	1
# define PRESEL_DEFAULT	2
#define C_DefaultUser		(C_TYPE_STR | 0x1023)
#define C_FocusPasswd		(C_TYPE_INT | 0x1024)
#define C_EchoMode		(C_TYPE_INT | 0x1025)
# define ECHO_ONE	0	/* HACK! This must be equal to KPasswordEdit::EchoModes (kpassdlg.h) */
# define ECHO_THREE	1
# define ECHO_NONE	2
#define C_AllowShutdown		(C_TYPE_INT | 0x1026)
# define SHUT_NONE	0
# define SHUT_ROOT	1
# define SHUT_ALL	2

#define C_BackgroundMode	(C_TYPE_STR | 0x1100)
#define C_BlendBalance		(C_TYPE_INT | 0x1101)
#define C_BlendMode		(C_TYPE_STR | 0x1102)
#define C_ChangeInterval	(C_TYPE_INT | 0x1103)
#define C_Color1		(C_TYPE_STR | 0x1104)
#define C_Color2		(C_TYPE_STR | 0x1105)
#define C_CurrentWallpaper	(C_TYPE_INT | 0x1106)
#define C_LastChange		(C_TYPE_INT | 0x1107)
#define C_MultiWallpaperMode	(C_TYPE_STR | 0x1108)
#define C_Pattern		(C_TYPE_STR | 0x1109)
#define C_Program		(C_TYPE_STR | 0x110a)
#define C_ReverseBlending	(C_TYPE_INT | 0x110b)
#define C_Wallpaper		(C_TYPE_STR | 0x110c)	
#define C_WallpaperList		(C_TYPE_ARGV | 0x110d)
#define C_WallpaperMode		(C_TYPE_STR | 0x110e)

#endif /* _KDM_CONFIG_H_ */
