/* boss_fsa.cpp    $Revision: 1.3 $ $Date: 2006/11/05 23:37:36 $
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

#include "boss_fsa.h"
#include <fstream>
#include <string.h>

#define DOPRINTF
#undef DOPRINTF

/** This constructor reads elements from file "filename".
   * Each row of the input file is expected to contain a regular expression X, which is extended to (^X).
   * After creation, the FSA is stored in "preg".
   * This constructor throws the following exceptions:
   *
   * "BOSS_FileOpenError", if file opening fails, or
   * "BOSS_FormatError", if there are problems creating the FSA.
   */

BOSS::FSA::FSA(const std::string & filename) throw(BOSS::FileOpenError, BOSS::FileFormatError)
{
  static const char  F_Name[] = "BOSS_FSA::BOSS_FSA(const std::string &)";

  // Open the file containing the regular expressions
  ifstream in(filename.c_str());

  // Error handling for file open error
  if(!in)
    throw BOSS::FileOpenError(F_Name, filename);

  // Read the phone definition file line by line and create a new regular
  // expression from the entries. Each regular expression should match the
  // first part of an input string. Therefore, the ^ sign is inserted.
  std::string tmp, regexp;
  while(getline(in, tmp))
//    regexp += "(^" + tmp + ")|";
    regexp += "(" + tmp + ")|";
  try {
    compileExpression(regexp);
  } catch(...) {
    throw;
  }
}

/** This constructor reads regular expressions from an input string vector (t_SVector).
 *  It works exactly like the constructor that reads from a file, except that the expressions
 *  are read from an input string vector instead of a file.
*/

BOSS::FSA::FSA(const BOSS::t_SVector & expressions) throw(BOSS::FileFormatError)
{

  // Put the elements of expressions into the string regexp. Each regular
  // expression should match the first part of an input string. Therefore,
  // the ^ sign is inserted.
	std::string regexp;
  for(unsigned i=0; i<expressions.size(); i++)
//    regexp += "(^" + expressions[i] + ")|";
    regexp += "(" + expressions[i] + ")|";

  try {
    compileExpression(regexp);
  } catch(...) {
    throw;
  }
}

// Compile the regular expression. If this fails - throw an exception
void BOSS::FSA::compileExpression(const std::string & regexp) throw(BOSS::FileFormatError) {
  static const char  F_Name[] = "BOSS_FSA::BOSS_FSA(const t_SVector &)";
  const char *error;
  int erroffset;

#ifdef DOPRINTF
  std::cerr << "BOSS::FSA::compileExpression( regexp = " << regexp << " )" << std::endl;
#endif

  preg = pcre_compile(
         regexp.c_str(),   /* the pattern */
         0,                /* default options */
         &error,           /* for error message */
         &erroffset,       /* for error offset */
         NULL);            /* use default character tables */
  if (preg == NULL)
  {
    std::cerr << "BOSS_FSA: Error: PCRE compilation failed at offset "
    	<< erroffset << ": " << error << std::endl;
    throw BOSS::FileFormatError(F_Name, regexp, 0);
  }
}

BOSS::FSA::~FSA() {
	//regfree(&preg);
	free(preg);
}

  /** Divide "trans" into substrings.
   * Each substring satisfies at least one regular
   * expression. If more than one regular expression matches,
   * the longest match is taken.
   * The result is returned as a vector.
   * Each element contains one substring.
   */

BOSS::t_SVector BOSS::FSA::parse(const std::string & trans) const throw(BOSS::NoRegexMatch)
{
  static const char  F_Name[] = "t_SVector BOSS_FSA::parse(const std::string &) const";

  BOSS::t_SVector   res;
  char *  pmatch[1024];
  unsigned    start = 0;
  const char *input     = trans.c_str();
  unsigned errorcount = 0;
  std::string errorstring = "";
  unsigned lasterror = 0;
  const int OVECITEMSIZE = 3;
  const int OVECCOUNT = 10 * OVECITEMSIZE;
  int outputVector[OVECCOUNT];
  const int WORKSPACESIZE = 1024*1024;
  int workspace[WORKSPACESIZE]; // any number greater than 20

#ifdef DOPRINTF
  std::cerr << "BOSS_FSA: entered BOSS::FSA::parse( trans = " << trans << " )" << std::endl;
#endif


  // Find the longest matching substring in "trans", beginning at "trans.begin()+start"
  while(start < trans.size()) {
    //    std::cout << trans << std::endl;
#ifdef DOPRINTF
	  std::cout << "BOSS_FSA: start = " << start << std::endl;
#endif
	int rc;
	rc = pcre_dfa_exec(
	  preg,                   /* the compiled pattern */
	  NULL,                 /* no extra data - we didn't study the pattern */
	  input,              /* the subject string */
	  strlen(input),       /* the length of the subject */
	  start,                    /* start at offset 0 in the subject */
	  0,                    /* default options */
	  outputVector,              /* output vector for substring information */
	  OVECCOUNT, //);           /* number of elements in the output vector */
	  workspace,
	  WORKSPACESIZE);
	
#ifdef DOPRINTF
	std::cout << "BOSS_FSA: pcre_exec returned rc = " << rc << std::endl;
#endif
    	if(rc < 0) {		
      switch(rc)
            {
            case PCRE_ERROR_NOMATCH:
		std::cerr << "No match: ";
		copy(res.begin(), res.end(), ostream_iterator< std::string >(std::cerr, " "));
      		std::cerr << std::endl;
      		throw BOSS::NoRegexMatch(F_Name, trans);
		break;
            /*
            Handle other special cases if you like
            */
            default:
		std::cerr << "Matching error " << rc << ": ";;
		copy(res.begin(), res.end(), ostream_iterator< std::string >(std::cerr, " "));
      		std::cerr << std::endl;
      		throw BOSS::NoRegexMatch(F_Name, trans);
            }

	}
    
        if (rc == 0)
          {
          rc = OVECCOUNT/OVECITEMSIZE;
          std::cerr << "BOSS_FSA: Warning: there are more matches available: the output vector only has room for " << rc << " captured substrings" << std::endl;
          }
	
#ifdef DOPRINTF
	std::cout << "BOSS_FSA: outputVector:" << std::endl;
	for (int i = 0; i < OVECCOUNT/3; i++) {
		std::cout << outputVector[2*i] << " " << outputVector[2*i+1] << " " << std::endl;
	}
#endif
	
	int i = 0; // the longest match is stored on the first position
	int substringStart = outputVector[2*i];
	int substringLength = outputVector[2*i+1] - outputVector[2*i];

	if (substringStart != start) {
		std::cerr << "BOSS_FSA: Something is rotten in the state of Denmark:" << std::endl;
		std::cerr << "	substringStart != start" << std::endl;
	}
    
    string chunk = std::string(input, substringStart, substringLength);
    if(chunk.size()!=0) {
	res.push_back(chunk);
    	start += substringLength;
    } else { // chunk is zero, error state
#ifdef DOPRINTF
	std::cerr << "BOSS_FSA: error state, zero match" << std::endl;
#endif
      errorcount++;
      if ((lasterror) && (lasterror < (start-1))) errorstring += ",";
      errorstring += std::string(trans,start,1);
      lasterror=start;
      start++;
    }
    if (errorcount > 4){
      std::cerr << "BOSS_FSA: 5 or more parse errors in: " ;
      std::cerr << trans;
      std::cerr << " Unknown symbols: ";
      std::cerr << errorstring << std::endl;
      return res;
    }
  }
  return res;
}

