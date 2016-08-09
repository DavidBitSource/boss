#ifndef __ABBREV__
#define __ABBREV__

#include <regexx.h>

class Abbrev {
private:
  //variables needed by abbrev
  /*vector< pair<string, string> > ad;
  vector< pair<string, string> > ar;
  vector< pair<string, string> > vn;
  vector< pair<string, string> > nv;
  vector< pair<string, string> > nn;
  vector< pair<string, string> > nz;
  vector< pair<string, string> > st;
  vector< pair<string, string> > or;
  vector< pair<string, string> > an;
  vector< pair<string, string> > empty;*/
  vector<string> pron;
  vector<string> unpron;
  regexx::Regexx rxx;
  regexx::Regexx pron_2;

  vector<string> getPron() const { return pron; }
  vector<string> getUnpron() const { return unpron; }
  //void Abbrev::split(string w, const char c, vector< string > & v) const;
  //void Abbrev::storeRegEx(const vector<string> & v);
  void Abbrev::readFromFile(const string & file, unsigned type);
  bool Abbrev::pronList(const string & s, const bool & flag);
  bool Abbrev::pronAbbrev(const string & s);

public:   
  Abbrev();
  ~Abbrev();
  vector<string> Abbrev::operator () (const string & text);
};

#endif
