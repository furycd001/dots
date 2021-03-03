/*  synthout.h	- class synthOut which handles the /dev/sequencer device
			for synths (as AWE32)
    This file is part of LibKMid 0.9.5
    Copyright (C) 1997,98  Antonio Larrosa Jimenez and P.J.Leonard
		  1999,2000 Antonio Larrosa Jimenez
    LibKMid's homepage : http://www.arrakis.es/~rlarrosa/libkmid.html                                         
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.                                                  

    Send comments and bug fixes to Antonio Larrosa <larrosa@kde.org>

***************************************************************************/
#ifndef _SYNTHOUT_H
#define _SYNTHOUT_H

#include <libkmid/midiout.h>

/**
 * Synth (AWE) device output class . SynthOut is used to send MIDI events to
 * a general synthesizer, such as AWE synth.
 *
 * SynthOut inherits @ref MidiOut and supports the same simple API.
 *
 * The preferred way to use this class is by selecting a synth (or AWE)
 * device with @ref MidiManager::setDefaultDevice(), and use a 
 * @ref MidiManager object.
 *
 * @short Sends MIDI events to AWE synthesizers
 * @version 0.9.5 17/01/2000
 * @author Antonio Larrosa Jimenez <larrosa@kde.org>
 */
class SynthOut : public MidiOut
{
  private:
    class SynthOutPrivate;
    SynthOutPrivate *di;

  public:
    /**
     * Constructor. See @ref MidiOut::MidiOut() for more information.
     */
    SynthOut(int d=0);

    /**
     * Destructor.
     */
    ~SynthOut();

    /**
     * See @ref MidiOut::openDev()
     */
    void openDev	(int sqfd);

    /**
     * See @ref MidiOut::closeDev()
     */
    void closeDev(void);

    /**
     * See @ref MidiOut::initDev()
     */
    void initDev	(void);

    /**
     * See @ref MidiOut::noteOn()
     */
    void noteOn		( uchar chn, uchar note, uchar vel );

    /**
     * See @ref MidiOut::noteOff()
     */
    void noteOff	( uchar chn, uchar note, uchar vel );

    /**
     * See @ref MidiOut::keyPressure()
     */
    void keyPressure	( uchar chn, uchar note, uchar vel );

    /**
     * See @ref MidiOut::chnPatchChange()
     */
    void chnPatchChange	( uchar chn, uchar patch );

    /**
     * See @ref MidiOut::chnPressure()
     */
    void chnPressure	( uchar chn, uchar vel );

    /**
     * See @ref MidiOut::chnPitchBender()
     */
    void chnPitchBender	( uchar chn, uchar lsb,  uchar msb );

    /**
     * See @ref MidiOut::chnController()
     */
    void chnController	( uchar chn, uchar ctl , uchar v ); 

    /**
     * It's an empty function, as AWE devices don't support System Exclusive
     * messages
     */
    void sysex		( uchar *data,ulong size);
};

#endif
