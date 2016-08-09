/* conman_unit.cpp    $Revision: 1.3 $ $Date: 2007/02/26 09:55:52 $
   Copyright (C) 2003 University of Bonn.
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/**
   @author Stefan Breuer
*/

#include "conman_unit.h"

ConmanUnit::ConmanUnit(BOSS::Config & __config, MySQL_Handler * __database, xercesc::DOMNode * n,
		 const string & t, const string & inventory)
  : config(__config), database(__database), node(n), tkey(t)
{
  pause = false;
  user_data = (DOM_UserData*)BOSS::getUserData(node);
  start_index = atoi(user_data->getResult("First").c_str());
  end_index = atoi(user_data->getResult("Last").c_str());
  cerr << endl
       << "::: Conman_Unit: " << tkey << endl
       << "::: start:       " << start_index << " " << endl
       << "::: end:         " << end_index  << " " << endl
       << "::: length:      " << end_index-start_index << " " << endl;
  file_name = getSentenceFileName(inventory);
  cerr << endl;

}

ConmanUnit::ConmanUnit(BOSS::Config & __config, MySQL_Handler * __database, xercesc::DOMNode * n, const string & t)
  : config(__config), database(__database), node(n), tkey(t), start_index(0), end_index(0) {
  pause=true;
  user_data = (DOM_UserData*)BOSS::getUserData(node);
}


ConmanUnit & ConmanUnit::operator=(const ConmanUnit & other) {
	config = other.config;
	database = other.database;
	node = other.node;
	user_data = other.user_data;
	tkey = other.tkey;
	start_index = other.start_index;
	end_index = other.end_index;
	file_name = other.file_name;
	pause = other.pause;
	return *this;
}



string ConmanUnit::getSentenceFileName(const string & inventory)
{
  MySQL_Result res;
  string file_name;
  try {	
    string num = user_data->getResult("Num");
    if(user_data->getType() == DOM_UserData::WORD) {
      stringstream query;
      query << "SELECT " << inventory << "_sentence_data.File FROM " << inventory << "_sentence_data, " << inventory << "_sentence_map WHERE "
	    << inventory << "_sentence_map.WordNum=\"" << num << "\" AND "
	    << inventory << "_sentence_data.Num=" << inventory << "_sentence_map.SentenceNum" << ends;
      res = database->query(query.str().c_str());
      cerr << "::: WORD Num:      " << num << endl;
    }
    else if(user_data->getType() == DOM_UserData::SYLLABLE) {
      stringstream query;
      query << "SELECT " << inventory << "_sentence_data.File FROM " << inventory << "_sentence_data, " << inventory << "_sentence_map, " << inventory << "_word_map WHERE "
	    << inventory << "_word_map.SyllableNum=\"" << num << "\" AND "
	    << inventory << "_sentence_data.Num="  << inventory << "_sentence_map.SentenceNum AND "
	    << inventory << "_word_map.WordNum = " << inventory << "_sentence_map.WordNum" << ends;
      res = database->query(query.str().c_str());
      cerr << "::: SYLLABLE Num: " << num << endl;
    }
    if(user_data->getType() == DOM_UserData::PHONE) {
      stringstream query;
      query << "SELECT " << inventory << "_sentence_data.File FROM " << inventory << "_sentence_data, " << inventory << "_sentence_map, " << inventory << "_word_map, " << inventory << "_syllable_map WHERE "
	    << inventory << "_syllable_map.PhoneNum=\"" << num << "\" AND "
	    << inventory << "_sentence_data.Num="  << inventory << "_sentence_map.SentenceNum AND "
	    << inventory << "_word_map.WordNum = " << inventory << "_sentence_map.WordNum AND "
	    << inventory << "_syllable_map.SyllableNum = " << inventory << "_word_map.SyllableNum" << ends;
      res = database->query(query.str().c_str());
      cerr << "::: PHONE Num:   " << num << endl;
    }
    else if(user_data->getType() == DOM_UserData::HALFPHONE) {
      stringstream query;
      query << "SELECT " << inventory << "_sentence_data.File FROM " << inventory << "_sentence_data, " << inventory << "_sentence_map, " << inventory << "_word_map, " << inventory << "_syllable_map, " << inventory << "_phone_map WHERE "
	    << inventory << "_phone_map.HalfphoneNum=\"" << num << "\" AND "
	    << inventory << "_sentence_data.Num="  << inventory << "_sentence_map.SentenceNum AND "
	    << inventory << "_word_map.WordNum = " << inventory << "_sentence_map.WordNum AND "
	    << inventory << "_syllable_map.SyllableNum = " << inventory << "_word_map.SyllableNum AND "
	    << inventory << "_phone_map.PhoneNum= " << inventory << "_syllable_map.PhoneNum" << ends;
      res = database->query(query.str().c_str());
      cerr << "::: HALFPHONE Num: " << num << endl;
    }
    file_name = res(0, "File");
    cerr << "::: source file:   " << file_name << endl;
  }
  catch(const MySQL_QueryFailed & e) {
    e.printError(true);
  }
  return file_name;
}


