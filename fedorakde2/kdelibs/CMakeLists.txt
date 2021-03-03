#=============================================================================
# Copyright 2017       Helio Chissini de Castro <helio@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(kdelibs
    VERSION 2.2.2
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

include(kde2)
include(kde2_configure)

include(GNUInstallDirs)
set(KDE2_APPSDIR ${CMAKE_INSTALL_FULL_DATADIR}/applications CACHE STRING "KDE2 Apps dir")
set(KDE2_CONFDIR ${CMAKE_INSTALL_FULL_DATADIR}/config CACHE STRING "KDE2 CONFIG dir")
set(KDE2_DATADIR ${CMAKE_INSTALL_FULL_DATADIR} CACHE STRING "KDE2 Data dir")
set(KDE2_BINDIR ${CMAKE_INSTALL_FULL_BINDIR} CACHE STRING "KDE2 Bin dir")
set(KDE2_ICONDIR ${CMAKE_INSTALL_FULL_DATADIR}/icons CACHE STRING "KDE2 Icons dir")
set(KDE2_LOCALE ${CMAKE_INSTALL_FULL_LOCALEDIR} CACHE STRING "KDE2 Locale dir")
set(KDE2_MIMEDIR ${CMAKE_INSTALL_FULL_DATADIR}/mimelnk CACHE STRING "KDE2 Mime dir")
set(KDE2_SERVICESDIR ${CMAKE_INSTALL_FULL_DATADIR}/services CACHE STRING "KDE2 Services dir")
set(KDE2_SERVICETYPESDIR ${CMAKE_INSTALL_FULL_DATADIR}/servicetypes CACHE STRING "KDE2 ServiceTypes dir")
set(KDE2_SOUNDDIR ${CMAKE_INSTALL_FULL_DATADIR}/sound CACHE STRING "KDE2 Sound dir")
set(KDE2_TEMPLATESDIR ${CMAKE_INSTALL_FULL_DATADIR}/templates CACHE STRING "KDE2 Templates dir")
set(KDE2_WALLPAPERDIR ${CMAKE_INSTALL_FULL_DATADIR}/wallpapers CACHE STRING "KDE2 wallpapers dir")
set(KDE2_HTMLDIR ${CMAKE_INSTALL_FULL_DOCDIR}/HTML CACHE STRING "KDE2 Doc dir")
set(KDE2_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}/kde2 CACHE STRING "KDE2 include dir")
set(KDE2_CA_CERT_PATH "${CMAKE_INSTALL_FULL_SYSCONFDIR}/ssl/cert.pem" CACHE STRING "Path to certificate authority database")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(XDG_COMPAT "Enable some compatibility with modern desktop standards" TRUE)
if(XDG_COMPAT)
    add_definitions(-DXDG_COMPAT)
endif()

