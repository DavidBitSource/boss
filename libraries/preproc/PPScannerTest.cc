/* Copyright (C) 1998  Thomas Portele

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Thomas Portele
tpo@ikp.uni-bonn.de http://www.ikp.uni-bonn.de/~tpo
IKP Poppelsdorfer Allee 47, D-53115 Bonn Germany

__________________________

*/

#include <unistd.h>
#include <queue>
#include <map>
#include <string>
#include <iostream>

#include "PPScanner.h"

FILE *errfile;
//#define errfile stderr

using namespace std;

#define SCAN_WORD 0
#define SCAN_PHRASE_DELIMITER 1
#define SCAN_SENTENCE_DELIMITER 2

int
main (int argc, char **argv)
{

  errfile = stderr;

  char *
    rulesfilename = NULL;
  char *
    abbrfilename = NULL;
  char *
    infilename = "-";
  char *
    outfilename = NULL;

    if (argc < 2) {
	fprintf(errfile,
		"Usage: preproc <Rulelist> <Abbreviationfile-optional> \
	       | read from stdin, write to stdout\n");
	return 1;
    }
  
    rulesfilename = argv[1];
  if (argc > 2)
  {
      abbrfilename = argv[2];
  }

  PPScanner
  scanner (rulesfilename, abbrfilename, infilename, outfilename);
  while (scanner.scan ())
  {
    Token
      token = scanner.getToken ();
    cout << "token data: " << token.first << " " << token.second << endl;
  }
  return 0;
}


