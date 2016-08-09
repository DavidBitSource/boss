/* boss_concat.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_concat.h
 * @author Stefan Breuer
 */

#ifndef BOSS_CONCAT_H
#define BOSS_CONCAT_H

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "boss_mysql/boss_mysql_handler.h"
#include "boss_utility/dom_userdata.h"
#include "boss_utility/dom_tools.h"
#include "boss_utility/boss_exceptions.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/boss_config.h"
#include "tdsp/dsp_weight_window.h"
#include "concat_unit.h"
#include "boss_utility/boss_module.h"
#include "boss_conman/boss_conman.h"

/**
 * @defgroup boss_concat BOSS Concat Module
 * @{
 */

#define F0_DIFFERENCE_LIMIT 10.0
#define UPPER_F0_DIFFERENCE_LIMIT 50.0

typedef vector< CM_Unit > CU_Vector;

/**
 * @brief A class for the concatenation and manipulation of speech units.
 */
class BOSS_Concat : public BOSS_ConMan {
  bool sflag;
  map<string,string> phones;
  map<string,string>::const_iterator pmi;
  void getCM_Units(xercesc::DOMNode * node,CU_Vector & cv);
  void smoothing(CM_Unit & ul, CM_Unit & ur, t_Signal & res, t_Signal add);
  void preEmph(t_Signal & res);
 public:

  BOSS_Concat(BOSS::Config & __cl, MySQL_Handler * __data_base);
  void operator() (xercesc::DOMNode * sentence);
  virtual string getDescription();
};

extern "C" BOSS::Module * create(BOSS::Config & config, MySQL_Handler * database) {
	return new BOSS_Concat(config, database);
}

extern "C" void destroy(BOSS_Concat * cm) {
	delete cm;
}

/** @} */ // end of boss_concat group
#endif

