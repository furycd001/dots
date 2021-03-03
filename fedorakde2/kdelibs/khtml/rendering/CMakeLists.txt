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

set(libkhtmlrender_SRCS
    bidi.cpp break_lines.cpp render_style.cpp render_object.cpp render_container.cpp
    render_box.cpp render_flow.cpp render_text.cpp render_image.cpp render_table.cpp
    render_hr.cpp render_replaced.cpp render_form.cpp render_list.cpp render_root.cpp
    render_frames.cpp render_br.cpp render_applet.cpp render_html.cpp render_body.cpp
    )

set(libkhtmlrender_HEADERS
    bidi.h break_lines.h render_hr.h render_style.h render_object.h
    render_container.h render_box.h render_flow.h render_text.h render_box.h
    render_replaced.h render_form.h render_list.h render_root.h render_frames.h
    render_br.h render_applet.h
    )

qt2_wrap_cpp(libkhtmlrender_SRCS
    SOURCES
        render_form.h
        render_frames.h
        render_replaced.h
    )

get_target_property(KFILE_INCLUDES kde2::kfile INCLUDE_DIRECTORIES)


add_library(libkhtmlrender OBJECT ${libkhtmlrender_SRCS})
target_include_directories(libkhtmlrender PUBLIC ${KFILE_INCLUDES})
set_target_properties(libkhtmlrender PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )

