/* boss_unitselection.cpp    $Revision: 1.6 $ $Date: 2006/03/28 01:07:27 $
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
 * @file boss_unitselection.cpp
 * @author Karlheinz Stoeber
 * @author Joerg Broeggelwirth
 * @author Stefan Breuer
 */

#include "sstream"
#include "boss_utility/dom_tools.h"
#include "boss_utility/dom_userdata.h"
#include "boss_utility/boss_utility.h"
#include "boss_unitselection.h"

/** @addtogroup boss_unitselection
 * @{
 */

/**
 * @brief BOSS_Unitselection constructor.
 * @param _cl system wide configuration class
 * @param _db database handler
 */

BOSS_Unitselection::BOSS_Unitselection(BOSS::Config & _cl, MySQL_Handler * _db): BOSS::Module(_cl, _db) {
  cerr << "Construction of class UnitSelection ..."  << endl;
  factory = UnitSelectionFactory::getFactory(_cl("-cf").optionarg);
  unit_cost = factory->createCost(_cl);
  // this is database dependent and will mess everything up if changed!
  // in fact it is dependent on the preselection algorithm which is the real database dependent thing here
  // the preselection algorithm can be changed tho (with more or less success)
  halfphone = new UnitLevel(_cl, "halfphone", "HALFPHONE", DOM_UserData::HALFPHONE, "-hpsf", NULL);
  phone = new UnitLevel(_cl, "phone", "PHONE", DOM_UserData::PHONE, "-ppsf", halfphone);
  syllable = new UnitLevel(_cl, "syllable", "SYLLABLE", DOM_UserData::SYLLABLE, "-spsf", phone);
  word = new UnitLevel(_cl, "word", "WORD", DOM_UserData::WORD, "-wpsf", syllable);
  entry = word;

  cerr << "... OK" << endl;
}


/**
 * @brief Returns a description string of this module.
 * @return description string of BOSS_Unitselection module
 */

string BOSS_Unitselection::getDescription() {
  return "BOSS_Unitselection: This module performs the unit selection for a given sentence.";
}

/**
 * @brief BOSS_Unitselection destructor.
 */

BOSS_Unitselection::~BOSS_Unitselection() {
  delete halfphone;
  delete phone;
  delete syllable;
  delete word;
  delete unit_cost;
  delete factory;
}


/**
 * @brief This function prints the NodeName and DOM_Userdata of a DOM node.
 * DOM_Userdata
 * is only printed if the node is an ELEMENT_NODE, the user_data pointer not equal to
 * zero and the field position in BOSS_Userdata not equal to DOM_UserData::npos.
 * These conditions cause only those nodes to be printed which are selected by
 * the unit selection algorithm.
 * Note that this function moves through the tree recursively.
 * @param node a DOMNode
 */
void printUserData(const xercesc::DOMNode * node)
{
  if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
    DOM_UserData * user_data;
    if((user_data = (DOM_UserData*)BOSS::getUserData(node)) != 0) {
      if(user_data->getPos() != DOM_UserData::npos) {
	cerr << node->getNodeName() << " ";
	user_data->printSelectedRow(cerr);
	cerr << endl;
      }
    }
    else { // recursion
      xercesc::DOMNodeList* nl = node->getChildNodes();
      for(unsigned i=0; i<nl->getLength(); i++) {
	xercesc::DOMNode* tmp = nl->item(i);
	if(tmp->getNodeType() == xercesc::DOMNode::ELEMENT_NODE)
	  printUserData(tmp);
      }
    }
  }
}

/**
 * @brief Builds a graph using BOSS_Nodes.
 * The UNIT_Graph is the main structure for computing the shortest path.
 * To avoid storing edges directly in the graph nodes, a table-like structure
 * is chosen for representing the graph. Each column of this table contains the
 * different tokens which are given by the database query.
 * At the beginning, the results of the database queries are contained only in the
 * DOM_UserData class. The DOM_UserData class contains all possible tokens for
 * the relevant unit in a structure called Result. Now, BOSS_Nodes are constructed.
 * Each node contains a node type, a pointer to the corresponding DOM_UserData class,
 * a pointer to the corresponding Row class and a position which gives the position
 * of the row in the Result class.
 */
