/* boss_optitem.h    $Revision: 1.3 $ $Date: 2007/04/16 09:43:33 $
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

//   @author Karlheinz Stoeber

#ifndef __BOSS_OPTITEMH__
#define __BOSS_OPTITEMH__


#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <string>

using namespace std;

namespace BOSS {
  
  typedef const char * t_ConstCharPtr;
  
  /** help class for command line parsing */
  class t_OptItem {
  public:
    /** name of the option */
    string option;
    
    /** argument of the option - empty string if no argument is needed by option */
    string optionarg;
    
    /** for use with the BOSS::CommandLine class, false means the option is necessary - true, not necessary
        for use with BOSS::Config, false means the option is of the boolean type, true
	        means the option is of type "-key value". */
    bool   optiontype;
  
    /** false means the option was not found in the command line - true, option was found */
    bool   optionflag;
  
    t_OptItem() : optionflag(false) { ; }
  
    t_OptItem(const string & o, const string & oa, const bool ot) :
      option(o), optionarg(oa), optiontype(ot), optionflag(false) { ; }
  
    t_OptItem(const string & o, const string & oa, const bool ot, const bool of) :
      option(o), optionarg(oa), optiontype(ot), optionflag(of) { ; }

    t_OptItem(const t_OptItem & O)
      {
	option     = O.option;     optionarg  = O.optionarg;
	optiontype = O.optiontype; optionflag = O.optionflag;
      }
  
    t_OptItem & operator = (const t_OptItem & O)
      {
	option     = O.option;     optionarg  = O.optionarg;
	optiontype = O.optiontype; optionflag = O.optionflag;
	return *this;
      }
  
    // Makes the Visual C++ Compiler happy
    bool operator < (const t_OptItem & O) const
      { return option < O.option; }
  
    // Makes the Visual C++ Compiler happy
    bool operator == (const t_OptItem & O) const
      {
	return option == O.option         && optionarg == O.optionarg    &&
	  optionflag == O.optionflag && optiontype == O.optiontype;
      }
  };
}
/*
ostream & operator << (ostream & s, const BOSS::t_OptItem & O)
{
  return s << O.option << " " << O.optionarg << " " << O.optionflag;
}
*/

#endif
