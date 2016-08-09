/* boss_numbers.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include "boss_numbers.h"

// global error pair
const BOSS::t_SSPair no_I("Item not found!", "Item not found!");
	
/** grammatical adjustment */
BOSS::t_SSPair BOSS_Numbers::patch(BOSS::t_SSPair number, BOSS::t_SSPair name) const
{
  static const BOSS::t_SSPair eins("eins", "?aIns"), n("n", "n"), en("en", "@n");
  unsigned length(0);
  // look up the digit length for "name" in the table
  unsigned c(0);
  while ((c<table.size())&&(name.first!=table[c].n_word)) c++;
  if (c<table.size()) length=table[c].length;
  // grammatical adjustment
  // "eins" is a special grammatical case!!!
  if (number==eins) {
    switch (length>6) {
      case false : number.first.erase(number.first.find("s", 0), 1);
	           number.second.erase(number.second.find("s", 0), 1); break;
      case true  : number.first.replace(number.first.find("s", 0), 1, "e");
	           number.second.replace(number.second.find("s", 0), 1, "@"); break;
    }
  }
  // handling of the endings of the number names
  else if (length>6) {
    switch (name.first[name.first.size()-1]) {
      case 'e' : name+=n; break;
      default  : name+=en;
    }
  }
  return number+name;
}

/** It is tried to read "input_number" directly from the mapping table */ 	
BOSS::t_SSPair BOSS_Numbers::get_number(const string& input_number) const
{
  // default value of the result is "Fehler"
  BOSS::t_SSPair result(no_I);
	
  unsigned c=0;
  while ((table[c].number!=input_number)&&(c<(table.size()-1))) c++;
  if (c<(table.size()-1)) {
    result.first=table[c].n_word;
    result.second=table[c].n_tra;
  }
  return result;
}

/** It is tried to read a number name from the table by means of the length of the number */
BOSS::t_SSPair BOSS_Numbers::get_name(const unsigned& length) const
{
  static const string failure("\nNumber too large!\n");
  static const BOSS::t_SSPair error(failure, failure);
  BOSS::t_SSPair result;
  unsigned c=0;
  while ((table[c].length<length)&&(c<(table.size()-1))) c++;
  switch (c<=(table.size()-1)) {
    case true : if (table[c].length>length) {
                  result.first=table[c-1].n_word;
	          result.second=table[c-1].n_tra;
                } 		
                if (table[c].length==length) {
	          result.first=table[c].n_word;
	          result.second=table[c].n_tra;
	        }
                if ((table[c].length<length)&&((length-table[c].length)<3)) {
	          result.first=table[c].n_word;
	          result.second=table[c].n_tra;
	        }; break;
    case false : result=error; break;
  }			
  return result;
}

// Converter of arabic numbers to orthographic words
BOSS::t_SSPair BOSS_Numbers::convert(string input_number) const
{
  unsigned input_length=input_number.length();
  string front="";
  static const BOSS::t_SSPair und("und", "?Unt"), zig("zig", "tsIC"), empty("", "");
  BOSS::t_SSPair result(empty);
	
  // For number strings that consist of up to 3 digits
  if (input_length<4) {
    // Default anchor, but not for "100"
    if ((input_length<3)&&(get_number(input_number)!=no_I)) {
      result = get_number(input_number);
    }
    else
    {				
      // In case of 2 digit strings, the last digit is the first to be looked up
      // add contains "und", if a 2 digit number string doesn't start with 1
      BOSS::t_SSPair add(empty);
      BOSS::t_SSPair zehner(empty);
     
      front.assign(input_number, 0, 1);
      input_number.assign(input_number, 1, input_length-1);
      switch (input_length) {
	// separate handling of 2 digit and 3 digit number strings
	// 10+ numbers are treated in a special way
        case 2 : if (front!="1") {
	           add = und;
		  zehner = zig;
	         }
	         // If one of the special cases occurs, append "0" and do without "zig"
	         if (specials.count(front+"0")!=0) {
		   front+="0";
		   zehner=empty;
		 }
		 // 2 digit numbers that end with "0" are simply "Ziffer"+"zig",
		 // "2.Ziffer"+"und"+"1.Ziffer"+"zig" else
		 if (input_number!="0") {
		   if (front!="0") result=patch(get_number(input_number), add) +convert(front)+zehner;
		   else result=get_number(input_number);
		 }
	         else if (front!="0") result = get_number(front) + zehner;
	           else result = empty;
	         break;
		 // In case of 3 digit numbers one only needs the name for "hundert"
		 // as well as the convert() computation of the rest
	case 3 : add=get_name(input_length);
	         if (front!="0") result=patch(get_number(front), add)+convert(input_number);
	         else result=convert(input_number);
	         break;
      }
    }
  }	
  // For number strings with at least 4 digits
  else {
    // First the number is divided into a number chain front + rest.
    // The recursive part takes a number + name with a maximum of 3 digits from the table
    // and then calls "convert()" again with the rest of the digit string to be processed.
    // The modulo division determines, how many digits have to be cut off at the front of the
    // digit string
    unsigned n=0;
    if (input_length%3!=0) n=input_length%3;
    else n=3;
    front.assign(input_number, 0, n);
    input_number.assign(input_number, n, input_length-n);
    // an additional patch for grammar is called
    if (front[0]!='0') result = patch(convert(front), get_name(input_length)) + convert(input_number);
      else result = convert(front) + convert(input_number);
  }

  return result;
}
