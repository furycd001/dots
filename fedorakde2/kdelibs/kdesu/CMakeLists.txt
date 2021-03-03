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

set(libkdesu_SRCS
    client.cpp process.cpp kcookie.cpp su.cpp ssh.cpp
    stub.cpp kdesu_pty.cpp
    )

set(libkdesu_HEADERS
    defaults.h client.h process.h kdesu_pty.h kcookie.h
    su.h ssh.h stub.h
    )

install(
    FILES ${libkdesu_HEADERS}
    DESTINATION ${KDE2_INCLUDEDIR}/kdesu
    )

install(FILES kdesu_pty.h DESTINATION ${KDE2_INCLUDEDIR}/kdesu RENAME pty.h)

kde2_library(libkdesu
    SOURCES
        ${libkdesu_HEADERS}
        ${moc_SRCS}
        ${libkdesu_SRCS}
    LIBS
        kde2::kdecore
    )

set(kdesu_stub_SRCS
    kdesu_stub.c
    )

add_executable(kdesu_stub ${moc_SRCS} ${kdesu_stub_SRCS})

target_link_libraries(kdesu_stub
    kde2::kdecore
    )

install(TARGETS kdesu_stub RUNTIME DESTINATION ${KDE2_BINDIR})
