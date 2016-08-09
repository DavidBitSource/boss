/* boss_transcription.cpp    $Revision: 1.6 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_transcription.cpp
   @author Karlheinz Stoeber
   @author Joerg Broeggelwirth
   @author Stefan Breuer
*/

#include <sstream>
#include <fstream>
#include "boss_transcription.h"
#include "boss_utility/dom_tools.h"
#include "boss_utility/boss_utility.h"

/**
 * @addtogroup boss_transcription
 * @{
 */

#ifndef BONT

#else
// A necessary global variable, sorry!
p_Vector p_table;

static const char phonetablefile[] = "./data/runtime/blf_symbols.txt";
static const char exceptionlexiconfile[] = "./data/runtime/exception_lexicon.txt";
#endif

// MH:  added instead the constructor with a flag as a parameter
void BOSS_Transcription::setFlag(const unsigned &flag = 7) {
	type_flag = flag;
}

BOSS_Transcription::BOSS_Transcription(BOSS::Config & c, MySQL_Handler * db)
	: BOSS::Module(c, db)

{
  string ccfile = c("-ccfile").optionarg;
  cclass = new BOSS_context(ccfile);
#ifndef BONT
  // query of the phone table in the database
  MySQL_Result res;
  try {
    res = data_base->query("SELECT Phon FROM phone_table", false);
  }
  catch(const MySQL_QueryFailed & e) {
    e.printError(true);
  }
  
  if(res.row()>0) {
    BOSS::t_SVector expressions(res.row());
    for(unsigned i=0; i<res.row(); i++)
      expressions[i] = res(i, 0);
    
#else
  // Reading of the phone table from file
  ifstream in(phonetablefile);
  if (in) in >> p_table;
  else cerr << "Can't open p_table file!" << endl;
  
  if(p_table.size()>0) {
    t_SVector expressions(p_table.size());
    for(unsigned i=0; i<p_table.size(); i++)
      expressions[i] = p_table[i].phon;
    // Reading of the phone table
    ifstream in(phonetablefile);
    if (in) in >> p_table;
    else cerr << "Can't open p_table file!" << endl;
    
#endif
    // a new fsa for phone parsing is created
    try {
      pfsa = new BOSS::FSA(expressions);
    }
    catch(const BOSS::FileFormatError & e) {
      e.printError(true);
      }
    }
    else {
      cerr << "Empty phone definition in database" << endl;
      exit(-1);
  }
#ifdef BONT
  // Reading of the exception lexicon from file
  string orth(""), trans("");
  ifstream in2(exceptionlexiconfile);
  if (!in2) cerr << "Can't open exception lexicon file!" << endl;
  else while (in2) {
    in2 >> orth >> trans;
    //cout << "Orth: " << orth << " Trans: " << trans << endl;
    e_table[orth]=trans;
  }
#endif	
}

string BOSS_Transcription::delMorphBound(string & s) const {
    unsigned j=0;
    string res(s);
  for(unsigned i=0; i<s.size(); i++) {
    if(s[i] == ',')
      continue;
    if (s[i] == ';') {
      res[j++] = '.';
      continue;
    }
    res[j++] = s[i];
  }
  res.resize(j);
  return res;
}

#ifndef BONT
void BOSS_Transcription::stringToDOM(const string & trans, xercesc::DOMNode * word) const
{
  BOSS::t_SVector trans_sequence;
  //XMLCh  tempStr[100];
  if(BOSS::getAttrByName(word, string("Orth")) == "$p" || trans == "$p") trans_sequence.push_back("$p");
  else trans_sequence = pfsa->parse(trans);
  xercesc::DOMDocument* doc      = word->getOwnerDocument();
  xercesc::DOMElement* syllable = BOSS::createElement(doc, "SYLLABLE");
  string sstress("0"), pstress("0"), sylltrans, wordtrans;
  for(unsigned i=0; i<trans_sequence.size(); i++) {
    string & trans_entry = trans_sequence[i];
    switch(trans_entry[0]) {
    case '.':
    case ';':
      if (i>0) {
      	BOSS::setAttribute(syllable, "TKey", sylltrans.c_str());
      	BOSS::setAttribute(syllable, "Stress", sstress.c_str());
      	word->appendChild(syllable);
      	sstress   = "0";
      	sylltrans = "";
	}
      //Prevent incorrectly placed syllable markers from creating an empty syllable element
      if (i+1 < trans_sequence.size()) { 
	syllable = BOSS::createElement(doc, "SYLLABLE");
      }
      break;
    case '%':
      pstress = "2";
      sstress = "2";
      break;
    case '"':
      pstress = "1";
      sstress = "1";
      break;
    case ',':
      break;
    default:
      xercesc::DOMElement* phone = BOSS::createElement(doc, "PHONE");
      BOSS::setAttribute (phone, "TKey", trans_sequence[i].c_str());
      BOSS::setAttribute (phone, "Stress", pstress.c_str());
      syllable->appendChild(phone);
      pstress = "0";
      sylltrans += trans_sequence[i];
      wordtrans += trans_sequence[i];
      break;
    }
  }
  BOSS::setAttribute(syllable, "TKey", sylltrans.c_str());
  BOSS::setAttribute(syllable, "Stress", sstress.c_str());
  //  DOMElement word_elem = (DOMElement &)word;
  //  word_elem.setAttribute(DOMString("TKey"),  DOMString(wordtrans.c_str()));
  BOSS::setAttribute((xercesc::DOMElement*)word, "TKey", wordtrans.c_str());
  word->appendChild(syllable);
}

void BOSS_Transcription::addPhonemeContext(xercesc::DOMNode * sentence) const
   {
     BOSS::DOMNodeVector plist, slist, wlist, ptmp; 
     BOSS::createLists(sentence, "PHONE",  plist);
     BOSS::createLists(sentence, "SYLLABLE", slist);
     BOSS::createLists(sentence, "WORD",     wlist);
     static const char *CLeft("CLeft"), *CRight("CRight"), *CCLeft("CCLeft"), *CCRight("CCRight"),
       *CCLeft2("CCLeft2"), *CCRight2("CCRight2"), *TKey("TKey");//, *PInt("PInt");
     
     // First set the context of all phones
     if(plist.size()>0) {
       xercesc::DOMElement* element = (xercesc::DOMElement *)plist.front();
       if(plist.size() == 1) {
	 BOSS::setAttribute(element, CLeft,  "$p");
      BOSS::setAttribute(element, CRight, "$p");
      BOSS::setAttribute(element, CCLeft,  (cclass->left1("$p")).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1("$p")).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2("$p")).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2("$p")).c_str());
    }
    else {
      BOSS::setAttribute(element, CLeft,  "$p");
      BOSS::setAttribute(element, CRight, BOSS::getAttrByName(plist[1], TKey).c_str());
      BOSS::setAttribute(element, CCLeft,  (cclass->left1("$p")).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1(BOSS::getAttrByName(plist[1], TKey))).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2("$p")).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2(BOSS::getAttrByName(plist[1], TKey))).c_str());
      element = (xercesc::DOMElement *)plist.back();
      BOSS::setAttribute(element, CLeft,  BOSS::getAttrByName(plist[plist.size()-2], TKey).c_str());
      BOSS::setAttribute(element, CRight, "$p");
      BOSS::setAttribute(element, CCLeft,  (cclass->left1(BOSS::getAttrByName(plist[plist.size()-2], TKey))).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1("$p")).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2(BOSS::getAttrByName(plist[plist.size()-2], TKey))).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2("$p")).c_str());
    }
  }
  for(unsigned i=1; i<plist.size()-1; i++) {
    xercesc::DOMElement* element = (xercesc::DOMElement *)plist[i];
    BOSS::setAttribute(element, CLeft,  BOSS::getAttrByName(plist[i-1], TKey).c_str());
    BOSS::setAttribute(element, CCLeft, (cclass->left1(BOSS::getAttrByName(plist[i-1], TKey))).c_str());
    BOSS::setAttribute(element, CCLeft2, (cclass->left2(BOSS::getAttrByName(plist[i-1], TKey))).c_str());
    BOSS::setAttribute(element, CRight, BOSS::getAttrByName(plist[i+1], TKey).c_str());
    BOSS::setAttribute(element, CCRight, (cclass->right1(BOSS::getAttrByName(plist[i+1], TKey))).c_str());
    BOSS::setAttribute(element, CCRight2, (cclass->right2(BOSS::getAttrByName(plist[i+1], TKey))).c_str());
  }
  
  // Next set the context of all syllables
  if(slist.size()>0) {
    xercesc::DOMElement* element = (xercesc::DOMElement *)slist.front();
    if(slist.size() == 1) {
      BOSS::setAttribute(element, CLeft,  "$p");
      BOSS::setAttribute(element, CRight, "$p");
      BOSS::setAttribute(element, CCLeft,  (cclass->left1("$p")).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1("$p")).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2("$p")).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2("$p")).c_str());
    }
    else {
      ptmp.clear();
      BOSS::createLists(slist.front(), "PHONE",  ptmp);
      BOSS::setAttribute(element, CLeft,  "$p");
      BOSS::setAttribute(element, CRight, BOSS::getAttrByName(ptmp.back(), CRight).c_str());
      BOSS::setAttribute(element, CCLeft,  (cclass->left1("$p")).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1(BOSS::getAttrByName(ptmp.back(), CRight))).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2("$p")).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2(BOSS::getAttrByName(ptmp.back(), CRight))).c_str());
      element = (xercesc::DOMElement *)slist.back();
      ptmp.clear();
      BOSS::createLists(slist.back(), "PHONE",  ptmp);
      BOSS::setAttribute(element, CLeft,  BOSS::getAttrByName(ptmp.front(), CLeft).c_str());
      BOSS::setAttribute(element, CRight, "$p");
      BOSS::setAttribute(element, CCLeft,  (cclass->left1(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1("$p")).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2("$p")).c_str());
    }
  }
  for(unsigned i=1; i<slist.size()-1; i++) {
    xercesc::DOMElement* element = (xercesc::DOMElement *)slist[i];
    ptmp.clear();
    BOSS::createLists(slist[i], "PHONE",  ptmp);
    BOSS::setAttribute(element, CLeft,  BOSS::getAttrByName(ptmp.front(), CLeft).c_str());
    BOSS::setAttribute(element, CRight, BOSS::getAttrByName(ptmp.back(),  CRight).c_str());
    BOSS::setAttribute(element, CCLeft,  (cclass->left1(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
    BOSS::setAttribute(element, CCRight, (cclass->right1(BOSS::getAttrByName(ptmp.back(),  CRight))).c_str());
    BOSS::setAttribute(element, CCLeft2,  (cclass->left2(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
    BOSS::setAttribute(element, CCRight2, (cclass->right2(BOSS::getAttrByName(ptmp.back(),  CRight))).c_str());
  }
  
  // Now set the context of all words
  if(wlist.size()>0) {
    xercesc::DOMElement* element = (xercesc::DOMElement *)wlist.front();
    if(wlist.size() == 1) {
      BOSS::setAttribute(element, CLeft,  "$p");
      BOSS::setAttribute(element, CRight, "$p");
      BOSS::setAttribute(element, CCLeft,  (cclass->left1("$p")).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1("$p")).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2("$p")).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2("$p")).c_str());
    }
    else {
      ptmp.clear();
      BOSS::createLists(wlist.front(), "PHONE",  ptmp);
      BOSS::setAttribute(element, CLeft,  "$p");
      BOSS::setAttribute(element, CRight, BOSS::getAttrByName(ptmp.back(), CRight).c_str());
      BOSS::setAttribute(element, CCLeft,  (cclass->left1("$p")).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1(BOSS::getAttrByName(ptmp.back(), CRight))).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2("$p")).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2(BOSS::getAttrByName(ptmp.back(), CRight))).c_str());
      element = (xercesc::DOMElement *)wlist.back();
      ptmp.clear();
      BOSS::createLists(wlist.back(), "PHONE",  ptmp);
      BOSS::setAttribute(element, CLeft,  BOSS::getAttrByName(ptmp.front(), CLeft).c_str());
      BOSS::setAttribute(element, CRight, "$p");
      BOSS::setAttribute(element, CCLeft,  (cclass->left1(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
      BOSS::setAttribute(element, CCRight, (cclass->right1("$p")).c_str());
      BOSS::setAttribute(element, CCLeft2,  (cclass->left2(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
      BOSS::setAttribute(element, CCRight2, (cclass->right2("$p")).c_str());
    }
  }
  for(unsigned i=1; i<wlist.size()-1; i++) {
    xercesc::DOMElement* element = (xercesc::DOMElement *)wlist[i];
    ptmp.clear();
    BOSS::createLists(wlist[i], "PHONE",  ptmp);
    BOSS::setAttribute(element, CLeft,  BOSS::getAttrByName(ptmp.front(), CLeft).c_str());
    BOSS::setAttribute(element, CRight, BOSS::getAttrByName(ptmp.back(),  CRight).c_str());
    BOSS::setAttribute(element, CCLeft,  (cclass->left1(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
    BOSS::setAttribute(element, CCRight, (cclass->right1(BOSS::getAttrByName(ptmp.back(),  CRight))).c_str());
    BOSS::setAttribute(element, CCLeft2,  (cclass->left2(BOSS::getAttrByName(ptmp.front(), CLeft))).c_str());
    BOSS::setAttribute(element, CCRight2, (cclass->right2(BOSS::getAttrByName(ptmp.back(),  CRight))).c_str());
  }
}

void BOSS_Transcription::addPhrases(xercesc::DOMNode * sentence) const {
   // Add phrasing information on syllable and phone levels
   // Get all nodes with tag name "WORD".
  xercesc::DOMElement* element = (xercesc::DOMElement *) sentence;
  xercesc::DOMNodeList* wl = BOSS::getElementsByTagName(element, "WORD");
  string pmode(""), pint("");
  for(int i=0; i<int(wl->getLength()); i++) {
    xercesc::DOMNode* wnode = wl->item(i);
    xercesc::DOMElement* welement = (xercesc::DOMElement *) wnode;
    pmode = BOSS::getAttrByName(wnode, "PMode");
    pint = BOSS::getAttrByName(wnode, "PInt");
    if (pmode != "") {
      xercesc::DOMNodeList* sl = BOSS::getElementsByTagName(welement, "SYLLABLE");
      for (int i=0;i<int(sl->getLength()); i++) {
      	xercesc::DOMNode* snode = sl->item(i);
        xercesc::DOMElement* selement = (xercesc::DOMElement *) snode;
        xercesc::DOMNodeList* pl = BOSS::getElementsByTagName(selement, "PHONE");
        if (i<int(sl->getLength())-1) {
	  BOSS::setAttribute(selement, "PMode", "");
	  BOSS::setAttribute(selement, "PInt", "0");
          for (int i=0;i<int(pl->getLength()); i++) {
	    xercesc::DOMNode* pnode = pl->item(i);
            xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
	    BOSS::setAttribute(pelement, "PMode", "");
	    BOSS::setAttribute(pelement, "PInt", "0");
	  }
        }
        else {
	  BOSS::setAttribute(selement, "PMode", pmode.c_str());
	  BOSS::setAttribute(selement, "PInt", pint.c_str());
          for (int i=0;i<int(pl->getLength()); i++) {
	    xercesc::DOMNode* pnode = pl->item(i);
            xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
            BOSS::setAttribute(pelement, "PMode", pmode.c_str());
            BOSS::setAttribute(pelement, "PInt", pint.c_str());
          }
        }
      }
    }
    else {
      xercesc::DOMNodeList* sl = BOSS::getElementsByTagName(welement, "SYLLABLE");
      for (int i=0;i<int(sl->getLength()); i++) {
      	xercesc::DOMNode* snode = sl->item(i);
        xercesc::DOMElement* selement = (xercesc::DOMElement *) snode;
        xercesc::DOMNodeList* pl = BOSS::getElementsByTagName(selement, "PHONE");
        BOSS::setAttribute(selement, "PMode", "");
        BOSS::setAttribute(selement, "PInt", "0");
        for (int i=0;i<int(pl->getLength()); i++) {
	  xercesc::DOMNode* pnode = pl->item(i);
          xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
          BOSS::setAttribute(pelement, "PMode", "");
          BOSS::setAttribute(pelement, "PInt", "0");
        }
      }
    }
  }
}
 
 void BOSS_Transcription::addPhonemeNumber(xercesc::DOMNode * sentence) const
   {
        BOSS::DOMNodeVector ptmp, stmp, wlist;
	//createLists(sentence, DOMString("SYLLABLE"), slist);
	BOSS::createLists(sentence, "WORD", wlist);
	static const char /* *TKey("TKey"),*/ *PNr("PNr");
	unsigned wpnr(0);
	stringstream* stream(NULL);
	
	// Set the number of phones in the syllable (PNr)
	if(wlist.size()) {
		for (unsigned i=0; i<wlist.size(); i++) {
			BOSS::createLists(wlist[i], "SYLLABLE", stmp);
			for (unsigned j=0; j<stmp.size(); j++) {
				BOSS::createLists(stmp[j], "PHONE", ptmp);
				xercesc::DOMElement* selement = (xercesc::DOMElement *) stmp[j];
				stream = new stringstream;
				*stream << ptmp.size();
				BOSS::setAttribute(selement, PNr, stream->str().c_str());
				wpnr+=ptmp.size();
				delete stream;
				ptmp.clear();
			}
			xercesc::DOMElement* welement = (xercesc::DOMElement *) wlist[i];
			stream = new stringstream;
			*stream << wpnr;
			BOSS::setAttribute(welement, PNr, stream->str().c_str());
			delete stream;
			wpnr=0;
			stmp.clear();
		}
	}
}

void BOSS_Transcription::addHalfphones(xercesc::DOMNode * sentence) const {
    xercesc::DOMDocument* doc = sentence->getOwnerDocument();
    xercesc::DOMElement* stelement = (xercesc::DOMElement *) sentence;
    xercesc::DOMNodeList* pl = BOSS::getElementsByTagName(stelement, "PHONE");
    for (int i = 0; i<int(pl->getLength()); i++) {
        xercesc::DOMNode* pnode = pl->item(i);
        if (pnode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
            xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
            xercesc::DOMNamedNodeMap* pnm = pnode->getAttributes();
            xercesc::DOMElement* hp1 = BOSS::createElement(doc, "HALFPHONE");
            xercesc::DOMElement* hp2 = BOSS::createElement(doc, "HALFPHONE");
            string name = BOSS::getAttrByName(pnode, "TKey");
            for (unsigned j = 0; j < pnm->getLength(); j++) {
                stringstream attr, val;
                attr << pnm->item(j)->getNodeName();
                val << pnm->item(j)->getNodeValue();
                if (attr.str() == "CLeft") { //complete for CCLeft/CCLeft2 and CCRight/CCRight2
                    BOSS::setAttribute(hp1, attr.str().c_str(), val.str().c_str());
                    BOSS::setAttribute(hp2, attr.str().c_str(), name.c_str());
                } else if (attr.str() == "CCLeft") {
                    BOSS::setAttribute(hp1, attr.str().c_str(), val.str().c_str());
                    BOSS::setAttribute(hp2, attr.str().c_str(), cclass->left1(name.c_str()).c_str());
                } else if (attr.str() == "CCLeft2") {
                    BOSS::setAttribute(hp1, attr.str().c_str(), val.str().c_str());
                    BOSS::setAttribute(hp2, attr.str().c_str(), cclass->left2(name.c_str()).c_str());
                } else if (attr.str() == "CRight") {
                    BOSS::setAttribute(hp1, attr.str().c_str(), name.c_str());
                    BOSS::setAttribute(hp2, attr.str().c_str(), val.str().c_str());
                } else if (attr.str() == "CCRight") {
                    BOSS::setAttribute(hp1, attr.str().c_str(), cclass->right1(name.c_str()).c_str());
                    BOSS::setAttribute(hp2, attr.str().c_str(), val.str().c_str());
                } else if (attr.str() == "CCRight2") {
                    BOSS::setAttribute(hp1, attr.str().c_str(), cclass->right2(name.c_str()).c_str());
                    BOSS::setAttribute(hp2, attr.str().c_str(), val.str().c_str());
                } else {
                    hp1->setAttribute(pnm->item(j)->getNodeName(), pnm->item(j)->getNodeValue());
                    hp2->setAttribute(pnm->item(j)->getNodeName(), pnm->item(j)->getNodeValue());
                }
            }
            BOSS::setAttribute(hp1, "Half", "1");
            BOSS::setAttribute(hp2, "Half", "2");
            pelement->appendChild(hp1);
            pelement->appendChild(hp2);
        }
    }
}
#endif

BOSS_Transcription::~BOSS_Transcription()
{	if (pfsa) delete pfsa; }

#ifndef BONT
MySQL_Result BOSS_Transcription::queryExceptionLexicon(const string & table_name, const string & item) const
#else
string BOSS_Transcription::queryExceptionLexicon(const string & item) const 
#endif
{
#ifndef BONT
	stringstream query;
	query << "SELECT Trans FROM " << table_name << " WHERE Orth=\"" << item << "\"" << ends;
	return data_base->query(query.str().c_str());
#else
	//return t_SSMap(e_table)[item]; 
	return t_SSMap(e_table)[item]; 
#endif
}
/** @} */ // end of boss_transcription group
