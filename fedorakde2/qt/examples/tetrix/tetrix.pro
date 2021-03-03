TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= gtetrix.h \
		  qdragapp.h \
		  qtetrix.h \
		  qtetrixb.h \
		  tpiece.h
SOURCES		= gtetrix.cpp \
		  qdragapp.cpp \
		  qtetrix.cpp \
		  qtetrixb.cpp \
		  tetrix.cpp \
		  tpiece.cpp
TARGET		= tetrix
DEPENDPATH=../../include
REQUIRES=small-config
