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

add_subdirectory(tests)

set(libkjava_SRCS
    kjavaapplet.cpp kjavaappletcontext.cpp kjavaappletserver.cpp kjavaappletwidget.cpp kjavaprocess.cpp
    kjavadownloader.cpp javaembed.cpp
    )

set(libkjava_HEADERS
    kjavaapplet.h kjavaappletcontext.h kjavaappletserver.h kjavaappletwidget.h kjavaprocess.h
    kjavadownloader.h
    )

set(libkjava_NOINST_HEADERS javaembed.h)

qt2_wrap_cpp(libkjava_SRCS
    SOURCES
        ${libkjava_HEADERS}
        ${libkjava_NOINST_HEADERS}
    )

install(FILES ${libkjava_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkjava
    SOURCES
        ${libkjava_SRCS}
    HEADERS
        kjavaapplet.h kjavaappletcontext.h
	kjavaappletserver.h kjavaappletwidget.h kjavaprocess.h
    LIBS
        kde2::kdecore kde2::kio
    )

configure_file(kjava.policy.in ${CMAKE_CURRENT_BINARY_DIR}/kjava.policy @ONLY)

install(FILES
    kjava.jar
    ${CMAKE_CURRENT_BINARY_DIR}/kjava.policy
    DESTINATION ${KDE2_DATADIR}/kjava)
