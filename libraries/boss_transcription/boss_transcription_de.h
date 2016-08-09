/* boss_transcription_de.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
   Copyright (C) 2000 University of Bonn.
   http://www.ikp.uni-bonn.de/boss

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/**
 * @file boss_transcription_de.h
   @author Karlheinz Stoeber
   @author Joerg Brroeggelwirth
   @author Stefan Breuer
*/

#ifndef BOSS_TRANSCRIPTION_DE_H
#define BOSS_TRANSCRIPTION_DE_H

#include "boss_transcription.h"
#include "boss_morphemes_de.h"
#include "boss_g2p/boss_g2p_de.h"

using namespace std;

/**
 * @addtogroup boss_transcription
 * @{
 */

/**
 * @brief German transcription module
 */
class BOSS_Transcription_DE : public BOSS_Transcription {

protected:
	
	unsigned type_flag;
	
	BOSS_Morphemes_DE morph_trans;
	BOSS_g2p_DE g2p_trans;
		

public:
	
  	BOSS_Transcription_DE(BOSS::Config & c, MySQL_Handler * db);
	virtual	void operator () (xercesc::DOMNode * sentence);
	virtual string getDescription();
};

/** @} */ // end of boss_transcription group
#endif
