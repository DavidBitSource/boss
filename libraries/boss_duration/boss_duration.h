/* boss_duration.h    $Revision: 1.4 $ $Date: 2005/12/10 20:33:54 $
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

#ifndef BOSS_DURATION_H
#define BOSS_DURATION_H

#include "boss_utility/boss_module.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/dom_tools.h"
#include "boss_cartreader/boss_cartreader.h"
#include <sstream>
#include <math.h>


/**
 * @file boss_duration.h
 * @author Joerg Broeggelwirth
 */

/**
 * @defgroup boss_duration BOSS Duration Module
 * @{
 */

/// @brief Stores the duration relevant parameters.
struct Duration {
  unsigned pdur;
  unsigned hdur1;
  unsigned hdur2;
  Duration() : pdur(0), hdur1(0), hdur2(0) {};
};

/**
 * @author Joerg Broeggelwirth
 * @brief Base class for all duration predicting modules.
 */
class BOSS_Duration : public BOSS::Module {
 protected:
  BOSS_CartReader * tree;
//  BOSS::DOMNodeVector wlist;
//   BOSS::DOMNodeVector stmp;
//   BOSS::DOMNodeVector ptmp;
//   BOSS::DOMNodeVector htmp;
  virtual void set_features(xercesc::DOMNode* sentence) const=0;
  void add_durations(xercesc::DOMNode* sentence) const;
  virtual Duration find_dur(const xercesc::DOMElement* element,
			    BOSS::DOMNodeVector & wlist,
			    BOSS::DOMNodeVector & stmp,
			    BOSS::DOMNodeVector & ptmp,
			    BOSS::DOMNodeVector & htmp,
			    unsigned w, unsigned s, unsigned p) const=0;
//   { 
//     Duration dur; return dur; 
//   };
	
 public:
  BOSS_Duration(BOSS::Config & __cl, MySQL_Handler * __data_base);
  virtual ~BOSS_Duration();
  virtual void operator () (xercesc::DOMNode* sentence);
};


/** @} */ // end of boss_duration group
#endif


