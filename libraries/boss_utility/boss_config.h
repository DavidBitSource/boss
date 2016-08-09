/* boss_config.cpp    $Revision: 1.5 $ $Date: 2005/12/17 16:06:11 $
   Copyright (C) 1998 University of Bonn.
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



#ifndef __BOSSCONFIGH__
#define __BOSSCONFIGH__

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

#include "xercesc/dom/DOM.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"

#include "boss_utility/boss_types.h"
#include "boss_utility/boss_optitem.h"
#include "boss_utility/dom_tools.h"

using namespace std;



namespace BOSS
{
class Config
{
  typedef vector< string >      t_StringVector;
  typedef list< string >        t_StringList;
  typedef map< string, t_OptItem> t_OptMap;
  typedef pair< string, t_OptMap *> t_InvPair;
  typedef map< string, t_InvPair> t_InvMap;
  
 protected:
  
  /** The options which are not bound by an option string */
  t_SVector farg;
  
  /** The usage message which is printed if an error occurred */
  string         usage;
  string         defInv;
  string         currInv;
  string         currLang;
  t_InvMap       invMap;
  t_OptMap *     currOptMap;
  t_SVector invVec;

  void error(const string & s);

  bool fileExists(string & file);
  string getConfPath(t_StringList & h, string file);
  void readXMLConfig(const string & file);
  bool procInvPrefix(string & arg);
  
 public:

  Config() { ; }

  Config(Config & cl) { ; }

  Config(int argc, char ** argv, const string & s);

  void print(ostream & s) const;

  Config & operator = (const Config & C);

  t_OptItem operator () (string opt) const;

  string operator [] (const unsigned i) const;
  
  unsigned size() const { return farg.size(); }

  bool setInv( const string & inv);

  string getInvName() { return currInv; };
  
  bool setInvtoDef();

  string getDefInv() { return defInv; };
  
  string getLang() { return currLang; };
  
  t_SVector listInv() { return invVec; };

  t_SVector * listInvPtr() { cout << "BOSS::Config::listInvPtr: invVec.size() = " << invVec.size() << endl; return &invVec; };
};
}
#endif
