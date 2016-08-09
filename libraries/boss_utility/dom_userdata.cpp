/* dom_userdata.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   @author Karlheinz Stoeber
*/

#include "dom_userdata.h"

DOM_UserData::~DOM_UserData() {}


ostream & DOM_UserData::printQueryItems(ostream & out)
{ return out << data_base_query << endl; }

ostream & DOM_UserData::printSelectedRow(ostream & out)
{
  unsigned field_num = data_base_query.col();
  for(unsigned i=0; i<field_num; i++)
    out << data_base_query(pos, i) << " ";
  return out;
}

void BOSS::getLabels(const xercesc::DOMNode * node, vector< vector< string > > & file, unsigned & last) {
  if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
    vector< string > line;
    stringstream start,end;
    if (BOSS::getAttrByName(node,"TKey") != "$p") {
      cerr << "getLabels: " << BOSS::getAttrByName(node,"TKey") << endl;
      DOM_UserData * user_data;
      if((user_data = (DOM_UserData*)BOSS::getUserData(node)) != 0) {
	if(user_data->getPos() != DOM_UserData::npos) {
	  start << last;
	  last += user_data->getSignalSize();
	  end << last;
	  line.push_back(start.str());
	  line.push_back(end.str());
	  line.push_back(string(user_data->getResult("TKey") + "-" + user_data->getResult("TReal")));
	  file.push_back(line);
	}
      }
      else {
	xercesc::DOMNodeList* nl = node->getChildNodes();
	for(unsigned i=0; i<nl->getLength(); i++) {
	  xercesc::DOMNode* tmp = nl->item(i);
	  if(tmp->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	    getLabels(tmp,file,last);
	  }
	}
      }
    }
    else {
      start << last;
      last += unsigned((atof(BOSS::getAttrByName(node,"Dur").c_str())*16)+0.5);
      end << last;
      line.push_back(start.str());
      line.push_back(end.str());
      line.push_back("$p");
      file.push_back(line);
    }
  }
}

void BOSS::writeLabels(const xercesc::DOMNode * sentence, const string & fname) {
  unsigned last(0);
  vector< vector< string > > v;
  getLabels(sentence,v,last);
  ofstream blf(string(fname + ".blf").c_str());
  ofstream ITier(string(fname + ".IntervalTier").c_str());
  ITier << "File type = \"ooTextFile short\"" << endl
	<< "\"IntervalTier\"" << endl << endl
	<< 0 << endl
	<< atof(v[v.size()-1][1].c_str())/16000 << endl
	<< v.size() << endl;
  for (unsigned i=0;i<v.size();i++) {
    blf << v[i][0] << " " << v[i][2] << endl;
    ITier << atof(v[i][0].c_str())/16000 << endl
	  << atof(v[i][1].c_str())/16000 << endl
	  << "\"" << v[i][2] << "\"" << endl;
  }
  blf.close();
  ITier.close();
}
