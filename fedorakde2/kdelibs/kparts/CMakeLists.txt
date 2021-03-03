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

set(KPARTS_SOURCES
    part.cpp plugin.cpp partmanager.cpp mainwindow.cpp
    dockmainwindow.cpp event.cpp browserextension.cpp factory.cpp
    historyprovider.cpp browserinterface.cpp
    )

set(KPARTS_HEADERS
    part.h plugin.h partmanager.h mainwindow.h dockmainwindow.h event.h
    browserextension.h factory.h  historyprovider.h browserinterface.h
    )

qt2_wrap_cpp(KPARTS_SOURCES
    SOURCES
        browserextension.h
        browserinterface.h
        dockmainwindow.h
        factory.h
        historyprovider.h
        mainwindow.h
        part.h
        partmanager.h
        plugin.h
    )

kde2_library(libkparts
    SOURCES
        ${KPARTS_SOURCES}
    LIBS
        kde2::kfile
        kde2::kio
    VERSION
        1.0.0
    )

install(
    FILES
        kpart.desktop krop.desktop krwp.desktop browserview.desktop
        DESTINATION ${KDE2_SERVICETYPESDIR}
    )

install(FILES ${KPARTS_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kparts)
