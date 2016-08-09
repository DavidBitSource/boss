/* conman_unit.h    $Revision: 1.3 $ $Date: 2006/07/04 14:44:07 $
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
   @author Stefan Breuer
   @author Michal Holcik
*/

#ifndef BOSS_CONMAN_UNIT_H
#define BOSS_CONMAN_UNIT_H

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdio>

#include "config.h"
#include "boss_utility/boss_exceptions.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/dom_userdata.h"
#include "boss_utility/dom_tools.h"
#include <xercesc/dom/DOM.hpp>
#include "boss_mysql/boss_mysql_handler.h"
#include "tdsp/dsp_vector.h"



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


class ConmanUnit {
 public:
  BOSS::Config & config;
  MySQL_Handler * database;
  xercesc::DOMNode* node; // reference to the DOM_Node of the unit.
  DOM_UserData * user_data; // pointer to the user data structure of the selected unit

  string tkey;
  string file_name;
  long start_index;
  long end_index;
  bool pause;

  // Constructor for non-pause units
  ConmanUnit(BOSS::Config & __config,
	  MySQL_Handler * __database,
	  xercesc::DOMNode * n,
	  const string & t,	//tkey
	  const string & i);	//inventory
  // Constructor for pause units
  ConmanUnit(BOSS::Config & __config, MySQL_Handler * __database,
	  xercesc::DOMNode * n, const string & t); 
  ConmanUnit & operator=(const ConmanUnit & other);
 protected:
  string getSentenceFileName(const string & inventory);
  
};

#endif

