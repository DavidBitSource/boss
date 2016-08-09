/* commandline.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
    A class for parsing and storing command line options.
    @author Karlheinz Stoeber
*/
using namespace std;

#include <commandline.h>

void BOSS::CommandLine::print(ostream & s) const
{
  t_OptItemMap::const_iterator it;
  for (it = barg.begin();it != barg.end();it++) {
    s << (*it).second.option << " " << (*it).second.optionarg 
      << " " << (*it).second.optiontype << " " << (*it).second.optionflag
      << endl;
  }
}

void BOSS::CommandLine::error(const string & s)
{ 
  if(usage.size()>0)
    cerr << "Usage: " << usage << endl;
  else
    cerr << "error: " << s << endl;
  exit(-1);
}


// Here, the basic idea is first to create a list of all arguments contained in
// argv. Then, for each argument name in v, the corresponding value in the list is
// located. If an argument name was found in the list, it will be copied to the map
// barg and deleted from the list. All remaining items in the list are so-called
// free arguments. These items are copied to the vector farg.
BOSS::CommandLine::CommandLine(int argc, char ** argv, const t_OptItemVector & v, const string & s,
			 const int fmin, const int fmax) : usage(s)
{
  int i;
  t_StringList h;
  // Create a list of all arguments of the command line
  for(i=1; i<argc; i++)
    h.push_back(string(argv[i]));

  t_StringList::iterator it0, it1;
  unsigned j;
  BOSS::t_OptItem opt;
  for(j=0; j<v.size(); j++) {
    it0 = find(h.begin(), h.end(), v[j].option);

    if(it0!=h.end()) {
      it1 = it0;
      it1++;
    }
    else
      it1 = h.end();

    // Option was not found in the command line. If optiontype is true an error, message
    // will be printed. If not, the default value is stored as optionarg
    if(it0==h.end() && v[j].optiontype)
      error(v[j].option);
			
    if(it0==h.end()) {
      barg[v[j].option] = v[j];
      continue;
    }

    // This option takes an argument, if optionarg.size() != 0.
    if(v[j].optionarg.size() != 0) {
      if(it1==h.end())
	error(v[j].option);
      opt              = v[j];
      opt.optionarg    = *it1;
      opt.optionflag   = true;
      barg[opt.option] = opt;
      h.erase(it0);
      h.erase(it1);
    }
    else {
      opt              = v[j];
      opt.optionflag   = true;
      barg[opt.option] = opt;
      h.erase(it0);
    }
  }
  for(it0=h.begin(); it0!=h.end(); it0++)
    farg.push_back(*it0);

  if(fmin>=0) {
    if(fmax>=0) {
      if(farg.size()<(unsigned)fmin|| farg.size()>(unsigned)fmax)
	error("farg");
    }
    else {
      if(farg.size()<(unsigned)fmin)
	error("farg");
    }
  }
}

BOSS::CommandLine & BOSS::CommandLine::operator = (const CommandLine & C)
{
  farg  = C.farg;
  barg  = C.barg;
  usage = C.usage;
  return *this;
}

BOSS::t_OptItem BOSS::CommandLine::operator () (const string & opt) const
{ 
  t_OptItemMap::const_iterator it;
  it = barg.find(opt);
  if(it==barg.end())
    return BOSS::t_OptItem();
  return (*it).second;
}

string BOSS::CommandLine::operator [] (const unsigned i) const
{
  if(i<farg.size())
    return farg[i];
  return string();
}
