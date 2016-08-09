/* boss_morphtree.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_morphtree.h
   @author Karlheinz Stoeber
*/

#ifndef BOSS_MORPHTREE_H
#define BOSS_MORPHTREE_H

#include <list>
#include "boss_transcription_de.h"

using namespace std;

/**
 * @addtogroup boss_transcription
 * @{
 */

struct c_MorphNode {
	t_SubStringEntry      inf;
	vector< c_MorphNode > nodes;
	c_MorphNode() {}
	c_MorphNode(const t_SubStringEntry & e) : inf(e) {}
};
typedef list< const c_MorphNode * > t_MorphNodeList;
typedef list< t_MorphNodeList >     t_MorphNodeDList;
inline bool operator < (const t_MorphNodeList & a, const t_MorphNodeList & b)
{ return a.size() < b.size(); }

inline ostream& operator << (ostream & out, const c_MorphNode & m) {
  return out << *m.inf.morph;
} 

/**
 * @brief documentation missing
 */
class c_MorphTree {

private:
	c_MorphNode root;
	vector<vector<c_MorphNode> > res;
	void append(c_MorphNode & node, const t_SubStringEntryVector & entries);
	void _shortpath(const c_MorphNode & node, t_MorphNodeList & nlist, t_MorphNodeDList & res);
	void _print_tree(const c_MorphNode & node, vector<c_MorphNode>& cur);
	
public:
	c_MorphTree(const t_SubStringEntryVector & entries);
	~c_MorphTree();
	void shortpath(t_SubStringEntryDList & reslist);
	void print_tree();
};

/** @} */ // end of boss_transcription group
#endif
