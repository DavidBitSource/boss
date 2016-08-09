/* boss_synthesis.h    $Revision: 1.5 $ $Date: 2007/04/16 09:44:37 $
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
 * @file boss_synthesis.h
   @author Karlheinz Stoeber
   @author Stefan Breuer
*/


#ifndef BOSS_SYNTHESIS_H
#define BOSS_SYNTHESIS_H

//#include "boss_conman/boss_conman.h"

#include "boss_utility/dom_tools.h"
#include "boss_utility/boss_module.h"
#include "boss_utility/boss_types.h"
#include "tdsp/dsp_weight_window.h"
#include "tdsp/dsp_vector.h"
#include "boss_mysql/boss_mysql_handler.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/dom_userdata.h"
#include "fstream"
#include "ltdl.h"

using namespace std;

/**
 * @defgroup boss_server BOSS Server
 * @{
 */

typedef dsp::vector< short > t_Signal;
/**
 *
 * The BOSS_Synthesis class is the main object for synthesis. It contains all
 * necessary information about the environment where the synthesis program is running.
 * Also, it is used to set up the modules needed by the synthesis.
 * The operator () calls all modules. Each module used from this has to define the
 * same () operator.
 * The result of the synthesis is contained in the variable "sentence". The subtree given
 * by "sentence" contains all information computed during synthesis. Each node of this tree
 * contains a special node. This node is a pointer to the class DOM_UserData. The whole
 * sentence signal is contained in sentence.getUserData()->signal. Because
 * sentence.getUserData() returns a void pointer, it is necessary to cast this pointer before
 * using it.
 *
 *@author Karlheinz Stoeber
 *@author Stefan Breuer
 */
 
class BOSS_Synthesis {
 protected:
  typedef pair<string, BOSS::Module *> ModulePointer;
  typedef vector<ModulePointer> Scheduler;
  typedef map<string, pair< Scheduler, MySQL_Handler * > > InvMap;
  InvMap invmap; 
  Scheduler * scheduler;
  MySQL_Handler * data_base;
  BOSS::Config & cl;
  //  BOSS_ConMan * con_man;
  
  void initModules(const string & inv);
  BOSS::Module * load(const string & name);
  void setInventory(xercesc::DOMNode * sentence);
  
 public:
  
  BOSS_Synthesis(BOSS::Config & c);
  t_Signal operator () (xercesc::DOMNode * sentence);
  ~BOSS_Synthesis();
};

/** @} */ // end of boss_server group
#endif
