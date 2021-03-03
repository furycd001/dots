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

add_subdirectory(management)
add_subdirectory(lpdunix)
add_subdirectory(lpd)
add_subdirectory(rlpr)
add_subdirectory(ext)
add_subdirectory(filters)
add_subdirectory(pics)
add_subdirectory(tests)
if(CUPS_FOUND)
    #Did it ever worked ?
    #add_subdirectory(cups)
endif()

kde2_kidl(kdeprintd.h)

set(KDED_KDEPRINTD_SOURCES
    kdeprintd_skel.cpp kdeprintd.cpp kprintprocess.cpp
    )

qt2_wrap_cpp(KDED_KDEPRINTD_SOURCES SOURCES kdeprintd.h kprintprocess.h)

kde2_module(libkded_kdeprintd
    SOURCES
        ${KDED_KDEPRINTD_SOURCES}
    LIBS
        kde2::kdeui
        kde2::kdecore
        kde2::kio
    ALLOW_UNDEFINED
    )


set(KDEPRINT_SOURCES
    kmfactory.cpp kmjob.cpp kmjobmanager.cpp kmmanager.cpp kmprinter.cpp
    kmuimanager.cpp kmvirtualmanager.cpp driver.cpp driveritem.cpp droptionview.cpp
    driverview.cpp maticscanner.cpp maticparser.cpp matic.cpp kmdbentry.cpp
    kprintpreview.cpp kmfiltermanager.cpp kprintdialog.cpp kprinter.cpp
    kprinterimpl.cpp kprintdialogpage.cpp kpcopiespage.cpp kpdriverpage.cpp
    kprinterpropertydialog.cpp kpgeneralpage.cpp kpqtpage.cpp kmthreadjob.cpp
    kpipeprocess.cpp kprintfilter.cpp kpfilterpage.cpp kmspecialmanager.cpp
    kdeprintcheck.cpp plugincombobox.cpp kpreloadobject.cpp
    )

set(PRIVATE_HEADERS
    kmdbentry.h kmjob.h kmjobmanager.h
    kmuimanager.h kmvirtualmanager.h driver.h driveritem.h
    droptionview.h driverview.h matic.h kprintpreview.h kprinterimpl.h
    kprintdialog.h kpcopiespage.h kpdriverpage.h kprinterpropertydialog.h
    kpgeneralpage.h kpqtpage.h kmthreadjob.h kpipeprocess.h
    kprintfilter.h kmfiltermanager.h kpfilterpage.h kmspecialmanager.h
    kdeprintcheck.h plugincombobox.h
    )

set(PUBLIC_HEADERS kprintdialogpage.h kpreloadobject.h kmprinter.h kmfactory.h kmmanager.h kmobject.h)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kdeprint)
install(FILES kprinter.h DESTINATION ${KDE2_INCLUDEDIR}/kdeprint)

qt2_wrap_cpp(KDEPRINT_SOURCES
    SOURCES
        driverview.h
        droptionview.h
        kpcopiespage.h
        kpfilterpage.h
        kpgeneralpage.h
        kpqtpage.h
        kprintdialog.h
        kprintdialogpage.h
        kprinterimpl.h
        kprinterpropertydialog.h
        kprintpreview.h
        plugincombobox.h
    )

kde2_library(libkdeprint
    SOURCES
        ${KDEPRINT_SOURCES}
    LIBS
        kde2::kparts
        ZLIB::ZLIB
    )

install(FILES testprint.ps specials.desktop DESTINATION ${KDE2_DATADIR}/kdeprint)
install(FILES kdeprintd.desktop DESTINATION ${KDE2_SERVICESDIR}/kded)
install(FILES kdeprintrc DESTINATION ${KDE2_CONFDIR})
install(FILES kprintpreviewui.rc DESTINATION ${KDE2_CONFDIR}/ui)
