/* main.cpp (addf0) $Revision: 1.5 $ $Date: 2007/04/16 09:45:03 $
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/**
   @author Karlheinz Stoeber
   @author Stefan Breuer
*/

using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include <cstdio>

// Include files needed for the xml library
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"

#include "boss_utility/dom_tools.h"
#include "boss_mysql/boss_mysql_handler.h"
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/boss_types.h"


typedef std::vector< float > VFloat;
typedef map <string, VFloat> VFloatMap;

MySQL_Handler * data_base(0);

// Some variables used as defaults for the command line class
static const string USAGE("\n\nXML mode:\naddf0 -inv inventory [-xdir xml_dir] [-xext xml_extension] [-bext backup_extension] [-b]\n\t[-fdir feature_dir] [-fext feature_extension] -list filelist | xmlfile1, xmlfile2, ... \n\nDB mode:\naddf0 -dbout -inv inventory [-u user_name] [-h host_name] [-p password] [-db database]\n\t[-fdir feature_dir] [-fext feature_extension] [file_name]\n ");
static float pshift;

string getFileName(const unsigned & num, const string & speaker, unsigned short * type)
{
  stringstream query;
  if (*type == 0)
  {
	  query << "select " << speaker << "_sentence_data.File from ";
	  query << speaker << "_sentence_data, " << speaker << "_sentence_map, " << speaker << "_word_map, " << speaker << "_syllable_map where ";
	  query << "\"" << num <<               "\" = " << speaker << "_syllable_map.PhoneNum && ";
	  query << speaker << "_syllable_map.SyllableNum  = " << speaker << "_word_map.SyllableNum    && ";
	  query << speaker << "_word_map.WordNum          = " << speaker << "_sentence_map.WordNum    && ";
	  query << speaker << "_sentence_data.Num         = " << speaker << "_sentence_map.SentenceNum" << ends;
  }
  else 
  {
	  if (*type == 1)
	  {
		  query << "select " << speaker << "_sentence_data.File from ";
		  query << speaker << "_sentence_data, " << speaker << "_sentence_map, " << speaker << "_word_map where ";
		  query << "\"" << num <<               "\" = " << speaker << "_word_map.SyllableNum && ";
		  query << speaker << "_word_map.WordNum  = " << speaker << "_sentence_map.WordNum    && ";
		  query << speaker << "_sentence_data.Num         = " << speaker << "_sentence_map.SentenceNum" << ends;
	  }
	  else 
	  {
		  if (*type = 2)
		  {
			  query << "select " << speaker << "_sentence_data.File from ";
			  query << speaker << "_sentence_data, " << speaker << "_sentence_map where ";
			  query << "\"" << num <<               "\" = " << speaker << "_sentence_map.WordNum && ";
			  query << speaker << "_sentence_map.SentenceNum  = " << speaker << "_sentence_data.Num" << ends;
		  }
	  }
  }
  MySQL_Result tmp;
  try {
    tmp = data_base->query(query.str().c_str(), true);
  }
  catch(const MySQL_QueryFailed & e) {
    e.printError(true);
  }
  if(!tmp.row()) {
    cerr << "Wrong segment number " << num << endl;
    return string();
  }
  return tmp(0, 0);
}

VFloatMap readF0(const string & file_name)
{
  VFloat vf0,vrms;
  VFloatMap FeaMap;
  ifstream in(file_name.c_str());
  if(!in) {
    cerr << "Can't open file " << file_name << endl;
    return FeaMap;
  }
  while (in) {
    float f0(0),voice(0),rms(0),ac(0);
    in >> f0 >> voice >> rms >> ac;
    vf0.push_back(f0);
    vrms.push_back(rms);
  }
  FeaMap["f0"]=vf0;
  FeaMap["rms"]=vrms;
  return FeaMap;
}

