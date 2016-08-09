/* boss_duration_de.h    $Revision: 1.6 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_duration_de.h
   @author Joerg Broeggelwirth
*/

#ifndef BOSS_DURATION_DE_H
#define BOSS_DURATION_DE_H

#include "boss_duration.h"
#include <string>
#include <fstream>
#include <map>

using namespace std;

/** @addtogroup boss_duration
 * @{
 */

/**
 * @author Joerg Broeggelwirth
 * @brief German class for prediction of duration.
 */
class BOSS_Duration_DE : public BOSS_Duration {
 protected:
	 
  FMap getFeatures(const xercesc::DOMElement* element) const;
  void set_features(xercesc::DOMNode* sentence) const;
  Duration find_dur(const xercesc::DOMElement* element,
		    BOSS::DOMNodeVector & wlist,
		    BOSS::DOMNodeVector & stmp,
		    BOSS::DOMNodeVector & ptmp,
		    BOSS::DOMNodeVector & htmp,
		    unsigned w, unsigned s, unsigned p) const;
  //  Duration find_dur(const xercesc::DOMElement* element, unsigned w, unsigned s, unsigned p);
  float correct_dur(float mean, const unsigned p, const unsigned pnumber) const;

 public:
  ///@brief BOSS_Duration_DE constructor.
  BOSS_Duration_DE(BOSS::Config & c, MySQL_Handler * db) : BOSS_Duration(c, db) {}
//  virtual ~BOSS_Duration_DE();
  virtual string getDescription();
};

extern "C" BOSS::Module * create(BOSS::Config & _cl, MySQL_Handler * _db) {
  return new BOSS_Duration_DE(_cl, _db);
}

extern "C" void destroy(BOSS_Duration_DE * cm) {
  delete cm;
}

/** @} */ // end of boss_duration group
#endif


