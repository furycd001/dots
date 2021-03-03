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

install(FILES
    x-dvi.desktop x-tex-gf.desktop x-gzip.desktop x-object.desktop x-tex-pk.desktop
    pdf.desktop x-tar.desktop x-trash.desktop x-zip.desktop x-core.desktop
    x-font.desktop x-java.desktop x-jar.desktop x-arj.desktop x-tarz.desktop
    postscript.desktop octet-stream.desktop x-shellscript.desktop x-desktop.desktop x-executable.desktop
    x-archive.desktop x-lha.desktop x-tbz.desktop x-tgz.desktop x-awk.desktop
    x-perl.desktop x-compress.desktop x-cpio.desktop x-rpm.desktop msword.desktop
    msword2.desktop x-sharedlib.desktop wordperfect.desktop x-troff.desktop x-troff-man.desktop
    x-python.desktop x-python-bytecode.desktop x-bzip.desktop x-bzip2.desktop mspowerpoint.desktop
    msexcel.desktop smil.desktop x-rar.desktop x-zoo.desktop x-tzo.desktop
    x-lzop.desktop x-applixword.desktop x-applixspread.desktop x-applixgraphics.desktop x-truetype-font.desktop
    msexcel2.desktop x-shockwave-flash.desktop x-designer.desktop x-linguist.desktop x-ogg.desktop
    DESTINATION ${KDE2_MIMEDIR}/application
    )

