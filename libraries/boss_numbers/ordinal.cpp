/* ordinal.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2001 University of Bonn.
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

#include "ordinal.h"
#include <sstream>

// class members of ordinal
/** constructor */
ordinal::ordinal(const char* ofile, const char* nfile) : nom(nfile) {
  // number-word assignment table is read
  ifstream source(ofile);
  source >> table;
  source.close();
}

ordinal::~ordinal() {}

BOSS::t_SSPair ordinal::roman(const string& input) const {
  int sum(0);
  string tmp("");
  BOSS::t_SSPair result("", "");
  stringstream stream;
  static CIMap roman_table;
  roman_table['I']=1; roman_table['i']=1;
  roman_table['V']=5; roman_table['v']=5;
  roman_table['X']=10; roman_table['x']=10;
  roman_table['L']=50; roman_table['l']=50;
  roman_table['C']=100; roman_table['c']=100;
  roman_table['M']=1000; roman_table['m']=1000;

  // Algorithm for converting roman numbers to arabic numbers
  for (unsigned i=0; i<(input.size()-1); i++) {
    if (roman_table[input[i]]>=roman_table[input[i+1]]) {
      sum+=roman_table[input[i]];
    }
    else {
      sum-=roman_table[input[i]];
    }
  }

  // conversion from int to string
  stream << sum;
  stream >> tmp;

  // conversion from arabic to orthographic numbers
  result = operator()(tmp);

  return result;
}

BOSS::t_SSPairVector ordinal::date(string date) const {
  BOSS::t_SSPairVector result;

  while (date.find(".", 0)!=string::npos) {
    result.push_back(operator()(date.substr(0, date.find(".", 0)+1)));
    // delete the first ordinal number of the string
    date.erase(0, date.find(".", 0)+1);
    // delete leading spaces
    while (date[0]==' ') date.erase(date.begin());
  }
  if (date.size()) result.push_back(nom.year(date));

  return result;
}

BOSS::t_SSPair ordinal::operator () (string input_number) const {
  BOSS::t_SSPair result("", "");
  // Deletion of the ordinal "."
  if (input_number.find(".", 0)!=string::npos) {
    input_number.erase(input_number.find(".", 0), input_number.size()-input_number.find(".", 0));
  }
  unsigned input_length=input_number.length();

  if (input_length<2) {
    result=ordinal::convert(input_number);
  }
  else {
    result = nom(input_number);
    //cerr << "ig-compare: " << result.first.compare("ig", result.first.size()-2, 2) << endl;
    if ((result.first.length() > 2) && (result.first.compare(result.first.size() - 2, 2, "ig") == 0)) {
      result.first+="ster";
      result.second+="st6";
    }
    else {
      result.first+="ter";
      result.second+="t6";
    }
  }
  return result;
}
