/* main.cpp (addphrases) $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>

// Local include files
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/dom_tools.h"

// Some variables used as defaults for the command line class
static const char USAGE[]      = "addphrases -inv inventory [-xext xml_extension] [-bext backup_extension] [-b] -list filelist | xmlfile1, xmlfile2, ...";


// This is the main routine. It takes two file names as arguments.
// The contents of the first file associated with "xml_file" is
// parsed and read into memory. This will generate the document
// represented by the DOM_Document class.
// After parsing, we will call functions which add the phrasing information
// to the user data field of each node.

void addPhrases(const string &xml_file, const string & backup_file)
{
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
    // Add phrasing information on syllable and phone levels
    // Get all nodes with tag name "WORD".
    xercesc::DOMNodeList* wl = BOSS::getElementsByTagName(doc, "WORD");
    string pmode,pint;
    for(int i=0; i<int(wl->getLength()); i++) {
      xercesc::DOMNode* wnode       = wl->item(i);
      xercesc::DOMElement* welement = (xercesc::DOMElement *) wnode;
      pmode = BOSS::getAttrByName(wnode, "PMode");
      pint = BOSS::getAttrByName(wnode, "PInt");
      if (pmode != "") {
	xercesc::DOMNodeList* sl = BOSS::getElementsByTagName(welement, "SYLLABLE");
	for (int i=0;i<int(sl->getLength()); i++) {
	  xercesc::DOMNode* snode = sl->item(i);
	  xercesc::DOMElement* selement = (xercesc::DOMElement *) snode;
	  xercesc::DOMNodeList* pl = BOSS::getElementsByTagName(selement, "PHONE");
	  if (i<int(sl->getLength())-1) {
	    BOSS::setAttribute(selement, "PMode", "");
	    BOSS::setAttribute(selement, "PInt", "0");
	    for (int i=0;i<int(pl->getLength()); i++) {
	      xercesc::DOMNode* pnode = pl->item(i);
	      xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
	      BOSS::setAttribute(pelement, "PMode", "");
	      BOSS::setAttribute(pelement, "PInt", "0");
	    }
	  }
	  else {
	    BOSS::setAttribute(selement, "PMode", pmode.c_str());
	    BOSS::setAttribute(selement, "PInt", pint.c_str());
	    for (int i=0;i<int(pl->getLength()); i++) {
	      xercesc::DOMNode* pnode = pl->item(i);
	      xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
	      BOSS::setAttribute(pelement, "PMode", pmode.c_str());
	      BOSS::setAttribute(pelement, "PInt", pint.c_str());
	    }
	  }
	}
      }
      else {
	xercesc::DOMNodeList* sl = BOSS::getElementsByTagName(welement, "SYLLABLE");
	for (int i=0;i<int(sl->getLength()); i++) {
	  xercesc::DOMNode* snode = sl->item(i);
	  xercesc::DOMElement* selement = (xercesc::DOMElement *) snode;
	  xercesc::DOMNodeList* pl = BOSS::getElementsByTagName(selement, "PHONE");
	  BOSS::setAttribute(selement, "PMode", "");
	  BOSS::setAttribute(selement, "PInt", "0");
	  for (int i=0;i<int(pl->getLength()); i++) {
	    xercesc::DOMNode* pnode = pl->item(i);
	    xercesc::DOMElement* pelement = (xercesc::DOMElement *) pnode;
	    BOSS::setAttribute(pelement, "PMode", "");
	    BOSS::setAttribute(pelement, "PInt", "0");
	  }
	}
      }
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
    if(backup) {
      string backup_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-bext").optionarg));
      addPhrases(xml_file, backup_file);
    }
    else {
      addPhrases(xml_file, "");
    }
  }

  return EXIT_SUCCESS;
}
