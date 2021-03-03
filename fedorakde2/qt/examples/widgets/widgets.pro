TEMPLATE	= app
CONFIG		+= qt warn_on release
INCLUDEPATH	= ../aclock ../dclock
HEADERS		= widgets.h ../aclock/aclock.h ../dclock/dclock.h
SOURCES		= main.cpp widgets.cpp ../aclock/aclock.cpp ../dclock/dclock.cpp
TARGET		= widgets
DEPENDPATH=../../include
REQUIRES=full-config
