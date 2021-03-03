TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= border.h \
		  card.h \
		  flow.h
SOURCES		= border.cpp \
		  card.cpp \
		  flow.cpp \
		  main.cpp
TARGET		= customlayout
DEPENDPATH=../../include
REQUIRES=medium-config
