/* main.cpp (melbounds) $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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
#include <sstream>
#include <cstdio>
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "tdsp/dsp_vector.h"
#include "tdsp/dsp_melcep.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/dom_tools.h"

typedef dsp::vector< short > t_Signal;

static const char USAGE[]      = "melbounds -inv inventory [-xext xml_extension] [-bext backup_extension] [-b] [-aext audio_file_extension] -list filelist | xmlfile1, xmlfile2, ...";

static dsp::MelCepstrum< double > melcep(24, 0, 8000, 1024, 16000, 12);
static dsp::vector< double > tmp(512);
static char buf[100];
static char lattr_name[10], rattr_name[10];

vector< string > getMelSpec(const t_Signal & signal, const unsigned & pos)
{
	
	unsigned l = pos - 256;
	unsigned r = pos + 256;
	if(pos<256) {
		l = 0;
		r = 512;
	}
	if(pos>signal.size()-256) {
		l = signal.size()-512;
		r = signal.size();
	}
	for(unsigned j=0, i=l; i<r; j++, i++)
		tmp[j] = signal[i];
	dsp::vector< double >  mcv = melcep(tmp);
	
	vector< string > res;
	for(unsigned i=0; i<mcv.size()-1; i++) {
		sprintf(buf, "%3.2f", mcv[i]);
		res.push_back(buf);
	}
	return res;
}

void setBounds(xercesc::DOMNode* node, const t_Signal & signal)
{
  static string First("First");
  static string Last("Last");
  static unsigned left, right;
  static string sleft, sright;
  static vector< string > melleft, melright;
  if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
    sleft  = BOSS::getAttrByName(node, First);
    sright = BOSS::getAttrByName(node, Last);
    if(sleft.size() && sright.size()) {
      left   = atoi(sleft.c_str());
      right  = atoi(sright.c_str());
      melleft  = getMelSpec(signal, left);
      melright = getMelSpec(signal, right);
      
      xercesc::DOMElement* element = (xercesc::DOMElement *) node;
      for(unsigned i=0; i<melleft.size(); i++) {
	sprintf(lattr_name, "LM%d", i);
	sprintf(rattr_name, "RM%d", i);
	BOSS::setAttribute(element, lattr_name, melleft[i].c_str());
	BOSS::setAttribute(element, rattr_name, melright[i].c_str());
      }
    }
  }
  xercesc::DOMNodeList* nl = node->getChildNodes();
  for(unsigned i=0; i<nl->getLength(); i++) {
    xercesc::DOMNode* tmp = nl->item(i);
    setBounds(tmp, signal);
  }
}

void melBounds(const string & xml_file, const string & backup_file, const string & signal_file)
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

	// Attempt to read the signal from entire stream
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
		delete signal;

	// Make a backup copy if a backup name is given
	  if(backup_file.size())
  		rename(xml_file.c_str(), backup_file.c_str());

  	// Open the file to store the result in
  	ofstream out(xml_file.c_str());
  	if(!out) {
  		cerr << "Can't open file " << xml_file << endl;
  		return;
  	}

  	// Write out the new xml structure
		out << doc << endl;
	}
  catch (const xercesc::XMLException& e) {
  	cerr << "An error occured during parsing\n   Message: "
         << e.getMessage() << endl;
  }
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
  	string adc_file    = BOSS::makeFilePath(cl("-adir").optionarg, string(files[i] + cl("-aext").optionarg));
  	if(backup) {
	  	string backup_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-bext").optionarg));
	  	melBounds(xml_file, backup_file, adc_file);
	  }
	  else {
	  	melBounds(xml_file, "", adc_file);
	  }
  }
  	
  return EXIT_SUCCESS;
}

