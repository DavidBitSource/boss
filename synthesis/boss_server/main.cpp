/* main.cpp (boss_server)   $Revision: 1.5 $ $Date: 2006/03/26 13:03:29 $
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
   @author Stefan Breuer
*/

using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <boss_utility/boss_utility.h>
#include <boss_utility/boss_config.h>
#include "boss_synthesis.h"
#include <boss_utility/dom_tools.h>
#include <boss_utility/dom_userdata.h>

#include "boss_cscomm/boss_cscomm.h"

typedef dsp::vector< short > t_Signal;
MySQL_Handler * data_base(0);

static const char USAGE[]      = "usage";

void freeUserData(xercesc::DOMNode * node)
{
  DOM_UserData * p;
  if((p = (DOM_UserData*)BOSS::getUserData(node)) != 0) {
    delete p;
    BOSS::setUserData(node, NULL);
  }
  else {
    xercesc::DOMNodeList* nl = node->getChildNodes();
    for(unsigned i=0; i<nl->getLength(); i++) {
      xercesc::DOMNode* tmp = nl->item(i);
      freeUserData(tmp);
    }
  }
}

int main(int argc, char *argv[])
{ 
  BOSS::Config cl(argc, argv, USAGE);
  // Initialize the XML system.
  BOSS::initXML();  

  // This is the main block where the synthesis is done.
  try {
    // Create the synthesis object. The synthesis object handles all module calls
    // necessary to compute the speech signal.
    cerr << "Initializing the BOSS_Synthesis object" << endl;
    BOSS_Synthesis synthesis(cl);
    // display weights
    cerr << " fac_dur: " << atof(cl("-fac_dur").optionarg.c_str())
	 << " ucw_dur: " << atof(cl("-ucw_dur").optionarg.c_str())
         << " ucw_intonation: " << atof(cl("-ucw_intonation").optionarg.c_str())
	 << " ucw_stress: " << atof(cl("-ucw_stress").optionarg.c_str())
	 << " ucw_phrase: " << atof(cl("-ucw_phrase").optionarg.c_str())
	 << " ucw_treal: " << atof(cl("-ucw_treal").optionarg.c_str()) << endl;
    cl.print(cerr);
    //Create a t_Signal object to store the output of the synthesis (sentence-wise)
    t_Signal signal;
    cerr << cl("-file").optionflag << endl;
    //Switch between network and file mode
    if (cl("-file").optionflag) {
      cerr << "Reading from file " << cl[0].c_str() << "." <<endl;
      xercesc::DOMDocument* doc = BOSS::parseXMLFile(cl[0].c_str());
      if (doc == NULL) {
	      cerr << "No such file " << cl[0].c_str() << "." <<endl;
	      return EXIT_FAILURE;
      }
      BOSS::Fileinfo file = BOSS::getFileinfo(cl[0]);
      file.base.append("_" + BOSS::getDate());
      file.base.append("_" + BOSS::getDate());
      // Get all nodes with tag name "SENTENCE".
      xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "SENTENCE");
      for(unsigned i=0; i<nl->getLength(); i++) {
	xercesc::DOMNode* sentence = nl->item(i);
	xercesc::DOMElement* element = (xercesc::DOMElement *) sentence;
	stringstream is,sname;
	is << i+1;
	if (!BOSS::getAttrByName(sentence,"SentenceID").size())
	  BOSS::setAttribute(element, "SentenceID", is.str().c_str());
	if (!BOSS::getAttrByName(sentence,"TargetPath").size())
	  BOSS::setAttribute(element, "TargetPath", file.path.c_str());
	if (!BOSS::getAttrByName(sentence,"TargetFile").size())
	  BOSS::setAttribute(element, "TargetFile", file.base.c_str());
	// Call the synthesis for each sentence. The variable "sentence" is regarded as
	// root node of a subtree containing the description of a sentence.
	signal = synthesis(sentence);
	sname << BOSS::getAttrByName(sentence, "TargetPath")
	      << "/"
	      << BOSS::getAttrByName(sentence, "TargetFile")
	      << "_"
	      << BOSS::getAttrByName(sentence, "SentenceID");
	if (cl("-stdout").optionflag) {
	  cerr << "Writing to stdout...";
	  signal.write(cout);
	} else {
	  ofstream raw(string(sname.str() + ".raw").c_str());
	  cerr << "Writing to file " << sname.str() << ".raw" << endl;
	  signal.write(raw);
	  raw.close();
	}
	if (cl("-lab").optionflag) BOSS::writeLabels(sentence, sname.str());
	freeUserData(sentence);
	cerr << "Signal size: " << (signal.size()*sizeof(short)) << " " << endl;
      }
      stringstream fname;
      fname << file.path
	    << "/"
	    << file.base;
      if (cl("-par").optionflag) {
	ofstream par(string(fname.str() + ".param").c_str());
	cl.print(par);
	par.close();
      }
      if (cl("-xml").optionflag) {
	ofstream xmlout(string(fname.str() + ".out.xml").c_str());
	xmlout << doc << endl;
	xmlout.close();
      }
      return EXIT_SUCCESS;
    }
    // Create CSComm object for client-server communication
    char server_name[BUFSIZ];
    if (gethostname(server_name, BUFSIZ)==-1) {
      cerr << "Bad server name" << endl;
      exit(EXIT_FAILURE);
    }
    BOSSCSComm handle(server_name, SERVER, atoi(cl("-port").optionarg.c_str()));
    // Endless loop: wait for network connections
    while (1) {
      cerr << endl << "Waiting for connections..." << endl;
      //Create a t_Signal object to store the whole signal in before it is sent to the client
      t_Signal serveroutput;
      //Open socket
      string ip = handle.acceptConnection();
      cerr << "Accepted connection on port " << cl("-port").optionarg << " from " << ip << " at " << BOSS::getDate() << "." << endl ; 
      //Receive XML document
      Document XMLinput = handle.receiveDocument();
      cerr << "Received document with id \"" << XMLinput.id << "\"." << endl;
      //Parse the XML document
      xercesc::DOMDocument * doc = BOSS::parseXMLFromMemory(XMLinput.doc);
      // Get all nodes with tag name "SENTENCE".
      xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "SENTENCE");
      for(unsigned i=0; i<nl->getLength(); i++) {
	cerr << "Processing sentence " << i + 1 << endl;
	xercesc::DOMNode* sentence = nl->item(i);
	// Call the synthesis for each sentence. The variable "sentence" is regarded as
	// root node of a subtree containing the description of a sentence.
	signal = synthesis(sentence);
	freeUserData(sentence);
	//Add signal for current sentence to signal vector
	serveroutput.cat(signal);
      } // for loop ends here
      	//Send signal vector "serveroutput" to client as char *
      cerr << "Sending signal with id \"" << XMLinput.id << "\" to " << ip << "." << endl;
      handle.sendSignal(XMLinput.id,serveroutput.getData(), serveroutput.size()*2);
      //Close network socket;
      handle.closeSock();
      cerr << "Closed connection to " << ip << " at " << BOSS::getDate() <<  "." << endl;
    } // while(1) loop ends here
  } // try block ends here
  catch (const xercesc::XMLException & e) {
    cerr << "An error occured during parsing\n   Message: "
	 << e.getMessage() << endl;
  }
  return EXIT_SUCCESS;
}
