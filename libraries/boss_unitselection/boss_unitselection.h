/* boss_unitselection.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_unitselection.h
 * @author Karlheinz Stoeber
 * @author Joerg Broeggelwirth
 * @author Stefan Breuer
 */


#ifndef BOSS_UNITSELECTION_H
#define BOSS_UNITSELECTION_H

#include <string>
#include "xercesc/dom/DOM.hpp"
#include "boss_utility/boss_module.h"
#include "boss_utility/boss_config.h"
#include "boss_mysql/boss_mysql_handler.h"
#include "unit_level.h"
#include "boss_node.h"
#include "cost.h"
#include "factory.h"

using namespace std;

/** @defgroup boss_unitselection BOSS UnitSelection Module
 * @{
 */

/**
 * @author Karlheinz Stoeber
 * @author Joerg Broeggelwirth
 * @author Stefan Breuer
 * @brief The main class of the unit selection module.
 * This class performs the graph pathfinding algoritm used for chosing the units for manipulation and concatenation.
 */
class BOSS_Unitselection : public BOSS::Module {

  UnitLevel * entry;
  UnitLevel * word;
  UnitLevel * syllable;
  UnitLevel * phone;
  UnitLevel * halfphone;
	
	UnitSelectionFactory * factory;
	Cost * unit_cost;

	string lang;

	// preselection functions
	bool rejectUnit (xercesc::DOMNode* node,
		const MySQL_Result & query_result, const unsigned &pos) const;
	void unitSelect(xercesc::DOMNode * sub_tree, UnitLevel * level);
	void PreSelection(UnitLevel * level,
		const xercesc::DOMNode* unit,
		MySQL_Result & query_result) const;

	// graph functions
	void createUnitGraph(const xercesc::DOMNode * node, UNIT_Graph & unit_graph);
	void shortpath(UNIT_Graph & unit_graph);


public:

        BOSS_Unitselection(BOSS::Config & _cl, MySQL_Handler * _db);
	~BOSS_Unitselection();
	void operator () (xercesc::DOMNode * sentence);
        virtual string getDescription();

};



/**
 * @brief Creates a new instance of BOSS_Unitselection class.
 */
extern "C" BOSS::Module * create(BOSS::Config & _cl, MySQL_Handler * _db) {
	return new BOSS_Unitselection(_cl, _db);
}

/**
 * @brief Destroys the instance of BOSS_Unitselection class.
 */
extern "C" void destroy(BOSS_Unitselection * cm) {
	delete cm;
}

/** @} */ //end of boss_unitselection group

#endif

