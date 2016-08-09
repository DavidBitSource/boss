/* boss_transcription_de.cpp    $Revision: 1.3 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_transcription_uk.cpp
   @author Karlheinz Stoeber
   @author Joerg Broeggelwirth
   @author Stefan Breuer
*/

#include "boss_transcription_uk.h"
#include "boss_utility/dom_tools.h"
#include "boss_utility/boss_utility.h"

/**
 * @addtogroup boss_transcription
 * @{
 */

#ifndef BONT

#endif

BOSS_Transcription_UK::BOSS_Transcription_UK(BOSS::Config & c,MySQL_Handler * db)
	: BOSS_Transcription(c,db) {
}


void BOSS_Transcription_UK::operator () (xercesc::DOMNode * sentence)
{
  // Get all Child nodes from the sentence. This must be the word nodes
  xercesc::DOMNodeList* nl = sentence->getChildNodes();
  char* tempStr = NULL;
  string word_tag("WORD");
  string word_orth("");
#ifdef BONT
  string query_result("");
#else
  MySQL_Result query_result;
#endif
  bool first(true);
  
  // Iterate over all nodes contained in the sentence
  for(unsigned i=0; i<nl->getLength(); i++) {
    xercesc::DOMNode* word = nl->item(i);
    // Check if the current node is a word node
    tempStr = xercesc::XMLString::transcode(word->getNodeName());
    if(word->getNodeType() == xercesc::DOMNode::ELEMENT_NODE && (word_tag.compare(tempStr) == 0)) {
      // Extract the orthographic form of the word given by the attribute Orth 		
      word_orth = BOSS::getAttrByName(word, "Orth");
      // Check if the current word node contains a pause label, skip transcription, if so
      if(word_orth == "$p") {
	stringToDOM("$p", word);
	continue;
      }
      // Query the TKey attribute. If TKey is already present, then the transcription
      // was done before and we can continue
      if(BOSS::getAttrByName(word, "TKey").size()) {
	cerr << "boss_transcription_de: " <<  word_orth << " "
	     << BOSS::getAttrByName(word, "TKey") << " (given_transcription)" << endl;
	continue;
      }
      if(BOSS::getAttrByName(word, "WordTrans").size()) {
	stringToDOM(BOSS::getAttrByName(word, "WordTrans"), word);
	cerr << "boss_transcription_de: " <<  word_orth << " "
	     << BOSS::getAttrByName(word, "WordTrans") << " (given_transcription)" << endl;
	continue;
      }
      cerr << "boss_transcription_de: " << word_orth << " ";
      try {
	if(type_flag & 4)
	  query_result = queryExceptionLexicon("exception_lexicon", word_orth);
      }
      catch(const MySQL_QueryFailed & e) {
	e.printError(true);
      }
      if(query_result.valid() && query_result.row() > 0) {
	// Ok, we found the word in the lexicon. The transcription is contained as formatted string in
	// query_result. The stringToDOM function converts this string into an XML subtree.
	cerr << query_result(0, 0) << " (exception_lexicon)" <<  endl;
	stringToDOM(query_result(0, 0), word);
      }
      else {
	if (islower(word_orth[0])) {
	  word_orth[0]=toupper(word_orth[0]);
	  try {
	    if(type_flag & 4)
	      query_result = queryExceptionLexicon("exception_lexicon", word_orth);
	  }
	  catch(const MySQL_QueryFailed & e) {
	    e.printError(true);
	  }
	  word_orth[0]=tolower(word_orth[0]);
	}
	else {
	  if(first) {
	    // In English, the first word in a sentence should always begin with a capital letter. So we
	    // should convert it, and do a second query.
	    try {
	      if(type_flag & 4)
		query_result = queryExceptionLexicon("exception_lexicon", BOSS::my_tolower(word_orth));
	    }
	    catch(const MySQL_QueryFailed & e) {
	      e.printError(true);
	    }
	  }
	}
	if(query_result.valid() && query_result.row() > 0) {
	  // Ok, we found the word in the lexicon. The transcription is contained as formatted string in
	  // query_result. The stringToDOM function converts this string into an XML subtree.
	  cerr << query_result(0, 0) << " (exception_lexicon)" << endl;
	  stringToDOM(query_result(0, 0), word);
	}
	else {
	  cerr << " $p (FAILED)" << endl;
	  xercesc::DOMElement* word_elem = (xercesc::DOMElement *)word;
	  BOSS::setAttribute(word_elem, "Dur", "100");
	  stringToDOM("$p",word);
	}
      }
    }
    first = false;
    xercesc::XMLString::release(&tempStr);
  }
  addPhrases(sentence);
  addPhonemeContext(sentence);
  addHalfphones(sentence);
  addPhonemeNumber(sentence);
}

string BOSS_Transcription_UK::getDescription() {
        return "BOSS_Transcription_UK description string placeholder.";
}

extern "C" BOSS::Module * create(BOSS::Config _cl, MySQL_Handler * _db) {
	return new BOSS_Transcription_UK(_cl, _db);
}

extern "C" void destroy(BOSS_Transcription_UK * cm) {
	delete cm;
}

/** @} */ // end of boss_transcription group
