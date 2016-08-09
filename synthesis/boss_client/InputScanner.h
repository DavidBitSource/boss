
/* Copyright (C) 

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
__________________________

*/

/**
 * @file InputScanner.h 
   @author Michal Holcik
*/

#ifndef _INPUTSCANNER_H_
#define _INPUTSCANNER_H_

#include <string>
#include "Token.h"

using namespace std;

/**
 * @addtogroup boss_client
 * @{
 */

// An abstract class defining a generic input scanner for boss client
// It follows the "enumeration" pattern

class InputScanner
{

public:
  // The scan method reads input and does the hasMoreElements enum method job
  virtual bool scan () = 0;
  // The getToken method returns current item
  virtual Token getToken () = 0;

};

/** @} */ // end of boss_client group
#endif

