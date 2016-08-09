/* ClientXMLDocument.h    $Revision: 1.3 $ $Date: 2007/04/16 09:44:37 $
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
 * @file ClientXMLDocument.h
   @author Michal Holcik
*/

#ifndef CLIENT_XML_DOCUMENT_H
#define CLIENT_XML_DOCUMENT_H

#include <fstream>
#include <sstream>
#include "Token.h"
#include "boss_utility/commandline.h"

using namespace std;

// ClientXMLDocument is a class representing the XML document created in the
// BOSS client. In current state it is only a wrapper to the original BOSS
// client code. 
// The XML document is created from a phrases of parsed words stored in phrases
// object. The XML is internally stored in a stringstream object from which 
// its is read and sent by the net code.

/**
 * @addtogroup boss_client
 * @{
 */

/**
 * @brief Simple XML Document wrapper class for use in BOSS_Client.
 */
class ClientXMLDocument {
  	ostringstream outs;
	BOSS::CommandLine & cl;
	
protected:
	void phraseToXML(const Phrase & phrase);
	
public:
	ClientXMLDocument(BOSS::CommandLine & config, const Phrases & phrases);
	ostringstream & getStringStream() {
		return outs;
	}
	
};

/** @} */ // end of boss_client group
#endif

