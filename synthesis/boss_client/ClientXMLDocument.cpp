/* ClientXMLDocument.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
 * @file ClientXMLDocument.cpp
   @author Michal Holcik
   @author Karlheinz Stoeber
   @author Stefan Breuer
*/


#include <fstream>
#include <string>
#include <sstream>
#include "ClientXMLDocument.h"

using namespace std;

/**
 * @addtogroup boss_client
 * @{
 */

/// @brief ClientXMLDocument constructor creates an XML document header and root element
/// and puts them in the output string stream.
ClientXMLDocument::ClientXMLDocument(BOSS::CommandLine & config,
		const Phrases & phrases): cl(config)
{

  outs << "<?xml version='1.0' encoding='ISO-8859-1' ?>" << endl;
  outs << "<TEXT>" << endl;

   for (int i = 0; i < phrases.size(); i++)
   {
	   phraseToXML(phrases[i]);
   }

  outs << "</TEXT>" << endl;
}


// phraseToXML() function converts a single phrase to its corresponding
// "SENTENCE" XML element representation.

void ClientXMLDocument::phraseToXML(const Phrase & phrase)
{
	int pint(0);
	  string pmode("");
	  if (cl("-inv").optionflag) {
		outs << "  <SENTENCE Type=\"" << phrase.back().second << "\" Inv=\"" << cl("-inv").optionarg << "\">" << endl;
	  }
	  else {
		outs << "  <SENTENCE Type=\"" << phrase.back().second << "\">" << endl;
	  }
	  for(unsigned i=0; i<phrase.size()-1; i++) {
	    if(phrase[i+1].first == TOKEN_PHRASE_DELIMITER) {
	      pint = 2;
	      switch (phrase[i+1].second[0]) {
	      case ':' : pmode="."; break;
	      default : pmode="?"; break;
	      }
	    }
	    if(phrase[i+1].first == TOKEN_SENTENCE_DELIMITER) {
	      pint = 5;
	      switch (phrase[i+1].second[0]) {
	      case '?' : pmode="?"; break;
	      default : pmode="."; break;
	      }
	    }
	    outs << "   <WORD Orth=\"" << phrase[i].second << "\" PInt=\"" <<pint
		<< "\" PMode=\"" << pmode << "\"></WORD>" << endl;
	    if(pint) {
	      pint = 0;
	      pmode="";
	      i++;
	    }
	  }
	  outs << "  </SENTENCE>" << endl;

}

/** @} */ // end of boss_client group
