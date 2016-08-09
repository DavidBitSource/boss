/* boss_node.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_node_de.h
   @author Karlheinz Stoeber
   @author Stefan Breuer
   @author Michal Holcik
*/

#ifndef BOSS_NODE_DE_H
#define BOSS_NODE_DE_H

#include <string>
#include "xercesc/dom/DOM.hpp"
#include "boss_utility/dom_userdata.h"
#include "boss_utility/boss_config.h"
#include "boss_node.h"

using namespace std;

/** @addtogroup boss_unitselection
 * @{
 */

/**
 * @brief German node class.
 */
class BOSS_NodeDE: public BOSS_Node {

protected:
  // Cost function speedup
  unsigned Num;
  string tkey;
  string treal;	 				
  string cleft;
  string cright;
  string pmode;
  int pint;
  unsigned first;
  unsigned last;
  unsigned stress;
  float lf0;
  float rf0;
  float avgf0;
  float rms;
  dsp::vector< float > lmel;
  dsp::vector< float > rmel;
	
 public:
  friend class CostDE;
  friend ostream & operator << (ostream & out, const BOSS_Node & n);
  BOSS_NodeDE(BOSS::Config & cl);
  BOSS_NodeDE(const BOSS_NodeDE & b);
  BOSS_NodeDE(BOSS::Config & cl, const Node_Type & t);
  BOSS_NodeDE(BOSS::Config & cl, const xercesc::DOMNode * node, const Node_Type & t, const unsigned & p);
  ~BOSS_NodeDE();
  BOSS_NodeDE & operator = (const BOSS_NodeDE & b);
};


/** @} */ //end of boss_unitselection group

#endif