void BOSS_Unitselection::createUnitGraph(const xercesc::DOMNode * node, UNIT_Graph & unit_graph)
{
  if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
    DOM_UserData * user_data;
    if((user_data = (DOM_UserData*)BOSS::getUserData(node)) != 0) {
      UNIT_Graph_Column node_column;
      BOSS_Node::Node_Type node_type;
      if (BOSS::getAttrByName(node, "TKey") == "$p") {
	node_type = BOSS_Node::WORD;
	node_column.push_back(factory->createNode(cl, node, node_type, 0));
      }
      else {
	if(BOSS::compareNodeName(node, "WORD")) {
	  node_type = BOSS_Node::WORD;
	} else {
	  if(BOSS::compareNodeName(node,"SYLLABLE")) {
	    node_type = BOSS_Node::SYLLABLE;
	  } else {
	    if(BOSS::compareNodeName(node,"PHONE")) {
	      node_type = BOSS_Node::PHONE;
	    } else {
	      if(BOSS::compareNodeName(node,"HALFPHONE")) {
		node_type = BOSS_Node::HALFPHONE;
	      } else {
		cerr << "Unknown node type " << node->getNodeName() << endl;
		exit(EXIT_FAILURE); // TODO: throw exception
	      }
	    }
	  }
	}
	if (cl("-v").optionflag)
	  cerr << "createUnitGraph: user_Data: "
	       << endl
	       << ((DOM_UserData*)BOSS::getUserData(node))->getResult()
	       << endl;
	for (unsigned i=0; i<user_data->getResult().row(); i++) {
	  //cerr << "node: " << (DOM_Node&) node << endl;
	  node_column.push_back(factory->createNode(cl, node, node_type, i));
	}
      }
      unit_graph.push_back(node_column);
    }
    else {
      xercesc::DOMNodeList* nl = node->getChildNodes();
      for(unsigned i=0; i<nl->getLength(); i++) {
	xercesc::DOMNode* tmp = nl->item(i);
	if(tmp->getNodeType() == xercesc::DOMNode::ELEMENT_NODE)
	  createUnitGraph(tmp, unit_graph);
      }
    }
  }
}

/**
 * @brief Computes shortest path in a graph.
 * The shortpath function moves through the graph given by the UNIT_Graph class.
 * It will enumerate all node pairs of subsequent columns and call the cost function.
 * The cost function is given by the class Cost. A cost function takes one or two
 * nodes and returns a value which is as low as the relevant unit matching the desired
 * unit.
 * First, the lowest value between the relevant node and all preceding nodes is located.
 * We call this cost type transition cost and a pointer is inserted in the BOSS_Node class
 * which marks the preceding node which leads to the lowest cost value. That costs which
 * depend only on the relevant unit are summed up to the cost value of the relevant unit.
 * This cost type is called unit cost. After calculating the costs, a path is given by
 * the pred pointers contained in the BOSS_Node classes.
 * We move along this path and set the pos value in the DOM_UserData classes to that row
 * which contains the selected unit.
 * @param unit_graph a graph in which the shortest path is computed
 */
void BOSS_Unitselection::shortpath(UNIT_Graph & unit_graph)
{
  //	cerr << "Running short path: " << time(0) << endl;
  float c;
  for(unsigned i=1; i<unit_graph.size(); i++) {
    //		cerr << "Running for unit " << i << " with " << unit_graph[i-1].size() << " to " <<	unit_graph[i].size() << " connections" << endl;
    // Transition cost
    for(unsigned k=0; k<unit_graph[i].size(); k++) {
      for(unsigned j=0; j<unit_graph[i-1].size(); j++) {
	c = unit_graph[i-1][j]->getCost() + (*unit_cost)(unit_graph[i-1][j], unit_graph[i][k]);
	if(c < unit_graph[i][k]->getCost() || unit_graph[i][k]->getCost() < 0) {
	  unit_graph[i][k]->setCost(c);
	  unit_graph[i][k]->setPredecessor(unit_graph[i-1][j]);
	}
      }
      // unit cost
      float tmpCost = unit_graph[i][k]->getCost() + (*unit_cost)(unit_graph[i][k]);
      unit_graph[i][k]->setCost(tmpCost);
    }
  }

  // Path tracking
  BOSS_Node * p = unit_graph[unit_graph.size()-1][0];
  while(p!=0) {
    DOM_UserData * userData = p->getUserData();
    if(userData) {
      userData->setPos(p->getPosition());
    }
    p = p->getPredecessor();
  }
  //	cerr << "Short path finished: " << time(0) << endl;
}


