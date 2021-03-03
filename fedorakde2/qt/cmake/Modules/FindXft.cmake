# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindXFT
# --------
#
# Find XFT
#
# Find the native XFT includes and library This module defines
#
# ::
#
#   XFT_FOUND  -If false, do not try to use XFT.
#   XFT_INCLUDE_DIR - where to find mng.h, etc.
#   XFT_LIBRARIES - the libraries needed to use XFT.
#   XFT_VERSION_STRING - the version of Xft found.
#

find_path(XFT_INCLUDE_DIR
	NAMES
        X11/Xft/Xft.h
	PATH_SUFFIXES
		include
	)

find_library(XFT_LIBRARIES
	NAMES
        Xft
	PATH_SUFFIXES
        lib
        lib64
	)

if(XFT_INCLUDE_DIR AND EXISTS "${XFT_INCLUDE_DIR}/X11/Xft/Xft.h")
    file(STRINGS "${XFT_INCLUDE_DIR}/X11/Xft/Xft.h" XFT_H_MAJOR REGEX "#define XFT_MAJOR [0-9]+")
    string(REGEX REPLACE "#define XFT_MAJOR ([0-9]+)" "\\1" XFT_MAJOR "${XFT_H_MAJOR}")
    file(STRINGS "${XFT_INCLUDE_DIR}/X11/Xft/Xft.h" XFT_H_MINOR REGEX "#define XFT_MINOR [0-9]+")
    string(REGEX REPLACE "#define XFT_MINOR ([0-9]+)" "\\1" XFT_MINOR "${XFT_H_MINOR}")
    file(STRINGS "${XFT_INCLUDE_DIR}/X11/Xft/Xft.h" XFT_H_REVISION REGEX "#define XFT_REVISION [0-9]+")
    string(REGEX REPLACE "#define XFT_REVISION ([0-9]+)" "\\1" XFT_REVISION "${XFT_H_REVISION}")
    set(XFT_VERSION_STRING "${XFT_MAJOR}.${XFT_MINOR}.${XFT_REVISION}")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
	Xft
    REQUIRED_VARS
        XFT_VERSION_STRING
		XFT_LIBRARIES
     	XFT_INCLUDE_DIR
  )

if(NOT TARGET Xft::Xft)
    add_library(Xft::Xft UNKNOWN IMPORTED)
    set_target_properties(Xft::Xft PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${XFT_INCLUDE_DIR}")
    set_property(TARGET Xft::Xft APPEND PROPERTY
        IMPORTED_LOCATION "${XFT_LIBRARIES}")
endif()

mark_as_advanced(XFT_INCLUDE_DIR)


