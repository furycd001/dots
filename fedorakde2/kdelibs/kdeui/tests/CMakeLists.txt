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

set(global_NOINST_HEADERS
    ktabctltest.h kwindowtest.h kstatusbartest.h kledtest.h kcontainertest.h
    krulertest.h kgradienttest.h kcolortest.h kunbalancedgrdtest.h kblendtest.h
    kdualcolortest.h khashtest.h knuminputtest.h kdockwidgetdemo.h kdocktabctltest.h
    kdockwidgettest.h itemcontainertest.h kpanelmenutest.h kalphaicontest.h kcompletiontest.h
    klineedittest.h kcomboboxtest.h kdesattest.h
    )

qt2_wrap_cpp(moc_SRCS
    SOURCES
        itemcontainertest.h
        kcomboboxtest.h
        kcompletiontest.h
        kcontainertest.h
        kdesattest.h
        kdocktabctltest.h
        kdockwidgetdemo.h
        kdockwidgettest.h
        kdualcolortest.h
        khashtest.h
        kledtest.h
        klineedittest.h
        knuminputtest.h
        kpanelmenutest.h
        krulertest.h
        kstatusbartest.h
        ktabctltest.h
        kunbalancedgrdtest.h
        kwindowtest.h
    )

install(FILES ${global_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kdeui)

set(kaboutdialogtest_SRCS
    kaboutdialogtest.cpp
    )

add_executable(kaboutdialogtest ${moc_SRCS} ${kaboutdialogtest_SRCS})

target_link_libraries(kaboutdialogtest
    kde2::kdeui
    )

set(kblendtest_SRCS
    kblendtest.cpp
    )

add_executable(kblendtest ${moc_SRCS} ${kblendtest_SRCS})

target_link_libraries(kblendtest
    kde2::kdeui
    )

set(kbuttonboxtest_SRCS
    kbuttonboxtest.cpp
    )

add_executable(kbuttonboxtest ${moc_SRCS} ${kbuttonboxtest_SRCS})

target_link_libraries(kbuttonboxtest
    kde2::kdeui
    )

set(kcharselecttest_SRCS
    kcharselecttest.cpp
    )

add_executable(kcharselecttest ${moc_SRCS} ${kcharselecttest_SRCS})

target_link_libraries(kcharselecttest
    kde2::kdeui
    )

set(kcolortest_SRCS
    kcolortest.cpp
    )

add_executable(kcolortest ${moc_SRCS} ${kcolortest_SRCS})

target_link_libraries(kcolortest
    kde2::kdeui
    )

set(kcolordlgtest_SRCS
    kcolordlgtest.cpp
    )

add_executable(kcolordlgtest ${moc_SRCS} ${kcolordlgtest_SRCS})

target_link_libraries(kcolordlgtest
    kde2::kdeui
    )

set(kcomboboxtest_SRCS
    kcomboboxtest.cpp
    )

add_executable(kcomboboxtest ${moc_SRCS} ${kcomboboxtest_SRCS})

target_link_libraries(kcomboboxtest
    kde2::kdeui
    )

set(kcompletiontest_SRCS
    kcompletiontest.cpp
    )

add_executable(kcompletiontest ${moc_SRCS} ${kcompletiontest_SRCS})

target_link_libraries(kcompletiontest
    kde2::kdeui
    )

set(kcontainertest_SRCS
    kcontainertest.cpp
    )

add_executable(kcontainertest ${moc_SRCS} ${kcontainertest_SRCS})

target_link_libraries(kcontainertest
    kde2::kdeui
    )

set(kdatepicktest_SRCS
    kdatepicktest.cpp
    )

add_executable(kdatepicktest ${moc_SRCS} ${kdatepicktest_SRCS})

target_link_libraries(kdatepicktest
    kde2::kdeui
    )

set(kdialogbasetest_SRCS
    kdialogbasetest.cpp
    )

add_executable(kdialogbasetest ${moc_SRCS} ${kdialogbasetest_SRCS})

target_link_libraries(kdialogbasetest
    kde2::kdeui
    )

set(kdockwidgetdemo_SRCS
    kdockwidgetdemo.cpp
    )

add_executable(kdockwidgetdemo ${moc_SRCS} ${kdockwidgetdemo_SRCS})

target_link_libraries(kdockwidgetdemo
    kde2::kdeui
    )

set(kdocktabctltest_SRCS
    kdocktabctltest.cpp
    )

add_executable(kdocktabctltest ${moc_SRCS} ${kdocktabctltest_SRCS})

target_link_libraries(kdocktabctltest
    kde2::kdeui
    )

set(kdockwidgettest_SRCS
    kdockwidgettest.cpp
    )

add_executable(kdockwidgettest ${moc_SRCS} ${kdockwidgettest_SRCS})

target_link_libraries(kdockwidgettest
    kde2::kdeui
    )

set(kdualcolortest_SRCS
    kdualcolortest.cpp
    )

add_executable(kdualcolortest ${moc_SRCS} ${kdualcolortest_SRCS})

target_link_libraries(kdualcolortest
    kde2::kdeui
    )

set(keditlistboxtest_SRCS
    keditlistboxtest.cpp
    )

add_executable(keditlistboxtest ${moc_SRCS} ${keditlistboxtest_SRCS})

target_link_libraries(keditlistboxtest
    kde2::kdeui
    )

set(kfontdialogtest_SRCS
    kfontdialogtest.cpp
    )

add_executable(kfontdialogtest ${moc_SRCS} ${kfontdialogtest_SRCS})

target_link_libraries(kfontdialogtest
    kde2::kdeui
    )

set(kgradienttest_SRCS
    kgradienttest.cpp
    )

add_executable(kgradienttest ${moc_SRCS} ${kgradienttest_SRCS})

target_link_libraries(kgradienttest
    kde2::kdeui
    )

set(khashtest_SRCS
    khashtest.cpp
    )

add_executable(khashtest ${moc_SRCS} ${khashtest_SRCS})

target_link_libraries(khashtest
    kde2::kdeui
    )

set(kdesattest_SRCS
    kdesattest.cpp
    )

add_executable(kdesattest ${moc_SRCS} ${kdesattest_SRCS})

target_link_libraries(kdesattest
    kde2::kdeui
    )

set(kledtest_SRCS
    kledtest.cpp
    )

add_executable(kledtest ${moc_SRCS} ${kledtest_SRCS})

target_link_libraries(kledtest
    kde2::kdeui
    )

set(klineedittest_SRCS
    klineedittest.cpp
    )

add_executable(klineedittest ${moc_SRCS} ${klineedittest_SRCS})

target_link_libraries(klineedittest
    kde2::kdeui
    )

set(klineeditdlgtest_SRCS
    klineeditdlgtest.cpp
    )

add_executable(klineeditdlgtest ${moc_SRCS} ${klineeditdlgtest_SRCS})

target_link_libraries(klineeditdlgtest
    kde2::kdeui
    )

set(kmessageboxtest_SRCS
    kmessageboxtest.cpp
    )

add_executable(kmessageboxtest ${moc_SRCS} ${kmessageboxtest_SRCS})

target_link_libraries(kmessageboxtest
    kde2::kdeui
    )

set(knuminputtest_SRCS
    knuminputtest.cpp
    )

add_executable(knuminputtest ${moc_SRCS} ${knuminputtest_SRCS})

target_link_libraries(knuminputtest
    kde2::kdeui
    )

set(kpalettetest_SRCS
    kpalettetest.cpp
    )

add_executable(kpalettetest ${moc_SRCS} ${kpalettetest_SRCS})

target_link_libraries(kpalettetest
    kde2::kdeui
    )

set(kpopuptest_SRCS
    kpopuptest.cpp
    )

add_executable(kpopuptest ${moc_SRCS} ${kpopuptest_SRCS})

target_link_libraries(kpopuptest
    kde2::kdeui
    )

set(kprogresstest_SRCS
    kprogresstest.cpp
    )

add_executable(kprogresstest ${moc_SRCS} ${kprogresstest_SRCS})

target_link_libraries(kprogresstest
    kde2::kdeui
    )

set(krulertest_SRCS
    krulertest.cpp
    )

add_executable(krulertest ${moc_SRCS} ${krulertest_SRCS})

target_link_libraries(krulertest
    kde2::kdeui
    )

set(kseparatortest_SRCS
    kseparatortest.cpp
    )

add_executable(kseparatortest ${moc_SRCS} ${kseparatortest_SRCS})

target_link_libraries(kseparatortest
    kde2::kdeui
    )

set(kstatusbartest_SRCS
    kstatusbartest.cpp
    )

add_executable(kstatusbartest ${moc_SRCS} ${kstatusbartest_SRCS})

target_link_libraries(kstatusbartest
    kde2::kdeui
    )

set(ktabctltest_SRCS
    ktabctltest.cpp
    )

add_executable(ktabctltest ${moc_SRCS} ${ktabctltest_SRCS})

target_link_libraries(ktabctltest
    kde2::kdeui
    )

set(kunbalancedgrdtest_SRCS
    kunbalancedgrdtest.cpp
    )

add_executable(kunbalancedgrdtest ${moc_SRCS} ${kunbalancedgrdtest_SRCS})

target_link_libraries(kunbalancedgrdtest
    kde2::kdeui
    )

set(kwindowtest_SRCS
    kwindowtest.cpp
    )

add_executable(kwindowtest ${moc_SRCS} ${kwindowtest_SRCS})

target_link_libraries(kwindowtest
    kde2::kdeui
    )

set(kwizardtest_SRCS
    kwizardtest.cpp
    )

add_executable(kwizardtest ${moc_SRCS} ${kwizardtest_SRCS})

target_link_libraries(kwizardtest
    kde2::kdeui
    )

set(itemcontainertest_SRCS
    itemcontainertest.cpp
    )

add_executable(itemcontainertest ${moc_SRCS} ${itemcontainertest_SRCS})

target_link_libraries(itemcontainertest
    kde2::kdeui
    )

set(kpanelmenutest_SRCS
    kpanelmenutest.cpp
    )

add_executable(kpanelmenutest ${moc_SRCS} ${kpanelmenutest_SRCS})

target_link_libraries(kpanelmenutest
    kde2::kdeui
    )

set(kedittest_SRCS
    kedittest.cpp
    )

add_executable(kedittest ${moc_SRCS} ${kedittest_SRCS})

target_link_libraries(kedittest
    kde2::kdeui
    )

set(kalphaicontest_SRCS
    kalphaicontest.cpp
    )

add_executable(kalphaicontest ${moc_SRCS} ${kalphaicontest_SRCS})

target_link_libraries(kalphaicontest
    kde2::kdeui
    )

set(kxmlguitest_SRCS
    kxmlguitest.cpp
    )

add_executable(kxmlguitest ${moc_SRCS} ${kxmlguitest_SRCS})

target_link_libraries(kxmlguitest
    kde2::kdeui
    )

set(kaccelgentest_SRCS
    kaccelgentest.cpp
    )

add_executable(kaccelgentest ${moc_SRCS} ${kaccelgentest_SRCS})

target_link_libraries(kaccelgentest
    kde2::kdeui
    )