float getFea(const VFloat & fea_values, const unsigned first, const unsigned & last)
{
  unsigned zeros(0);
  float    avg(0.0);
  unsigned fi = unsigned(rint(float(first) / pshift));
  unsigned la = min(unsigned(rint(float(last) / pshift)), (unsigned)fea_values.size());
  for(unsigned i=fi; i<la; i++) {
    if(fea_values[i] == 0)
      zeros++;
    else
      avg += fea_values[i];
  }
  //pre
    if(zeros <= (la-fi)*0.75 && (la-fi)-zeros != 0)
    return avg / ((la-fi)-zeros);

  return 0.0;
}

void setAttributes(xercesc::DOMNode* node, const VFloatMap & FeaMap) {
  static string sleft, sright;
  if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
    sleft  = BOSS::getAttrByName(node, "First");
    sright = BOSS::getAttrByName(node, "Last");
    if(sleft.size() && sright.size()) {
      ostringstream lf0,rf0,avgf0,rms;
      xercesc::DOMElement* element = (xercesc::DOMElement *) node;
      VFloatMap::const_iterator fmi;
      if ((fmi = FeaMap.find("f0"))!= FeaMap.end()) {
	lf0 << getFea((*fmi).second, atoi(sleft.c_str()),atoi(sleft.c_str())+unsigned(0.33*(atof(sright.c_str())-atof(sleft.c_str()))));
	rf0 << getFea((*fmi).second, atoi(sright.c_str())-unsigned(0.33*(atof(sright.c_str())-atof(sleft.c_str()))), atoi(sright.c_str()));
	avgf0 << getFea((*fmi).second, atoi(sleft.c_str()), atoi(sright.c_str()));
      }
      else cerr << "No f0 features in map" << endl;
      if ((fmi = FeaMap.find("rms"))!= FeaMap.end())
	rms << getFea((*fmi).second, atoi(sleft.c_str()), atoi(sright.c_str()));
      else cerr << "No rms features in map" << endl;
      BOSS::setAttribute(element, "LF0", lf0.str().c_str());
      BOSS::setAttribute(element, "RF0", rf0.str().c_str());
      BOSS::setAttribute(element, "AVGF0", avgf0.str().c_str());
      BOSS::setAttribute(element, "RMS",rms.str().c_str());
    }
  }
  xercesc::DOMNodeList* nl = node->getChildNodes();
  for(unsigned i=0; i<nl->getLength(); i++) {
    xercesc::DOMNode* tmp = nl->item(i);
    setAttributes(tmp, FeaMap);
  }
}


