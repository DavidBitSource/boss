/* main.cpp (blf2xml) $Revision: 1.4 $ $Date: 2005/12/17 16:06:11 $
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
   @author Karlheinz Stoeber
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"

#include "sentenceentry.h"

static const char XMLHeader[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
// Some variables used as defaults for the command line class
static const char USAGE[]      = "blf2xml -inv inventory [-lext blf_extension] [-xext xml_extension] [-ldir blf_dir] [-xdir xml_dir] -list filelist | labelfile1, labelfile2, ...";

bool isComment(const string & s)
{
  unsigned i;
  for(i=0; i<s.size(); i++) {
    if(s[i] != ' ' && s[i] != '\t')
      break;
  }
  if(s[i] == '#')
    return true;
  return false;
}

string basename(const string & name, const string & suffix)
{
  string tmp(name);
  unsigned pos;
  if((pos = name.rfind("/"))!=string::npos)
    tmp = string(tmp, pos+1);
  if((pos = name.rfind(suffix))!=string::npos)
    tmp.resize(tmp.size()-suffix.size());
  return tmp;
}

void blf2xml(const string & file_name, const string & result_name, const string & ext)
{
  ifstream in(file_name.c_str());
  if(!in) {
    cerr << "Warning: Can't open file " << file_name << " ignoring." << endl;
    return;
  }
  ofstream out(result_name.c_str());
  if(!out) {
    cerr << "Warning: Can't open file " << result_name << " ignoring." << endl;
    return;
  }

  c_SentenceEntry sentence(basename(file_name, ext), "#", "#.");

  string tmp;
  vector< c_LabelEntry > label;
  while(getline(in, tmp)) {
    if(!isComment(tmp)) {
      c_LabelEntry tmplabel;
      stringstream ins(tmp.c_str());
      ins >> tmplabel.first >> tmplabel.phon >> tmplabel.pros;
      label.push_back(tmplabel);
    }
  }
		
  for(unsigned i=0; i<label.size()-1; i++)
    label[i].last = label[i+1].first;
  if(label.size())
    label.back().last = label.back().first;

  for(unsigned i=0; i<label.size(); i++)
    sentence.add(label[i]);
		
  out << XMLHeader << endl;
  sentence.printXML(out, "");
}

int main(int argc, char *argv[])
{
  BOSS::Config cl(argc, argv, USAGE); // Process the command line options

  // Input files are given as command line argument or in a file
  // indicated by the "-list" option
  vector< string > files;
  {
    if(cl("-list").optionflag) {
      ifstream in(cl("-list").optionarg.c_str());
      if(!in) {
	cerr << "Can't open " << cl("-list").optionarg << endl;
	exit(-1);
      }
      copy(istream_iterator<string>(in), istream_iterator<string>(),
	   back_inserter(files));
    }
    else {
      for(unsigned i=0; i<cl.size(); i++)
	files.push_back(cl[i]);
    }

    if(!files.size())
      cerr << "You have to enter at least one filename." << endl;
  }

  for(unsigned i=0; i<files.size(); i++) {
    cerr << files[i] << endl;
    string flabel = BOSS::makeFilePath(cl("-ldir").optionarg, string(files[i] + cl("-lext").optionarg));
    string fout   = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-xext").optionarg));
    blf2xml(flabel, fout, cl("-lext").optionarg);
  }

  return EXIT_SUCCESS;
}
