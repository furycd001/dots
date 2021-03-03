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


if(TIFF_FOUND)
    list(APPEND TIFF_KIMGIO tiff.kimgio g3.kimgio)
    kde2_module(kimg_tiff
        SOURCES
            tiffr.cpp
        LIBS
            TIFF::TIFF
        )

    kde2_module(kimg_g3
        SOURCES
            g3r.cpp
        LIBS
            TIFF::TIFF
        )
endif()

kde2_module(kimg_xview
    SOURCES
        xview.cpp
    )

kde2_module(kimg_krl
    SOURCES
        krl.cpp
    )

kde2_module(kimg_eps
    SOURCES
        eps.cpp
    PRIVATE_LIBS
        kde2::kdecore
    )

kde2_module(kimg_ico
    SOURCES
        ico.cpp
    )

install(FILES
        png.kimgio xpm.kimgio bmp.kimgio gif.kimgio pnm.kimgio xbm.kimgio
        jpeg.kimgio xv.kimgio krl.kimgio eps.kimgio ${TIFF_KIMGIO} ico.kimgio
    DESTINATION
        ${KDE2_SERVICESDIR}
    )
