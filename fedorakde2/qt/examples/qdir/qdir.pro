TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= qdir.h ../dirview/dirview.h
SOURCES		= qdir.cpp ../dirview/dirview.cpp
TARGET		= qdir
DEPENDPATH=../../include
REQUIRES=full-config
