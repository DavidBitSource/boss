/* dom_userdata.h    $Revision: 1.5 $ $Date: 2007/04/16 09:43:33 $
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

#ifndef DOM_USERDATA_H
#define DOM_USERDATA_H


#include "boss_mysql/boss_mysql_handler.h"
#include <xercesc/dom/DOM.hpp>
#include "boss_utility/dom_tools.h"
#include <vector>
#include <sstream>
#include "tdsp/dsp_vector.h"

using namespace std;

/**
  A class to store the results of pre-selection and unit selection in.
  @author Karlheinz Stoeber
*/

typedef dsp::vector<short> t_Signal;

class DOM_UserData {
public:

	enum Node_Type {
		WORD     = 1,
		MORPHEME = 2,
		SYLLABLE = 3,
		PHONE  = 4,
		HALFPHONE = 5,
	};

 private:

	MySQL_Result   data_base_query;
	size_t       pos;
	Node_Type      type;
	unsigned       signal_size;
	t_Signal       signal;

public:

  static const size_t npos = static_cast<size_t>(-1);

 	DOM_UserData()
 		: pos(npos), signal_size(0) {}
	DOM_UserData(const MySQL_Result & dbq, const Node_Type & t)
		: data_base_query(dbq), pos(npos), type(t), signal_size(0) {}
	~DOM_UserData();

	ostream & printQueryItems(ostream & out);
	ostream & printSelectedRow(ostream & out);
	const MySQL_Result & getResult() const
		{ return data_base_query; }
	string getResult(const unsigned & p, const string & r) const
		{ return data_base_query(p, r); }
	string getResult(const string & r) const
		{ return data_base_query(pos, r); }
	Node_Type getType() const
		{ return type; }
	unsigned getPos() const
		{ return pos; }
	void setPos(const unsigned & p)
		{ pos = p; }
	void setSignalSize(const unsigned & s)
	  { signal_size=s; }
	unsigned getSignalSize()
	  { return signal_size; }
	void setSignal(const t_Signal & s)
	  { signal=s; }
	t_Signal & getSignal() {
		return signal;
	}
};

namespace BOSS
{
  void writeLabels(const xercesc::DOMNode * sentence, const string & fname);
  void getLabels(const xercesc::DOMNode * node, vector< vector< string > > & file, unsigned & last);
}

#endif
