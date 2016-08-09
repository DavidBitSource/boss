/* main.cpp (blfxml2db) $Revision: 1.5 $ $Date: 2005/12/17 16:06:11 $
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include <fstream.h>
#include <sstream>
#include <fstream>
#include <stdlib.h>

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeFilter.hpp"
#include "xercesc/dom/DOMTreeWalker.hpp"
#include "boss_mysql/boss_mysql_handler.h"
#include <sstream>
#include <string>
#include <vector>

#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/dom_tools.h"

MySQL_Handler * data_base(0);

// Some variables used as defaults for the command line class
static const char USAGE[]      = "blfxml2db -inv inventory [-c db_key] [-sql sql_template] [-xext xml_extension] [-u user_name] [-h host_name] [-p password] [-db database] -list filelist | xmlfile1, xmlfile2, ...";
static const char NODE_NAMES[] = "SENTENCE WORD SYLLABLE PHONE HALFPHONE";

/*
ostream& operator<<(ostream& target, const DOMString& s)
{
  char *p = s.transcode();
  target << p;
  delete [] p;
  return target;
}
*/

string myToLower(const string & s)
{
  string tmp(s);
  for(unsigned i=0; i<s.size(); i++)
    tmp[i] = tolower(s[i]);
  return tmp;
}

string quote_backslashes(const string & s)
{
  string tmp(s);
  for(unsigned i=0; i<tmp.size(); i++) {
    if(tmp[i] == '\\') {
      tmp.insert(i, 1,'\\');
      i++;
    }
  }
  return tmp;
}

void putToDB(const xercesc::DOMDocument * doc, const string & level, const string & invent_name)
{
  xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, level.c_str());
  xercesc::DOMNode *tmp, *tmp1;
  unsigned *p, *wp;
  string tablename = myToLower(invent_name+"_"+level+"_DATA");
  // First create the SQL command which will create entries in the data table
  for(unsigned i=0; i<nl->getLength(); i++) {
    stringstream query;
    query << "INSERT INTO " << tablename << " SET ";
    tmp = nl->item(i);

    p = (unsigned *)BOSS::getUserData(tmp);
    query << "Num=\"" << *p << "\",";

    xercesc::DOMNamedNodeMap* nm = tmp->getAttributes();
    unsigned j;
    for(j=0; j<nm->getLength()-1; j++) {
      query << nm->item(j)->getNodeName()  << "=\"" << quote_backslashes(BOSS::getNodeValue(nm->item(j))) << "\",";
    }
    query << nm->item(j)->getNodeName() << "=\"" << quote_backslashes(BOSS::getNodeValue(nm->item(j))) << "\"" << ends;

    // Database action
    try {
      data_base->query(query.str().c_str(), true);
    }
    catch(const MySQL_QueryFailed & e) {
      cerr << "Data: " << query.str() << endl;
      e.printError(true);
    }
  }

  // Now create the SQL command which will create entries in the mapping table
  tablename = myToLower(invent_name+"_"+level+"_MAP");
  for(unsigned i=0; i<nl->getLength(); i++) {
    tmp = nl->item(i);
    p = (unsigned *)BOSS::getUserData(tmp);
    xercesc::DOMNodeList* cn = tmp->getChildNodes();
    for(unsigned j=0; j<cn->getLength(); j++) {
      tmp1 = cn->item(j);
      if(tmp1->getNodeType() == xercesc::DOMElement::ELEMENT_NODE) {
	wp = (unsigned *)BOSS::getUserData(tmp1);
	// Database action
	stringstream query;
	query << "INSERT INTO " << tablename << " VALUES(" << *p << "," << *wp << ")" << ends;
	try {
	  data_base->query(query.str().c_str(), true);
	}
	catch(const MySQL_QueryFailed & e) {
	  cerr << "Data: " << query.str() << endl;
	  e.printError(true);
	}
      }
    }
  }
}

