TEMPLATE	= app
CONFIG		+= qt warn_on release dll
HEADERS		= 
SOURCES		= main.cpp
unix:LIBS	= -lqnp -lXt
TARGET		= npmovies
DEPENDPATH=../../include
