/* main.cpp (boss_numbers)   $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   @author Joerg Broeggelwirth
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include "ordinal.h"

const char* numb_tab_file("../../data/runtime/de/nominal_word_tab.txt");
const char* ordinal_tab_file("../../data/runtime/de/ordinal_word_tab.txt");
//const char* zahl_file = "../data/runtime/de/test/numbers.txt";

int main(int argc, char *argv[])
{
  // local variables
  string user_number("");
  char opt_flag='h';
  bool numb_flag=false;
  // control of the commands corresponding to the command line options
  if ((argc==2)||(argc==3)) {
    string arg_1=argv[1];
    if ((arg_1=="-o")||(arg_1=="--orthography")) opt_flag='o';
    if ((arg_1=="-t")||(arg_1=="--transcription")) opt_flag='t';
    if ((arg_1=="-a")||(arg_1=="--all")) opt_flag='a';
    if (argc==3) {
      user_number=argv[2];
      numb_flag=true;
    }
  }
  else opt_flag='h';
  // output of user information in case of option "-h" or error
  if (opt_flag=='h') {	
    cerr << "Benutzung: boss_numbers OPTION [NUMBER]\n"
	 << "  -o, --orthography\torthographische Ausgabe\n"
	 << "  -t, --transcription\ttranskriptorische Ausgabe\n"
	 << "  -a, --all\t\tgibt sowohl Orthographie, als auch Transkription aus\n"
	 << "  -h, --help\t\tgibt diese Information aus\n\n"
	 << "Report bugs to <jbr@ikp.uni-bonn.de>.\n";
  }
  else
  {
    // local variables
    string input_number="";
    // class BOSS_Numbers or ordinal
    BOSS_Numbers *z(NULL);
    //z = new BOSS_Numbers();
    z = new nominal(numb_tab_file);
    nominal test(numb_tab_file);
    ordinal test2(ordinal_tab_file, numb_tab_file);
    //z = new ordinal(ordinal_tab_file, numb_tab_file);
    // the number to be converted is taken from the commandline if "numb_flag==true"
    if (numb_flag) {
      input_number=user_number;
      switch (opt_flag) {
        //case 'o' : cout << (*z)(input_number).first << endl; break;
	// Test of the "roman" function
        case 'o' : cout << test.roman(input_number).first << endl; break; 
        case 't' : cout << (*z)(input_number).second << endl; break;
        case 'a' : cout << (*z)(input_number).first << "\t"
			<< (*z)(input_number).second << endl;
      }
    }
    else {
      // if "numb_flag==false", take the number to be converted from stdin
      //ifstream source(zahl_file);
      while (cin) {
	cin >> input_number;
	switch (opt_flag) {
	  case 'o' : cout << (*z)(input_number).first << endl; break;
	  case 't' : cout << (*z)(input_number).second << endl; break;
	  case 'a' : cout << (*z)(input_number).first << "\t"
			  << (*z)(input_number).second << endl;
	}
	input_number="";
      }
      //source.close();
    }
  }
  return EXIT_SUCCESS;
}