/**
 * @brief Marks units unsuitable for further processing.
 * Units with high costs are rejected in the preselection functions.
 * @param node a dom node
 * @param query_result a query result
 * @param pos is the row number of the database query result containing the chosen unit. See also dom_userdata.h
 * @return true iff the unit is unsuitable for further processing; false otherwise
 */

bool BOSS_Unitselection::rejectUnit(xercesc::DOMNode* node, const MySQL_Result & query_result, const unsigned &pos) const {
  bool flag(false);
  static const unsigned threshold(11);
  BOSS_Node::Node_Type b_node_type;
  DOM_UserData::Node_Type d_node_type;
  DOM_UserData *tmp(NULL);
  // definition of the current node type
  if (BOSS::compareNodeName(node,"WORD")) {
    b_node_type=BOSS_Node::WORD;
    d_node_type=DOM_UserData::WORD;
  }
  else if (BOSS::compareNodeName(node, "SYLLABLE")) {
    b_node_type=BOSS_Node::SYLLABLE;
    d_node_type=DOM_UserData::SYLLABLE;
  }
  else if (BOSS::compareNodeName(node, "PHONE")) {
    b_node_type=BOSS_Node::PHONE;
    d_node_type=DOM_UserData::PHONE;
  }
  
  // temporary definition of the node's user_data
  tmp = new DOM_UserData(query_result, d_node_type);
  BOSS::setUserData(node, tmp);
  // rejection decision on the basis of the unit's cost (threshold: 11)
  BOSS_Node * tempNode = factory->createNode(cl, node, b_node_type, pos);
  double cost = (*unit_cost)(tempNode);
  cerr << "rejection function: cost: " << cost << endl;
  if (cost>threshold) {
    flag=true;
  }
  // deletion of the temporary user_data
  delete tmp;
  delete tempNode;
  BOSS::setUserData(node, NULL);
  return flag;
}

/**
 * @brief The preselection selects candidate words, syllables, phones or halfphones from the database.
 * @param level defines the current unit level
 * @param unit a DOM node
 * @param query_result a result of preselection query
 */

void BOSS_Unitselection::PreSelection(UnitLevel * level,
				      const xercesc::DOMNode* unit,
				      MySQL_Result & query_result) const {
  const vector< PSA_Vector > ps = level->getAttrMatrix();
  string column, value;
  string inventory = cl.getInvName();
  cerr << "ps.size(): " << ps.size() << endl;
  cerr << "UNIT LEVEL: " << level->getTag() << endl;
  for (unsigned q = 0; q < ps.size(); q++) {
    stringstream where;
    where << " WHERE ";
    for (unsigned a=0;a<ps[q].size();a++) {
      if (a>0) where << " AND ";
      column = ps[q][a].column;
      value = ps[q][a].value;
      if (column == value) {
	where << column << "=\"" << BOSS::getAttrByName(unit, value.c_str()) << "\"";
      }
      else {
 	where << column << "=\"" << value << "\"";
      }
    }
    stringstream query;
    query << "SELECT * FROM "
	  << inventory << "_" << level->getName() << "_data"
	  << where.str() << ends;
    cerr << query.str() << endl;
    //    if (cl("-v").optionflag) cerr << query.str() << endl;
    query_result=data_base->query(query.str().c_str());
    if(query_result.row() > 0) {
      cerr << "Found " << query_result.row() << " units " << endl;
      break;
    }
  }
}


/**
 * @brief Performs unit preselection for current unit level.
 * This function performs the unit selection for a given sentence. It works in a recursive
 * way using the UnitLevel class . Lets assume for the sake of simplicity of explanation that
 * we have started at the very top sentence level. The function first queries all
 * child nodes from the sentence. As defined in our DOM structure, it must be the word
 * nodes or the text node of the sentence. Each node with node type ELEMENT_NODE must be
 * a word node. Now, we try to find a set of units in the database which has the same
 * canonical transcription as the desired word. If we find units in the database, we
 * store them in the DOM_UserData class. Otherwise, we switch to syllable selection.
 */
