TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= biff.h
SOURCES		= biff.cpp \
		  main.cpp
TARGET		= biff
DEPENDPATH=../../include
REQUIRES=full-config
