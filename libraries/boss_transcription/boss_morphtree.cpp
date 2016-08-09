/* boss_morphtree.cpp    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_morphtree.cpp
   @author Karlheinz Stoeber
*/

#include <algorithm>
#include "boss_morphtree.h"
	
/**
 * @addtogroup boss_transcription
 * @{
 */

c_MorphTree::c_MorphTree(const t_SubStringEntryVector & entries)
{
	append(root, entries);
}

c_MorphTree::~c_MorphTree(){
}

void c_MorphTree::append(c_MorphNode & node, const t_SubStringEntryVector & entries)
{ 
  for(unsigned i=0; i<entries.size()-1; i++) 
  {
    if(node.inf.right == entries[i].left) 
    {
      node.nodes.push_back(c_MorphNode(entries[i]));		
      append(node.nodes.back(), entries);
    }
  }
}

void c_MorphTree::_print_tree(const c_MorphNode & node, vector<c_MorphNode>& cur) {
  //if (node.inf.morph) cerr << *(node.inf.morph) << " ";
  if (node.inf.morph) cur.push_back(node);
  for (unsigned i=0; i<node.nodes.size(); i++) {
    _print_tree(node.nodes[i], cur);
    cur.pop_back();
  }

  if (!node.nodes.size()) {
    //for (unsigned j=0; j<cur.size(); j++) {
    //  cerr << cur[j] << " ";
    //}
    //cerr << endl;
  res.push_back(cur);
  }
}

void c_MorphTree::print_tree() {
  vector<c_MorphNode> cur;
  _print_tree(root, cur);
  for (unsigned i=0; i<res.size(); i++) {
    cerr << i+1 << ". ";
    copy(res[i].begin(), res[i].end(), ostream_iterator<c_MorphNode>(cerr, " "));
    cerr << endl;
  }
}

// ****************************************************************************
// Modified by PB.
// ****************************************************************************

void c_MorphTree::shortpath(t_SubStringEntryDList & reslist)
{
	t_MorphNodeList nlist;
	t_MorphNodeDList res;	
	_shortpath(root, nlist, res);
	res.sort();
	t_MorphNodeDList::iterator it0, it1;
	it0 = it1 = res.begin();

  if (res.size() > 0)       // Added by PB; otherwise runtime error when res is an empty list
  {
	  for(it1++; it1!=res.end(); it0++, it1++) 
    {
      if ((*it0).size() != (*it1).size()) { break; }
	  }
	  res.erase(it1, res.end());
  }

	t_MorphNodeList::const_iterator it;
	
  for(it0=res.begin(); it0!=res.end(); it0++) 
  {
		t_SubStringEntryList tmplist;
    for(it=(*it0).begin(); it!=(*it0).end(); it++) { tmplist.push_back((*it)->inf); }
		reslist.push_back(tmplist);
	}
}

// ****************************************************************************
// ****************************************************************************

void c_MorphTree::_shortpath(const c_MorphNode & node, t_MorphNodeList & nlist, t_MorphNodeDList & res)
{
	if(!node.nodes.size() && nlist.size())
		res.push_back(nlist);
		
	for(unsigned i=0; i<node.nodes.size(); i++) {
		nlist.push_back(&(node.nodes[i]));
		_shortpath(node.nodes[i], nlist, res);
		nlist.pop_back();
	}
}
/** @} */ // end of boss_transcription group
