TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= life.h \
		  lifedlg.h
SOURCES		= life.cpp \
		  lifedlg.cpp \
		  main.cpp
TARGET		= life
DEPENDPATH=../../include
REQUIRES=medium-config
