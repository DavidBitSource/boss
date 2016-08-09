/* boss_synthesis.cpp    $Revision: 1.7 $ $Date: 2006/03/26 13:03:29 $
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
 * @file boss_synthesis.cpp
   @author Stefan Breuer
   @author Michal Holcik
*/

#include "boss_synthesis.h"

/**
 * @addtogroup boss_server
 * @{
 */

BOSS_Synthesis::BOSS_Synthesis(BOSS::Config & c): cl(c), scheduler(NULL)
{
  cerr << "BOSS_Synthesis constructor" << endl;
  BOSS::t_SVector inventories = c.listInv();
  for (unsigned i=0;i<inventories.size();i++) {
    string inv = inventories[i];
    cerr << "Setting up configuration for inventory \"" << inv << "\":" << endl;
    if (cl.setInv(inv))
      initModules(inv);
    else {
      cerr << "No configuration options defined for inventory - exiting!"
	   << endl;
      exit(EXIT_FAILURE);
    }
  }
}

void BOSS_Synthesis::initModules(const string & inv) {
  InvMap::iterator it;
  it = invmap.find(inv);
  if(it!=invmap.end()) {
    cerr << "Module order for inventory is already defined - skipping!"
	 << endl;
    return;
  }
  ifstream in;
  if (cl("-modules").optionarg != "") {
    in.open(cl("-modules").optionarg.c_str());
    if (!in) {
      cerr << "No (valid) module order file specified for inventory - exiting!"
	   << endl;
      exit(EXIT_FAILURE);
    }
  } else {
      cerr << "Empty \"modules\" parameter provided, exiting" << endl;
      exit(EXIT_FAILURE);
  }
  
  // Initialise the database connection
  try {
    cerr << "Initialising the database connection - " ;
    data_base = new MySQL_Handler(cl("-h").optionarg,cl("-u").optionarg,cl("-p").optionarg,cl("-db").optionarg);
    cerr << "OK" << endl;
  }
  catch (const MySQL_ConnectFailed & e) {
    e.printError(true);
    cerr << "Cannot establish database connection - exiting!"
	 << endl;
    in.close();
    exit(EXIT_FAILURE);
  }
  /* Initialise libltdl. */
  if (lt_dlinit()) {
    cerr << "BOSS_Synthesis: couldn't initialise libtool." << endl;
    in.close();
    delete data_base;
    exit(EXIT_FAILURE);
  }
  Scheduler s;
  string name;
  while(getline(in, name)) {
	cerr << "name = " << name << endl;
    if (name != "") {
      BOSS::Module * module = load(name);
      if (module != NULL) {
	cerr << module->getDescription() << endl; //NOTE: this line can be deleted, mho
	ModulePointer mp(name,module);
	s.push_back(mp);
      } else {
	      cerr << "got an error while loading the module " << name << endl;
      }
    }
  }
  invmap[inv]=pair<Scheduler, MySQL_Handler *>(s,data_base);
  in.close();
}

static char * dlerrordup (char *errormsg);

BOSS::Module * BOSS_Synthesis::load(const string & name) {
  char * errormsg = NULL;
  //load the module library
  cerr << "BOSS_Synthesis: loading " << name << endl;
  lt_dlhandle lib = lt_dlopenext(name.c_str());
  if (!lib) {
    errormsg = dlerrordup (errormsg);
    cerr << "Cannot load library: " << errormsg << endl;
    return 0;
  }
  
  std::cerr << "retrieving library symbols" << std::endl;
  // load the symbols
  create_module * create = (create_module*) lt_dlsym(lib, "create");

  // the destroy function is by current design not so necessary
  //  destroy_module * destroy = (destroy_module*) lt_dlsym(lib, "destroy");
  if (!create ) { //|| !destroy) {
    errormsg = dlerrordup (errormsg);
    cerr << "Cannot load symbols: " << errormsg << endl;
    return 0;
  }
  
  std::cerr << "creating module class instance" << std::endl;
  // create an instance of the class
  BOSS::Module * module = create(cl, data_base);
  return module;
}

t_Signal BOSS_Synthesis::operator () (xercesc::DOMNode * sentence)
{
  setInventory(sentence);
  if (scheduler != NULL) {
    for (unsigned i=0; i<scheduler->size(); i++) {
      ModulePointer mp = (*scheduler)[i];
      BOSS::Module * module = mp.second;
      (*module)(sentence);
      if (cl("-v").optionflag) cerr << mp.first << " finished" << endl;
    }
   
  }
  
  t_Signal signal;
    DOM_UserData * user_data = (DOM_UserData*)BOSS::getUserData(sentence);
    if (user_data != 0) {
	    signal = user_data->getSignal();
	    if (cl("-v").optionflag) cerr << "Successful extraction of t_Signal signal" << endl;
   }
   return signal;
}

void BOSS_Synthesis::setInventory(xercesc::DOMNode * sentence) {
  if (BOSS::hasAttribute(sentence, "Inv")) {
    cerr << "Inventory \"" << BOSS::getAttrByName(sentence, "Inv")
	 << "\" requested - ";
    if (cl.setInv(BOSS::getAttrByName(sentence, "Inv")))
      cerr << " OK!" << endl;
    else 
      cerr << " Not available!" << endl;
  } 
  //  else cl.setInvToDef();
  cerr << "Using inventory \"" 
       << cl.getInvName() << "\" ("
       << cl.getLang() << ")." << endl;
  InvMap::iterator iit;
  iit = invmap.find(cl.getInvName());
  scheduler = &(*iit).second.first;
  data_base = (*iit).second.second;
}


/* since the  next API call may overwrite the original. */
static char * dlerrordup (char *errormsg)
{
  char *error = (char *) lt_dlerror ();
  if (error && !errormsg)
    errormsg = strdup (error);
  return errormsg;
}

BOSS_Synthesis::~BOSS_Synthesis() {
  //Delete all database handler pointers:
  InvMap::iterator iit = invmap.begin();
  while (iit != invmap.end()) {
    MySQL_Handler * db = (*iit).second.second;
    delete db;
    iit++;
  }
}

/** @} */ // end of boss_server group