void BOSS_Unitselection::unitSelect(xercesc::DOMNode * subtree, UnitLevel * level)
{
  if (level == NULL) return;
  xercesc::DOMNodeList* nl = subtree->getChildNodes();
  string levelTag(level->getTag());
  for(unsigned i=0; i<nl->getLength(); i++) {
	MySQL_Result queryResult;
	DOM_UserData * userData;
	xercesc::DOMNode * childNode = nl->item(i);

	// this function is a mess
	if (childNode->getNodeType() != xercesc::DOMNode::ELEMENT_NODE) {
		// there is nothing to process
		cerr << "is not of element type, skipping" << endl;
		continue;
	}

	if (BOSS::getAttrByName(childNode, "TKey")=="$p") {
		userData = new DOM_UserData(queryResult, level->getNodeType());
		BOSS::setUserData(childNode, userData);
		cerr << "found a pause unit" << endl;
		continue;
	}

	if (BOSS::compareNodeName(childNode, levelTag)
	   && level->getLevelOfChoice()) {
		PreSelection(level, childNode, queryResult); //generic unit object
		if (queryResult.row() > 0) {
			// we found some suitable units
			cerr << "found suitable candidate(s)" << endl;
			userData = new DOM_UserData(queryResult, level->getNodeType());
			BOSS::setUserData(childNode, userData);
			continue;
		} else {
			cerr << "must crash here" << endl;
		}
	}
        if (level->moreLevelsOfChoiceBelow()) {
		// if there are other levels
		// recursively go through all children of this node
		  UnitLevel * lowerLevel = level->getNextLevel();
		  if (lowerLevel != NULL) {
		    cerr << "going down" << endl;
		    unitSelect(childNode, lowerLevel); // dive deeper
		  }
	} else {
	    // if we are on the last unit level and there are no units in the database
	    // insert a pause unit instead
	    cerr << "No units found for " << BOSS::getAttrByName(childNode, "TKey")
		 << "!" << endl << "Inserting a pause instead!" << endl;
	    BOSS::setAttribute(childNode, "TKey", "$p");
	}
   }
}

/**
 * This function performs the unit selection for a given sentence.
 *
 * First, it runs the preselection phase. It queries all nodes from the sentence xml
 * document. If we find suitable units in the database, we store them in the DOM_UserData
 * class of the relevant nodes.
 * After retrieving all necessary units, it is possible to construct a graph. This graph
 * has a table-like structure and contains BOSS_Nodes as nodes. We add two special nodes
 * to the graph to have unique start and end points and compute a path in this graph
 * by calling the shortpath function.
 */

void BOSS_Unitselection::operator() (xercesc::DOMNode* sentence)
{
  // do the recursive preselection for all unit levels
  unitSelect(sentence, entry);
  // graph structure is constructed from the DOM.
  UNIT_Graph unitGraph;
  UNIT_Graph_Column nodeColumn;
  nodeColumn.push_back(factory->createNode(cl, BOSS_Node::INITIAL));
  unitGraph.push_back(nodeColumn);
  createUnitGraph(sentence, unitGraph);
  nodeColumn.clear();
  nodeColumn.push_back(factory->createNode(cl, BOSS_Node::FINAL));
  unitGraph.push_back(nodeColumn);
  // compute the shortest path within the graph
  shortpath(unitGraph);

  // delete graph nodes // TODO: get it somewhere else
  for(int i = 0; i < unitGraph.size(); i++ ) {
    for( int j = 0; j < unitGraph[i].size(); j++ ) {
      delete unitGraph[i][j];
    }
  }

  // Ok, ready now. The selected units are marked in the DOM
  // Each selected unit satisfies the condition:
  // dom_node.getUserData() != 0 && dom_node.getUserData()->pos != DOM_UserData::npos
  // The value retrieved by dom_node.getUserData()->pos gives the position of the row
  // containing the data from the data base query.}
}

/** @} */ //end of boss_unitselection group

