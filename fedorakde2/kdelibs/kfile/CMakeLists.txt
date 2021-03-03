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

add_subdirectory(acl_prop_page)

set(libkfile_SRCS
    kfilebookmark.cpp kfiledialogconf.cpp kfilereader.cpp kfileviewitem.cpp booktoken.cpp
    kfileview.cpp kfileiconview.cpp kfilefilter.cpp krecentdocument.cpp kfiledialog.cpp
    kdiroperator.cpp kfiledetailview.cpp kcombiview.cpp kurlrequester.cpp kfilepreview.cpp
    kurlcombobox.cpp kurlrequesterdlg.cpp kopenwith.cpp kpropsdlg.cpp kicondialog.cpp
    kdirsize.cpp krecentdirs.cpp kdiskfreesp.cpp kimagefilepreview.cpp
    )

set(libkfile_HEADERS
    kfiledialog.h kfileviewitem.h kfilebookmark.h kfilereader.h kdiroperator.h
    kfileview.h booktoken.h kfiledetailview.h kcombiview.h kfileiconview.h
    kfilefilter.h krecentdocument.h kfileview.h kfiledialog.h kurlrequester.h
    kfilepreview.h kfile.h kurlcombobox.h kurlrequesterdlg.h kopenwith.h
    kpropsdlg.h kicondialog.h kdirsize.h kpreviewwidgetbase.h kimagefilepreview.h
    kdiskfreesp.h
    )

set(libkfile_NOINST_HEADERS
    kfiledialogconf.h kfilefilter.h config-kfile.h krecentdirs.h
    )

qt2_wrap_cpp(libkfile_SRCS
    SOURCES
        kcombiview.h
        kdiroperator.h
        kdirsize.h
        kdiskfreesp.h
        kfilebookmark.h
        kfiledetailview.h
        kfiledialogconf.h
        kfiledialog.h
        kfilefilter.h
        kfileiconview.h
        kfilepreview.h
        kfilereader.h
        kfileview.h
        kicondialog.h
        kimagefilepreview.h
        kopenwith.h
        kpreviewwidgetbase.h
        kpropsdlg.h
        kurlcombobox.h
        kurlrequesterdlg.h
        kurlrequester.h
    )

install(FILES ${libkfile_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kfile)

kde2_library(libkfile
    SOURCES
        ${libkfile_SRCS}
    LIBS
        kde2::ksycoca
    VERSION
        3.0.0
    )

set(kfstest_SRCS
    kfstest.cpp
    )

add_executable(kfstest ${kfstest_SRCS})

target_link_libraries(kfstest
    kde2::kfile
    )

set(kurlrequestertest_SRCS
    kurlrequestertest.cpp
    )

add_executable(kurlrequestertest ${kurlrequestertest_SRCS})

target_link_libraries(kurlrequestertest
    kde2::kfile
    )

install(FILES
    kpropsdlgplugin.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

install(FILES
    kpropsdlgplugin.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

