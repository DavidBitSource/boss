/* boss_node.cpp    $Revision: 1.3 $ $Date: 2006/12/15 00:19:00 $
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
 * @file boss_node.cpp
   @author Karlheinz Stoeber
   @author Stefan Breuer
*/

#include <sstream>
#include <cstdio>
#include "boss_node.h"

/** @addtogroup boss_unitselection
 * @{
 */

ostream & operator << (ostream & out, const BOSS_Node::Node_Type & t)
{
	switch(t) {
		case BOSS_Node::UNKNOWN:
			out << "UNKNOWN";
			break;
		case BOSS_Node::WORD:
			out << "WORD";
			break;
		case BOSS_Node::MORPHEME:
			out << "MORPHEME";
			break;
		case BOSS_Node::SYLLABLE:
			out << "SYLLABLE";
			break;
		case BOSS_Node::PHONE:
			out << "PHONE";
			break;
		case BOSS_Node::HALFPHONE:
			out << "HALFPHONE";
			break;
		case BOSS_Node::INITIAL:
			out << "INITIAL";
			break;
		case BOSS_Node::FINAL:
			out << "FINAL";
			break;
	}
	return out;
}

ostream & operator << (ostream & out, const BOSS_Node & n)
{
	out << n.type << " ";
	unsigned num_fields = n.user_data->getResult().col();
	for(unsigned i=0; i<num_fields; i++)
		out << n.user_data->getResult()(n.pos, i) << " ";
	return out;
}

ostream & operator << (ostream & out, const UNIT_Graph_Column & c)
{
	for(unsigned i=0; i<c.size(); i++)
		out << c[i] << " ";
	return out;
}

ostream & operator << (ostream & out, const UNIT_Graph & g)
{
	for(unsigned i=0; i<g.size(); i++)
		out << g[i] << endl;
	return out;
}

BOSS_Node::BOSS_Node(BOSS::Config & __cl) :
        cl(__cl),
	unit_node(NULL), user_data(0), type(UNKNOWN), pos(0), cost(-1.0), pred(0)
{}


BOSS_Node::BOSS_Node(BOSS::Config & __cl,const Node_Type & t) :
    cl(__cl), unit_node(NULL),
    user_data(0), type(t), pos(0), cost(-1.0), pred(0)
{
	if(t==BOSS_Node::INITIAL) {
		cost = 0.0;
	}
}


BOSS_Node::BOSS_Node(BOSS::Config & __cl, const xercesc::DOMNode * node, const Node_Type & t, const unsigned & p) :
	cl(__cl), unit_node(node),
	user_data((DOM_UserData*)BOSS::getUserData(node)),
	type(t), pos(p), cost(-1.0), pred(0)
{}

BOSS_Node::~BOSS_Node() {}

BOSS_Node & BOSS_Node::operator = (const BOSS_Node & b)
{
	unit_node = b.unit_node;
	user_data = b.user_data;
	type      = b.type;
	pos       = b.pos;
	cost      = b.cost;
	pred      = b.pred;
	return *this;
}

/** @} */ //end of boss_unitselection group

