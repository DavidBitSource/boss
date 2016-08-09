/* boss_config.cpp    $Revision: 1.7 $ $Date: 2006/04/05 10:12:58 $
   Copyright (C) 1998 University of Bonn.
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

#include "boss_utility/boss_config.h"

void BOSS::Config::print(ostream & s) const
{
  t_OptMap::const_iterator it;
  for (it = currOptMap->begin();it != currOptMap->end();it++) {
    s << (*it).second.option << " " << (*it).second.optionarg
      << " " << " " << (*it).second.optionflag
      << endl;
  }
}

void BOSS::Config::error(const string & s)
{
  if(usage.size()>0)
    cerr << "Usage: " << usage << endl;
  else
    cerr << "error: " << s << endl;
  exit(EXIT_FAILURE);
}

// Get the position of the configuration file. If this is not given by the commandline option -conf, 
// first, the current working directory is searched, then $HOME/.boss3/.
string BOSS::Config::getConfPath(t_StringList & h, string file) {
  t_StringList::iterator it;
  it = find(h.begin(), h.end(), "-conf");
  if(it!=h.end()) {
    it = h.erase(it);
    if (it!=h.end() && it->size() && (*it)[0] != '-') {
      if (fileExists(*it)) {
	file=*it;
	h.erase(it);
	return file;
      }
      cerr << "Warning: configuration file " << *it
	   << " not found!" << endl 
	   << "Searching default locations." << endl;
    }
    else cerr << "-conf needs a filename as argument!" << endl;
  }
  
  if (fileExists(file)) return file;
  char * env = getenv("HOME");
  if (env) {
    stringstream name;
    name << env << "/.boss3/" << file;
    ifstream in(name.str().c_str());
    string filename;
    if (in) filename = name.str();
    in.close();
    if (filename != "") return filename;
  }
  stringstream name;
  name << "/etc/boss3/" << file;
  ifstream in(name.str().c_str());
  if (in) file = name.str();
  else file = "";
  in.close();
  return file;
}

bool BOSS::Config::fileExists(string & file) {
  bool x = false;
  ifstream in(file.c_str());
  if (in) x = true;
  in.close();
  return x;
}

void BOSS::Config::readXMLConfig(const string & file) {
  xercesc::DOMDocument *doc = BOSS::parseXMLFile(file.c_str());
  xercesc::DOMNodeList * rootNL = BOSS::getElementsByTagName(doc,"CONFIGS");
  //  xercesc::DOMNodeList * rootNL = doc->getElementsByTagName(xercesc::XMLString::transcode("CONFIGS"));
  xercesc::DOMNodeList * invNL;
  if (rootNL->getLength()) {
    if (rootNL->getLength()>1) cerr << "Only one CONFIGS element is allowed per configuration file!" << endl;
  }
  else if (rootNL->getLength()<1) {
    cerr << "No configuration profiles found in " << file << endl;
    exit(EXIT_FAILURE);
  }
  xercesc::DOMElement * rootElement = (xercesc::DOMElement *)rootNL->item(0);
  defInv = BOSS::getAttrByName(rootElement, "DefaultInventory");
  if (defInv == "") {
  	cerr << "No default inventory specified. Exiting!" << endl;
	exit(EXIT_FAILURE);
  }
  cerr << "Using " << defInv << " as default inventory." << endl;
  invNL = BOSS::getElementsByTagName((xercesc::DOMElement*)rootNL->item(0),"INVENTORY");
  t_OptMap * omap;
  for (unsigned i=0;i<invNL->getLength();i++) {
    xercesc::DOMElement * inventory = (xercesc::DOMElement *)invNL->item(i);
    currInv = BOSS::getAttrByName(inventory, "Name");
    currLang = BOSS::getAttrByName(inventory, "Lang");
    omap = new t_OptMap();
    t_InvPair p = t_InvPair(currLang,omap);
    invVec.push_back(currInv);
    invMap[currInv] = p;
      // invMap[currInv] = t_InvPair(currLang,omap);
    xercesc::DOMNodeList * optNL = BOSS::getElementsByTagName((xercesc::DOMElement*)invNL->item(i),"OPTION");
    for (unsigned j=0;j<optNL->getLength();j++) {
      xercesc::DOMElement * optElement = (xercesc::DOMElement *)optNL->item(j);
      string optName = BOSS::getAttrByName(optElement, "Name");
      string optValue = BOSS::getAttrByName(optElement, "Value");
      string optType = BOSS::getAttrByName(optElement, "Type");
      string optFlag = BOSS::getAttrByName(optElement, "Flag");
      if (optType == "") {
      	cerr << "No option type defined for " << optName << "! Exiting." << endl;
	exit(EXIT_FAILURE);
      }
      bool type(false),flag(false);
      if (optType=="1") type = true;
      if (optFlag=="1" || optValue != "") flag = true;
      BOSS::t_OptItem opt(optName,optValue,type,flag);
      (*omap)[optName] = opt;
    }
  }
  if (!setInv(defInv)) {
    cerr << "No options defined for default inventory: " << defInv 
	 << " specified in " << file << ". Exiting!" << endl;
    exit(EXIT_FAILURE);
  }
  cerr << "Loaded options for the following inventories: ";
  t_InvMap::const_iterator it = invMap.begin();
  while (it != invMap.end()) {
  	cerr << (*it).first << " (" << (*it).second.first << ")  ";
  	it++;
  }
  cerr << endl;
}

bool BOSS::Config::setInv(const string & inv) {
  t_InvMap::const_iterator it;
  if (!inv.size()) return false;
  it = invMap.find(inv);
  if(it==invMap.end()) return false;
  currOptMap = (*it).second.second;
  currLang = (*it).second.first;
  currInv = inv;
  return true;
}

bool BOSS::Config::setInvtoDef() {
  if (setInv(defInv)) return true;
  return false;
}

bool BOSS::Config::procInvPrefix(string & arg) {
  size_t pos = arg.find(':');				//long in order to work on 64 bit systems
  if (pos == string::npos) return true;
  if (pos == 0 || pos == arg.size()-1 || setInv(string(arg,0,pos))) return false;
  arg = string(arg,pos+1,arg.size());
  return true;
}

// Here, the basic idea is first to create a list of all arguments contained in
// argv. Then, for each argument name in v, the corresponding value in the list is
// located. If an argument name was found in the list, it will be copied to the map
// barg and deleted from the list. All remaining items in the list are so-called
// free arguments. These items are copied to the vector farg.

BOSS::Config::Config(int argc, char ** argv, const string & s) : usage(s)
{
  BOSS::initXML();
  t_StringList args;
  // Create a list of all arguments of the command line
  for(int i=1; i<argc; i++)
    args.push_back(string(argv[i]));
  string file;
  // Get path and name of the configuration file
  if ((file = getConfPath(args, "boss3conf.xml")) == "") {
    cerr << "No configuration file found. Terminating!" << endl;
    exit(EXIT_FAILURE);
  }
  cerr << "Using configuration from " << file << endl;
  // Read options from configuration file
  readXMLConfig(file);
  
  // Iterate through all arguments of the command line
  t_StringList::const_iterator li = args.begin();
  while (li != args.end()) {
    // If the string starts with a "-" and is longer than 1 character, it
    // is considered an option string. Otherwise, it is appended to the vector of free
    // commandline arguments
    if ((*li)[0] == '-' && (*li).size() > 1) {
      // Strip option of '-'.
      string as;
      as.assign(*li,1,(*li).size());
      if (!procInvPrefix(as)) {
	cerr << "Option \"" << as << "\" refers to unknown inventory. Exiting! " << endl;
	exit(EXIT_FAILURE);
      }
      t_OptMap::const_iterator it;
      it = currOptMap->find(as);
      // If the option is found in currOptMap it is set according
      // to the command line argument.
      if(it!=currOptMap->end()) {
	// If the option takes an argument, the value is inserted into the map,
	// and the option flag is set to "true". 
	// Otherwise, only the flag is set.
//	if ((*it).second.optionarg != "" && (*it).second.optionflag == true) {
	if ((*it).second.optiontype) {
	  // Check that the option's value is supplied. If not, warn!
	  t_StringList::const_iterator lin = li;
	  if (++lin != args.end()) {
	    if ((*lin)[0] != '-') {
	      BOSS::t_OptItem opt(as,*lin,false,true);
	      (*currOptMap)[as] = opt;
	      li++;
	    } else cerr << *li << " expects an argument!" << endl;
	  } else cerr << *li << " expects an argument!" << endl;
	}
	else {
	  BOSS::t_OptItem opt(as,"",false,true);
	  (*currOptMap)[as] = opt;
	}
      }
      else {
	cerr << "Unknown option: " << *li << endl;
      }
    }
    else farg.push_back(*li);
    li++;
    setInv(defInv);
  }
}

BOSS::Config & BOSS::Config::operator = (const BOSS::Config & C)
{
  farg  = C.farg;
  invMap = C.invMap;
  invVec = C.invVec;
  currOptMap  = C.currOptMap;
  currInv = C.currInv;
  currLang = C.currLang;
  defInv = C.defInv;
  usage = C.usage;
  return *this;
}

BOSS::t_OptItem BOSS::Config::operator () (string opt) const
{
  if (opt.size() && opt[0] == '-')
    opt.assign(opt,1,opt.size());
  t_OptMap::const_iterator it;
  it = currOptMap->find(opt);
  if(it==currOptMap->end())
    return BOSS::t_OptItem();
  return (*it).second;
}

string BOSS::Config::operator [] (const unsigned i) const
{
  if(i<farg.size())
    return farg[i];
  return string();
}
