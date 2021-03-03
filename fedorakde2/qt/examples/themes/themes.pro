TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= themes.h \
		  ../buttongroups/buttongroups.h \
		  ../lineedits/lineedits.h \
		  ../listboxcombo/listboxcombo.h \
		  ../checklists/checklists.h \
		  ../progressbar/progressbar.h \
		  ../rangecontrols/rangecontrols.h \
		  ../richtext/richtext.h \
		  wood.h \
		  methal.h
		
SOURCES		= themes.cpp \
		  main.cpp \
		  ../buttongroups/buttongroups.cpp \
		  ../lineedits/lineedits.cpp \
		  ../listboxcombo/listboxcombo.cpp \
		  ../checklists/checklists.cpp \
		  ../progressbar/progressbar.cpp \
		  ../rangecontrols/rangecontrols.cpp \
		  ../richtext/richtext.cpp \
		  wood.cpp \
		  metal.cpp

TARGET		= themes
DEPENDPATH=../../include
REQUIRES=full-config
