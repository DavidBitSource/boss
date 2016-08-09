/* boss_utility.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_utility.h
   @author Karlheinz Stoeber
*/

#ifndef __BOSS_UTILITYH__
#define __BOSS_UTILITYH__


#include "boss_types.h"
#include "boss_exceptions.h"
#include <iterator>
#include <iostream>

using namespace std;

namespace BOSS
{

  /** \brief This function reads an s1 label file (obsolete) and returns it as a pair vector.*/
  t_USPairVector readS1LabelTable(const string & name)
  throw(BOSS::FileOpenError, BOSS::FileFormatError);

  /** \brief This function reads tables with two columns and returns them as a string pair vector. */
  t_SSPairVector readTwoStringTable(const string & name)
  throw(BOSS::FileOpenError, BOSS::FileFormatError);

  /** \brief This function adds a slash to the string s and returns the modified string. */
  string addSlash(const string & s);
  
  /** \brief This function substitutes the <INVENTORY> tag with the string given by inv */
  string makeInvFileName(const string & pref,  const string & inv, const string & file)
    throw(BOSS::FileFormatError);
  
  /** \brief This function concatenates a file path with a file name */
  string makeFilePath(const string & pref, const string & file)
    throw(BOSS::FileFormatError);
  
  /** \brief This function removes a punctuation symbol (.,;:!?)
   * from the string given as argument. */
  t_CSPair removePunctuation(const string & word);
  
  /** \brief This function maps upper-case characters of string s to lower case ones. */
  string my_tolower(string s);
  
  /** This function removes a punctuation symbol (.,;:!?) from the string given
   * as argument. The return value is an STL pair. The first element contains the
   * puncuation symbol, the second the string without the symbol.
   */
  t_CSPair cleanWordPunctuation(const string & w);
  
  BOSS::Fileinfo getFileinfo(const string & s);
  
  string getDate();
}


#endif