unsigned xml2db(const string &xml_file, const string & invent_name, const unsigned counter_set = 1)
{
  unsigned counter(counter_set);

  xercesc::DOMDocument *doc = 0;
  XMLCh tempStr[100];
  xercesc::XMLString::transcode("LS", tempStr, 99);
  xercesc::DOMImplementation *impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
#if XERCES_VERSION_MAJOR == 3
  xercesc::DOMLSParser* parser = ((xercesc::DOMImplementationLS*)impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#else if XERCES_VERSION_MAJOR == 2
  xercesc::DOMBuilder* parser = ((xercesc::DOMImplementationLS*)impl)->createDOMBuilder(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#endif
  // Create a vector containing the node names
  vector< string > node_names;
  {
    string node_name;
    stringstream ins(NODE_NAMES);
    while(ins >> node_name)
      node_names.push_back(node_name);
  }

  //  Parse the XML file, catching any XML exceptions that might propagate
  //  out of it.
  try {
    doc = parser->parseURI(xml_file.c_str());
    // Assign numbers as userData to all nodes
    unsigned * p;
    for(unsigned i=0; i<node_names.size(); i++) {
      xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, node_names[i].c_str());
      for(unsigned i=0; i<nl->getLength(); i++) {
	p  = new unsigned;
	*p = counter++;
	BOSS::setUserData(nl->item(i), p);
      }
    }

    // Put data into the tables of the database
    for(unsigned i=0; i<node_names.size(); i++)
	 putToDB(doc, node_names[i], invent_name);

    // Free the allocated memory
    for(unsigned i=0; i<node_names.size(); i++) {
      xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, node_names[i].c_str());
      for(unsigned i=0; i<nl->getLength(); i++) {
 	p = (unsigned *)BOSS::getUserData(nl->item(i));
	delete p;
      }
    }		
  }
  catch (const xercesc::XMLException& e) {
    cerr << "An error occured during parsing\n   Message: "
         << e.getMessage() << endl;
  }
  return counter;
}

void database_query(string str)
{
  stringstream query;
  query << str << ends;
  try 
  {
    data_base->query(query.str().c_str(), true);
  }
  catch(const MySQL_QueryFailed & e) 
  {
    cerr << "Database error: " << str << endl;
    e.printError(true);
  }
}

int main(int argc, char *argv[])
{
  BOSS::Config cl(argc, argv, USAGE); // Process the command line options
  try {
    // Initialise the XML4C2 system. Taken from original distribution
    xercesc::XMLPlatformUtils::Initialize();
    // Initialize the database connection
    data_base = new MySQL_Handler(cl("-h").optionarg,cl("-u").optionarg,cl("-p").optionarg,cl("-db").optionarg);
  }
  catch (const MySQL_ConnectFailed & e) {
    e.printError(true);
  }
  catch(const xercesc::XMLException& toCatch) {
    cerr << "Error during Xerces-c Initialization.\n"
	 << "  Exception message:"
	 << toCatch.getMessage() << endl;
    return 1;
  }

  //Create database tables
  string str, qstr;
  unsigned find1;
  string query;
  if ((atoi(cl("-c").optionarg.c_str()) == 1) && cl("-sql").optionflag)
  {
    ifstream in(cl("-sql").optionarg.c_str());
    if (in)
    {
      while (getline(in, str))
      {
	if (str.find("CREATE") != string::npos)
	{
	  find1 = str.find("<INVENTORY>");
	  str.erase(find1,11);
	  str.insert(find1,cl.getInvName());

	  // Delete old table
	  query = "DROP TABLE IF EXISTS " + str.substr(find1, str.length() - find1);
	  cout << query << endl;
	  database_query(query);

	  /*
	  try
	  {
	    data_base->query(query, true);
	  }
	  catch(const MySQL_QueryFailed & e)
	  {
	    cerr << "Unable to delete: " << query << endl;
	    e.printError(true);
	  }
	  */

	  // Create new table
	  query = str;
	  while (str[0] != ')')
	  {
	    query += "\n";
	    getline(in, str);
	    query += str;
	  }
	  cout << query << endl;
	  database_query(query);

	  /*
	  try
	  {
	    data_base->query(query.str(), true);
	  }
	  catch(const MySQL_QueryFailed & e)
	  {
	    cerr << "Unable to create: " << query.str() << endl;
	    e.printError(true);
	  }
	  */
	}
      }
      in.close();
    }
    else cerr << "Unable to open sql template " << cl("-sql").optionarg.c_str() << endl;
  }

  // Input files are given as command line argument or in a file
  // indicated by the "-list" option
  vector< string > files;
  {
    if(cl("-list").optionflag) {
      ifstream in(cl("-list").optionarg.c_str());
      if(!in) {
	cerr << "Can't open " << cl("-list").optionarg << endl;
	exit(-1);
      }
      copy(istream_iterator<string>(in), istream_iterator<string>(),
	   back_inserter(files));
    }
    else {
      for(unsigned i=0; i<cl.size(); i++)
	files.push_back(cl[i]);
    }

    if(!files.size())
      cerr << "You have to enter at least one filename-" << endl;
  }

  unsigned free_counter = atoi(cl("-c").optionarg.c_str());
  for(unsigned i=0; i<files.size(); i++) {
    cerr << "Processing: " << files[i] << endl;
    string xmlfile = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-xext").optionarg));
    free_counter = xml2db(xmlfile, cl.getInvName(), free_counter);
  }

  return EXIT_SUCCESS;
}
