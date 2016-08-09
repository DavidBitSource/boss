/* main.cpp (boss_transcription)   $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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
   author Stefan Breuer
*/
using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include "boss_mysql/boss_mysql_handler.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/dom_tools.h"
#include "boss_transcription_de.h"

MySQL_Handler * data_base(0);
static const char USAGE[]     = "blabla";

int main(int argc, char *argv[])
{
  BOSS::Config cl(argc, argv, USAGE); // Process the command line options

  unsigned tflag(7);
  if(cl("-e").optionflag)
    tflag -= 4;
  if(cl("-m").optionflag)
    tflag -= 2;
  if(cl("-g").optionflag)
    tflag -= 1;

  		
  // Initialize the XML system.
  BOSS::initXML();

  try {
    // Initialize the database connection
    data_base = new MySQL_Handler(cl("-h").optionarg, cl("-u").optionarg, cl("-p").optionarg, cl("-db").optionarg);

    BOSS_Transcription * transcription(NULL);

    transcription = new BOSS_Transcription_DE(cl, data_base);
    transcription->setFlag(tflag);
    // Input files are given as command line argument or in a file
    // indicated by the "-list option"
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
	cerr << "You have to enter at least one file" << endl;
    }
    for(unsigned i=0; i<files.size(); i++) {
      cerr << files[i] << endl;
      stringstream xml_file;
      xml_file << cl("-xdir").optionarg << "/" << files[i] << cl("-xext").optionarg;
      xercesc::DOMDocument * doc = BOSS::parseXMLFile(xml_file.str().c_str());
      xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "SENTENCE");
      for(unsigned j=0; j<nl->getLength(); j++) {
	xercesc::DOMNode* sentence = nl->item(j);
	// Call the synthesis for each sentence. The variable sentence is regarded as
	// root node of a subtree containing the description of a sentence.
	(*transcription)(sentence);
      }
      if (cl("-stdout").optionflag) {
	cout << doc << endl;
      }
      else {
	stringstream  output_file;
	output_file << cl("-tdir").optionarg << "/" << files[i] << cl("-text").optionarg;
	ofstream out(output_file.str().c_str());
	out << doc << endl;
	out.close();
      }
    }
  }
  catch (const MySQL_ConnectFailed & e) {
    e.printError(true);
  }
  catch (const xercesc::XMLException& e) {
    cerr << "An error occured during parsing\n   Message: "
         << e.getMessage() << endl;
  }

  delete data_base;
  	
  return EXIT_SUCCESS;
}
