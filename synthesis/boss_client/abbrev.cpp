#include <fstream>
//#include <sstream>
#include "abbrev.h"

Abbrev::Abbrev() {
 //reading of Abbrev tables
  //cerr << "Reading of regex.txt ... ";
  //readFromFile("./data/runtime/regex.txt",1);
  //cerr << "OK!" << endl;
  cerr << "Reading of pron.txt ... ";
  readFromFile("./data/runtime/pron.txt",2);
  cerr << "OK!" << endl;
  cerr << "Reading of unpron.txt ... ";
  readFromFile("./data/runtime/unpron.txt",3);
  cerr << "OK!" << endl;
}

Abbrev::~Abbrev() {
  pron.clear();
  unpron.clear();
}

/*void Abbrev::split(string w, const char c, vector< string > & v) const
{
  if(w.size()) {
    unsigned pos;
    if((pos = w.find(c))!=string::npos) {
      v.push_back(string(w, 0, pos));
      split(string(w, pos+1),c,v);
    }
    else {
      v.push_back(w);
      return;
    }
  }
  }*/  

/*void Abbrev::storeRegEx(const vector<string> & v) {
  pair<string,string> p;
  for (unsigned i=0;i<v.size();i++) {
    if (i==1) p.first=v[0];p.second=v[1];
    if (i==2) {
      vector<string> n;
      split(v[2],',',n);
      for (unsigned j=0;j<n.size();j++) {
	if(n[j]=="ad") ad.push_back(p);
	if(n[j]=="ar") ar.push_back(p);
	if(n[j]=="vn") vn.push_back(p);
	if(n[j]=="nv") nv.push_back(p);
	if(n[j]=="nn") nn.push_back(p);
	if(n[j]=="nz") nz.push_back(p);
	if(n[j]=="st") st.push_back(p);
	if(n[j]=="or") or.push_back(p);
	if(n[j]=="an") an.push_back(p);
      }
    }
    if (i>2) cerr << "Überflüssige Spalten in kt_regex.txt: " << v[i-2] << " " << v[i-1] << " " << v[i] <<endl;
  }
  }*/

void Abbrev::readFromFile(const string & file, unsigned type) {
  ifstream config(file.c_str());
  if (!config) {
    cerr << "Kann Konfigurationsdatei " << file << " nicht finden." << endl;
    exit(EXIT_FAILURE);
  }
  char c(0);
  string s;
  vector<string> v;
  while (config.get(c)) {
    if (c!=32 && c!=9 && c!='\n') {
      s+=c;
    }
    else {
      if (s.size()>0) {
	v.push_back(s);
	s.erase(0,s.size()-1);
	s.resize(0);
      }
      if (c=='\n') {
	switch(type) {
	  //case 1: storeRegEx(v);
	  //break;
	case 2: if (v.size()>0) pron.push_back(v[0]);
	  break;
	case 3: if (v.size()>0) unpron.push_back(v[0]);
	  break;
	}
	v.clear();
      }
    }
  }
  //storeRegEx(v);
  config.close();
}

bool Abbrev::pronList(const string & s, const bool & flag) {
  vector<string> v;
  if (flag==true) v = getPron();
  else v = getUnpron();
  string tmp;
  for (unsigned i=0;i<v.size();i++) {
    tmp = '^' + v[i] + '$';
    if (pron_2.exec(s,tmp,regexx::Regexx::nocase))
      return true;
  }
  return false;
}

bool Abbrev::pronAbbrev(const string & s) {
  try {
    // "y" als Nukleus entfernt
    if (pron_2.exec(s,"^[a-zäöüß]{3,}$",regexx::Regexx::nocase) && pron_2.exec(s,"^(([wrtzpsdfghjklyxcvbnm]){0,1}([euioaäöü]){1}([iuao]){0,1}([nmrtpsdfkl]){0,2})+$",regexx::Regexx::nocase) && !pronList(s,false))
      return true;
  }
  catch(regexx::Regexx::CompileException &e) {
    cerr << e.message() << endl;
  }
  return false;
}

