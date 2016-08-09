/* boss_intonation_de.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2002 University of Bonn.
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
 * @file boss_intonation_de.cpp
 * @author Philip Gross
 */

#include "boss_intonation_de.h"
#include "boss_utility/boss_config.h"
//#include <dom_tools.h>
#include <sstream>
#include <math.h>


/**
 * @addtogroup boss_intonation
 * @{
 */

BOSS_Intonation_DE::BOSS_Intonation_DE(BOSS::Config & _cl, MySQL_Handler * _db):
	BOSS_Intonation(_cl, _db)
{
  // Read the init file, load BLFs and neural networks
  string verz,str,filename;
  ifstream in(cl("-intocfg").optionarg);
  if (in)
    {
      getline(in,verz);
      getline(in,str);
      filename=verz;
      filename.append(str);
      ifstream inblf(filename.c_str());
      if(inblf)
	{
	  while (getline(inblf,str)) BLFStr.push_back(str);
	  BLFs = BLFStr.size();
	  inblf.close();
	}
      else
	{
	  cerr << filename << " open error." << endl;
	  exit (-1);
	}

      getline(in,str);
      filename=verz;
      filename.append(str);
      if (!decide.LoadNN(filename)) 
	{
	  cerr << filename << " open error." << endl;
	  exit (-1);
	}
      getline(in,str);
      filename=verz;
      filename.append(str);
      if (!phrase.LoadNN(filename)) 
	{
	  cerr << filename << " open error." << endl;
	  exit (-1);
	}
      getline(in,str);
      filename=verz;
      filename.append(str);
      if (!accent.LoadNN(filename)) 
	{
	  cerr << filename << " open error." << endl;
	  exit (-1);
	}
    }
  in.close();
}

string BOSS_Intonation_DE::getDescription() {
        return "BOSS Module Description: BOSS_Intonation_DE description string placeholder.";
}

BOSS_Intonation_DE::~BOSS_Intonation_DE() 
{
}

bool BOSS_Intonation_DE::ParseBLF(const string & parse, float blf[5]) const
{

  //cout << "Entering ParseBLF " << parse << endl;
  unsigned start=0;
  int n,pos;
  unsigned maxl;
  for (int i=0; i < 5; i++)
    {
      if (start < parse.length())
	{
	  maxl=0;
	  pos=-1;
	  for (n = 0; n < BLFs; n++)     // Find longest match in BLF list
	    {
	      if (parse.find(BLFStr[n],start) == start) if (BLFStr[n].length() > maxl)
		{
		  maxl = BLFStr[n].length();
		  pos = n;
		}
	    }
	  if (pos == -1)
	    {
	      cerr << "Unknown symbol in " << parse << " : " << string(parse,start) << endl;
	      return false;
	    }
	  blf[i]=pos;
	  //cout << BLFStr[n] << " = " << n << "  ";
	  start+=strlen(BLFStr[pos].c_str());
	  if (parse[start] == '-')          // Ersetzung uebergehen
	    {
	      start++;
	      maxl=0;
	      pos=0;
	      for (n = 0; n < BLFs; n++)
		{
		  if (parse.find(BLFStr[n],start) == start) if (BLFStr[n].length() > maxl)
		    {
		      maxl = BLFStr[n].length();
		      pos = n;
		    }
		}
	      start+=strlen(BLFStr[pos].c_str());
	    }
	}
      else 
	{
	  blf[i]=-BLFs;
	}
    }
  return true;
}

