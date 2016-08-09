/* factory.h    $Revision: 1.6 $ $Date: 2007/04/16 09:43:33 $
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
 * @file factory.h
   @author Michal Holcik
*/

#ifndef FACTORY_H
#define FACTORY_H

#include <string>
#include <vector>
#include "xercesc/dom/DOM.hpp"
#include "tdsp/dsp_vector.h"
#include "boss_utility/boss_config.h"
#include "boss_node.h"
#include "cost.h"
#include "ltdl.h"

using namespace std;

/** @addtogroup boss_unitselection
 * @{
 */


/**
 * @author Michal Holcik
 * @brief Base class for generation of language dependent components used by the non-uniform unit selection algorithm.
 * First an instance of unit selection factory for one language is created. This factory can be used to create instances
   of the desired types.
 */
class UnitSelectionFactory {
public: 
	static UnitSelectionFactory * getFactory(const string lang);
	virtual Cost * createCost(BOSS::Config & __cl) = 0;
	virtual BOSS_Node * createNode(BOSS::Config & cl) = 0;
	virtual BOSS_Node * createNode(BOSS_Node & b) = 0;
	virtual BOSS_Node * createNode(BOSS::Config & cl,
			const BOSS_Node::Node_Type & t) = 0;
	virtual BOSS_Node * createNode(BOSS::Config &cl,
			const xercesc::DOMNode *node,
			const BOSS_Node::Node_Type & t,
			const unsigned & p) = 0;
};



typedef UnitSelectionFactory * create_factory();
typedef void destroy_factory(UnitSelectionFactory *);

/** @} */ //end of boss_unitselection group
#endif


