/* concat_unit.h    $Revision: 1.3 $ $Date: 2006/07/04 14:44:07 $
   Copyright (C) 2003 University of Bonn.
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
 * @file concat_unit.h
   @author Stefan Breuer
*/

/**
 * @addtogroup boss_concat
 * @{
 */

#ifndef BOSS_CM_UNIT_H
#define BOSS_CM_UNIT_H

#include "boss_utility/boss_exceptions.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/dom_userdata.h"
#include "boss_utility/dom_tools.h"
#include <xercesc/dom/DOM.hpp>
#include "boss_mysql/boss_mysql_handler.h"
#include "tdsp/dsp_vector.h"
#include "tdsp/dsp_weight_window.h"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "boss_conman/conman_unit.h"

#define BOSS_PAUSE_SYMBOL "$p"
#define DEFAULT_PAUSE 8000
#define DEFAULT_PAUSE_STRING "8000"
#define MAXIMAL_PAUSE 32000
#define LEFTRIGHTOL 500
#define SMOODIST 17 // the size of 1/4 of a hanning window for smoothing

//Used for debugging purposes
//#define NOTVERB
#define BREAKPOINT cerr << "Bis hier" << endl;
#ifndef NOTVERB
#define PRINT(X) cerr << (#X) << ": " << (X) << endl;
#endif
#ifdef NOTVERB
#define PRINT(X) ;
#endif

typedef dsp::vector< short > t_Signal;

class CM_Unit: public ConmanUnit {
 public:
  t_Signal signal; // the raw unmanipulated signal vector of the selected unit + overlap;
  unsigned first; // the first sample value of the selected unit in the ORIGINAL signal file
  unsigned last; // the last sample value of the selected unit in the ORIGINAL signal file
  unsigned start; // the starting point of the unit in the signal
  unsigned end; // the last point of the unit in the signal
  CM_Unit(BOSS::Config & config,
	  MySQL_Handler * db,
	  xercesc::DOMNode * n,
	  const string & t,
	  const string & p,
	  const string & i,
	  const string & s); // Constructor for non-pause units
  CM_Unit(BOSS::Config & config, MySQL_Handler * db,
	  xercesc::DOMNode * n, const string & t); // Constructor for pause units
  CM_Unit & operator=(const CM_Unit & other);
 protected:
  string getSentenceFileName(const string & inventory);
  void loadFromFile(const string & file_name,
		    unsigned loadbegin,
		    unsigned loadend,
		    const unsigned & leftOL,
		    const unsigned & rightOL);
};

/** @} */ // end of boss_concat group
#endif