void addFea(const string & fea_file, const string & xml_file, const string & backup_file)
{

  //DOMParser parser;
  //parser.setDoValidation(false);

  xercesc::DOMDocument *doc = 0;
  XMLCh tempStr[100];
  xercesc::XMLString::transcode("LS", tempStr, 99);
  xercesc::DOMImplementation *impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
#if XERCES_VERSION_MAJOR == 3
  xercesc::DOMLSParser* parser = ((xercesc::DOMImplementationLS*)impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#else if XERCES_VERSION_MAJOR == 2
  xercesc::DOMBuilder* parser = ((xercesc::DOMImplementationLS*)impl)->createDOMBuilder(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#endif
  try {
    VFloatMap FeaMap;
    FeaMap = readF0(fea_file);
    // Parse the XML file, catching any XML exceptions that might propagate out of it.
    doc = parser->parseURI(xml_file.c_str());
    setAttributes(doc, FeaMap);
    // Do a backup copy if a backup name is given

    if(backup_file.size())
      rename(xml_file.c_str(), backup_file.c_str());

    // Open the file to store the result in
    ofstream out(xml_file.c_str());
    if(!out) {
      cerr << "Can't open file " << xml_file << endl;
      return;
    }

    // Write out the the new xml structure
    out << doc << endl;
  }
  catch (const xercesc::XMLException& e) {
    cerr << "An error occured during parsing\n   Message: "
         << e.getMessage() << endl;
  }
}

int main(int argc, char *argv[])
{
  BOSS::Config cl(argc, argv, USAGE); // Process the command line options

  //pshift = atof(cl("-pshift").optionarg.c_str()) * 16;   //correct if f0-values are extracted every 10th ms
  pshift = atof(cl("-pshift").optionarg.c_str()) * 8;	   //correct if f0-values are extracted every 5th ms
  string speaker = cl.getInvName();

  if (cl("-dbout").optionflag) {
    try {
      // Initialise the database connection
      data_base = new MySQL_Handler(cl("-h").optionarg,cl("-u").optionarg,cl("-p").optionarg,cl("-db").optionarg);

      stringstream query;
      stringstream syll;
      stringstream word;
      if(cl.size() == 0) {
	query << "select " << speaker << "_phone_data.Num, " << speaker << "_phone_data.TKey, " << speaker << "_phone_data.First, " << speaker << "_phone_data.Last from " << speaker << "_phone_data";
	syll << "select " << speaker << "_syllable_data.Num, " << speaker << "_syllable_data.TKey, " << speaker << "_syllable_data.First, " << speaker << "_syllable_data.Last from " << speaker << "_syllable_data";
	word << "select " << speaker << "_word_data.Num, " << speaker << "_word_data.TKey, " << speaker << "_word_data.First, " << speaker << "_word_data.Last from " << speaker << "_word_data";

      }
      else {
	query << "select " << speaker << "_phone_data.Num, " << speaker << "_phone_data.Tkey, " << speaker << "_phone_data.First, " << speaker << "_phone_data.Last from ";
	query << speaker << "_sentence_data, " << speaker << "_sentence_map, " << speaker << "_word_map, " << speaker << "_syllable_map, " << speaker << "_phone_data ";
	query << "where ";
	query << speaker << "_sentence_data.File      = \"" << cl[0] << "\" && ";
	query << speaker << "_sentence_data.Num       = " << speaker << "_sentence_map.SentenceNum && ";
	query << speaker << "_sentence_map.WordNum    = " << speaker << "_word_map.WordNum         && ";
	query << speaker << "_word_map.SyllableNum    = " << speaker << "_syllable_map.SyllableNum && ";
	query << speaker << "_syllable_map.PhoneNum = " << speaker << "_phone_data.Num" << ends;
      }

      MySQL_Result phones, syllables, words;
      try {
	phones = data_base->query(query.str().c_str(), true);
	syllables = data_base->query(syll.str().c_str(), true);
	words = data_base->query(word.str().c_str(), true);
	//updating f0 values for phones
	string prev_file_id;
	VFloatMap FeaMap;
	for(unsigned i=0; i<phones.row(); i++) {
	  short unsigned type = 0;
	  string file_id = getFileName(atoi(phones(i, 0).c_str()),speaker, &type);
	  if(file_id != prev_file_id) {
	    string file_name = BOSS::makeFilePath(cl("-fdir").optionarg, string(file_id + cl("-fext").optionarg));
	    cout << file_name << endl;
	    prev_file_id = file_id;
	    FeaMap.clear();
	    FeaMap = readF0(file_name);
	  }
  	  VFloatMap::const_iterator fmi;
	  fmi = FeaMap.find("f0");
	  float f0 = getFea((*fmi).second, atoi(phones(i, 2).c_str()), atoi(phones(i, 3).c_str()));
	  fmi = FeaMap.find("rms");
	  float rms = getFea((*fmi).second, atoi(phones(i, 2).c_str()), atoi(phones(i, 3).c_str()));
	  stringstream update;
	  try {
	    update << "UPDATE " << speaker << "_phone_data SET AVGF0 = \"" << f0 << "\",RMS = \"" << rms << "\" WHERE Num = \"" << phones(i, 0) << "\"" << ends;
	    cerr << update.str() << " " << phones(i, 1) << endl;
	    data_base->query(update.str().c_str(), true);
	  }
	  catch(const MySQL_QueryFailed & e) {
	    e.printError(true);
	  }
	}
	//updating f0 values for syllables
	prev_file_id = "";
	VFloatMap FeaMapSyll;
	for (unsigned i=0; i<syllables.row(); i++)
	{
		short unsigned type = 1;
		string file_id = getFileName(atoi(syllables(i, 0).c_str()),speaker, &type);
		if (file_id != prev_file_id)
		{
			string file_name = BOSS::makeFilePath(cl("-fdir").optionarg, string(file_id + cl("-fext").optionarg));
			cout << file_name << endl;
			prev_file_id = file_id;
			FeaMapSyll.clear();
			FeaMapSyll = readF0(file_name);
		}
		VFloatMap::const_iterator fmi;
		fmi = FeaMapSyll.find("f0");
		float f0 = getFea((*fmi).second, atoi(syllables(i, 2).c_str()), atoi(syllables(i, 3).c_str()));
		fmi = FeaMapSyll.find("rms");
		float rms = getFea((*fmi).second, atoi(syllables(i, 2).c_str()), atoi(syllables(i, 3).c_str()));
		stringstream update;
		try 
		{
			update << "UPDATE " << speaker << "_syllable_data SET AVGF0 = \"" << f0 << "\",RMS = \"" << rms << "\" WHERE Num = \"" << syllables(i, 0) << "\"" << ends;
			cerr << update.str() << " " << syllables(i, 1) << endl;
			data_base->query(update.str().c_str(), true);
		}
		catch(const MySQL_QueryFailed & e)
		{
			e.printError(true);
		}
	}
	//updating f0 values for words
	prev_file_id = "";
	VFloatMap FeaMapWord;
	for (unsigned i=0; i<words.row(); i++)
	{
		short unsigned type = 2;
		string file_id = getFileName(atoi(words(i, 0).c_str()),speaker, &type);
		if (file_id != prev_file_id)
		{
			string file_name = BOSS::makeFilePath(cl("-fdir").optionarg, string(file_id + cl("-fext").optionarg));
			cout << file_name << endl;
			prev_file_id = file_id;
			FeaMapWord.clear();
			FeaMapWord = readF0(file_name);
		}
		VFloatMap::const_iterator fmi;
		fmi = FeaMapWord.find("f0");
		float f0 = getFea((*fmi).second, atoi(words(i, 2).c_str()), atoi(words(i, 3).c_str()));
		fmi = FeaMapWord.find("rms");
		float rms = getFea((*fmi).second, atoi(words(i, 2).c_str()), atoi(words(i, 3).c_str()));
		stringstream update;
		try 
		{
			update << "UPDATE " << speaker << "_word_data SET AVGF0 = \"" << f0 << "\",RMS = \"" << rms << "\" WHERE Num = \"" << words(i, 0) << "\"" << ends;
			cerr << update.str() << " " << words(i, 1) << endl;
			data_base->query(update.str().c_str(), true);
		}
		catch(const MySQL_QueryFailed & e)
		{
			e.printError(true);
		}
	}
	
      }
      catch(const MySQL_QueryFailed & e) {
	e.printError(true);
      }
    }
    catch (const MySQL_ConnectFailed & e) {
      e.printError(true);
    }
    delete data_base;
    return EXIT_SUCCESS;
  }
  else {
    try {
      // Initialise the XML4C2 system. Taken from the original distribution
      xercesc::XMLPlatformUtils::Initialize();
    }
    catch(const xercesc::XMLException& toCatch) {
      cerr << "Error during Xerces-c Initialization.\n"
	   << "  Exception message:"
	   << toCatch.getMessage() << endl;
      return 1;
    }

    // Input files are given as command line arguments or in a file
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

    bool backup = cl("-b").optionflag;
    for(unsigned i=0; i<files.size(); i++) {
      cerr << files[i] << endl;
      string xml_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-xext").optionarg));
      string fea_file = BOSS::makeFilePath(cl("-fdir").optionarg, string(files[i] + cl("-fext").optionarg));
      if(backup) {
	string backup_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-bext").optionarg));
	addFea(fea_file, xml_file, backup_file);
      }
      else {
	addFea(fea_file,xml_file, "");
      }
    }
    return EXIT_SUCCESS;
  }
}
