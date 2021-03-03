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

# Make the include headers painless
# Again not the desired or optimal solution, but is the fastest

get_target_property(QT2_INCLUDES Qt::Qt2 INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(KDECORE_INCLUDES kde2::kdecore INCLUDE_DIRECTORIES)
get_target_property(KIO_INCLUDES kde2::kio INCLUDE_DIRECTORIES)
get_target_property(KDEUI_INCLUDES kde2::kdeui INCLUDE_DIRECTORIES)
get_target_property(DCOP_INCLUDES kde2::DCOP INCLUDE_DIRECTORIES)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    css
    dom
    html
    java
    misc
    xml
    ${QT2_INCLUDES}
    ${KIO_INCLUDES}
    ${KDEUI_INCLUDES}
    ${KDECORE_INCLUDES}
    ${DCOP_INCLUDES}
    )

add_subdirectory(misc)
add_subdirectory(dom)
add_subdirectory(css)
add_subdirectory(xml)
add_subdirectory(html)
add_subdirectory(rendering)
add_subdirectory(pics)
add_subdirectory(java)
add_subdirectory(ecma)

set(libkhtml_SRCS
    khtmlview.cpp khtml_part.cpp khtml_run.cpp khtml_factory.cpp khtml_settings.cc
    khtml_events.cpp khtml_find.cpp khtml_ext.cpp khtml_pagecache.cpp
    )

set(libkhtml_HEADERS
    khtmlview.h khtml_part.h khtml_events.h khtml_settings.h khtmldefaults.h
    khtml_run.h khtml_factory.h
    )

set(libkhtml_NOINST_HEADERS
    design.h testkhtml.h testrender.h khtml_find.h khtml_ext.h khtml_pagecache.h
    khtmlimage.h
    )

qt2_wrap_cpp(libkhtml_SRCS
    SOURCES
    domtreeview.h
        khtml_ext.h
        khtml_factory.h
        khtml_find.h
        khtml_pagecache.h
        khtml_part.h
        khtml_run.h
        khtmlview.h
        testkhtml.h
        testrender.h
    )

install(FILES ${libkhtml_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/khtml)

kde2_library(libkhtml
    SOURCES
        ${libkhtml_SRCS}
        $<TARGET_OBJECTS:libkhtmlxml>
        $<TARGET_OBJECTS:libkhtmlhtml>
        $<TARGET_OBJECTS:libkhtmlrender>
        $<TARGET_OBJECTS:libkhtmlcss>
        $<TARGET_OBJECTS:libkhtmlmisc>
        $<TARGET_OBJECTS:libkhtmldom>
    LIBS
        Qt::Qt2
        kde2::kio
        kde2::kparts
        kde2::kfile
        kde2::kssl
        kde2::kdeprint
        kde2::kjava
    VERSION
        3.0.0
    )
target_link_libraries(libkhtml PRIVATE JPEG::JPEG)

set(LAPROP_SONAME libkhtml-2)
get_target_property(LAPROP_DEPENDENCY_LIBS libkhtml INTERFACE_LINK_LIBRARIES)
configure_file(${PROJECT_SOURCE_DIR}/cmake/kde2_libtool_lib_template.la.in
    ${CMAKE_CURRENT_BINARY_DIR}/libkhtml.la
    @ONLY
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkhtml.la
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2"
    )

set(libkhtmlimage_SRCS
    khtmlimage.cpp
    )

set(libkhtmlimage_HEADERS
    khtmlview.h khtml_part.h khtml_events.h khtml_settings.h khtmldefaults.h
    )

qt2_wrap_cpp(libkhtmlimage_SRCS
    SOURCES
        khtmlimage.h
    )

install(FILES ${libkhtmlimage_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/khtmlimage)

kde2_module(libkhtmlimage
    SOURCES
        ${libkhtmlimage_SRCS}
    LIBS
        kde2::khtml
    )

install(FILES
    khtml.desktop khtmlimage.desktop
    DESTINATION ${KDE2_SERVICESDIR}
    )

install(FILES khtml.rc khtml_browser.rc khtml_popupmenu.rc DESTINATION ${KDE2_DATADIR}/khtml)
