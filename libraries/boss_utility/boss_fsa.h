/* boss_fsa.h    $Revision: 1.4 $ $Date: 2006/11/05 23:37:36 $
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
 * @file boss_fsa.h
   @author Karlheinz Stoeber
*/

#ifndef BOSS_FSA_H
#define BOSS_FSA_H

#include "boss_types.h"
#include "boss_exceptions.h"
#include <pcre.h>
#include <iterator>


namespace BOSS
{
/**
 * @brief Exception Handler. Used by BOSS::FSA.
 * @author Karlheinz Stoeber
 */
  struct NoRegexMatch {
	std::string name;
	std::string inf;
    NoRegexMatch(const std::string & N, const std::string & I, const bool f = false) :
      name(N), inf(I) { if(f) printError(false); }
    void printError(const bool & fatal = false) const
    {
      if(fatal) {
        std::cerr << name << " : Fatal: Unknown phone symbol in: " << inf << std::endl; exit(-1);
      }
      else {
        std::cerr << name << " : Unknown phone symbol in: " << inf << ". Ignoring!" << std::endl;
      }
    }
  };
}

namespace BOSS
{
 
 /** @brief BOSS::FSA is a finite-state automaton that splits a string into substrings defined by regular expressions.
  * @author Karlheinz Stoeber
  *
  * This class is used to divide a string into substrings. It is also used as
  * a base class for classes with special adaptation to the format of the set
  * of regular expressions used by this class.
  * A substring is defined by a regular expression which is normally given in
  * a file. Each row of this file forms a regular expression.
  * The constructor reads such a file and creates the finite state automaton from
  * it. The parse function divides a string into its substrings by matching the
  * longest substrings in the given string from left to right.
  * This class uses the regex-library from POSIX-libc!
  *
  */
  class FSA {

  protected:
    pcre * preg;
    FSA() {}
    void compileExpression(const std::string & regexp) throw(BOSS::FileFormatError);

  public:
    /** \brief This constructor reads regular expressions from a file. */
    FSA(const std::string & filename) throw(BOSS::FileOpenError, BOSS::FileFormatError);

    /** \brief This constructor reads regular expressions from an input string vector (t_SVector). */
    FSA(const BOSS::t_SVector & expressions) throw(BOSS::FileFormatError);

    /**
     * \brief Destroy the FSA.
     */
    ~FSA();

    /** \brief Divide "trans" into substrings. */
    BOSS::t_SVector parse(const std::string & trans) const throw(BOSS::NoRegexMatch);
  };
}
#endif
