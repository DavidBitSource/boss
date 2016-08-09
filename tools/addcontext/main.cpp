/* main.cpp (addcontext) $Revision: 1.5 $ $Date: 2005/12/17 16:06:11 $
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

// Include files needed for the xml library
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"

// Standard include files
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>

// Local include files
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/dom_tools.h"
#include "boss_utility/dom_userdata.h"
#include "boss_context/boss_context.h"

// Each item for which the context is computed is a node in the DOM.
// For such a node, the user data pointer of the DOM class is used to
// store the context information. This information is just a pointer
// to an instance of the following class.
class USER_Data {
  string left;
  string right;
public:
  friend ostream & operator << (ostream & out, const USER_Data & d);
  USER_Data() {}
  USER_Data(const string & l, const string & r) : left(l), right(r)
  {
    // If there is no context, use the "$p" context as default
    if(!left.size())
      left = "$p";
    if(!right.size())
      right = "$p";
  }
  const string & getLeft()  { return left; }
  const string & getRight() { return right; }
};

ostream & operator << (ostream & out, const USER_Data & d)
{ return out << d.left << " " << d.right; }

BOSS::Config cl;

// Some variables used as defaults for the command line class
static const char USAGE[]      = "addcontext -inv inventory [-xext xml_extension] [-bext backup_extension] [-b] -list filelist | xmlfile1, xmlfile2, ...";

void addPhoneContext(xercesc::DOMDocument* doc)
{
  xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "PHONE");
  xercesc::DOMNode *phone, *l, *r;
  if(nl->getLength()>1) {
    phone = nl->item(0);
    r = nl->item(1);
    BOSS::setUserData(phone, new USER_Data("", BOSS::getAttrByName(r, "TKey")));
    for(unsigned i=1; i<nl->getLength()-1; i++) {
      phone = nl->item(i);
      l = nl->item(i-1);
      r = nl->item(i+1);
      BOSS::setUserData(phone, new USER_Data(BOSS::getAttrByName(l, "TKey"), BOSS::getAttrByName(r, "TKey")));
    }
    phone = nl->item(nl->getLength()-1);
    l = nl->item(nl->getLength()-2);
    BOSS::setUserData(phone, new USER_Data(BOSS::getAttrByName(l, "TKey"), ""));
  }
  else {
    if(nl->getLength()>0) {
      phone = nl->item(0);
      BOSS::setUserData(phone, new USER_Data("", ""));
    }
  }
}

void addSyllableContext(xercesc::DOMDocument * doc)
{
  xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "SYLLABLE");
  xercesc::DOMNode *l, *r;
  string phone_tag("PHONE");
  for(unsigned i=0; i<nl->getLength(); i++) {
    xercesc::DOMNode* syllable = nl->item(i);
    int bla = 0;
    for(l = syllable->getFirstChild(); l != 0 && !BOSS::compareNodeName(l, phone_tag); l = l->getNextSibling());
    for(r = syllable->getLastChild();  r != 0 && !BOSS::compareNodeName(r, phone_tag); r = r->getPreviousSibling());
    USER_Data * lu = (USER_Data*)BOSS::getUserData(l);
    USER_Data * ru = (USER_Data*)BOSS::getUserData(r);
    BOSS::setUserData(syllable, new USER_Data(lu->getLeft(), ru->getRight()));
  }	  	
}

void addWordContext(xercesc::DOMDocument* doc)
{
  xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "WORD");
  xercesc::DOMNode *l, *r;
  string syllable_tag("SYLLABLE");
  for(unsigned i=0; i<nl->getLength(); i++) {
    xercesc::DOMNode* word = nl->item(i);
    for(l = word->getFirstChild(); l != 0 && !BOSS::compareNodeName(l, syllable_tag); l = l->getNextSibling());
    for(r = word->getLastChild();  r != 0 && !BOSS::compareNodeName(r, syllable_tag); r = r->getPreviousSibling());
    USER_Data * lu = (USER_Data *)BOSS::getUserData(l);
    USER_Data * ru = (USER_Data *)BOSS::getUserData(r);
    BOSS::setUserData(word, new USER_Data(lu->getLeft(), ru->getRight()));
  }
}

void setContAttributes(xercesc::DOMNodeList* nl)
{
  string ccfile = cl("-ccfile").optionarg;
  static BOSS_context cclass(ccfile);
  for(unsigned i=0; i<nl->getLength(); i++) {
    xercesc::DOMNode* node       = nl->item(i);
    xercesc::DOMElement* element = (xercesc::DOMElement *) node;
    USER_Data *u        = (USER_Data *) BOSS::getUserData(node);
    if(u == 0) {
      cerr << "No user data contained in node" << endl;
      exit(-1);
    }
    BOSS::setAttribute(element, "CLeft", u->getLeft().c_str());
    BOSS::setAttribute(element, "CRight", u->getRight().c_str());
    BOSS::setAttribute(element, "CCLeft", (cclass.left1(u->getLeft())).c_str());
    BOSS::setAttribute(element, "CCRight", (cclass.right1(u->getRight())).c_str());
    if (cclass.left1((u->getLeft()).c_str()) == "") cerr << "Warning: no first left context class for " << u->getLeft() << endl;
    if (cclass.right1((u->getRight()).c_str()) == "") cerr << "Warning: no first right context class for " << u->getRight() << endl;
    BOSS::setAttribute(element, "CCLeft2", (cclass.left2(u->getLeft())).c_str());
    BOSS::setAttribute(element, "CCRight2", (cclass.right2(u->getRight())).c_str());
    if (cclass.left2((u->getLeft()).c_str()) == "") cerr << "Warning: no second left context class for " << u->getLeft() << endl;
    if (cclass.right2((u->getRight()).c_str()) == "") cerr << "Warning: no second right context class for " << u->getRight() << endl;
    delete u;
    BOSS::setUserData(node, NULL);
  }
}

// This is the main routine. It takes two file names as arguments.
// The contents of the first file associated with "xml_file" is
// parsed and read into memory. This will give the document
// represented by the DOM_Document class.
// After parsing, we will call functions which add the context information
// to the user data field of each node.
// The context information for higher levels (e.g. syllables and words) is
// taken from the underlying level, so the sequence of calling the functions
// is important.
void addContext(const string &xml_file, const string & backup_file)
{
  //DOMParser parser;
  //parser->setDoValidation(false);

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
    // Parse the XML file, catching any XML exceptions that might propagate out of it.
    doc = parser->parseURI(xml_file.c_str());

    // Add context on each level
    addPhoneContext(doc);
    addSyllableContext(doc);
    addWordContext(doc);
	
    // Put the context information contained in the USER_Data class to attributes
    xercesc::DOMNodeList* nl = BOSS::getElementsByTagName(doc, "WORD");
    setContAttributes(nl);
    nl = BOSS::getElementsByTagName(doc, "SYLLABLE");
    setContAttributes(nl);
    nl = BOSS::getElementsByTagName(doc, "PHONE");
    setContAttributes(nl);
		
    // Make a backup copy if a backup name is given
    if(backup_file.size())
      rename(xml_file.c_str(), backup_file.c_str());

    // Open the file to store the result in
    ofstream out(xml_file.c_str());
    if(!out) {
      cerr << "Can't open file " << xml_file << endl;
      return;
    }

    // Write the the new xml structure
    out << doc << endl;
  }
  catch (const xercesc::XMLException& e) {
    cerr << "An error occured during parsing\n   Message: "
         << e.getMessage() << endl;
  }
}

int main(int argc, char *argv[])
{
  // ugly workaround for gcc 4.0:
  BOSS::Config tmp(argc, argv, USAGE);

  // Process the command line options;
  cl = tmp;

  try {
    // Initialise the XML4C2 system. Taken from original distribution
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch(const xercesc::XMLException& toCatch) {
    cerr << "Error during Xerces-c Initialization.\n"
	 << "  Exception message:"
	 << toCatch.getMessage() << endl;
    return 1;
  }
	
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

  bool backup = cl("-b").optionflag;
  for(unsigned i=0; i<files.size(); i++) {
    cerr << files[i] << endl;
    string xml_file  = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-xext").optionarg));
    if(backup) {
      string backup_file = BOSS::makeFilePath(cl("-xdir").optionarg, string(files[i] + cl("-bext").optionarg));
      addContext(xml_file, backup_file);
    }
    else {
      addContext(xml_file, "");
    }
  }

  return EXIT_SUCCESS;
}
