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

set(libkdeprint_cups_SOURCES
    cupsinfos.cpp
    ipprequest.cpp
    kmcupsfactory.cpp
    kmcupsjobmanager.cpp
    kmcupsmanager.cpp
    kmcupsuimanager.cpp
    kmpropbanners.cpp
    kmwbanners.cpp
    kmwipp.cpp
    kmwippselect.cpp
    kcupsprinterimpl.cpp
    kmcupsconfig.cpp
    kmcupsconfigwidget.cpp
    kmconfigcups.cpp
    kmconfigcupsdir.cpp
    kmwippprinter.cpp
    kmwfax.cpp
    )

set(PRIVATE_HEADERS
    cupsinfos.h
    ipprequest.h
    kmcupsfactory.h
    kmcupsjobmanager.h
    kmcupsmanager.h
    kmcupsuimanager.h
    kmpropbanners.h
    kmwbanners.h
    kmwipp.h
    kmwippselect.h
    kcupsprinterimpl.h
    kmcupsconfig.h
    kmcupsconfigwidget.h
    kmconfigcups.h
    kmconfigcupsdir.h
    kmwippprinter.h
    kmwfax.h
    )

qt2_wrap_cpp(libkdeprint_cups_SOURCES
    SOURCES
        ${PRIVATE_HEADERS}
    )

kde2_module(libkdeprint_cups
    SOURCES
        ${libkdeprint_cups_SOURCES}
    INCLUDE_DIRECTORIES
        ${CUPS_INCLUDE_DIR}
    LIBS
        kde2::kdeprint_management
        kde2::kdeui
        ${CUPS_LIBRARIES}
    )

install(FILES lpd.print
    DESTINATION ${KDE2_DATADIR}/kdeprint/plugins
    )

add_executable(make_driver_db_cups make_driver_db_cups.c)
target_link_libraries(make_driver_db_cups ZLIB::ZLIB)
add_executable(cupsdoprint cupsdoprint.c)
target_link_libraries(cupsdoprint ${CUPS_LIBRARIES})

install(TARGETS make_driver_db_cups cupsdoprint RUNTIME DESTINATION ${KDE2_BINDIR})
