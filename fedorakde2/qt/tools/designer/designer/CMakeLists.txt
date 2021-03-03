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

set(DESIGNER_SRCS
    command.cpp
    formwindow.cpp
    defs.cpp
    layout.cpp
    main.cpp
    mainwindow.cpp
    metadatabase.cpp
    pixmapchooser.cpp
    propertyeditor.cpp
    resource.cpp
    sizehandle.cpp
    orderindicator.cpp
    widgetfactory.cpp
    config.cpp
    hierarchyview.cpp
    listboxeditorimpl.cpp
    connectioneditorimpl.cpp
    newformimpl.cpp
    formlist.cpp
    help.cpp
    editslotsimpl.cpp
    listvieweditorimpl.cpp
    connectionviewerimpl.cpp
    customwidgeteditorimpl.cpp
    paletteeditorimpl.cpp
    styledbutton.cpp
    previewstack.cpp
    iconvieweditorimpl.cpp
    helpdialogimpl.cpp
    topicchooserimpl.cpp
    multilineeditorimpl.cpp
    formsettingsimpl.cpp
    asciivalidator.cpp
    import.cpp
    splashloader.cpp
    )

set(DESIGNER_UIS
    listboxeditor.ui
    connectioneditor.ui
    editslots.ui
    newform.ui
    listvieweditor.ui
    connectionviewer.ui
    customwidgeteditor.ui
    texteditpreview.ui
    listviewpreview.ui
    paletteeditor.ui
    radiopreview.ui
    checkboxpreview.ui
    widgetpreview.ui
    iconvieweditor.ui
    preferences.ui
    helpdialog.ui
    topicchooser.ui
    multilineeditor.ui
    formsettings.ui
    about.ui
    pixmapfunction.ui
    createtemplate.ui
    )

set(DESIGNER_HEADERS
    asciivalidator.h
    command.h
    config.h
    connectioneditorimpl.h
    connectionviewerimpl.h
    customwidgeteditorimpl.h
    defs.h
    editslotsimpl.h
    formlist.h
    formsettingsimpl.h
    formwindow.h
    helpdialogimpl.h
    help.h
    hierarchyview.h
    iconvieweditorimpl.h
    import.h
    layout.h
    listboxeditorimpl.h
    listvieweditorimpl.h
    mainwindow.h
    metadatabase.h
    multilineeditorimpl.h
    newformimpl.h
    orderindicator.h
    paletteeditorimpl.h
    pixmapchooser.h
    previewstack.h
    propertyeditor.h
    resource.h
    sizehandle.h
    splashloader.h
    styledbutton.h
    topicchooserimpl.h
    widgetfactory.h
    )

qt2_wrap_moc(DESIGNER_SRCS
    SOURCES ${DESIGNER_HEADERS})

qt2_wrap_ui(DESIGNER_SRCS
    SOURCES ${DESIGNER_UIS}
    )

add_executable(designer-qt2 ${DESIGNER_SRCS}
    $<TARGET_OBJECTS:qutil>
    $<TARGET_OBJECTS:shared>
    $<TARGET_OBJECTS:integration>
    )

target_link_libraries(designer-qt2
    Qt::Qt2
    ZLIB::ZLIB
    )

target_compile_definitions(designer-qt2 PRIVATE DESIGNER)
target_include_directories(designer-qt2
    PRIVATE
        ../util
        ../shared
    )

install(TARGETS designer-qt2 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
