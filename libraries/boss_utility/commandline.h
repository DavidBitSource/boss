/* commandline.h    $Revision: 1.3 $ $Date: 2007/04/16 09:43:33 $
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

#ifndef __COMMANDLINEH__
#define __COMMANDLINEH__


#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <string>
#include "boss_utility/boss_optitem.h"

using namespace std;

typedef vector< string >                         t_StringVector;
typedef vector< BOSS::t_OptItem >                      t_OptItemVector;
typedef list< string >                           t_StringList;
typedef map< string, BOSS::t_OptItem, less< string > > t_OptItemMap;

namespace BOSS {
  /**
     \brief A class for parsing and storing command line options.
     
     The class CommandLine takes the command line arguments and parses them. Which
     options are recognized is defined in the t_OptItem vector v. This vector
     determines the behavior of this class.
     There are two choices to recall the options. Options without a flag are
     stored as an array and can be recalled with the operator []. Options with
     a flag are stored in a map. They are recalled by the operator ().
     
     @author Karlheinz Stoeber
  */
  class CommandLine
    {
      
    private:
      
      /** The options which are not bound by an option string */
      t_StringVector farg;
      
      /** The options which are bound by an option string */
      t_OptItemMap   barg;
      
      /** The usage message which is printed if an error occurred */
      string         usage;

      void error(const string & s);
      
    public:
      
      CommandLine() { ; }
      
      void print(ostream & s) const;
      
      CommandLine(int argc, char ** argv, const t_OptItemVector & v,
		  const string & s, const int fmin = -1, const int fmax = -1);
      
      CommandLine & operator = (const CommandLine & C);
      
      BOSS::t_OptItem operator () (const string & opt) const;
      
      string operator [] (const unsigned i) const;
      
      unsigned size() const { return farg.size(); }
    };
} 

#endif
