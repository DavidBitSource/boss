/* boss_numbers.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   @author Joerg Broeggelwirth
*/

#ifndef BOSS_NUMBERS_H
#define BOSS_NUMBERS_H
#include "boss_utility/boss_types.h"
#include <fstream>
#include <set>
#include <iterator>

/**
  *@author Joerg Broeggelwirth
  */

// Struct for a table entry
// (number of digits of the number, number, word, transcription of the word)
struct NW_mapper
{
	unsigned length;
	string number;
	string n_word;
	string n_tra;
	NW_mapper() {;}
	NW_mapper(unsigned l, string n, string w, string t) : length(l), number(n), n_word(w), n_tra(t) {;}
};

// a vector of ZW_mapper Structs (corresponds to the assignment table as a file)
typedef vector<struct NW_mapper> NW_Vector;

/**************************************************************************************************
    The class "BOSS_Numbers" converts "signed float" numbers. A string of digits is transformed into a
    string of orthographic words. The constructor "BOSS_Numbers::BOSS_Numbers(const char* file)" is called
    with the file name of the table containing the entries of the names of the numbers
    ("nominal_word_tab.txt"). The "public" member functions are "string BOSS_Numbers::operator ()
    (string input_number) const" and "t_SSPair BOSS_Numbers::operator()(const string& input_number) const".
    The class "ordinal" is derived from class "BOSS_Numbers".
*/
class BOSS_Numbers {
protected:	
  NW_Vector table;
  set<string> specials;	
	
  virtual BOSS::t_SSPair patch(BOSS::t_SSPair number, BOSS::t_SSPair name) const;
  virtual BOSS::t_SSPair get_number(const string& input_number) const;
  virtual BOSS::t_SSPair get_name(const unsigned& length) const;
  virtual BOSS::t_SSPair convert(string input_number) const;
	
public:
  virtual BOSS::t_SSPair roman(const string& input) const = 0;
  virtual BOSS::t_SSPair operator () (string input_number) const = 0;
  BOSS_Numbers() {};
  virtual ~BOSS_Numbers() {};
};



/** Input/output operators for NW_mapper */
inline istream & operator >> (istream & in, NW_mapper & m) {
	return in >> m.length >> m.number >> m.n_word >> m.n_tra;
}

inline ostream & operator << (ostream & out, const NW_mapper & m) {
	return out << m.length << " " << m.number << " " << m.n_word << " " << m.n_tra;
}

/** Input/output operators for NW_Vector */
inline istream & operator >> (istream & in, NW_Vector & v) {
	copy(istream_iterator< NW_mapper >(in), istream_iterator< NW_mapper >(), back_inserter(v));
	return in;
}

inline ostream & operator << (ostream & out, const NW_Vector & v) {
	copy(v.begin(), v.end(), ostream_iterator< NW_mapper >(out, "\n"));
	return out;
}

// '+' operator for t_SSPair
inline BOSS::t_SSPair operator + (BOSS::t_SSPair a, const BOSS::t_SSPair& b) {
	a.first=a.first+b.first;
	a.second=a.second+b.second;
	return a;
}

// '+=' operator for t_SSPair
inline void operator += (BOSS::t_SSPair& a, const BOSS::t_SSPair& b) {
	a=a+b;
}

#endif
