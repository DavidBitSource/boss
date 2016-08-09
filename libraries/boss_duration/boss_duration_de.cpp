/* boss_duration_de.cpp    $Revision: 1.5 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2001 University of Bonn.
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/**
 * @file boss_duration_de.cpp
 * @author Joerg Broeggelwirth
 */

#include "boss_duration_de.h"

/** @addtogroup boss_duration
 * @{
 */

/**
 * @brief Returns the BOSS_Duration_DE module description string.
 * @return description string
 */
string BOSS_Duration_DE::getDescription() {
	return "BOSS Module Description: BOSS_Duration_DE description string placeholder.";
}

/**
 * @brief Sets module features.
 */
void BOSS_Duration_DE::set_features(xercesc::DOMNode* sentence) const {
  BOSS::DOMNodeVector plist, wlist, stmp, ptmp, htmp;
  BOSS::createLists(sentence, "WORD", wlist);
  BOSS::createLists(sentence, "PHONE", plist);
  static const char *Laut("TKey"), *PInt("PInt");
	
  // Set FLAUT2
  for (unsigned c=0; c<plist.size(); c++) {
    xercesc::DOMElement* element = (xercesc::DOMElement *)plist[c];
    if ((c<(plist.size()-2))&&(plist.size()>3)) {
      BOSS::setAttribute(element, "CRight2", BOSS::getAttrByName(plist[c+2], Laut).c_str());
    }
    else BOSS::setAttribute(element, "CRight2", "$p");
  }
  //Set PHRPOS and BODI
  string value("");
  unsigned prevpos=0;
  unsigned sylnr=0;
  for (unsigned c=0; c<wlist.size(); c++) {
    BOSS::createLists(wlist[c], "SYLLABLE", stmp);
    sylnr+=stmp.size();
    stmp.clear();
    if (BOSS::getAttrByName(wlist[c], PInt)!="0") {
      unsigned maxsylnr=sylnr;
      for (unsigned d=prevpos; d<=c; d++) {
	BOSS::createLists(wlist[d], "SYLLABLE", stmp);
	// BODI is inserted on syllable level
	for (unsigned e=0; e<stmp.size(); e++) {
	  xercesc::DOMElement* selement=(xercesc::DOMElement *)stmp[e];
	  stringstream tmp;
	  tmp << --sylnr;
	  tmp >> value;
	  BOSS::setAttribute(selement, "Bodi", value.c_str());
	  // PHRPOS is inserted on phone level
	  BOSS::createLists(stmp[e], "PHONE", ptmp);
	  for (unsigned f=0; f<ptmp.size(); f++) {
	    xercesc::DOMElement* pelement=(xercesc::DOMElement *)ptmp[f];
	    if (sylnr==0) BOSS::setAttribute(pelement, "Phrpos", "F");
	    else {
	      if ((float(sylnr)/float(maxsylnr-1))>0.5) {
		BOSS::setAttribute(pelement, "Phrpos", "I");
	      }
	      else BOSS::setAttribute(pelement, "Phrpos", "M");
	    }
	    BOSS::createLists(ptmp[f], "HALFPHONE", htmp);
	    for (unsigned g=0; g<htmp.size(); g++) {
	      xercesc::DOMElement* helement=(xercesc::DOMElement *)htmp[g];
	      BOSS::setAttribute(helement, "Phrpos", BOSS::getAttrByName(pelement, "Phrpos").c_str());
	    }
	  }
	  // PHRPOS is inserted on syllable level, as well
	  BOSS::setAttribute(selement, "Phrpos", BOSS::getAttrByName(ptmp[ptmp.size()-1], "Phrpos").c_str());
	  ptmp.clear();
	}
	// PHRPOS is inserted on word level, as well (value of the last syllable in the word)
	xercesc::DOMElement* welement=(xercesc::DOMElement *)wlist[d];
	BOSS::setAttribute(welement, "Phrpos", BOSS::getAttrByName(stmp[stmp.size()-1], "Phrpos").c_str());
	stmp.clear();
      }
      prevpos=c+1;
      sylnr=0;
    }
  }
}

/**
 * @brief Finds duration.
 * @return duration
 */
Duration BOSS_Duration_DE::find_dur(const xercesc::DOMElement* element,
			    BOSS::DOMNodeVector & wlist,
			    BOSS::DOMNodeVector & stmp,
			    BOSS::DOMNodeVector & ptmp,
			    BOSS::DOMNodeVector & htmp,
		  unsigned w, unsigned s, unsigned p) const {
  //Duration BOSS_Duration_DE::find_dur(const xercesc::DOMElement* element, unsigned w, unsigned s, unsigned p) {
  static FMap ftable;
  ftable["PLAUT"]=BOSS::getAttrByName(element, "CLeft");
  ftable["FLAUT"]=BOSS::getAttrByName(element, "CRight");
  ftable["FLAUT2"]=BOSS::getAttrByName(element, "CRight2");
  ftable["PHRPOS"]=BOSS::getAttrByName(element, "Phrpos");
  ftable["LAUT"]=BOSS::getAttrByName(element, "TKey");
  CART_node * res = tree->Classify(ftable);
  stringstream stream;
  float mean(0.0);
  float stddev(0.0);
  stream << res->question;
  stream >> stddev >> mean;
  //rule-based correction of the statistically predicted values
  if (BOSS::getAttrByName(stmp[s], "Phrpos")=="F") mean = correct_dur(mean, p, ptmp.size());
  Duration dur;
  dur.pdur = unsigned(mean+0.5);
  dur.hdur1 = dur.hdur2 = unsigned((mean/2)+0.5);
  return dur; 
}  

float BOSS_Duration_DE::correct_dur(float mean, const unsigned p, const unsigned pnumber) const {
  static const float screw(0.65);
  mean *= screw * exp(float(p+1)/float(pnumber));
  return mean;
}
/** @} */ // end of boss_duration group
