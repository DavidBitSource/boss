/* main.cpp (traing2p)    $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
   Copyright (C) 2002 University of Bonn.
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

#include "traing2p.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include "boss_utility/boss_config.h"

using namespace std;

static const char USAGE[] = "traing2p -lex lexfile -phon phonfile -out outfile";

int main(int argc, char *argv[])
{
 BOSS::Config cl(argc, argv, USAGE); // Process the command line options
  
  if (cl("-g2p").optionflag || cl("-syll").optionflag || cl("-stress").optionflag) {
    bool createflag=0;
    if (cl("-create").optionflag) createflag=1;
    if (cl("-g2p").optionflag) Traing2p gptrain(cl, "phon",createflag,cl("-g2pphon").optionarg,cl("-g2ptree").optionarg,cl("-lex").optionarg,cl("-g2pfeat").optionarg,BLF);
    if (cl("-syll").optionflag) Traing2p sylltrain(cl, "syll",createflag,cl("-syllphon").optionarg,cl("-sylltree").optionarg,cl("-lex").optionarg,cl("-syllfeat").optionarg,BLF);
    if (cl("-stress").optionflag) Traing2p stresstrain(cl, "stress",createflag,cl("-stressphon").optionarg,cl("-stresstree").optionarg,cl("-lex").optionarg,cl("-stressfeat").optionarg,BLF);
  }
  else {
    cerr << "No training option specified. Exiting!" << endl;
  }
  return EXIT_SUCCESS;
}
