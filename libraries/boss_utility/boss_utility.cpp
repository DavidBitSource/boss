/* boss_utility.cpp    $Revision: 1.4 $ $Date: 2006/02/19 20:55:41 $
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
 * @file boss_utility.cpp
 * @author Karlheinz Stoeber
 */

#include "boss_utility.h"
#include <fstream>

/** 
 * @brief This function reads an s1 label file (obsolete).
 *
 * S1 label file contains a header ending with the word hend. Each label
 * consists of an unsigned value and a string. The unsigned value gives the
 * start position of the label in samples.
 * The return value of this function is an STL vector containing the labels
 * as STL pairs. The first element of such a pair is of type unsigned, the
 * second element of type std::string
 * @throw FileOpenError  if file open fails
 * @throw FileFormatError if file read fails
 */
BOSS::t_USPairVector BOSS::readS1LabelTable(const string & name)
  throw(BOSS::FileOpenError, BOSS::FileFormatError)
{
  static const char F_Name[] = "readS1LabelTable()";

  ifstream in(name.c_str());
  if(!in)
    throw BOSS::FileOpenError(F_Name, name);

  string   tmp;
  while(getline(in, tmp)) {
    if(tmp=="hend")
      break;
  }

  t_USPairVector res;
  //copy(istream_iterator<t_USPair>(in), istream_iterator<t_USPair>(), back_inserter(res));
  if(in.bad())
    throw BOSS::FileFormatError(F_Name, name, 0);

  return res;
}

/** This function reads tables with two columns. Both columns are interpreted
 * as std::strings.
 * The function returns a vector containing the rows of the table as an STL pair.
 * @return a vector containing the read table
 * @throw FileOpenError  if file open fails.
 * @throw FileFomatError if file read fails.
*/
BOSS::t_SSPairVector BOSS::readTwoStringTable(const string & name)
  throw(BOSS::FileOpenError, BOSS::FileFormatError)
{
  static const char F_Name[] = "readTwoStringTable()";

  ifstream in(name.c_str());
  if(!in)
    throw BOSS::FileOpenError(F_Name, name);

  t_SSPairVector res;
  //copy(istream_iterator<t_SSPair>(in), istream_iterator<t_SSPair>(), back_inserter(res));
  if(in.bad())
    throw BOSS::FileFormatError(F_Name, name, 0);

  return res;
}

/**
 * @brief This function adds a slash to the string s and returns the modified string.
 * The input string is modified only if it is is not empty and does not contain a slash as the
 * last character
 * @param s input string
 * @return modified string
*/
string BOSS::addSlash(const string & s)
{
  if(s[s.size()-1] == '/')
    return s;
  if(s.size()>0)
    return s + "/";
  return s;
}

/**
 * This function substitutes the <INVENTORY> tag with the string given by inv. The
 * inventory tag must be contained in "pref". (obsolete)
 * @throw StringFormatError if the inventory tag is not contained in pref
*/
string BOSS::makeInvFileName(const string & pref,  const string & inv, const string & file)
  throw(BOSS::FileFormatError)
{
  static const char F_Name[] = "makeInvFileName()";
  if (pref == "" || inv == "" || file == "") throw BOSS::StringFormatError(F_Name, pref+","+inv+","+file);
  string res;
  size_t pos = pref.find("<INVENTORY>");
  if(pos == string::npos)
    res = addSlash(pref) + addSlash(inv) + file;
  else {
  res  = pref.substr(0, pos) + inv;
  res += addSlash(pref.substr(pos+11)) + file;
  }
  return res;
}

/**
 * @brief Concatenates a path and a filename to form a full filepath.
 * Adds directory delimiter where necessary.
 * @throw StringFormatError if both path and filename are empty strings
 * @return string containing filepath
 */
string BOSS::makeFilePath(const string & pref, const string & file)
  throw(BOSS::FileFormatError)
{
  static const char F_Name[] = "makeFilePath()";
  if (pref == "" || file == "") throw BOSS::StringFormatError(F_Name, pref+","+file);
  return addSlash(pref) + file;
}

/**
 * @brief Removes punctuation symbols.
 * This function removes a punctuation symbol (.,;:!?) from the string given
 * as argument. The return value is an STL pair. The first element contains the
 * puncuation symbol, the second the string without the symbol.
 * @param w input string
 * @return pair of punctuation symbol and modified string
*/
BOSS::t_CSPair BOSS::cleanWordPunctuation(const string & w)
{
  t_CSPair res((t_UChar)-1, w);
  unsigned size = w.size()-1;
  switch(w[size]) {
  case ',':
  case ';':
  case ':':
  case '.':
  case '?':
  case '!':
    res.first  = w[size];
    res.second = string(w, 0, size);
    break;
  }
  return res;
}

/** 
 * @brief Translates input string characters to lower case.
 * This function maps upper-case characters of string s to lower case ones.
 * The modified string is returned.
 * @param s input string
 * @return modified string
*/
string BOSS::my_tolower(string s)
{
  for(unsigned i=0; i<s.size(); i++)
    s[i] = tolower(s[i]);
  return s;
}
/**
 * @brief Extracts file information.
 * This function separates path, name and extension from a given filename/path string
 * and returns a struct of type Fileinfo (see boss_types.h) containing this information
 * @param s path
 * @return file info object
 */
BOSS::Fileinfo BOSS::getFileinfo(const string & s) {
       Fileinfo file;
       file.path="";
       file.base="";
       file.ext="";
       file.exists=false;
       ifstream in(s.c_str());
       if (in && !in.fail() && !in.eof()) file.exists=true;
       in.close(); 
       size_t pos = s.rfind('/');
       if (pos == string::npos) {
       file.path = ".";
       file.base = s;
      }
      else {
	file.path = string(s,0,pos);
	file.base = string(s,pos+1,s.size());
      }
      pos = file.base.find('.');
      if (pos != string::npos) {
	file.ext = string(file.base,pos+1,file.base.size());
      	file.base = string(file.base,0,pos);
      }
      return file;
}

/**
 * @brief Returns date.
 * @return formated date string
 */ 
string BOSS::getDate() {
  time_t rawtime;
  struct tm * timeinfo;
  char form[] = "%y%m%d-%H%M%S";
  char res[20];
  int size;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  size = strftime(&res[0], 14, &form[0],timeinfo);
  return string(res);
}

