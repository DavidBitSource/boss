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

/**
 * @file PreprocScanner.h
 * @author Michal Holcik
 * @author Thomas Portele
 */

#ifndef _PPSCANNER_H_
#define _PPSCANNER_H_


#include "preproc/PPRullst.h"
#include "preproc/PPInput.h"
#include "preproc/PPAbblst.h"
#include "preproc/PPFnclst.h"
#include <unistd.h>
#include "InputScanner.h"
#include "Token.h"

#include <queue>
#include <map>
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

/**
 * @addtogroup boss_client
 * @{
 */

/// @brief The PreprocScanner class is an implementation of the InputScanner for the 
/// BOSS client. It is a wrapper class to the functions of the preproc
/// library and uses the code from preproc main() function.
class PreprocScanner: public InputScanner
{
protected:

  PPFunctionlist fl;
  PPRulelist *rl;
  PPAbbreviationlist *al;
  PPInput input;
  FILE *outfile;
  queue < Token > tokenQueue;
  void checkRulesDefinition (char *xinput, int g);
  void checkAbbrevDefinition (char *xinput, int g);

public:
    PreprocScanner (const char *rulefilename, const char *abbfilename, const char *infilename);
    ~PreprocScanner ();
  bool scan ();
  Token getToken ();

};

/** @} */ // end of boss_client group
#endif

