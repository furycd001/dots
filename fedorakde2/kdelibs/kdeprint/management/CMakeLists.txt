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
    kmdbcreator.h kmdriverdb.h kmdriverdbwidget.h kmiconview.h kminfopage.h
    kmmainview.h kmpages.h kmprinterpage.h kmprinterview.h kmjobviewer.h
    jobitem.h kmtimer.h kmpropwidget.h kmpropcontainer.h kmpropertypage.h
    kmpropgeneral.h cjanuswidget.h kmpropmembers.h kmpropbackend.h kmpropdriver.h
    kmlistview.h kminstancepage.h kmwizard.h kmwizardpage.h kmwinfopage.h
    kmwpassword.h kmwsocketutil.h kmwbackend.h kmwsocket.h kmwdriver.h
    kmwdriverselect.h kmwdrivertest.h kmwinfobase.h kmwname.h kmwend.h
    kmwclass.h kmwlpd.h kmwfile.h kmdriverdialog.h kmwsmb.h
    smbview.h kmconfigdialog.h kmconfigpage.h kmconfiggeneral.h kmspecialprinterdlg.h
    kmwlocal.h
    )

qt2_wrap_cpp(moc_SRCS
    SOURCES
        cjanuswidget.h
        kmconfigdialog.h
        kmconfiggeneral.h
        kmconfigpage.h
        kmdbcreator.h
        kmdriverdb.h
        kmdriverdbwidget.h
        kmiconview.h
        kminstancepage.h
        kmjobviewer.h
        kmlistview.h
        kmmainview.h
        kmprinterview.h
        kmpropcontainer.h
        kmpropertypage.h
        kmpropwidget.h
        kmspecialprinterdlg.h
        kmwclass.h
        kmwdrivertest.h
        kmwizard.h
        kmwizardpage.h
        kmwlocal.h
        kmwsmb.h
        kmwsocket.h
        kmwsocketutil.h
        smbview.h
    )

set(libkdeprint_management_SRCS
    kmdbcreator.cpp kmdriverdb.cpp kmdriverdbwidget.cpp kmiconview.cpp kminfopage.cpp
    kmmainview.cpp kmpages.cpp kmprinterview.cpp kmjobviewer.cpp jobitem.cpp
    kmtimer.cpp kmpropwidget.cpp kmpropcontainer.cpp kmpropertypage.cpp kmpropgeneral.cpp
    cjanuswidget.cpp kmpropmembers.cpp kmpropbackend.cpp kmpropdriver.cpp kmlistview.cpp
    kminstancepage.cpp kmwizard.cpp kmwizardpage.cpp kmwinfopage.cpp kmwpassword.cpp
    kmwsocketutil.cpp kmwbackend.cpp kmwsocket.cpp kmwdriver.cpp kmwdriverselect.cpp
    kmwdrivertest.cpp kmwinfobase.cpp kmwname.cpp kmwend.cpp kmwclass.cpp
    kmwlpd.cpp kmwfile.cpp kmdriverdialog.cpp kmwsmb.cpp smbview.cpp
    kmconfigdialog.cpp kmconfigpage.cpp kmconfiggeneral.cpp kmspecialprinterdlg.cpp kmwlocal.cpp
    )

set(libkdeprint_management_HEADERS
    kmmainview.h kmjobviewer.h kmprinterpage.h
    )

qt2_wrap_cpp(libkdeprint_management_SRCS
    SOURCES
        kmmainview.h kmjobviewer.h
    )

kde2_library(libkdeprint_management
    SOURCES
        ${libkdeprint_management_HEADERS}
        ${moc_SRCS}
        ${libkdeprint_management_SRCS}
    HEADERS
        ${libkdeprint_management_HEADERS}
    LIBS
        kde2::kdeprint
    )