void BOSS_Intonation_DE::write_fuji(const xercesc::DOMNode* sentence) const {  
  cerr << "Writing of Fuji parameters!" << endl;
  static const float Fb(125.0), a(2.0), b(20.5);
  unsigned apc(0), aac(0);
  ostringstream file_name;
  BOSS::DOMNodeVector syllist, wordlist;
  BOSS::createLists(sentence, "SYLLABLE", syllist);
  BOSS::createLists(sentence, "WORD", wordlist);
  file_name << BOSS::getAttrByName(sentence, "TargetFile") << "_" << BOSS::getAttrByName(sentence, "SentenceID") << ".fuji";
  ofstream target(file_name.str().c_str());
  string old_T0(""), old_T1("");
  for (unsigned i=0; i<syllist.size(); i++) {
    if ((BOSS::getAttrByName(syllist[i], "T0") != old_T0)&&(BOSS::getAttrByName(syllist[i], "Ap") != "0")) apc++;
    if ((BOSS::getAttrByName(syllist[i], "T1") != old_T1)&&(BOSS::getAttrByName(syllist[i], "Aa") != "0")) aac++;
    old_T0 = BOSS::getAttrByName(syllist[i], "T0");
    old_T1 = BOSS::getAttrByName(syllist[i], "T1");
  }
  float sentence_pdur(0);
  for (unsigned j=0; j<wordlist.size(); j++) 
    sentence_pdur += atof(BOSS::getAttrByName(wordlist[j], "Dur").c_str());
  sentence_pdur/=1000;
  
  target << "<Filename>: " << endl
         << endl << endl << endl << endl << endl << "<Nr of frames per 10 ms>" << endl
	 << " " << apc << endl << " " << aac << endl << Fb << endl
	 << "<Length of frame>" << endl << "0.0" << endl << "0" << endl 
	 << endl << endl << endl << endl << endl << endl << endl;
  old_T0 = "";
  for (unsigned s=0; s<syllist.size(); s++) {
    if ((BOSS::getAttrByName(syllist[s], "Ap") != "0") && (BOSS::getAttrByName(syllist[s], "T0") != old_T0)) {
      target << BOSS::getAttrByName(syllist[s], "T0") << "  " << sentence_pdur << "  "
	     << BOSS::getAttrByName(syllist[s], "Ap") << "  " << a << endl;
      old_T0 = BOSS::getAttrByName(syllist[s], "T0");
    }
  }    
  old_T1="";
  for (unsigned s=0; s<syllist.size(); s++) {
    if ((BOSS::getAttrByName(syllist[s], "Aa") != "0") && (BOSS::getAttrByName(syllist[s], "T1") != old_T1)) {
      target << BOSS::getAttrByName(syllist[s], "T1") << "  " << BOSS::getAttrByName(syllist[s], "T2") 
	     << "  " << BOSS::getAttrByName(syllist[s], "Aa") << "  " << b << endl;
      old_T1 = BOSS::getAttrByName(syllist[s], "T1");
    }
  }     
  target.close();
}

void PrintData(float* data, int n)
{
  // Helper function
  for (int i=0;i < n; i++)
    {
      printf(" %f ", data[i]);
    }
  printf("\n");
}

