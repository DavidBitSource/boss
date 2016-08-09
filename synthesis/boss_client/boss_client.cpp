/* boss_client.cpp    $Revision: 1.8 $ $Date: 2005/12/29 15:52:11 $
   Copyright (C) 1999 University of Bonn.
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
 * @file boss_client.cpp
   @author Karlheinz Stoeber
   @author Stefan Breuer
   @author Joerg Broeggelwirth
*/

#include "boss_client.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "boss_cscomm/boss_cscomm.h"
#include "boss_numbers/ordinal.h"
#include "InputScanner.h"
#include "PreprocScanner.h"
#include "ClientXMLDocument.h"

using namespace std;

/**
 * @addtogroup boss_client
 * @{
 */

int main(int argc, char ** argv)
{
  BOSS::CommandLine cl;
   // Process the command line options
    const char USAGE[] = "boss_client [-xml] [-server server_name] [-port port_number] input_textfile";
    const char bossserver[] = "localhost";
    const char port[] = "2050";

    // set default arguments
    vector< BOSS::t_OptItem > vcl;
     // Set inventory
    vcl.push_back(BOSS::t_OptItem("-inv", "def", false));
    // Set server name
    vcl.push_back(BOSS::t_OptItem("-server", bossserver, false));
    // Set port number on server
    vcl.push_back(BOSS::t_OptItem("-port", port, false));
    // Set transaction id
    vcl.push_back(BOSS::t_OptItem("-id", "default", false));
    // Set output mode to either signal or XML
    vcl.push_back(BOSS::t_OptItem("-xml","", false));
    // Preproc rules files
    vcl.push_back(BOSS::t_OptItem("-preproc-rules","/usr/local/share/boss3/client/de/Rules.lst", false));
    vcl.push_back(BOSS::t_OptItem("-preproc-abbrev","/usr/local/share/boss3/client/de/Hadifix.abk", false));
    // debug flag
    vcl.push_back(BOSS::t_OptItem("-debug","", false));
    cl = BOSS::CommandLine(argc, argv, vcl, USAGE, 0, 1);

    // if there is any input file mentioned on the command line
    // try to open it to check if its there and available for reading
    // TODO: use exceptions for error handling
    // TODO: the file should be checked when we really need it
    // not here and not using this method
  if(cl[0].size()) {
    ifstream * in(0);
    in = new ifstream(cl[0].c_str());
    if(!in) {
      cerr << "Can't open file " << cl[0] << endl;
      exit(EXIT_FAILURE);
    }
    in->close();
  }
  BOSS_Client client(cl);
  client.run();
}

// gets current date and time and formats it for use as a timestamp 

string getDate() {
  time_t rawtime;
  struct tm * timeinfo;
  char form[] = "%y%m%d-%H%M%S";
  char res[20];
  int size;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  size = strftime(&res[0], 14, &form[0],timeinfo);
  return string(res);
}



void BOSS_Client::run() {
  
  string inputFileName = "-"; // read from standard input by default
  if (cl[0].size()) {
	  inputFileName = cl[0];
  }
  InputScanner * scanner = new PreprocScanner(cl("-preproc-rules").optionarg.c_str(),
				cl("-preproc-abbrev").optionarg.c_str(),
				inputFileName.c_str());
  Phrases phrases(1);
  
  //get tokens and push back in the phrases
  while (scanner->scan()) {
	  Token token = scanner->getToken();
	  processToken(token, phrases);
  }
  phrases.pop_back(); // removes the last empty phrase

  if (cl("-debug").optionflag) {
  	for (int i = 0; i < phrases.size(); i++)
  	{
		  for (int j = 0; j < phrases[i].size(); j++) {
			cout << phrases[i][j].first << " " << phrases[i][j].second << endl;
		  }
  	}
  }
 
  
  //dispatch the xml to the server or print it to the console
  ClientXMLDocument doc(cl, phrases);
  
  if (cl("-xml").optionflag) {
    cout << doc.getStringStream().str();
  } else {
    BOSSCSComm handle(cl("-server").optionarg, CLIENT, atoi(cl("-port").optionarg.c_str()));
    string id;
    if (cl("-id").optionarg != "default") id = cl("-id").optionarg;
    else id = "boss_client-linux-" + getDate();
    handle.sendDocument(id, doc.getStringStream().str());
    Signal res = handle.receiveSignal();
    cout.write((char*)res.signal,res.size);
  }
  
  delete scanner; 
}


// Take an action depending on what kind of token we have.
// i.e. place the tokens in the phrase and start a new sentence
// if we go over a sentence delimiter

// TODO: use an exception for unknown token indication
void BOSS_Client::processToken(Token & token, Phrases & phrases)
{
    int tag = token.first;
    switch(tag) {
    case TOKEN_WORD:
      phrases.back().push_back(token);
      break;
    case TOKEN_PHRASE_DELIMITER:
      phrases.back().push_back(token);
      break;
    case TOKEN_SENTENCE_DELIMITER:
      if ((phrases.back().back().first)==TOKEN_PHRASE_DELIMITER) {
	      phrases.back().pop_back();
      }
      phrases.back().push_back(token);
      phrases.push_back(Phrase(0));
      break;
    default:
      cout << "WARNING: unknown token, skipping: " << token.second << endl;
    }
}

/** @} */ // end of boss_client group
