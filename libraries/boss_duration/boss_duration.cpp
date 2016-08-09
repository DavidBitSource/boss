/* boss_duration.cpp    $Revision: 1.3 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2005 University of Bonn.
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
 * @file boss_duration.cpp
 * @author Joerg Broeggelwirth
 */

#include "boss_duration.h"

/** @addtogroup boss_duration
 * @{
 */

/**
 * @brief BOSS_Duration constructor.
 */
BOSS_Duration::BOSS_Duration(BOSS::Config & __cl, MySQL_Handler * __data_base): Module(__cl, __data_base) {
  string filename = __cl("-durfile").optionarg;
  tree = new BOSS_CartReader(filename);
}

/**
 * @brief BOSS_Duration destructor.
 */
BOSS_Duration::~BOSS_Duration() {
  tree->~BOSS_CartReader();
}

/**
 * @brief Adds duration attributes to the BOSS DOM document.
 * @param sentence the dom document
 */
void BOSS_Duration::add_durations(xercesc::DOMNode* sentence) const {
  static const char *Dur("Dur");
  unsigned sdur(0), wdur(0);
  BOSS::DOMNodeVector wlist, stmp, ptmp, htmp;
//   stmp.clear();
//   ptmp.clear();
//   htmp.clear();
  BOSS::createLists(sentence, "WORD", wlist);
  for (unsigned w=0; w<wlist.size(); w++) {
    BOSS::createLists(wlist[w], "SYLLABLE", stmp);
    for (unsigned s=0; s<stmp.size(); s++) {
      BOSS::createLists(stmp[s], "PHONE", ptmp);
      for (unsigned p=0; p<ptmp.size(); p++) {
	BOSS::createLists(ptmp[p], "HALFPHONE", htmp);
	Duration dur;
	xercesc::DOMElement* element = (xercesc::DOMElement *)ptmp[p];
	stringstream stream;
	stringstream hstream1;
	stringstream hstream2;
	if (BOSS::getAttrByName(element, Dur).size()) {
	  //If a Dur value is predefined for a phone element, read it and
	  // calculate half-phone values from it.
	  stream << BOSS::getAttrByName(element, Dur);
	  float predur(0.0);
	  stream >> predur;
	  hstream1 << unsigned((predur/2)+0.5);
	  hstream2 << unsigned((predur/2)+0.5);
	}
	else {
	  //duration predicted by the CART tree
	  dur = find_dur(element, wlist, stmp, ptmp, htmp, w, s, p);
	  if (dur.pdur > 0) {
	    stream << dur.pdur;
	    hstream1 << dur.hdur1;
	    hstream2 << dur.hdur2;
	  }
	  else { 
	    stream << "";
	    hstream1 << "";
	    hstream2 << "";
	  }
	}
	//insertion of Dur attribute on phone level
	BOSS::setAttribute(element, Dur, stream.str().c_str());
	for (unsigned h=0; h<htmp.size(); h++) {
	  xercesc::DOMElement* helement = (xercesc::DOMElement *)htmp[h];
	  if (!BOSS::getAttrByName(helement, Dur).size()) {
	    switch (h) {
	    case 0:
	      BOSS::setAttribute(helement, Dur, hstream1.str().c_str());	
	      break;
	    case 1:
	      BOSS::setAttribute(helement, Dur, hstream2.str().c_str());	
	    default:
	      break;
	    }
	  }
	}
	sdur+=dur.pdur;
	htmp.clear();
      }
      xercesc::DOMElement* element = (xercesc::DOMElement *)stmp[s];
      stringstream stream;
      stream << sdur;
      // insertion of Dur attribute on syllable level
      if (!BOSS::getAttrByName(element, Dur).size()) {
	BOSS::setAttribute(element, Dur, stream.str().c_str());
      }
      wdur+=sdur;
      sdur=0;
      ptmp.clear();
    }
    xercesc::DOMElement* element = (xercesc::DOMElement *)wlist[w];
    stringstream stream;
    stream << wdur;
    // insertion of Dur attribute on word level
    if (!BOSS::getAttrByName(element, Dur).size()) {
      BOSS::setAttribute(element, Dur, stream.str().c_str());
    }
    wdur=0;
    stmp.clear();	
  }
}

/**
 * @brief Runs the module.
 */
void BOSS_Duration::operator () (xercesc::DOMNode* sentence) {
  // Addition of features necessary for duration prediction
  set_features(sentence);
  // calculation of duration for every phone, syllable and word
  add_durations(sentence);
}

/** @} */ // end of boss_duration group
