TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= showimg.h imagetexteditor.h \
		  imagefip.h
SOURCES		= main.cpp \
		  imagetexteditor.cpp \
		  showimg.cpp \
		  imagefip.cpp
TARGET		= showimg
DEPENDPATH=../../include
REQUIRES=full-config