vector<string> Abbrev::operator () (const string & text) {
  vector<string> result;
  bool flag(false);
  try {
    string tmp;
    vector< pair<string, string> > v;
    if (rxx.exec(text,"((([A-ZÜÖÄa-züöäß])([A-ZÜÖÄa-züöäß]*))(\\.+))",regexx::Regexx::global)) {
      //    if (rxx.exec(text,"((([A-ZÜÖÄ])([A-ZÜÖÄa-züöäß]*))(\\.+))",regexx::Regexx::global)) {
      for (unsigned i=0;i<rxx.match.size();i++) {
	//ostringstream st;
	tmp = rxx.match[i].atom[1];
	if (!pronAbbrev(tmp) && !pronList(rxx.match[i],true)) { //pronList muss mit rxx.match[i] statt tmp abgefragt werden, damit der "." enthalten ist!
	  for (unsigned j=0;j<tmp.size();j++) {
	    if (islower(tmp[j])) tmp[j]=toupper(tmp[j]);
	    //st << tmp[j] << " ";
	    if (!flag) result.push_back(string(1, tmp[j]));
	  }
	  flag=true;
	  //st << "Punkt";
	  //v.push_back(pair<string,string>(rxx.match[i],st.str()));
	}
	//else if (pronAbbrev(tmp) && !pronList(rxx.match[i],true)) {
	  //text=pron_2.replace(text,tmp + "\\.",tmp /*+ " Punkt"*/); 
	//}
      }
      //for (unsigned i=0;i<v.size();i++) 
	//text=rxx.replace(text,v[i].first,v[i].second);
      if (!flag) {
	result.push_back(tmp);
	flag=true;
      }
    }
    //v.clear();
    if (rxx.exec(text,"^[A-ZÜÖÄ]{1,}\\.{0}(\\,|$)",regexx::Regexx::global)) {
      for (unsigned i=0;i<rxx.match.size();i++) {
	//ostringstream st;
	tmp = rxx.match[i];
	//cerr << "rxx.match[i]: " << tmp << endl;
	if (!pronAbbrev(tmp) && !pronList(tmp,true)) {
	  for (unsigned j=0;j<tmp.size();j++) {
	    if (islower(tmp[j])) tmp[j]=toupper(tmp[j]);
	    //st << tmp[j] << " ";
	    if (!flag) result.push_back(string(1, tmp[j]));
	  }
	  flag=true;
	  //v.push_back(pair<string,string>(rxx.match[i],st.str()));
	}
      }
      //for (unsigned i=0;i<v.size();i++) 
      //text=rxx.replace(text,v[i].first,v[i].second);
      if (!flag) {
	result.push_back(tmp);
	flag=true;
      }
    }
    //v.clear();
    if (rxx.exec(text,"[A-ZÜÖÄ]+[a-züöäß]+[A-ZÜÖÄ]+",regexx::Regexx::global)) {
      for (unsigned i=0;i<rxx.match.size();i++) {
	//ostringstream st;
	tmp = rxx.match[i];
	if (!pronAbbrev(tmp) && !pronList(tmp,true)) {
	  for (unsigned j=0;j<tmp.size();j++) {
	    if (islower(tmp[j])) tmp[j]=toupper(tmp[j]);
	    //st << tmp[j] << " ";
	    if (!flag) result.push_back(string(1, tmp[j]));
	  }
	  flag=true;
	  //v.push_back(pair<string,string>(rxx.match[i],st.str()));
	}
      }
      //for (unsigned i=0;i<v.size();i++)
      //text=rxx.replace(text,v[i].first,v[i].second);
      if (!flag) {
	result.push_back(tmp);
	flag=true;
      }
    }
  }
  catch(regexx::Regexx::CompileException &e) {
    cerr << e.message() << endl;
  }
  return result;
}
