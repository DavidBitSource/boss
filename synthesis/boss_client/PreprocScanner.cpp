/* Copyright (C) 1998  Thomas Portele

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

Thomas Portele
tpo@ikp.uni-bonn.de http://www.ikp.uni-bonn.de/~tpo
IKP Poppelsdorfer Allee 47, D-53115 Bonn Germany

__________________________

*/

/**
 * @file PreprocScanner.cpp
   @author Michal Holcik
   @author Thomas Portele
*/

#include "PreprocScanner.h"

#include <queue>
#include <map>
#include <string>
#include <iostream>
#include <string.h>

FILE *errfile;
//#define errfile stderr

using namespace std;

/**
 * @addtogroup boss_client
 * @{
 */


// The PreprocScanner contructor reads the definition files (a rule list
// and optionally an abbreviation list) needed by the scanner.

// TODO: use exceptions for error handling

PreprocScanner::PreprocScanner (const char *rulefilename, const char *abbfilename,
		      const char *infilename):
input (infilename, "()&%")
{
  errfile = stderr;
  FILE *rlfile = fopen (rulefilename, "r");
  FILE *abbfile = NULL;
  if (rlfile == NULL)
  {
    // no rulefile, we have a problem
    fprintf (errfile, "PreprocScanner: Cannot open rule list in file: \"%s\"\nexiting\n", rulefilename);
    exit(2);
//    TODO: throw an exception
  }
  if (abbfilename != NULL && strlen(abbfilename))
  {
    abbfile = fopen (abbfilename, "r");
    if (abbfile != NULL)
    {
      al = new PPAbbreviationlist (abbfile);
      fclose (abbfile);
    }
    else
    {
      al = NULL;
      fprintf (errfile, "PreprocScanner: Cannot open abbreviation list in file: \"%s\"\nexiting\n", abbfilename);
      exit(2);
    }
  }
  else
  {
    al = NULL;
  }

  char *n;
  char p;
  rl = new PPRulelist (rlfile);
  fclose (rlfile);
}

PreprocScanner::~PreprocScanner ()
{
  if (al != NULL)
    delete (al);
  delete (rl);
}

// the getToken() function returns a token from the token queue

Token PreprocScanner::getToken ()
{
  Token result (tokenQueue.front ());
  tokenQueue.pop ();
  return result;
}


void
PreprocScanner::checkRulesDefinition (char *xinput, int g)
{
  char *n;
  char p;
  if (strncmp (&xinput[g], "{UsePrepocRules:", 16) == 0)
  {
    n = strchr (&xinput[g], '}');
    if (n != NULL)
    {
      p = *n;
      n[0] = '\0';
      FILE *rlfile = fopen (&xinput[g + 16], "r");
      if (rlfile != NULL)
      {
	delete (rl);
	rl = new PPRulelist (rlfile);
	rl->list (errfile);
	fclose (rlfile);
      }
    }
  }

}

void
PreprocScanner::checkAbbrevDefinition (char *xinput, int g)
{
  char *n;
  char p;
  if (strncmp (&xinput[g], "{UseAbbreviationList:", 21) == 0)
  {
    n = strchr (&xinput[g], '}');
    if (n != NULL)
    {
      p = *n;
      n[0] = '\0';
      FILE *abbfile = fopen (&xinput[g + 21], "r");
      if (abbfile != NULL)
      {
	if (al != NULL)
	  delete (al);
	al = new PPAbbreviationlist (abbfile);
	fclose (abbfile);
      }
    }
  }

}

// the scan() function checks if there are any tokens left in the buffer
// queue tokenQueue. If so, it returns `true' as an indication for the
// getToken() function that there are tokens present and available for
// reading. In the case the tokenQueue is empty, it scans the input
// for new tokens and places them in the tokenQueue.

bool PreprocScanner::scan ()
{
  char *
    xinput;
  char *
    output;
  char
    p;
  int
    g;
  if (!tokenQueue.empty ())
  {
    return true;
  }
  if ((xinput = input.get_next_token_with_lookahead (g)) != NULL)
  {
    checkRulesDefinition (xinput, g);
    checkAbbrevDefinition (xinput, g);

    output = rl->apply (xinput, g);

    if (al != NULL)
      output = al->apply (output);

    output = fl.call (output);
    if (strlen (output) >= 1)
    {
      p = output[strlen (output) - 1];
      char
	p_str[2];
      p_str[0] = p;
      p_str[1] = '\0';
      output[strlen (output) - 1] = '\0';
      if (strchr (",.;:?!", p) != NULL)
	tokenQueue.push (Token (TOKEN_WORD, string(output)));
      else
	tokenQueue.push (Token (TOKEN_WORD, string(output)+p));

      if (strchr (",.;:?!", p) != NULL)
      {
	tokenQueue.push (Token (TOKEN_PHRASE_DELIMITER, p_str));
      }
      if (strchr (".;?!", p) != NULL)
      {
	tokenQueue.push (Token (TOKEN_SENTENCE_DELIMITER, p_str));
      }
    }
    delete (output);
    return true;
  }
  else
    return false;
}

/** @} */ // end of boss_client group
