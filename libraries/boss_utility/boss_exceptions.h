/* boss_exceptions.h    $Revision: 1.5 $ $Date: 2007/04/16 09:43:33 $
   Copyright (C) 1999 University of Bonn.
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
 * @file exceptions.h
   @author Karlheinz Stoeber
   @author Michal Holcik
   @brief Set of general exceptions.
*/

#ifndef __BOSS_EXCEPTIONSH__
#define __BOSS_EXCEPTIONSH__


#include <string>
#include <stdexcept>
#include <iostream>
#include <stdlib.h>

using namespace std;

#define ERROR_ON false

namespace BOSS{

/**
 * @brief General critical module problem.
 */
/*class ModuleException : public logic_error {
  public:
    ModuleException(string message): logic_error(message) {}
  };

*/
/**
 * @brief Is thrown when there is no object available for the specified language
 */
/*class UnknownLanguageException : public logic_error {
    string reason;
  public:
    UnknownLanguageType(string lang) {
      reason = "Cannot create language dependent component of type \"" + lang + "\"";
    }
    const char *what() const { 
      return reason.c_str(); 
    }
  };
*/


struct FileOpenError {
  string name;
  string inf;
  FileOpenError(const string & N, const string & I, const bool f = ERROR_ON) : 
    name(N), inf(I) { if(f) printError(false); }
  void printError(const bool & fatal = false) const
    {
      if(fatal) {
	cerr << name << " : Fatal: Can't open file: " << inf << endl; exit(-1);
      }
      else {
	cerr << name << " : Can't open file: " << inf << ". Ignoring file!" << endl;
      }
    }
};

struct FileFormatError {
  string   name;
  string   inf;
  unsigned line;
  FileFormatError(const string & N, const string & I, const unsigned & L, const bool f = ERROR_ON) :
    name(N), inf(I), line(L) { if(f) printError(false); }
  void printError(const bool & fatal = false) const
    {
      if(fatal) {
	cerr << name << " : Fatal: Format error in file: " << inf << " at line " << line << endl; exit(-1);
      }
      else {
	cerr << name << " : Format error in file: " << inf << " at line " << line << ". Ignoring rest of file!" << endl;
      }
    }
};

struct StringFormatError {
  string name;
  string inf;
  StringFormatError(const string & N, const string & I, const bool f = ERROR_ON) : 
    name(N), inf(I) { if(f) printError(false); }
  void printError(const bool & fatal = false) const
    {
      if(fatal) {
	cerr << name << " : Fatal: Wrong string format: " << inf << endl; exit(-1);
      }
      else {
	cerr << name << " : Wrong string format: " << inf << ". No modifications made!" << endl;
      }
    }
};

struct ItemNotFound {
  string name;
  string inf;
  ItemNotFound(const string & N, const string & I, const bool f = ERROR_ON) : name(N), inf(I)
    { if(f) printError(false); }
  void printError(const bool & fatal = false) const
    {
      if(fatal) {
        cerr << name << " : Fatal: Item: " << inf << " not present" << endl; exit(-1);
      }
      else {
        cerr << name << " : Item: " << inf << " not present. Ignoring item!" << endl;
      }
    }
};
 
struct KeyNotFound {
  string name;
  unsigned inf;
  KeyNotFound(const string & N, const unsigned & I, const bool f = ERROR_ON) : name(N), inf(I)
    { if(f) printError(false); }
  void printError(const bool & fatal = false) const
    {
      if(fatal) {
        cerr << name << " : Fatal: Key: " << inf << " not present" << endl; exit(-1);
      }
      else {
        cerr << name << " : Key: " << inf << " not present. Ignoring key!" << endl;
      }
    }
};
}

#undef ERROR_ON

#endif

  
