/* nominal.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include "nominal.h"
#include <sstream>
#include <string>

nominal::nominal(const char* file) {
  // number-word assignment table is read
  ifstream source(file);
  source >> table;
  source.close();
  //cout << table << endl;
  
  // A set for special cases that ar stored in the table is set up
  // special cases are all "[2367]0"
  BOSS::t_SVector help;
  unsigned c(0);
  while ((c<table.size())&&(table[c].length<=2)) {
    if ((table[c].length==2)&&(table[c].number[1]=='0')) help.push_back(table[c].number);
    c++;
  }
  specials.insert(&help[0], &help[help.size()]);
  //copy(specials.begin(), specials.end(), ostream_iterator<string>(cout, "\n"));
}

nominal::~nominal(){
}

/*t_SSPair nominal::get_number(const string& input_number) const {
  return BOSS_Numbers::get_number(input_number);
}

BOSS::t_SSPair nominal::get_name(const unsigned& laenge) const {
  return BOSS_Numbers::get_name(laenge);
}

BOSS::t_SSPair nominal::convert(string input_number) const {
  return BOSS_Numbers::convert(input_number);
}

BOSS::t_SSPair nominal::patch(t_SSPair number, t_SSPair name) const {
  return BOSS_Numbers::patch(number, name);
}
*/

BOSS::t_SSPair nominal::roman(const string& input) const {
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
  sum+=roman_table[input[input.size()-1]];

  // conversion from int to string
  stream << sum;
  stream >> tmp;
  //cerr << "sum: " << sum << endl;
  //cerr << "result: " << result << endl;
  
  // conversion from arabic to orthographic numbers
  result = operator()(tmp);
  
  return result;
}

BOSS::t_SSPair nominal::year(const string& input_number) const {
  static const BOSS::t_SSPair hundert("hundert", "hUnd6t");
  stringstream *stream(NULL);
  unsigned length=input_number.length();
  float f_number(0);
  BOSS::t_SSPair result("", "");
	
  if (length<=4) {
    stream = new stringstream;
    *stream << input_number;
    *stream >> f_number;
    delete stream;
    switch (length) {
      case 4 :
	if ((1099<f_number)&&(f_number<2000)) {
	  result=operator()(input_number.substr(0, 2));
	  result+=hundert;
	  result+=operator()(input_number.substr(2, 2));
	}
	else result=operator()(input_number);
	break;
      case 3 :
	result=operator()(input_number.substr(0, 1));
	result+=hundert;
	result+=operator()(input_number.substr(1, 2));
	break;
      default :
	result=operator()(input_number);
	result+=hundert;
	break;
    }							
  }
  else {
    cerr << input_number << " is'nt a year!" << endl;
    result=convert(input_number);
  }
  
  return result;
}

BOSS::t_SSPair nominal::operator () (string input_number) const
{
  // local variables and constants
  static const BOSS::t_SSPair leer("", ""), plus("plus", "plUs"), minus("minus", "mi:nUs"), komma("komma", "kOma:");
  BOSS::t_SSPair result(leer), sign(leer), punct(leer), tmp(leer);
  const char punkt('.'), char_komma(',');
  string front="", back="";
  size_t punct_pos=string::npos;
  char c(0);
  if (input_number.size()) c = input_number[0];
	
  // delete "+" or "-" and translate them into the corresponding words
  switch (c) {
    case '+' : sign=plus; input_number.erase(input_number.find('+', 0), 1); break;
    case '-' : sign=minus; input_number.erase(input_number.find('-', 0), 1); break;
  }
  // find a possible comma/dot position in tthe digit string
  // and name the character "Komma"
  if (input_number.find(char_komma, 0)!=string::npos) {
    punct_pos=input_number.find(char_komma, 0);
    punct = komma;
  }
  if (input_number.find(punkt, 0)!=string::npos) {
    punct_pos=input_number.find(punkt, 0);
    punct = komma;
  }
  // The cases float and integer are differentiated with respect to their return value
  if (punct_pos!=string::npos) {
    front=input_number.substr(0, punct_pos);
    back=input_number.substr(punct_pos+1, input_number.size()-(punct_pos+1));
    // leading zeros are deleted from the integer part
    while ((front.find("0", 0)==0)&&(front.size()>1)) front.erase(0, 1);
    // Final zeros in the post comma part (back) are deleted
    while ((back.find("0", back.size()-1)==(back.size()-1))&&(back.size()>1))
      back.erase(back.size()-1, 1);
    // The post comma digits are taken from the table digit-wise
    for (unsigned c=0; c<back.size(); c++) tmp+=get_number(back.substr(c, 1));
    result = sign+convert(front)+punct+tmp;
  }
  else {
    // leading zeros are deleted from the integer part
    while ((input_number.find("0", 0)==0)&&(input_number.size()>1)) input_number.erase(0, 1);
    result = sign+convert(input_number);
  }
  return result;
}
