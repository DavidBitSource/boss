/* main.cpp (optbounds) $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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

#include <fstream>
#include <iomanip>
#include <cstdio>
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "tdsp/dsp_vector.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/dom_tools.h"

typedef dsp::vector< short > t_Signal;

// Some variables used as defaults for the command line class
static const char USAGE[]      = "optbounds -inv inventory [-xext xml_extension] [-bext backup_extension] [-b] [-aext audio_file_extension] -list filelist | xmlfile1, xmlfile2, ...";

unsigned searchNearestZero(const t_Signal & signal, const unsigned & pos, const unsigned & range)
{
	int i;
	bool lzero(false);
	unsigned p=min(pos, signal.size()-1);
	unsigned l=max(1, int(pos-range));
	for(i=p; i>=int(l); i--) {
		if(signal[i]>=0 && signal[i-1]<0) {
			lzero = true;
			break;
		}
	}
	if(lzero && signal[i]<signal[i-1]) l = i; else l = i-1;
	
	bool rzero(false);
	unsigned r=min(signal.size()-2, pos+range);
	for(i=p; i<int(r); i++) {
		if(signal[i]>=0 && signal[i+1]<0) {
			rzero = true;
			break;
		}
	}
	if(rzero && signal[i]<signal[i+1]) r = i; else r = i+1;
	
	if(lzero  && !rzero)
		return l;
	if(!lzero && rzero)
		return r;
	if(lzero && rzero) {
		if(pos-l < r-pos)
			return l;
		else
			return r;
	}
	return pos;
}

void setBounds(xercesc::DOMNode* node, const t_Signal & signal)
{
	static char tmp[20];
	if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
		string sleft  = BOSS::getAttrByName(node, "First");
		string sright = BOSS::getAttrByName(node, "Last");
		if(sleft.size() && sright.size()) {
			unsigned left  = atoi(sleft.c_str());
			unsigned right = atoi(sright.c_str());
			left  = searchNearestZero(signal, left,  100);
			right = searchNearestZero(signal, right, 100);
		
			{
				xercesc::DOMElement* element = (xercesc::DOMElement *) node;
 				sprintf(tmp, "%d", left);
 				BOSS::setAttribute(element, "First", tmp);
 			}
			{
				xercesc::DOMElement* element = (xercesc::DOMElement *) node;
 				sprintf(tmp, "%d", right);
 				BOSS::setAttribute(element, "Last", tmp);
 			}
 		}
	}
	xercesc::DOMNodeList* nl = node->getChildNodes();
	for(unsigned i=0; i<nl->getLength(); i++) {
	  xercesc::DOMNode* tmp = nl->item(i);
		setBounds(tmp, signal);	
	}
}

void optimizeBounds(const string & xml_file, const string & backup_file, const string & signal_file)
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

	ifstream in(signal_file.c_str());
	if(!in) {
		cerr << "Can't open " << signal_file << endl;
		exit(-1);
	}
	
	// Try reading the signal from whole stream in
	t_Signal * signal(0);
	try {
		in.seekg(0, ios::end);
		unsigned size = in.tellg() / 2;
		in.seekg(0, ios::beg);
		signal = new t_Signal(in, size);
	}
	catch(const dsp::AllocationError & e) {
	  cerr << e << endl;
	}
	catch(const dsp::StreamError & e) {
	  cerr << e << endl;
	}

  try {
	// Parse the XML file, catching any XML exceptions that might propagate out of it.
        doc = parser->parseURI(xml_file.c_str());

		setBounds(doc, *signal);

		// Make a backup copy, if a backup name is given
	  if(backup_file.size())
  		rename(xml_file.c_str(), backup_file.c_str());

  	// Open the file to store the result in
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

	if(signal) delete signal;
}

int main(int argc, char *argv[])
{
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

	// Input files are given as command line argument or in a file
	// indicated by the "-list" option
	vector< string > files;
	{
		if(cl("-list").optionflag) {
			ifstream in(cl("-list").optionarg.c_str());
			if(!in) {
				cerr << "Cant open " << cl("-list").optionarg << endl;
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
			cerr << "You have to enter at least one filename" << endl;
	}

  bool backup = cl("-b").optionflag;
  for(unsigned i=0; i<files.size(); i++) {
  	cerr << files[i] << endl;
  	string xml_file    = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-xext").optionarg));
  	string audio_file    = BOSS::makeFilePath(cl("-adir").optionarg, string(files[i] + cl("-aext").optionarg));
  	if(backup) {
	  	string backup_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-bext").optionarg));
	  	optimizeBounds(xml_file, backup_file, audio_file);
	  }
	  else {
	  	optimizeBounds(xml_file, "", audio_file);
	  }
  }

  return EXIT_SUCCESS;
}
