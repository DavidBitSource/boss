/* boss_cartreader.h    $Revision: 1.9 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_cartreader.h
   @author Joerg Broeggelwirth
*/

#ifndef BOSS_CARTREADER_H
#define BOSS_CARTREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

using namespace std;

/** @addtogroup boss_duration
 * @{
 */

typedef map<string, string> FMap;

/// @brief A question node of a CART tree.
struct CART_node {
  string question;
  CART_node* up;
  CART_node* left;
  CART_node* right;
  CART_node() : question(""), up(NULL), left(NULL), right(NULL) {};
};


/**
 * @author Joerg Broeggelwirth
 */
class BOSS_CartReader{
 public:

  //BOSS_CartReader();
  BOSS_CartReader(string file);
  CART_node* Classify(FMap & features) const;
  ~BOSS_CartReader();

 private:

  CART_node* root;
  CART_node* LoadTree(ifstream & source);
  void DelTree(CART_node* cur);
  void PrintTree(CART_node* cur) const;
};

/** @} */ // end of boss_cartreader group

#endif // BOSS_CARTREADER_H
