/* boss_node.h    $Revision: 1.5 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_node.h
   @author Karlheinz Stoeber
   @author Stefan Breuer
*/

#ifndef BOSS_NODE_H
#define BOSS_NODE_H

#include <string>
#include <vector>
#include "xercesc/dom/DOM.hpp"
#include "tdsp/dsp_vector.h"
#include "boss_utility/dom_userdata.h"
#include "boss_utility/boss_config.h"

using namespace std;

/** @addtogroup boss_unitselection
 * @{
 */


/**
 * @brief Language dependent graph node class for the unit selection graph path finding algorithm.
 */
class BOSS_Node {
  
  friend ostream & operator << (ostream & out, const BOSS_Node & n);

 public:
  enum Node_Type {
    UNKNOWN  = 0,
    WORD     = 1,
    MORPHEME = 2,
    SYLLABLE = 3,
    PHONE  = 4,
    HALFPHONE = 5,
    INITIAL  = 6,
    FINAL    = 7
  };
  
 protected:
  BOSS::Config & cl;
  const xercesc::DOMNode * unit_node;
  DOM_UserData * user_data;
  Node_Type type;
  unsigned pos;
  double cost;
  BOSS_Node *pred;
  
 public:
  BOSS_Node(BOSS::Config & cl);
  BOSS_Node(const BOSS_Node & b);
  BOSS_Node(BOSS::Config & cl, const Node_Type & t);
  BOSS_Node(BOSS::Config & cl, const xercesc::DOMNode * node, const Node_Type & t, const unsigned & p);
  virtual ~BOSS_Node();
  BOSS_Node & operator = (const BOSS_Node & b);
  virtual string getResult(const string & q) const { return user_data->getResult(pos, q); }
  virtual DOM_UserData * getUserData() const { return user_data; }
  virtual Node_Type getNodeType() const { return type; }
  virtual bool realNode() const
    {
      switch(type) {
      case INITIAL:
      case FINAL:
	return false;
	break;
      default:
	return true;
	break;
      }
      return true;
    }
  virtual bool virtualNode() const { return !realNode(); }
  virtual double getCost() const { return cost; }
  virtual void setCost(double c) { cost = c; }
  virtual void setPredecessor(BOSS_Node * predecessor) { pred = predecessor; }
  virtual BOSS_Node * getPredecessor() const { return pred; }
  virtual unsigned getPosition() const { return pos; }
  
};


/**
 * @todo TODO: no doc
 */
typedef vector< BOSS_Node* > UNIT_Graph_Column;
/**
 * @todo TODO: no doc
 */
typedef vector< UNIT_Graph_Column > UNIT_Graph;

ostream & operator << (ostream & out, const BOSS_Node::Node_Type & t);
ostream & operator << (ostream & out, const UNIT_Graph_Column & c);
ostream & operator << (ostream & out, const UNIT_Graph & g);

/** @} */ //end of boss_unitselection group

#endif

