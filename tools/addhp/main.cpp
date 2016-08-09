/* main.cpp (addhp) $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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
   @author Stefan Breuer
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Include files needed for the xml library
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"

// Standard include files
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>

// Local include files
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/dom_tools.h"
#include "boss_context/boss_context.h"

// Some variables used as defaults for the command line class
static const char USAGE[]      = "addhp [-inv inventory] [-list filelist] [xmlfile1, xmlfile2, ...]";

void addHalfphones(const string &xml_file, const string &bhp_file, const string & backup_file, BOSS_context & cclass)
{	
 ifstream in(bhp_file.c_str());
  if(!in) {
    cerr << "Warning: No half-phone label file " << bhp_file << " - setting arbitrary boundaries." << endl;
  }
  string tmp;
  vector< unsigned > hp_bounds;
  while(getline(in, tmp)) {
    if(tmp != "") {
      string bstring, label;
      stringstream ins(tmp.c_str());
      ins >> bstring >> label;
      hp_bounds.push_back(atoi(bstring.c_str()));
    }
  }
  in.close();
  
  xercesc::DOMDocument *doc = 0;
  XMLCh tempStr[100];
  xercesc::XMLString::transcode("LS", tempStr, 99);
  xercesc::DOMImplementation *impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
#if XERCES_VERSION_MAJOR == 3
  xercesc::DOMLSParser* parser = ((xercesc::DOMImplementationLS*)impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#else if XERCES_VERSION_MAJOR == 2
  xercesc::DOMBuilder* parser = ((xercesc::DOMImplementationLS*)impl)->createDOMBuilder(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#endif
  try {
	// Parse the XML file, catching any XML exceptions that might propagate out of it.
        doc = parser->parseURI(xml_file.c_str());
    // Get all nodes with tag name "PHONE".
	xercesc::DOMNodeList* pl = BOSS::getElementsByTagName(doc, "PHONE");
    for(unsigned i=0; i<pl->getLength(); i++) {
      xercesc::DOMNode* pnode = pl->item(i);
      xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
      stringstream m;
      float first = atof((BOSS::getAttrByName(pnode, "First")).c_str());
      float last = atof((BOSS::getAttrByName(pnode, "Last")).c_str());
	  // Calculate the middle of the phone and use this value if none is provided.
      m << (int((last-((last-first)/2))+0.5));
      string middle(m.str());
      cerr << "calc. m. " << middle << endl;
      xercesc::DOMElement* hp1 = BOSS::createElement(doc, "HALFPHONE");
	  xercesc::DOMElement* hp2 = BOSS::createElement(doc, "HALFPHONE");
      // Copy all attributes of the phone:
      xercesc::DOMNamedNodeMap* attributes = pnode->getAttributes();
	  int attrCount = attributes->getLength();
	  string name = BOSS::getAttrByName(pnode, "TKey");
      for (int j = 0; j < attrCount; j++) {
        stringstream attr,val,cont;
	    xercesc::DOMNode*  attribute = attributes->item(j);
	    attr << attribute->getNodeName();
        val << attribute->getNodeValue();
        if (attr.str().c_str() == "CLeft")		//complete for CCLeft/CCLeft2 and CCRight/CCRight2
        {
        	BOSS::setAttribute(hp1, attr.str().c_str(), val.str().c_str());
            BOSS::setAttribute(hp2, attr.str().c_str(), name.c_str());
        }
        else if (attr.str().c_str() == "CCLeft") {
                	BOSS::setAttribute(hp1, attr.str().c_str(), val.str().c_str());
                	BOSS::setAttribute(hp2, attr.str().c_str(), cclass.left1(name.c_str()).c_str());
        }
        else if (attr.str().c_str() == "CCLeft2") {
                    BOSS::setAttribute(hp1, attr.str().c_str(), val.str().c_str());
                    BOSS::setAttribute(hp2, attr.str().c_str(), cclass.left2(name.c_str()).c_str());
        }
        else if (attr.str().c_str() == "CRight")
        {
        	BOSS::setAttribute(hp1, attr.str().c_str(), name.c_str());
        	BOSS::setAttribute(hp2, attr.str().c_str(), val.str().c_str());
        }
        else if (attr.str().c_str() == "CCRight")
        {
            BOSS::setAttribute(hp1, attr.str().c_str(), cclass.right1(name.c_str()).c_str());
            BOSS::setAttribute(hp2, attr.str().c_str(), val.str().c_str());
        }
        else if (attr.str().c_str() == "CCRight2")
        {
            BOSS::setAttribute(hp1, attr.str().c_str(), cclass.right2(name.c_str()).c_str());
            BOSS::setAttribute(hp2, attr.str().c_str(), val.str().c_str());
        }
        else
        {
        	BOSS::setAttribute(hp1, attr.str().c_str(), val.str().c_str());
        	BOSS::setAttribute(hp2, attr.str().c_str(), val.str().c_str());
        }
        }
	  // Set "Half" attributes
      BOSS::setAttribute(hp1, "Half", "1");
      BOSS::setAttribute(hp2, "Half", "2");
	  // If a half-phone label file exists, set the "Last" and "First" attributes accordingly.
      // Use the first label between "First" and "Last", then exit the loop.
      bool found(false);
	  for (unsigned h=0;h<hp_bounds.size();h++) {
        stringstream tmp;
        tmp << hp_bounds[h];
        string boundstr(tmp.str());
        unsigned bound = hp_bounds[h];
        if (bound <= unsigned(last) && bound >= unsigned(first)) {
            cerr << "found m. " << bound << endl;
			middle = boundstr;
            found = true;
            //Clear the boundary vector up to this point:
            while (bound != hp_bounds.front())
              hp_bounds.erase(hp_bounds.begin());
            hp_bounds.erase(hp_bounds.begin());
            break;          
		}
	  }
      if (!found) 
        cerr << "Using arbitrary value for half-phone boundary in "
             << BOSS::getAttrByName(pelement, "TKey") << endl;
      BOSS::setAttribute(hp1, "Last", middle.c_str());
      BOSS::setAttribute(hp2, "First", middle.c_str());
      pelement->appendChild(hp1);
      pelement->appendChild(hp2);
    }
    // Make a backup copy, if a backup name is given
    if(backup_file.size())
      rename(xml_file.c_str(), backup_file.c_str());

    // Open the file to write the result to
    ofstream out(xml_file.c_str());
    if(!out) {
      cerr << "Can't open file " << xml_file << endl;
      return;
    }

    // Write the new xml structure
    out << doc << endl;
  }
  catch (const xercesc::XMLException& e) {
    cerr << "An error occured during parsing\n   Message: "
         << e.getMessage() << endl;
  }
}

int main(int argc, char *argv[])
{
  // Process the command line options
  BOSS::Config cl(argc, argv, USAGE); // Process the command line options
  string ccfile = cl("-ccfile").optionarg;
  BOSS_context cclass(ccfile);

  try {
    // Initialise the XML4C2 system. Taken from original distribution
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch(const xercesc::XMLException& toCatch) {
    cerr << "Error during Xerces-c Initialization.\n"
	 << "  Exception message:"
	 << toCatch.getMessage() << endl;
    return 1;
  }

  // Input files are given as command line arguments or in a file
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
      cerr << "You have to enter at least one filename." << endl;
  }

  bool backup = cl("-b").optionflag;
  for(unsigned i=0; i<files.size(); i++) {
    cerr << files[i] << endl;
    string xml_file    = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-xext").optionarg));
	string bhp_file    = BOSS::makeFilePath(cl("-ldir").optionarg, string(files[i] + cl("-hpext").optionarg));
    if(backup) {
      string backup_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-bext").optionarg));
      addHalfphones(xml_file, bhp_file, backup_file, cclass);
    }
    else {
      addHalfphones(xml_file, bhp_file, "", cclass);
    }
  }

  return EXIT_SUCCESS;
}
