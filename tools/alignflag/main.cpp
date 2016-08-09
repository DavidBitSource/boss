/* main.cpp (alignflag) $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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

// Some variables used as defaults for the command line class
static const char USAGE[]      = "alignflag -inv inventory [-manual] [-xext xml_extension] [-bext backup_extension] [-b] -list filelist | xmlfile1, xmlfile2, ...";

void setFlag(xercesc::DOMNodeList* nl, bool manual_flag)
{
	for(unsigned i=0; i<nl->getLength(); i++) {
		xercesc::DOMNode* node       = nl->item(i);
		xercesc::DOMElement* element = (xercesc::DOMElement *) node;
		//Set to Manual if the manual optionflag is set, otherwise set to Auto
		if (manual_flag) {
			BOSS::setAttribute(element, "Align", "Manual");
		}
		else {
			BOSS::setAttribute(element, "Align", "Auto");
		}
	}
}

// This is the main routine. It takes two file names as arguments.
// The contents of the first file associated with "xml_file" is
// parsed and read into the memory. This will generate the document
// represented by the DOM_Document class.
// After parsing, we will call functions which add the alignment flag to
// the user data field of each node.

void addAlignFlag(const string &xml_file, const string & backup_file, bool manual_flag)
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
  	// Set "Align" at phone level
	xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "WORD");
	setFlag(nl, manual_flag);
	nl = BOSS::getElementsByTagName(doc, "SYLLABLE");
	setFlag(nl, manual_flag);
	nl = BOSS::getElementsByTagName(doc, "PHONE");
	setFlag(nl, manual_flag);


	// Do a backup copy if a backup name is given
	  if(backup_file.size())
  		rename(xml_file.c_str(), backup_file.c_str());

  	// Open the file to store the result in
  	ofstream out(xml_file.c_str());
  	if(!out) {
  		cerr << "Can't open file " << xml_file << endl;
  		return;
  	}

  	// Write out the the new xml structure
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
		// Initialize the XML4C2 system. Taken from original distribution
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch(const xercesc::XMLException& toCatch) {
		cerr << "Error during Xerces-c Initialization.\n"
				 << "  Exception message:"
				 << toCatch.getMessage() << endl;
		return 1;
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
			cerr << "You have to enter at least one filename." << endl;
	}

  bool backup = cl("-b").optionflag;
  for(unsigned i=0; i<files.size(); i++) {
  	cerr << files[i] << endl;
  	string xml_file    = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-xext").optionarg));
  	if(backup) {
	  	string backup_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-bext").optionarg));
	  	addAlignFlag(xml_file, backup_file, cl("-manual").optionflag);
	  }
	  else {
	  	addAlignFlag(xml_file, "", cl("-manual").optionflag);
	  }
  }

  return EXIT_SUCCESS;
}
