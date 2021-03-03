/*
  Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>
  Copyright (C) 2001 Carsten Duvenhorst <duvenhorst@m2.uni-hannover.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef AUDIO_CD_H
#define AUDIO_CD_H

#include <qstring.h>

#include <kio/global.h>
#include <kio/slavebase.h>

struct cdrom_drive;

class AudioCDProtocol : public KIO::SlaveBase
{
  public:

    AudioCDProtocol(const QCString & pool, const QCString & app);
    virtual ~AudioCDProtocol();

    virtual void get(const KURL &);
    virtual void stat(const KURL &);
    virtual void listDir(const KURL &);

  protected:

    enum FileType
    {
      FileTypeUnknown,
      FileTypeOggVorbis,
      FileTypeMP3,
      FileTypeWAV
    };

    enum DirType
    {
      DirTypeUnknown,
      DirTypeDevice,
      DirTypeByName,
      DirTypeByTrack,
      DirTypeTitle,
      DirTypeInfo,
      DirTypeRoot,
      DirTypeMP3,
      DirTypeVorbis
    };

    void                  writeHeader(long);
    struct cdrom_drive *  findDrive(bool &noPermission);
    void                  parseArgs(const KURL &);

    void getParameters();

    void paranoiaRead(
        struct cdrom_drive  * drive,
        long                  firstSector,
        long                  lastSector,
        QString               fileType
    );

    struct cdrom_drive *  initRequest(const KURL &);
    uint                  discid(struct cdrom_drive *);
    void                  updateCD(struct cdrom_drive *);

    FileType fileType(const QString & filename);

    class Private;
    Private * d;
    
  private:
    cdrom_drive * pickDrive();
    unsigned int get_discid(cdrom_drive *);
};

#endif
// vim:ts=2:sw=2:tw=78:et:
