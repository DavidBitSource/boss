/* unit_level.h    $Revision: 1.5 $ $Date: 2007/04/16 09:43:33 $
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
 * @file unit_level.h
 * @author Michal Holcik
*/

#ifndef UNIT_LEVEL_H
#define UNIT_LEVEL_H

#include <string>
#include <vector>
#include "boss_utility/boss_config.h"
#include "boss_node.h"
#include "boss_utility/dom_userdata.h"
#include "boss_utility/boss_utility.h"

using namespace std;

/** @addtogroup boss_unitselection
 * @{
 */


/**
 * @brief A structure storing preselection attributes.
 */
struct PreSelAttr {
  /// the column The column
  string column; 
  /// the value A value in the column
  string value; 
};



/**
 * @brief A column of preselection attribute matrix.
 */
typedef vector< PreSelAttr > PSA_Vector;


/**
 * @author Michal Holcik
 * @brief Data structure holding attributes of one speech unit level (word, syllable, etc.)
 * @todo This class may need renaming, I'm not sure if the name sufficiently describes what it represents
 */
class UnitLevel {
  /// configuration class storing system wide settings
  BOSS::Config & cl; 
  // name of the preselection configuration file for this unit level
  string file;
  /// name of this unit level, this is btw used in the database table names and queries
    string name; 
    /// name of tag of this unit level in BOSS XML file
      string tag; 
      /// pointer to finer unit level e.g. from Word to Syllable or from Syllable to Phone
	UnitLevel * next; 
	/// matrix of PreSelAttr
	  vector< PSA_Vector > attrMatrix;
	  //BOSS_Node::Node_Type nodeType;
	  DOM_UserData::Node_Type nodeType;
	  bool isComment(const string & s);
	  bool loadPreSelFile();
  /// self-explanatory
  bool levelOfChoice;
 public:
	  /**
	 * @brief Constructs a UnitLevel.
	 * @param config system wide configuration container
	 * @param levelName level name
	 * @param elementTag name of the xml element containing data of this unit level
	 * @param fileFlag commandline option switch to set a name of a file containing preselection attributes
	 * @param nextLevel pointer to finer unit level
	 */
	    UnitLevel(BOSS::Config & config, string levelName, string elementTag,
		      DOM_UserData::Node_Type type,
		      string fileFlag, UnitLevel * nextLevel
		      ) : cl(config), name(levelName), tag(elementTag), nodeType(type), next(nextLevel)
	      {
		//cerr << "creating unit level " << endl;
  		BOSS::Fileinfo bflu(BOSS::getFileinfo(config(fileFlag).optionarg));
		levelOfChoice = false;
		
  		if (bflu.base != "" && bflu.exists) {
			file = cl(fileFlag).optionarg;
			if (loadPreSelFile()) {
    				cerr << "Using " << tag << " level for pre-selection." << endl;
				levelOfChoice = true;
			} else {
				cerr << "Warning: UnitLevel " << tag << " definition file is empty!" << endl;
			}
		}
	      }
	/**
	 * @brief Returns unit level name.
	 * @return level name
	 */
	string getName() { return name; }
	/**
	 * @brief Returns unit level tag.
	 * @return level name
	 */
	string getTag() { return tag; }
	/**
	 * @brief Returns pointer to lower unit level e.g. Word -> Syllable.
	 * @return pointer to finer unit level
	 */
	UnitLevel * getNextLevel() { return next; }
	/**
	 * @brief Returns node type.
	 * @return node type
	 */
	DOM_UserData::Node_Type getNodeType() { return nodeType; }
	/**
	 * @brief Returns attribute matrix
	 * @return attribute matrix
	 */
	vector< PSA_Vector > getAttrMatrix() { return attrMatrix; }
	int checkMatrixSize();
	bool getLevelOfChoice() {
		return levelOfChoice;
	}
	bool moreLevelsOfChoiceBelow();
};

/** @} */ //end of boss_unitselection group
#endif