void BOSS_Intonation_DE::operator () (xercesc::DOMNode* sentence)
{
  // Prediction of the fujisaki parameters for each syllable in 'sentence'
  BOSS::DOMNodeVector wordlist, syllist, phonlist, hphonlist;
  xercesc::DOMElement* element;
  static const char* Str_TKey("TKey");
  static const char* Str_Dur("Dur");
  static const char* Str_Stress("Stress");
  static const char* Str_PInt("PInt");
  static const char* Str_PMode("PMode");
  static const char* Str_StressSyl("StressSyl");
  static const char* Str_T0("T0");
  static const char* Str_T1("T1");
  static const char* Str_T2("T2");
  static const char* Str_Ap("Ap");
  static const char* Str_Aa("Aa");
  unsigned w,s,k,p,hp;
  string attr;
  char* tmpstr;
  float phraselength=0;
  int precision=3;
  int LastAccent=0;
  char buffer[100];
  bool StressSyl;
  t_Set set;
  vector<t_Set> Set;
  unsigned SetCount;
  float T0=0,T1=0,T2=0,Ap=0,Aa=0;
  vector<float> DecideIn(decide.layer[0].NumberOfNeurons);
  vector<float> PhraseIn(phrase.layer[0].NumberOfNeurons);
  vector<float> AccentIn(accent.layer[0].NumberOfNeurons);
  vector<float> Output, Decision;

  // determine length of sentence in seconds
  BOSS::createLists(sentence, "WORD", wordlist);
  for (w=0; w < wordlist.size(); w++) phraselength+=strtod(BOSS::getAttrByName(wordlist[w], Str_Dur).c_str(), &tmpstr);
  phraselength /= 1000;
  cout << "Phrase length is " << phraselength << endl;
  set.Length = 0;
  set.Pos = 0;
  // parse XML data for prediction parameters
  for (w=0; w < wordlist.size(); w++) 
    {
      BOSS::createLists(wordlist[w], "SYLLABLE", syllist);
      for (s=0; s < syllist.size(); s++) 
	{
	  set.syl = BOSS::getAttrByName(syllist[s], Str_TKey);
	  set.Pos += set.Length;    // length vom letzten Mal (pos=0 am Anfang)
	  set.Length = strtod(BOSS::getAttrByName(syllist[s], Str_Dur).c_str(), &tmpstr) / 1000;
	  set.Stress = atoi(BOSS::getAttrByName(syllist[s], Str_Stress).c_str());
	  if (set.Stress != 1) set.Stress = -1;
	  if (s == 0) set.Word = 1; else set.Word = -1;
	  set.PInt = atoi(BOSS::getAttrByName(syllist[s], Str_PInt).c_str());
	  if ( BOSS::getAttrByName(syllist[s], Str_PMode) == "." ) set.PInt = -set.PInt;
	  if ( !ParseBLF(set.syl,set.blf) ) cerr << "boss_intonation_de: Unknown symbol in " << set.syl << endl;
	  Set.push_back(set);
	}
      syllist.clear();	
    }
  // Convert parameters for neural networks and predict fujisaki for each syllable
  SetCount=0;
  for (w=0; w < wordlist.size(); w++) 
    {
      BOSS::createLists(wordlist[w], "SYLLABLE", syllist);
      StressSyl=false;
      for (s=0; s < syllist.size(); s++) 
	{
	  // run decide-network
	  // Input data for previous syllable
	  if (SetCount == 0) for (k=0; k < 6; k++) DecideIn[k] = -5;
	  else
	    {
	      DecideIn[0]=Set[SetCount-1].PInt / 5;
	      if (SetCount==1) DecideIn[1]=1; else DecideIn[1]=-1;
	      DecideIn[2]=(Set[SetCount-1].Pos / phraselength) * 4 - 2;
	      DecideIn[3]=Set[SetCount-1].Length;
	      DecideIn[4]=Set[SetCount-1].Stress;
	      DecideIn[5]=Set[SetCount-1].Word;
	    }
	  for (k=0; k < 5; k++) if (SetCount > 0) DecideIn[6+k] = (Set[SetCount-1].blf[k] / BLFs) * 4 - 2; else DecideIn[6+k] = -5;
	  
	  // Input data for current syllable
	  DecideIn[11+0]=Set[SetCount].PInt / 5;
	  if (SetCount==0) DecideIn[11+1]=1; else DecideIn[11+1]=-1;
	  DecideIn[11+2]=(Set[SetCount].Pos / phraselength) * 4 - 2;
	  DecideIn[11+3]=Set[SetCount].Length;
	  DecideIn[11+4]=Set[SetCount].Stress;
	  DecideIn[11+5]=Set[SetCount].Word;
	  for (k=0; k < 5; k++) DecideIn[17+k] = (Set[SetCount].blf[k] / BLFs) * 4 - 2;
	  
	  // Input data for next syllable
	  if (SetCount >= Set.size() - 1) for (k=0; k < 6; k++) DecideIn[22+k] = -5;
	  else
	    {
	      DecideIn[22+0]=Set[SetCount+1].PInt / 5;
	      DecideIn[22+1]=-1;
	      DecideIn[22+2]=(Set[SetCount+1].Pos / phraselength) * 4 - 2;
	      DecideIn[22+3]=Set[SetCount+1].Length;
	      DecideIn[22+4]=Set[SetCount+1].Stress;
	      DecideIn[22+5]=Set[SetCount+1].Word;
	    }
	  for (k=0; k < 5; k++) if (SetCount < Set.size() - 1) DecideIn[28+k] = (Set[SetCount+1].blf[k] / BLFs) * 4 - 2; else DecideIn[28+k] = -5;
	  //PrintData(DecideIn,decide.layer[0].NumberOfNeurons);
	  Decision=decide.Calculate(DecideIn);
	  cout << "boss_intonation_de: " << SetCount << ". "<< Set[SetCount].syl << " (l" << Set[SetCount].Length << " p" << Set[SetCount].Pos << " s" << Set[SetCount].Stress << " pi" << Set[SetCount].PInt <<  ")\t-> ";
	  cout << Decision[0] << ", ";
	  cout << Decision[1] << ", ";
	  cout << Decision[2] << ", ";
	  cout << Decision[3] << "\t: ";
      
	  // If an accent is predicted: Calculate fuji parameters (only if last 2 syllables have no accent)
	  if ((Decision[1] > 0.1) || (Decision[3] > 0)) // || ((Decision[0] < 0.23) && (Decision[1] > -0.13) && (Set[SetCount].Stress == 1)))
	    {
	      LastAccent++;
	      if (LastAccent < 3)
		{
		  // Input data for previous syllable
		  if (SetCount == 0) for (k=0; k < 5; k++) AccentIn[k] = -5;
		  else
		    {
		      AccentIn[0] = Set[SetCount-1].PInt / 5;
		      AccentIn[1] = (Set[SetCount-1].Pos / phraselength) * 4 - 2;
		      AccentIn[2] = Set[SetCount-1].Length;
		      AccentIn[3] = Set[SetCount-1].Stress;
		      AccentIn[4] = Set[SetCount-1].Word;
		    }
		  for (k=0; k < 5; k++) if (SetCount > 0) AccentIn[5+k] = (Set[SetCount-1].blf[k] / BLFs) * 4 - 2; else AccentIn[5+k] = -5;

		  // Input data for active syllable
		  AccentIn[10+0] = Set[SetCount].PInt / 5;
		  AccentIn[10+1] = (Set[SetCount].Pos / phraselength) * 4 - 2;
		  AccentIn[10+2] = Set[SetCount].Length;
		  AccentIn[10+3] = Set[SetCount].Stress;
		  AccentIn[10+4] = Set[SetCount].Word;
		  for (k=0; k < 5; k++) AccentIn[15+k] = (Set[SetCount].blf[k] / BLFs) * 4 - 2;

		  // Input data for next syllable
		  if (SetCount == Set.size() - 1) for (k=0; k < 5; k++) AccentIn[k] = -5;
		  else
		    {
		      AccentIn[20+0] = Set[SetCount+1].PInt / 5;
		      AccentIn[20+1] = (Set[SetCount+1].Pos / phraselength) * 4 - 2;
		      AccentIn[20+2] = Set[SetCount+1].Length;
		      AccentIn[20+3] = Set[SetCount+1].Stress;
		      AccentIn[20+4] = Set[SetCount+1].Word;
		    }
		  for (k=0; k < 5; k++) if (SetCount < Set.size() - 1) AccentIn[25+k] = (Set[SetCount+1].blf[k] / BLFs) * 4 - 2; else AccentIn[25+k] = -5;
		  Output = accent.Calculate(AccentIn);
		  Aa = (1 / Output[2]) - 1;
		  T1 = Output[0] * phraselength;
		  T2 = Output[1] * phraselength;
		  //	cout << "(" << Output[0] << Output[1] << Output[2] << ")";
		  Output.clear(); // NOTE: fast fix, not sure if necessary
		}
	      else
		{
		  Aa = 0;
		  T1 = 0;
		  T2 = 0;
		}
	    }
	  else
	    {
	      LastAccent = 0;
	      Aa = 0;
	      T1 = 0;
	      T2 = 0;
	    }

	  // If a phrasecommand is predicted: Calculate fuji parameters
	  if (((Decision[2] > 0) || (Decision[3] > 0)) && ((Set[SetCount].Stress == 1) || (w == s == 0)))
	    {
	      LastAccent = 0;
	      // Input data for previous syllable
	      if (SetCount == 0) for (k=0; k < 5; k++) PhraseIn[k] = -5;
	      else
		{
		  PhraseIn[0] = Set[SetCount-1].PInt / 5;
		  PhraseIn[1] = (Set[SetCount-1].Pos / phraselength) * 4 - 2;
		  PhraseIn[2] = Set[SetCount-1].Length;
		  PhraseIn[3] = Set[SetCount-1].Stress;
		  PhraseIn[4] = Set[SetCount-1].Word;
		}
	      for (k=0; k < 5; k++) if (SetCount > 0) PhraseIn[5+k] = (Set[SetCount-1].blf[k] / BLFs) * 4 - 2; else PhraseIn[5+k] = -5;
	
	      // Input data for active syllable
	      PhraseIn[10+0] = Set[SetCount].PInt / 5;
	      PhraseIn[10+1] = (Set[SetCount].Pos / phraselength) * 4 - 2;
	      PhraseIn[10+2] = Set[SetCount].Length;
	      PhraseIn[10+3] = Set[SetCount].Stress;
	      PhraseIn[10+4] = Set[SetCount].Word;
	      for (k=0; k < 5; k++) PhraseIn[15+k] = (Set[SetCount].blf[k] / BLFs) * 4 - 2;
	
	      // Input data for next syllable
	      if (SetCount == Set.size() - 1) for (k=0; k < 5; k++) PhraseIn[k] = -5;
	      else
		{
		  PhraseIn[20+0] = Set[SetCount+1].PInt / 5;
		  PhraseIn[20+1] = (Set[SetCount+1].Pos / phraselength) * 4 - 2;
		  PhraseIn[20+2] = Set[SetCount+1].Length;
		  PhraseIn[20+3] = Set[SetCount+1].Stress;
		  PhraseIn[20+4] = Set[SetCount+1].Word;
		}
	      for (k=0; k < 5; k++) if (SetCount < Set.size() - 1) PhraseIn[25+k] = (Set[SetCount+1].blf[k] / BLFs) * 4 - 2; else PhraseIn[25+k] = -5;
	      
	      Output = phrase.Calculate(PhraseIn);
	      Ap = (1 / Output[1]) - 1;
	      T0 = Output[0] * phraselength;
	      //	cout << "(" << Output[0] << Output[1] << ")";
	      Output.clear();
	    }
	  else
	    {
	      Ap = 0;
	      T0 = 0;
	    }
//	  free (Decision);

	  cout << " LA=" << LastAccent;
	  cout << " T0=" << T0;
	  cout << " T1=" << T1;
	  cout << " T2=" << T2;
	  cout << " Aa=" << Aa;
	  cout << " Ap=" << Ap << endl;
	  // Write the predicted fujisaki parameters to XML data on syllable level
	  element = (xercesc::DOMElement *) syllist[s];
	  attr = gcvt(T0, precision, buffer);
	  BOSS::setAttribute(element, Str_T0, attr.c_str());
	  attr = gcvt(T1, precision, buffer);
	  BOSS::setAttribute(element, Str_T1, attr.c_str());
	  attr = gcvt(T2, precision, buffer);
	  BOSS::setAttribute(element, Str_T2, attr.c_str());
	  attr = gcvt(Ap, precision, buffer);
	  BOSS::setAttribute(element, Str_Ap, attr.c_str());
	  attr = gcvt(Aa, precision, buffer);
	  BOSS::setAttribute(element, Str_Aa, attr.c_str());

	  // Write the predicted fujisaki parameters to XML data on phone level
	  BOSS::createLists(sentence, "PHONEME", phonlist);
	  for (p=0; p < phonlist.size(); p++) 
	  {
	    element = (xercesc::DOMElement *) phonlist[p];
	    attr = gcvt(T0, precision, buffer);
	    BOSS::setAttribute(element, Str_T0, attr.c_str());
	    attr = gcvt(T1, precision, buffer);
	    BOSS::setAttribute(element, Str_T1, attr.c_str());
	    attr = gcvt(T2, precision, buffer);
	    BOSS::setAttribute(element, Str_T2, attr.c_str());
	    attr = gcvt(Ap, precision, buffer);
	    BOSS::setAttribute(element, Str_Ap, attr.c_str());
	    attr = gcvt(Aa, precision, buffer);
	    BOSS::setAttribute(element, Str_Aa, attr.c_str());
	    // Write the predicted fujisaki parameters to XML data on halfphone level
	    BOSS::createLists(sentence, "HALFPHONE", hphonlist);
	    for (hp=0; hp < hphonlist.size(); hp++) 
	    {
	      element = (xercesc::DOMElement *) hphonlist[hp];
	      attr = gcvt(T0, precision, buffer);
	      BOSS::setAttribute(element, Str_T0, attr.c_str());
	      attr = gcvt(T1, precision, buffer);
	      BOSS::setAttribute(element, Str_T1, attr.c_str());
	      attr = gcvt(T2, precision, buffer);
	      BOSS::setAttribute(element, Str_T2, attr.c_str());
	      attr = gcvt(Ap, precision, buffer);
	      BOSS::setAttribute(element, Str_Ap, attr.c_str());
	      attr = gcvt(Aa, precision, buffer);
	      BOSS::setAttribute(element, Str_Aa, attr.c_str());
	    }
	    hphonlist.clear();
	  }
	  phonlist.clear();
	  if ((Set[SetCount].Stress == 1) && (!StressSyl))   // If a syllable is stressed set parameters on word level
	    {
	      StressSyl = true;
	      element = (xercesc::DOMElement *) wordlist[w];
	      attr = gcvt(T0, precision, buffer);
	      BOSS::setAttribute(element, Str_T0, attr.c_str());
	      attr = gcvt(T1, precision, buffer);
	      BOSS::setAttribute(element, Str_T1, attr.c_str());
	      attr = gcvt(T2, precision, buffer);
	      BOSS::setAttribute(element, Str_T2, attr.c_str());
	      attr = gcvt(Ap, precision, buffer);
	      BOSS::setAttribute(element, Str_Ap, attr.c_str());
	      attr = gcvt(Aa, precision, buffer);
	      BOSS::setAttribute(element, Str_Aa, attr.c_str());
	      attr = gcvt(s, 1, buffer);
	      BOSS::setAttribute(element, Str_StressSyl, attr.c_str());
	    }
	  SetCount++;
	}
      if (!StressSyl)
	{
	  element = (xercesc::DOMElement *) wordlist[w];
	  attr = "0";
	  BOSS::setAttribute(element, Str_T0, attr.c_str());
	  BOSS::setAttribute(element, Str_T1, attr.c_str());
	  BOSS::setAttribute(element, Str_T2, attr.c_str());
	  BOSS::setAttribute(element, Str_Ap, attr.c_str());
	  BOSS::setAttribute(element, Str_Aa, attr.c_str());
	  BOSS::setAttribute(element, Str_StressSyl, attr.c_str());
	}
      syllist.clear();	
    }
  wordlist.clear();
  Set.clear();
  
// NOTE: remove at will, mho
//  free (DecideIn);
//  free (AccentIn);
//  free (PhraseIn);

  // Output of fujisaki parameters
  if (cl("-fuji").optionflag) write_fuji(sentence);
}


/** @} */ // end of boss_intonation group
