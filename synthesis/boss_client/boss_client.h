/* boss_client.h    $Revision: 1.5 $ $Date: 2006/07/04 14:44:07 $
   Copyright (C) 1999 University of Bonn.
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
   @author Karlheinz Stoeber
   @author Stefan Breuer
   @author Joerg Broeggelwirth
*/

#ifndef BOSS_CLIENT_H
#define BOSS_CLIENT_H

#include "boss_utility/commandline.h"
#include "Token.h"

/**
 * @defgroup boss_client BOSS Client (preproc)
 * @{
 */

/**
 * @brief Client is a user interface to the boss server capabilities. It provides text parsing and tokenization.
 */
class BOSS_Client {
        BOSS::CommandLine & cl;
	void processToken(Token & token, Phrases & phrases);
   public:
	BOSS_Client(BOSS::CommandLine & config): cl(config) {}
	virtual void run();
};

/** @} */ // end of boss_client group
#endif

