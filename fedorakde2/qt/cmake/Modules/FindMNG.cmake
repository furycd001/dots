# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindMNG
# --------
#
# Find MNG
#
# Find the native MNG includes and library This module defines
#
# ::
#
#   MNG_FOUND, If false, do not try to use MNG.
#   MNG_INCLUDE_DIR, where to find mng.h, etc.
#   MNG_LIBRARIES, the libraries needed to use MNG.
#

find_path(MNG_INCLUDE_DIR
    NAMES
        libmng.h
    PATHS_SUFFIXES
        include
	)

find_library(MNG_LIBRARIES
    NAMES
        mng
    PATH_SUFFIXES
        lib
        lib64
	)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MNG
    REQUIRED_VARS
        MNG_LIBRARIES
        MNG_INCLUDE_DIR
    )

if(NOT TARGET MNG::MNG)
    add_library(MNG::MNG UNKNOWN IMPORTED)
    set_target_properties(MNG::MNG PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${MNG_INCLUDE_DIR}")
    set_property(TARGET MNG::MNG APPEND PROPERTY
        IMPORTED_LOCATION "${MNG_LIBRARIES}")
endif()
