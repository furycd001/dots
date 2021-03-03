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

#add_subdirectory(KDE-ICE)
add_subdirectory(dcopidl)
add_subdirectory(dcopidl2cpp)
add_subdirectory(client)

set(testdcop_SRCS
    testdcop.cpp
    )

set(testdcopic_SRCS
    testdcopc.c
    dcopc.c
    )

#--------------------------------------------------------------
# libdcop Library

set(libdcop_SRCS
    dcopstub.cpp
    dcopclient.cpp
    dcopobject.cpp
    dcopref.cpp
    )

set(libdcop_HEADERS
    dcopclient.h dcopobject.h dcopref.h dcopstub.h
    )

qt2_wrap_cpp(libdcop_SRCS
    SOURCES
        dcopclient.h
    )

kde2_library(libDCOP
    SOURCES
        ${libdcop_SRCS}
    LIBS
        Qt::Qt2
    VERSION
        1.0.0
    )

target_link_librarieS(libDCOP PRIVATE X11::ICE)

install(FILES ${libdcop_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/DCOP)

set(dcopserver_SRCS
    dcopserver.cpp
    dcopsignals.cpp
    )

qt2_wrap_cpp(dcopserver_SRCS
    SOURCES
        dcopserver.h
    )
kde2_kinit_executable(dcopserver
    SOURCES
        ${dcopserver_SRCS}
    LIBS
        kde2::DCOP X11::ICE
    )

set(testdcop_SRCS testdcop.cpp)
qt2_wrap_cpp(testdcop_SRCS
    SOURCES
        testdcop.h
    )
add_executable(testdcop
    ${testdcop_SRCS}
    )
target_link_libraries(testdcop kde2::kdecore)

#add_executable(testdcopc
#    testdcopc.c
#    dcopc.c
#    $<TARGET_OBJECTS:kICE>
#    )

#target_compile_definitions(dcopserver PRIVATE DCOP_DEBUG)
