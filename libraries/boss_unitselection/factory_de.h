/* factory_de.h    $Revision: 1.3 $ $Date: 2007/04/16 09:43:33 $
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
 * @file factory_de.h
   @author Michal Holcik
*/

#ifndef FACTORY_DE_H
#define FACTORY_DE_H

#include <string>
#include <vector>
#include "xercesc/dom/DOM.hpp"
#include "tdsp/dsp_vector.h"
#include "boss_utility/boss_config.h"
#include "boss_node.h"
#include "boss_node_de.h"
#include "cost.h"
#include "cost_de.h"
#include "factory.h"
#include <exception>
#include <iostream>

using namespace std;

/** @addtogroup boss_unitselection
 * @{
 */


/**
 * @author Michal Holcik
 * @brief Factory implementation for generation of german cost and node classes.
 */
class UnitSelectionFactoryDE: public UnitSelectionFactory {
	
	/// @brief Returns a CostDE instance.
	/// @return CostDE instance
	Cost * createCost(BOSS::Config & cl) {
		return new CostDE(cl);
	}
	
	/// @brief Returns a BOSS_NodeDE instance.
	/// @return BOSS_NodeDE instance
	BOSS_Node * createNode(BOSS::Config & cl) {
		return new BOSS_NodeDE(cl);
	}
	
	/// @brief Returns a BOSS_NodeDE instance.
	/// @return BOSS_NodeDE instance
	BOSS_Node * createNode(BOSS_Node & node) {
		try {
			BOSS_NodeDE & nodeDE = dynamic_cast<BOSS_NodeDE&>(node);
			return new BOSS_NodeDE(nodeDE);
		} catch (bad_alloc & ex) {
			std::cerr << "Bad cast exception: BOSS_NodeDE nodeDE = dynamic_cast<BOSS_NodeDE>(node);" << std::endl;
			std::cerr << "UnitSelectionFactoryDE::createNode(const BOSS_Node & node): got non-BOSS_NodeDE as parameter!" << std::endl;
			std::cerr << "UnitSelectionFactoryDE::createNode(const BOSS_Node & node): returning NULL pointer!" << std::endl;
			return NULL;
		}
	}
	
	/// @brief Returns a BOSS_NodeDE instance.
	/// @return BOSS_NodeDE instance
	BOSS_Node * createNode(BOSS::Config & cl,
			const BOSS_Node::Node_Type & t) {
		return new BOSS_NodeDE(cl, t);
	}
	/// @brief Returns a BOSS_NodeDE instance.
	/// @return BOSS_NodeDE instance
	BOSS_Node * createNode(BOSS::Config &cl,const xercesc::DOMNode *node,
			const BOSS_Node::Node_Type & t, const unsigned & p) {
		return new BOSS_NodeDE(cl, node, t, p);
	}
};


/**
 * @brief Creates a new instance of UnitSelectionFactoryDE class.
 */
extern "C" UnitSelectionFactory * create_fac() {
	return new UnitSelectionFactoryDE();
}


/**
 * @brief Destroys the instance of UnitSelectionFactoryDE class.
 */
extern "C" void destroy_fac(UnitSelectionFactory * cm) {
	delete cm;
}


/** @} */ //end of boss_unitselection group
#endif

