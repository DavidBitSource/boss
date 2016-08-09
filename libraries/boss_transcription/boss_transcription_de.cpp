/* boss_transcription_de.cpp    $Revision: 1.6 $ $Date: 2010/11/24 11:40:37 $
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
 * @file boss_transcription_de.cpp
 @author Karlheinz Stoeber
 @author Joerg Broeggelwirth
 @author Stefan Breuer
 @author Filip Müllers
*/

#include "boss_transcription_de.h"
#include "boss_utility/dom_tools.h"
#include "boss_utility/boss_utility.h"

/**
 * @addtogroup boss_transcription
 * @{
 */

#ifndef BONT

#endif


BOSS_Transcription_DE::BOSS_Transcription_DE(BOSS::Config & c, MySQL_Handler * db) :
  BOSS_Transcription(c, db), type_flag(7), morph_trans(c, db), g2p_trans(c, BLF) {}



void BOSS_Transcription_DE::operator () (xercesc::DOMNode * sentence)
{
  // Get all Child nodes from the sentence. This must be the word nodes
  xercesc::DOMNodeList* nl = sentence->getChildNodes();
  char* tempStr = NULL;
  string word_tag("WORD");
  string w("");
#ifdef BONT
  string query_result("");
#else
  MySQL_Result query_result;
#endif
  bool first(true);
	
  // Iterate over all nodes contained in the sentence
  for(unsigned i=0; i<nl->getLength(); i++) {
    string word_orth("");
    xercesc::DOMNode* word = nl->item(i);
    // Check if the current node is a word node
    tempStr = xercesc::XMLString::transcode(word->getNodeName());
    if(word->getNodeType() == xercesc::DOMNode::ELEMENT_NODE && (word_tag.compare(tempStr) == 0)) {
      // Extract the orthographic form of the word given by the attribute Orth 		
      w = BOSS::getAttrByName(word, "Orth");
      for (unsigned i=0; i < w.length(); i++) {
      	  if (w.at(i)!=-61) {
	    word_orth =word_orth + (w[i]);
	  } else {
	    i = i+3;
	    if (w.at(i)==-74){
	      word_orth =word_orth + ("ö");
	    } else if (w.at(i)==-92){
		word_orth =word_orth + ("ä");
	    } else if (w.at(i)==-68){
		word_orth =word_orth + ("ü");
	    } else if (w.at(i)==-97){
		word_orth =word_orth + ("ß");
	    } else if (w.at(i)==-124){
		word_orth =word_orth + ("Ä");
	    } else if (w.at(i)==-106){
		word_orth =word_orth + ("Ö");
	    } else if (w.at(i)==-100){
	      word_orth =word_orth + ("Ü");
	    }
	  }
      }
//      cerr << word_orth << endl;
#ifndef BONT
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
      }
      else {
	cerr << "boss_transcription_de: " << word_orth << " ";
#else
	cout << "boss_transcription_de: " << word_orth << " ";
#endif
	// Now query the database. There are three query and construction levels:
	// 1. Exception lexicon
	// 2. Morpheme lexicon with morpheme dividing algorithm (language specific!)
	// 3. Substring rule lexicon with substring mapping algorithm (language specific!)
	//cerr << "type_flag: " << type_flag << " type_flag & 4 = " << (type_flag & 4) << endl; 
#ifndef BONT
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
#else
      if(type_flag & 4)
        query_result = queryExceptionLexicon(word_orth);
      if(query_result!="") {
	// Ok, we found the word in the lexicon. The transcription is contained as formatted string in
	// query_result. The stringToDOM function converts this string into an XML subtree.
	cout << query_result << " (exception_lexicon)" <<  endl;
      }
#endif
      else {
	if (islower(word_orth[0])) {
	  word_orth[0]=toupper(word_orth[0]);
#ifndef BONT
	  try {
	    if(type_flag & 4)
	      query_result = queryExceptionLexicon("exception_lexicon", word_orth);
	  }
	  catch(const MySQL_QueryFailed & e) {
	    e.printError(true);
	  }
#else
	  if(type_flag & 4)
	    query_result = queryExceptionLexicon(word_orth);
#endif
	  word_orth[0]=tolower(word_orth[0]);
	}
	else {
	  if(first) {
	    // In the german language, the first word should always begin with a capital letter. So we
	    // should convert it, and do a second query.
#ifndef BONT
	    try {
	      if(type_flag & 4)
		query_result = queryExceptionLexicon("exception_lexicon", BOSS::my_tolower(word_orth));
	    }
	    catch(const MySQL_QueryFailed & e) {
	      e.printError(true);
	    }
#else
	    if(type_flag & 4)
	      query_result = queryExceptionLexicon(BOSS::my_tolower(word_orth));
#endif
	  }
	}
#ifndef BONT
	if(query_result.valid() && query_result.row() > 0) {
#else
	  if(query_result!="") {
#endif
	    // Ok, we found the word in the lexicon. The transcription is contained as formated string in
	    // query_result. The stringToDOM function converts this string in a XML subtree.
#ifndef BONT
	    cerr << query_result(0, 0) << " (exception_lexicon)" << endl;
	    stringToDOM(query_result(0, 0), word);
#else
	    cout << query_result << " (exception_lexicon)" << endl;
#endif
	  }
	  else {
	    // The orthographic form of the word is not contained in the exception lexicon. Let's try
	    // the morpheme based transcription
	    string morph_trans_result;
	    if (type_flag & 2) morph_trans_result = morph_trans(BOSS::my_tolower(word_orth));
	    if (morph_trans_result.size()) {
	      morph_trans_result = g2p_trans.assign_stress(morph_trans_result);
	      //workaround for issue with symbol-internal morpheme boundaries.
	      morph_trans_result = delMorphBound(morph_trans_result);
#ifndef BONT
	      cerr << morph_trans_result << " (morph_trans_result)" << endl;
	      stringToDOM(morph_trans_result, word);
#else
	      cout << morph_trans_result << " (morph_trans_result)" << endl;
#endif
	    }
	    else {
	      string g2p_trans_result;
	      if (type_flag & 1) g2p_trans_result = g2p_trans.transcribe(word_orth);
	      if(g2p_trans_result.size()) {
#ifndef BONT
		cerr << g2p_trans_result << " (g2p_trans_result)" << endl;
		stringToDOM(g2p_trans_result, word);
#else
		cout << g2p_trans_result << " (g2p_trans_result)" << endl;
#endif
	      }
	      else {
#ifndef BONT
		cerr << " $p (FAILED)" << endl;
		xercesc::DOMElement* word_elem = (xercesc::DOMElement *)word;
		BOSS::setAttribute(word_elem, "Dur", "100");
		stringToDOM("$p",word);
#else
		cerr << "Transcription failed for " << word_orth << endl;
#endif
	      }
	    }
	  }
	}
      }
      first = false;
      }
      xercesc::XMLString::release(&tempStr);
    }
#ifndef BONT
    addPhrases(sentence);
    addPhonemeContext(sentence);
    addHalfphones(sentence);
    addPhonemeNumber(sentence);
#endif
  }
  
  string BOSS_Transcription_DE::getDescription() {
    return "BOSS_Transcription_DE description string placeholder.";
  }
  
  extern "C" BOSS::Module * create(BOSS::Config _cl, MySQL_Handler * _db) {
    return new BOSS_Transcription_DE(_cl, _db);
  }
  
  extern "C" void destroy(BOSS_Transcription_DE * cm) {
    delete cm;
  }
  
  /** @} */ // end of boss_transcription group
