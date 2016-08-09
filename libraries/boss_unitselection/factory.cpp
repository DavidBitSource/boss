/* factory.cpp    $Revision: 1.1 $ $Date: 2006/03/26 13:08:31 $
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
 * @file factory.cpp
   @author Michal Holcik
*/

using namespace std;

#include "factory.h"

typedef UnitSelectionFactory * create_factory();
typedef void destroy_factory(UnitSelectionFactory *);

/**
 * @brief Creates a new instance of a UnitSelectionFactory dependent on a language of choice
 * @param lang language identificator
 * @return instance of UnitSelectionFactory
 * Currently creates only instances of UnitSelectionFactoryDE despite of the lang parameter.
 */

static char * dlerrordup (char *errormsg);

UnitSelectionFactory * UnitSelectionFactory::getFactory(const string name) {
	char * errormsg = NULL;


  string libname = "libunitselectionfactory" + name;
  //load factory implementation
  cerr << "UnitSelectionFactory::getFactory loading: " << libname << endl;
  lt_dlhandle lib = lt_dlopenext(libname.c_str());
  if (!lib) {
    errormsg = dlerrordup (errormsg);
    cerr << "Cannot load library: " << errormsg << endl;
    return 0;
  }
  
  std::cerr << "Retrieving library symbols." << std::endl;
  // load the symbols
  create_factory * create = (create_factory*) lt_dlsym(lib, "create_fac");

  // the destroy function is by current design not so necessary
  //  destroy_module * destroy = (destroy_module*) lt_dlsym(lib, "destroy");
  if (!create ) { //|| !destroy) {
    errormsg = dlerrordup (errormsg);
    cerr << "Cannot load symbols: " << errormsg << endl;
    return 0;
  }

  std::cerr << "Creating UnitSelectionFactory class instance" << std::endl;
  // create an instance of the class
  UnitSelectionFactory * factory = create();
  return factory;
}

/* since the  next API call may overwrite the original. */
static char * dlerrordup (char *errormsg)
{
  char *error = (char *) lt_dlerror ();
  if (error && !errormsg)
    errormsg = strdup (error);
  return errormsg;
}