option(ENABLE_SANITIZERS "Enable runtime sanitizing (for development)")
if (ENABLE_SANITIZERS)
    message("Enabling asan and ubsan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
endif()


option(ENABLE_XFT "Enable support for Anti-Aliased fonts." ON)
option(ENABLE_SESSION_MANAGEMENT "Support X Session Management, links in -lSM -lICE" ON)
option(ENABLE_THREAD_SUPPORT "Compile with Threading Support" ON)
option(ENABLE_OPENGL "Compile OpenGL module" ON)
option(ENABLE_ALSA "Enable ALSA support in arts" ON)
option(ENABLE_OSS "Enable OSS support in arts" OFF)
option(ENABLE_AUDIOIO "Enable AudioIO (Solaris) support in arts" OFF)
option(ENABLE_AUDIO_AIX "Enable AIX support in arts" OFF)
option(ENABLE_AUDIO_SUN "Enable Sun support in arts" OFF)
option(ENABLE_NAS "Enable Network Audio System in arts" OFF)
option(SYSTEM_ZLIB "Use a zlib from the operating system" ON)
option(SYSTEM_LIBPNG "Use libpng from the operating system." ON)
option(SYSTEM_LIBMNG "Use libmng from the operating system." ON)
option(SYSTEM_JPEG "Use jpeglib from the operating system." ON)
option(BUILD_TUTORIAL "Build tutorials." OFF)
option(BUILD_EXAMPLES "Build examples." OFF)
option(BUILD_TESTS "Build tests." OFF)
set(KDEDIR ${CMAKE_INSTALL_PREFIX} CACHE STRING "Default path ofr KDEDIR install")

find_package(PkgConfig REQUIRED)

find_package(Qt2 REQUIRED)

pkg_search_module(libpcreposix IMPORTED_TARGET libpcreposix)
if (libpcreposix_FOUND)
    set(HAVE_PCREPOSIX ON)
endif()

if(ENABLE_XFT)
    pkg_search_module(XFT REQUIRED xft)
    pkg_search_module(FONTCONFIG REQUIRED fontconfig)
    find_package(Freetype REQUIRED)
endif()

if(ENABLE_SESSION_MANAGEMENT)
    pkg_search_module(SM REQUIRED sm)
endif()

if(ENABLE_THREAD_SUPPORT)
    find_package(Threads REQUIRED)
endif()

if(ENABLE_OPENGL)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    find_package(GLUT REQUIRED)
endif()

if(SYSTEM_ZLIB)
    find_package(ZLIB REQUIRED)
endif()
if(ZLIB_FOUND)
    set(HAVE_LIBZ TRUE)
else()
    set(HAVE_LIBZ FALSE)
endif()

if(SYSTEM_LIBPNG)
    find_package(PNG REQUIRED)
endif()
if (PNG_FOUND)
    set(HAVE_LIBPNG ON)
endif()

if(SYSTEM_LIBMNG)
    pkg_search_module(MNG libmng REQUIRED)
endif()

if(SYSTEM_JPEG)
    find_package(JPEG REQUIRED)
    pkg_search_module(XMU xmu REQUIRED)
endif()
if (JPEG_FOUND)
    set(HAVE_LIBJPEG ON)
endif()

find_package(TIFF)
if(TIFF_FOUND)
    set(HAVE_LIBTIFF TRUE)
else()
    set(HAVE_LIBTIFF FALSE)
endif()

find_package(ALSA)

find_package(X11 REQUIRED)
if (X11_XShm_FOUND)
    set(HAVE_MITSHM ON)
endif()
if (X11_Xshape_FOUND)
    set(HAVE_X11_EXTENSIONS_SHAPE_H ON)
endif()
if (X11_ICE_FOUND)
    set(HAVE_X11_ICE_ICELIB_H ON)
endif()

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

pkg_search_module(PCRE libpcre REQUIRED)

find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)

find_package(BZip2 REQUIRED)
if(NOT TARGET BZip2::BZip2)
	add_library(BZip2::BZip2 UNKNOWN IMPORTED)
	set_target_properties(BZip2::BZip2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${BZIP2_INCLUDE_DIRS}")
        set_property(TARGET BZip2::BZip2 APPEND PROPERTY IMPORTED_LOCATION "${BZIP2_LIBRARY}")
endif()

if(BZIP2_FOUND)
    set(HAVE_BZIP2_SUPPORT TRUE)
else()
    set(HAVE_BZIP2_SUPPORT FALSE)
endif()

pkg_search_module(audiofile IMPORTED_TARGET audiofile)
if (audiofile_FOUND)
    set(HAVE_LIBAUDIOFILE TRUE)
else()
    set(HAVE_LIBAUDIOFILE FALSE)
endif()

find_package(Cups)

find_package(OpenSSL)
if(OpenSSL_FOUND)
    set(HAVE_SSL TRUE)
else()
    set(HAVE_SSL FALSE)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/kdelibs2Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kdelibs2Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kdelibs2)

create_kde2_config_header()

add_subdirectory(dcop)
add_subdirectory(kdecore)
add_subdirectory(kio)
add_subdirectory(kdoctools)
add_subdirectory(kdeui)
add_subdirectory(kdesu)
add_subdirectory(kssl)
add_subdirectory(kjs)
add_subdirectory(kimgio)
add_subdirectory(kfile)
add_subdirectory(kparts)
add_subdirectory(kdeprint)
if(ALSA_FOUND)
    add_subdirectory(libkmid)
endif()
add_subdirectory(kconf_update)
add_subdirectory(kinit)
add_subdirectory(arts)
add_subdirectory(interfaces)
add_subdirectory(kab)
add_subdirectory(khtml)
add_subdirectory(kspell)
add_subdirectory(kstyles)
add_subdirectory(libkscreensaver)
add_subdirectory(licenses)
add_subdirectory(mimetypes)
add_subdirectory(pics)
add_subdirectory(doc)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kdelibs2Config.cmake
    cmake/kde2.cmake
    cmake/kde2_configure.cmake
    cmake/kde2_icon.cmake
    cmake/kde2_kidl.cmake
    cmake/kde2_library.cmake
    cmake/kde2_module.cmake
    cmake/kde2_kinit_executable.cmake
    cmake/kde2_utils.cmake
    cmake/kde2_libtool_lib_template.la.in
    cmake/kde2_libtool_template.la.in
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kdelibs2)

install(FILES
    cmake/artsConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/arts
    )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    DESTINATION ${KDE2_INCLUDEDIR}
    )
