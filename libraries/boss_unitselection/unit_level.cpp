/* unit_level.cpp    $Revision: 1.4 $ $Date: 2006/03/28 01:07:27 $
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
 * @file unit_level.cpp
 * @author Stefan Breuer  
 * @author Joerg Broeggelwirth
 * @author Michal Holcik
 * @author Karlheinz Stoeber
 */

using namespace std;

#include "unit_level.h"


/**
 * @brief Decides if the line is a comment.
 * @param s An input line
 * @return true if the input line is a comment, false otherwise
 */
bool UnitLevel::isComment(const string & s)
{
  unsigned i;
  for(i=0; i<s.size(); i++) {
    if(s[i] != ' ' && s[i] != '\t')
      break;
  }
  if(s[i] == '#')
    return true;
  return false;
}

/**
 * @brief Parses the file with preselection arguments and stores them in the ps PSA_Vector.
 * This function works ok if the input file doesn't exist
 */
bool UnitLevel::loadPreSelFile() {
  PSA_Vector a;
  ifstream in(file.c_str());
  // is some other checking necessary???
  // looks good for me... o,-
  if (!in) {
	  cerr << "Error: Definition input file cannot be read " << file.c_str() << endl;
	  return false;
  }
  string tmp;
  while(getline(in, tmp)) {
    if (tmp == "") {
      if (a.size()) {
	attrMatrix.push_back(a);
	a.clear();
      }
    }
    else {
      if(!isComment(tmp)) {
	stringstream ins(tmp.c_str());
	struct PreSelAttr at;
	ins >> at.column >> at.value;
	if (at.column != "" && at.value != "") a.push_back(at);
      }
    }
  }
  if (a.size()) attrMatrix.push_back(a);
  a.clear();
  in.close();
  if (attrMatrix.size()) {
    return true;
  }
  return false;
    
}

/**
 * Returns the total size of attrMatrix of this and lower unit levels.
 * @return sum of attribute matrices' sizes
 */

int UnitLevel::checkMatrixSize() {
	int size = attrMatrix.size();
	if (next != NULL) {
		size += next->checkMatrixSize();
	}
	return size;
}

bool UnitLevel::moreLevelsOfChoiceBelow() {
	if (next != NULL) {
		return next->levelOfChoice || next->moreLevelsOfChoiceBelow();
	}
	return false;
}
