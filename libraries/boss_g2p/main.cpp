/* main.cpp (boss_g2p_de)    $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/**
   @author Stefan Breuer
*/

using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include "boss_utility/boss_config.h"
#include "boss_g2p_de.h"

using namespace std;

static const char USAGE[] = "boss_g2p -lex lexfile -phon phonfile -out outfile";

int main(int argc, char *argv[])
{
  BOSS::Config cl(argc, argv, USAGE);
  BOSS_g2p_DE gp(cl, BLF);
  string str = cl[0];
  ifstream in(str.c_str());
  if (in)
  {
    while (getline(in,str)) cout << str << "\t" << gp.transcribe(str) << endl;;
    in.close();
  }
  else if (cl[0] != "") cout << gp.transcribe(cl[0]) << endl;
  return EXIT_SUCCESS;
}
