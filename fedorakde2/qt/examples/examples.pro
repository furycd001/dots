# All extension modules are listed here
# This is duplicated in qt.pro
MODULES_BASE	= tools kernel widgets dialogs
MODULES_PRO	= iconview workspace
MODULES_ENT	= network canvas table xml opengl
MODULES		= $$MODULES_BASE $$MODULES_PRO
enterprise:MODULES	+= $$MODULES_ENT

internal:MODULES	+= $$MODULES_ENT

CONFIG += $$MODULES

TEMPLATE    =	subdirs
SUBDIRS     =	aclock \
		action \
		addressbook \
		application \
		buttongroups \
		checklists \
		cursor \
		customlayout \
		dclock \
		dirview \
		dragdrop \
		drawdemo \
		drawlines \
		forever \
		hello \
		helpviewer \
		i18n \
		layout \
		life \
		lineedits \
		listbox \
		listboxcombo \
		listviews \
		menu \
		movies \
		picture \
		popup \
		progress \
		progressbar \
		qdir \
		qfd \
		qmag \
		qwerty \
		rangecontrols \
		richtext \
		rot13 \
		scribble \
		scrollview \
		showimg \
		sound \
		splitter \
		tabdialog \
		tetrix \
		themes \
		tictac \
		tooltip \
		validator \
		widgets \
		wizard \
		xform

canvas:SUBDIRS +=   canvas
opengl:SUBDIRS +=   box \
		    gear \
		    glpixmap \
		    overlay \
		    sharedbox \
		    texture
iconview:SUBDIRS += fileiconview \
		    iconview
network:SUBDIRS +=  ftpclient \
		    httpd \
		    mail \
		    networkprotocol
workspace:SUBDIRS+= mdi
table:SUBDIRS +=    statistics \
		    table
xml:SUBDIRS +=	    xmlquotes

embedded:SUBDIRS += winmanager \
		launcher

X11DIRS	    =   biff \
		desktop
win32:SUBDIRS += trayicon