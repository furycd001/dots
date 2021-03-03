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

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(OPENGL_SRCS
    qgl.cpp
    qgl_x11.cpp
)

set(OPENGL_HEADERS
    qgl.h
)
install(FILES ${OPENGL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt2)

qt2_wrap_cpp(OPENGL_SRCS
    SOURCES ${OPENGL_SRCS}
)

qt2_wrap_moc(OPENGL_SRCS
    SOURCES qgl.h)

add_qt2_object_library(opengl
    SOURCES
        ${OPENGL_SRCS}
    COMPILE_DEFINITIONS
        ${QT_DEFINITIONS}
    INCLUDE_DIRS
	${X11_INCLUDE_DIRS}
	${X11_X11_INCLUDE_PATH}
	${OPENGL_INCLUDE_DIR}
    )
