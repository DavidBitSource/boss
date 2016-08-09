/* boss_context.cpp    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
   Copyright (C) 2002 University of Bonn.
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/**
   @author Stefan Breuer
*/

#ifndef BOSSCONTEXT_H
#define BOSSCONTEXT_H



#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

struct CCStruct {
  string left1;
  string right1;
  string left2;
  string right2;
};

class BOSS_context {
 public:
  BOSS_context(string & file);
  string left1(const string & s) {
    if((cmi = cmap.find(s)) == cmap.end()) return "";
    else return (*cmi).second.left1;
  }
  string right1(const string & s) {
    if((cmi = cmap.find(s)) == cmap.end()) return "";
    else return (*cmi).second.right1;
  }
  string left2(const string & s) {
    if((cmi = cmap.find(s)) == cmap.end()) return "";
    else return (*cmi).second.left2;
  }
  string right2(const string & s) {
    if((cmi = cmap.find(s)) == cmap.end()) return "";
    else return (*cmi).second.right2;
  }
 protected:
  map< string,CCStruct > cmap;
  map< string,CCStruct >::const_iterator cmi;
};

#endif


