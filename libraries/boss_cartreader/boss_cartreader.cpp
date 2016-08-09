/* boss_cartreader.cpp    $Revision: 1.8 $ $Date: 2007/06/22 13:03:01 $
   Copyright (C) 2005 University of Bonn.
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
 * @file boss_cartreader.cpp
   @author Joerg Broeggelwirth
*/

#include "boss_cartreader.h"

/** @addtogroup boss_duration
 * @{
 */

/// @brief BOSS_CartReader constructor.
/// @param file input file
BOSS_CartReader::BOSS_CartReader(string file)
{
  ifstream source(file.c_str());
  root=LoadTree(source);
  source.close();
}

/// @brief BOSS_CartReader destructor.
BOSS_CartReader::~BOSS_CartReader()
{
  DelTree(root);
}

/**
 * @brief Loads cart reader tree.
 */
CART_node* BOSS_CartReader::LoadTree(ifstream & source) {
  CART_node *start(NULL), *cur(NULL), *pre(NULL);
  string entry("");
  char c(0);

  while (source.get(c)&&c!=';') {
    switch (c) {
      // In case of '(', a new node is created
    case '(' :
      // Set start pointer to the first node
      if (start==NULL) pre=cur=start=new CART_node;
      else {
	pre=cur;
	if (cur->left==NULL) {
	  cur->left=new CART_node;
	  cur=cur->left;
	}
	else {
	  cur->right=new CART_node;
	  cur=cur->right;
	}
      }
      // Read all characters up to ')' and assign the resulting string to "question"
      while (source.get(c)&&c!=')') {
	if (c!='(') entry+=c;
      }
      cur->question=entry;
      // Set the up pointer of the current node to the previous node
      if (cur!=pre) {
	cur->up=pre;
	pre=cur;
      }
      entry="";
      break;
      // ')' causes an upward movement
    case ')' :
      cur=pre->up;
      pre=cur;
      break;
    }
  }
  return start;
}

/// @brief Deletion of the CART tree.
void BOSS_CartReader::DelTree(CART_node* cur) {
  if (cur!=NULL) {
    if (cur->left!=NULL) {
      cur=cur->left;
      DelTree(cur);
    }
    else if (cur->up!=NULL) {
      cur=cur->up;
      if (cur->left!=NULL) {
	delete cur->left;
	cur->left=NULL;
      }
      if ((cur->right->left!=NULL)||(cur->right->right!=NULL)) cur=cur->right;
      else {
	delete cur->right;
	cur->right=NULL;
      }
      DelTree(cur);
    }
    else delete cur;
  }
}

/**
 * @brief Full tree search with output of the nodes.
 */
void BOSS_CartReader::PrintTree(CART_node* cur) const {	
  if (cur!=NULL) {
    // At this point, all nodes can be accessed
    cout << cur->question << endl;
    if (cur->left!=NULL) {
      cur=cur->left;
      PrintTree(cur);
    }
    else {
      while ((cur->up!=NULL)&&(cur==cur->up->right)) cur=cur->up;
      if (cur->up!=NULL) {
	cur=cur->up->right;
	PrintTree(cur);
      }			
    }
  }
}

/**
 * @brief Classification: Classify expects a map of feature names
 * and associated values and returns the appropriate leaf of the tree. 
 * @return tree leaf
 */
CART_node* BOSS_CartReader::Classify(FMap & features) const {
  CART_node* cur(root);
  string feature(""), value(""), is("");
  
  while ((cur!=NULL)&&(cur->left!=NULL)&&(cur->right!=NULL)) {
    stringstream stream;
    stream << cur->question;
    stream >> feature >> is >> value;
    if (is == "<") {
      float rThresh, rActVal;
      stringstream strThresh(value), strActVal(features[feature]);
      strThresh >> rThresh; strActVal >> rActVal;
      if (rActVal < rThresh)   cur =  cur->left;
      else   cur =  cur->right;
    }
    else { // assuming is == "is"
      if (value==features[feature])   cur = cur->left;
      else   cur =  cur->right;
    }
  }
  return cur;
}

/** @} */ // end of boss_cartreader group

