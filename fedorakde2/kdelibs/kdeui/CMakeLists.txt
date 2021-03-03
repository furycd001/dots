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

set(libkdeui_SRCS
    qxembed.cpp ksharedpixmap.cpp kthemestyle.cpp kurllabel.cpp knuminput.cpp
    ktoolbar.cpp klineedit.cpp klistview.cpp kprogress.cpp kimageeffect.cpp
    kcolordlg.cpp kselect.cpp kdatepik.cpp kdatetbl.cpp kfontdialog.cpp
    kpopupmenu.cpp ktabctl.cpp kstatusbar.cpp kmainwindow.cpp ktmainwindow.cpp
    kmenubar.cpp kpixmapio.cpp keditlistbox.cpp kseparator.cpp krestrictedline.cpp
    kcolorbtn.cpp ksystemtray.cpp kbuttonbox.cpp keditcl1.cpp keditcl2.cpp
    kled.cpp kdbtn.cpp kwizard.cpp kcontainer.cpp kkeydialog.cpp
    kruler.cpp kcursor.cpp kaccelmenu.cpp klineeditdlg.cpp kthemebase.cpp
    kcharselect.cpp kcolordrag.cpp knumvalidator.cpp kdialog.cpp kdialogbase.cpp
    kjanuswidget.cpp kaboutdialog.cpp kpixmapeffect.cpp kauthicon.cpp kmessagebox.cpp
    kdualcolorbtn.cpp ktoolbarradiogroup.cpp ktoolbarbutton.cpp ktextbrowser.cpp kaction.cpp
    khelpmenu.cpp kcmenumngr.cpp kpanelmenu.cpp kstdaction.cpp kdrawutil.cpp
    kiconview.cpp klistbox.cpp kbugreport.cpp kcombobox.cpp kpassdlg.cpp
    kxmlguiclient.cpp kxmlgui.cpp kxmlguibuilder.cpp kedittoolbar.cpp kdocktabctl.cpp
    kdockwidget.cpp kdockwidget_private.cpp kanimwidget.cpp krootpixmap.cpp kaboutkde.cpp
    kaboutapplication.cpp kpanelapplet.cpp kdcopactionproxy.cpp kcolorcombo.cpp kpushbutton.cpp
    kpanelextension.cpp kcompletionbox.cpp kalphapainter.cpp ksqueezedtextlabel.cpp kcommand.cpp
    kwindowlistmenu.cpp kfontcombo.cpp ktip.cpp kdatewidget.cpp karrowbutton.cpp
    )

set(libkdeui_mocable_HEADERS
    kprogress.h kcolordialog.h kselect.h kdatepik.h
    kdatetbl.h kfontdialog.h kpopupmenu.h ktabctl.h kstatusbar.h
    kmainwindow.h ktmainwindow.h ktoolbar.h kmenubar.h
    knuminput.h kseparator.h klineedit.h krestrictedline.h
    kcolorbtn.h ksystemtray.h kdockwindow.h kbuttonbox.h kcontainer.h
    keditcl.h kled.h keditlistbox.h kdbtn.h kwizard.h
    kkeydialog.h kurllabel.h kruler.h kaccelmenu.h
    klineeditdlg.h kthemebase.h kthemestyle.h kcharselect.h
    kcolordrag.h qxembed.h kdialog.h kdialogbase.h
    kjanuswidget.h kaboutdialog.h kauthicon.h  kdialogbase_priv.h
    ksharedpixmap.h kdualcolorbtn.h ktoolbarbutton.h ktoolbarradiogroup.h ktextbrowser.h
    kaction.h khelpmenu.h kcmodule.h kcmenumngr.h kpanelmenu.h
    kcombobox.h kiconview.h klistview.h
    klistbox.h kbugreport.h kpassdlg.h kxmlgui.h
    kedittoolbar.h kdocktabctl.h kdockwidget.h
    kanimwidget.h krootpixmap.h kpanelapplet.h
    kpushbutton.h kcolorcombo.h kpanelextension.h kcompletionbox.h
    ksqueezedtextlabel.h kcommand.h kwindowlistmenu.h kfontcombo.h
    ktip.h kdatewidget.h karrowbutton.h
    )

set(libkdeui_HEADERS
    kcursor.h kwidgetid.h knumvalidator.h kpixmapeffect.h kmessagebox.h
    kxmlguiclient.h kstdaction.h kdrawutil.h kxmlguibuilder.h kpixmapio.h
    kaboutkde.h kaboutapplication.h kdcopactionproxy.h kalphapainter.h
    kaccelgen.h kcolordlg.h kimageeffect.h  kcolorbutton.h
    ${libkdeui_mocable_HEADERS}
    )

set(libkdeui_mocable_NOINST_HEADERS
    kdockwidget_private.h klistviewlineedit.h kkeybutton.h
    kaboutdialog_private.h kcursor_private.h
    )
set(libkdeui_NOINST_HEADERS
    kdepackages.h kdeui-doc.h
    ${libkdeui_mocable_NOINST_HEADERS}
    )

qt2_wrap_cpp(libkdeui_SRCS
    SOURCES
        ${libkdeui_mocable_HEADERS}
        ${libkdeui_mocable_NOINST_HEADERS}
    )

install(FILES ${libkdeui_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kdeui)

kde2_library(libkdeui
    SOURCES
        ${libkdeui_SRCS}
    LIBS
        kde2::kdecore
    VERSION
        3.0.0
)

target_compile_options(libkdeui PUBLIC -Wno-narrowing)

install(FILES
    aboutkde.png keys.png ktip-bulb.png ktip-background.png
    DESTINATION ${KDE2_DATADIR}/kdeui/pics
    )

install(FILES ui_standards.rc DESTINATION ${KDE2_CONFDIR}/ui)
