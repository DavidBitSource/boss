/* phimbl.cpp  $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2003 University of Bonn.
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
   @author Philip Gross
*/

#include "phimbl.h"
#include <string>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

BOSS::Phimbl::Phimbl()
{
  line_count = 0;
  feature_count = 0;
  seperator = " ";
  tree = NULL;
}

BOSS::Phimbl::~Phimbl()
{
  features.clear();
  classes.clear();
  allfeatures.clear();
  permutation.clear();
  if (tree) DeleteNode(tree);
}

bool BOSS::Phimbl::Learn(const string & filename)
{
  // Load data
  if (!Load(filename)) return false;

  // Permutation
  if ((int)permutation.size() != feature_count)
  {
    permutation.clear();
    Calculate_Permutation();
  }

  // Free some memory
  features.clear();
  for (unsigned i = 0; i < classes.size(); i++) classes[i].features.clear();

  // Build the tree
  Bulid_Tree(filename);
  
  // Count the classes & Reduce tree
  ClassCount(tree);

  // Reduce the tree
  //ReduceTree(tree);
  return true;
}

bool BOSS::Phimbl::Write(const string & filename)
{
  // Save the tree
  return Save(filename);
}

bool BOSS::Phimbl::operator () (string & filename)
{
  cout << endl;

  // Load data
  cout << "Loading data..." << endl;
  if (!Load(filename)) return false;

  // Permutation
  cout << "Calculating Permutation..." << endl;
  if ((int)permutation.size() != feature_count)
  {
    permutation.clear();
    Calculate_Permutation();
  }
  Print_Permutation();

  // Free some memory
  cout << "Free some memory..." << endl;
  features.clear();
  for (unsigned i = 0; i < classes.size(); i++) classes[i].features.clear();

  // Build the tree
  cout << "Building tree..." << endl;
  Bulid_Tree(filename);
  
  // Count the classes
  cout << "Counting Classes & Reducing tree..." << endl;
  ClassCount(tree);

  // Reduce the tree
  //cout << "Reducing tree..." << endl;
  //ReduceTree(tree);

  // Save the tree
  cout << "Saving tree..." << endl;
  cout << filename.substr(0,filename.find_last_of(".")) + ".tree" << endl;
  if (!Save(filename.substr(0,filename.find_last_of(".")) + ".tree")) return false;

  cout << "Deleting tree..." << endl;
  cout << endl;

  return true;
}

bool BOSS::Phimbl::Load(string filename)
{
  int i, find1, find2, pos, spos, cls;
  int class_count = 0;
  string str, feature_string, class_string;
  string outfile;
  t_features* ft;
  t_classes* ct;
  vector <t_features>* fv;
  vector <double> weight;
  ifstream in(filename.c_str());
  if (in)
  {
    getline(in,str);
    pos = 0;
    spos = str.length();

    // Determine seperator
    if ((pos = str.find(" ",0) != string::npos))
    {
      seperator = " ";
      spos = pos;
    }
    if ((pos = str.find("\t",0) != string::npos) && (pos < spos))
    {
      seperator = "\t";
      spos = pos;
    }

    /* "," and ";" might appear in featurestrings, so they're not taken into account
    if ((pos = str.find(";",0) != string::npos) && (pos < spos))
    {
      seperator = ";";
      spos = pos;
    }
    if ((pos = str.find(",",0) != string::npos) && (pos < spos))
    {
      seperator = ",";
      spos = pos;
    }
    */

    // Count features
    feature_count = -1;
    for (pos = 0; pos != (int)string::npos; pos = str.find(seperator, pos + 1))
    {
      feature_count++;
      fv = new vector <t_features>;
      features.push_back(*fv);
    }

    line_count = 0;
    do
    {
      line_count++;
      spos = str.find_last_of(seperator) + 1;
      class_string = str.substr(spos, str.length() - spos);
      cls = find_str(class_string, classes);
      if (cls != -1)
      {
	classes[cls].count++;
      }
      else
      {
	ct = new t_classes;
	ct->class_string = class_string;
	ct->count = 1;
	for (i = 0; i < feature_count; i++) ct->features.push_back(seperator);
	classes.push_back(*ct);
	class_count++;
	cls = class_count - 1;
      }
      find2 = -1;
      for (i = 0; i < feature_count; i++)
      {
	find1 = find2 + 1;
	find2 = str.find(seperator, find1);
	feature_string = str.substr(find1, find2 - find1);
	pos = find_str(feature_string, features[i]);
	if (pos != -1)
	{
	  features[i][pos].count++;
	}
	else
	{
	  ft = new t_features;
	  ft->feature_string = feature_string;
	  ft->count = 1;
	  features[i].push_back(*ft);
	}
	pos = find_str(feature_string, allfeatures);
	if (pos != -1)
	{
	  allfeatures[pos].count++;
	}
	else
	{
	  ft = new t_features;
	  ft->feature_string = feature_string;
	  ft->count = 1;
	  allfeatures.push_back(*ft);
	}
	if (classes[cls].features[i].find(seperator + feature_string + seperator) == string::npos) classes[cls].features[i] += feature_string + seperator;
	//cout << classes[cls].features[i] << endl;
      }
    } while (getline(in,str));
    in.close();
  }
  else return false;
  return true;
}

void BOSS::Phimbl::Calculate_Permutation()
{
  int i, j = 0, pos, itc;
  unsigned itf;
  string str, feature_string, class_string;
  string outfile;
  double p = 0, hc = 0, hcv = 0, log2 = 0, sum = 0, si = 0, min, max = 0;
  vector <double> weight;

  // Do statistics
  // H(C) = -Sum[ c<C ] ( P(c) * log2(P(c)) )
  log2 = log10((double)2);
  for (itc = 0; itc < (int)classes.size(); itc++)
  {
    p = (double) classes[itc].count / line_count;
    hc += p * (log10(p) / log2);
  }
  hc = -hc;
  // w(i) = ...
  for (i = 0; i < feature_count; i++)
  {
    // Sum[ v<V(i) ] ( P(v) * H(C|v) )
    // si(i) = -Sum[ v<V(i) ] ( P(v) * log2(P(v)) )
    sum = 0;
    si = 0;
    for (itf = 0; itf < features[i].size(); itf++)
    {
      // H(C|v) = ...
      hcv = 0;
      for (itc = 0; itc < (int)classes.size(); itc++)
      {
	if (classes[itc].features[i].find(seperator + features[i][itf].feature_string + seperator) != string::npos)
	{
	  p = (double) classes[itc].count / line_count;
	  hcv += p * (log10(p) / log2);
	}
      }
      hcv = -hcv;
      p = (double) features[i][itf].count / line_count;
      sum += p * hcv;
      si += p * (log10(p) / log2);
    }
    si = -si;
    //si = 1;
    weight.push_back((hc - sum) / si);
    cout << "Feature " << i + 1 << " has weight " << ((hc - sum) / si) << endl;;
  }
  
  // Permutation
  min = INT_MAX;
  for (i = 0; i < feature_count; i++)
  {
    max = 0;
    for (j = 0; j < feature_count; j++)
    {
      if ((weight[j] > max) && (weight[j] < min))
      {
	max = weight[j];
	pos = j;
      }
    }
    min = weight[pos];
    permutation.push_back(pos);
  }
}

void BOSS::Phimbl::Print_Permutation()
{
  for (int i = 0; i < feature_count; i++)
  {
    cout << i + 1 << ".\t" << permutation[i] + 1 << endl;
  }
}

void BOSS::Phimbl::Bulid_Tree(string filename)
{
  int i, j = 0, find1, find2, spos, cls;
  string str, feature_string, class_string;
  vector <double> weight;
  ifstream in;
  tree = new t_Node;
  tree->parent = NULL;
  t_Node* node;
  vector <int> feat;
  bool found;
  in.open(filename.c_str());
  if (in)
  {
    while (getline(in,str))
    {
      node = tree;
      find2 = -1;
      feat.clear();
      for (i = 0; i < feature_count; i++)
      {
	find1 = find2 + 1;
	find2 = str.find(seperator, find1);
	feature_string = str.substr(find1, find2 - find1);
	feat.push_back(find_str(feature_string, allfeatures));
      }
      for (i = 0; i < feature_count; i++)
      {
	found = false;
	if (node->child.size() != 0)
	{
	  for (j = 0; j < (int)node->child.size(); j++)
	  {
	    if (node->child[j]->Feature == feat[permutation[i]])
	    {
	      found = true;
	      break;
	    }
	  }
	}
	if (found) node = node->child[j]->node;
	else
	{
	  t_Node::t_Child* newchild = new t_Node::t_Child;
	  newchild->Feature = feat[permutation[i]];
	  newchild->node = new t_Node;
	  newchild->node->parent = node;
	  node->child.push_back(newchild);
	  node = newchild->node;
	}
      }
      spos = str.find_last_of(seperator) + 1;
      class_string = str.substr(spos, str.length() - spos);
      cls = find_str(class_string, classes);
      map <int,int>::iterator it = node->count.find(cls);
      if (it != node->count.end()) (*it).second++;
      else node->count[cls] = 1;
    }
  }
}  

void BOSS::Phimbl::ClassCount(t_Node * node)
{
  map <int,int>::iterator itc, itp, pos;
  int max = 0;
  vector <t_Node::t_Child*>::iterator it;
  if (node)
  {
    for (unsigned j = 0; j < node->child.size(); j++) ClassCount(node->child[j]->node);
    for (itc = node->count.begin(); itc != node->count.end(); itc++)
    {
      if (node->parent)
      {
	itp = node->parent->count.find((*itc).first);
	if (itp != node->parent->count.end()) (*itp).second += (*itc).second;
	else node->parent->count[(*itc).first] = (*itc).second;
      }
      if (max < (*itc).second)
      {
	max = (*itc).second;
	pos = itc;
      }
    }  
    node->Class = (*pos).first;

    // Reduce tree
    for (it = node->child.begin(); it != node->child.end(); it++) 
    {
      if (((*it)->node->child.size() == 0) && ((*it)->node->Class == node->Class))
      {
	(*it)->node->count.clear();
	delete (*it)->node;
	it = node->child.erase(it) - 1;
	if (node->child.size() == 0) break;
      }
    }
  }
}

void BOSS::Phimbl::ReduceTree(t_Node * node)
{
  vector <t_Node::t_Child*>::iterator it;
  if (node)
  {
    for (it = node->child.begin(); it != node->child.end(); it++) 
    {
      ReduceTree((*it)->node);
      if (((*it)->node->child.size() == 0) && ((*it)->node->Class == node->Class))
      {
	(*it)->node->count.clear();
	delete (*it)->node;
	it = node->child.erase(it) - 1;
	if (node->child.size() == 0) break;
      }
    }
  }
}

bool BOSS::Phimbl::Save(string outfile)
{
  int i;
  fstream out(outfile.c_str(), fstream::out);
  if (out)
  {
    out << "# Permutation: < ";
    for (i = 0; i < feature_count; i++)
    {
      out << permutation[i] + 1;
      if (i < feature_count - 1) out << ", ";
    }
    out << " >" << endl;
    out << "Classes" << endl;
    for (i = 0; i < (int)classes.size(); i++)
    {
      out << i+1 << "\t" << classes[i].class_string << endl;
    }
    out << "Features" << endl;
    for (i = 0; i < (int)allfeatures.size(); i++)
    {
      out << i+1 << "\t" << allfeatures[i].feature_string << endl;
    }
    out << endl;
    SaveTree(tree, out);
    out.close();
  }
  else return false;
  return true;
}

void BOSS::Phimbl::SaveTree(t_Node * node, fstream & out)
{
  map <int,int>::iterator it;
  if (node)
  {
    out << "(" << node->Class + 1;
    out << "{";
    for (it = node->count.begin(); it != node->count.end(); it++)
    {
      if (it != node->count.begin()) out << ",";
      out << " " << (*it).first + 1 << " " << (*it).second;
    }
    out << " }";
    for (unsigned j = 0; j < node->child.size(); j++) 
    {
      if (j != 0) out << ",";
      else out << "[" ;
      out << node->child[j]->Feature + 1;
      SaveTree(node->child[j]->node, out);
    }
    if (node->child.size() != 0) out << "]" << endl;;
    out << ")" << endl;;
  }
}

void BOSS::Phimbl::DeleteNode(t_Node * node)
{
  if (node)
  {
    for (unsigned j = 0; j < node->child.size(); j++) 
    {
      DeleteNode(node->child[j]->node);
      delete node->child[j];
    }
    node->child.clear();
    node->count.clear();
    delete node;
  }
}

int BOSS::Phimbl::find_str(string & str, vector <t_classes> & data)
{
  unsigned i;
  if (data.size() == 0) return -1;
  for (i = 0; i < data.size(); i++) if (data[i].class_string == str) break;
  if (i < data.size()) return i; else return -1;
}

int BOSS::Phimbl::find_str(string & str, vector <t_features> & data)
{
  unsigned i;
  if (data.size() == 0) return -1;
  for (i = 0; i < data.size(); i++) if (data[i].feature_string == str) break;
  if (i < data.size()) return i; else return -1;
}

void BOSS::Phimbl::AddChild(t_Node* parent, string & str, int pos, t_Data & Data)
{
  int find1 = 0, find2 = 0;
  t_Node::t_Child* Feature = new t_Node::t_Child;
  t_Node* Class = new t_Node;;
  find1 = pos;
  find2 = str.find("(",find1);
  Feature->Feature = atoi(str.substr(find1,find2 - find1).c_str());
  Feature->node = Class;
  parent->child.push_back(Feature);
  pos = find2 + 1;

  Class->parent = parent;
  find1 = pos;
  find2 = str.find("{",find1);
  Class->Class = atoi(str.substr(find1,find2 - find1).c_str());

  /*  Dieser Teil l�dt zus�tzlich die (nicht ben�tigten) Informationen �ber die H�ufigkeit der Klassen in jedem Knoten
  find3 = str.find("}",find2);
  Class->max = -1;
  pos = find2;
  do
  {
    find1 = str.find(" ",pos) + 1;
    find2 = str.find(" ",find1);
    pos = find2;
    t1 = atoi(str.substr(find1,find2 - find1).c_str());
    find1 = str.find(" ",pos) + 1;
    find2 = str.find(",",find1);
    if ((find2 == -1) || (find2 > find3)) find2 = find3;
    pos = find2;
    t2 = atoi(str.substr(find1,find2 - find1).c_str());
    if (t2 > max)
    {
      Class->max = t1;
      max = t2;
    }
    Class->count[t1] = t2;
  } while (str[pos] != '}');
  pos++;
  */

  pos = str.find("}",find2) + 1;
  if (str[pos] == '[')
  {
    Data.currentNode = Class;
    AddChild(Class,str,pos + 1, Data);
  }
}
  

bool BOSS::Phimbl::LoadTree(const char* filename, t_Data & Data)
{
  int pos = 0, find1 = 0, find2 = 0;
  string str;
  ifstream in(filename);
  t_Node* Tree = new t_Node;
  if (in)
  {
    while (getline(in,str))                         // tree-Datei einlesen
    {
      //cout << str << endl;
      pos = 0;
      if (str[pos] == '#')
      {
	if ((int)str.find("Permutation") != -1)
	{
	  pos = str.find("<");
	  do
	  {
	    find1 = str.find(" ",pos);
	    find2 = str.find(",",find1);
	    if (find2 == -1) find2 = str.find(">",find1);
	    Data.permutation.push_back(atoi(str.substr(find1,find2 - find1).c_str()));
	    pos = find2;
	  } while (str[pos] != '>');
	}
      }
      else if (str == "Classes") 
      {
	do
	{
	  getline(in,str);
	  find1 = str.find("\t") + 1;
	  find2 = str.find("\n",find1);
	  Data.classes.push_back(str.substr(find1,find2 - find1).c_str());
	} while (str != "Features");
	Data.classes.pop_back();
	do
	{
	  getline(in,str);
	  find1 = str.find("\t") + 1;
	  find2 = str.find("\n",find1);
	  Data.features.push_back(str.substr(find1,find2 - find1).c_str());
	} while (str != "");
	Data.features.pop_back();
	getline(in,str);
      }
      if (str[pos] == '(')
      {
	find1 = 1;
	find2 = str.find("{",find1);
	Tree->Class = atoi(str.substr(find1,find2 - find1).c_str());
	Tree->parent = NULL;

	/*  Dieser Teil laedt zusaetzlich die (nicht benoetigten) Informationen ueber die Haeufigkeit der Klassen in jedem Knoten
	find3 = str.find("}",find2);
	pos = find2;
	do
	{
	  find1 = str.find(" ",pos) + 1;
	  find2 = str.find(" ",find1);
	  pos = find2;
	  t1 = atoi(str.substr(find1,find2 - find1).c_str());
	  find1 = str.find(" ",pos) + 1;
	  find2 = str.find(",",find1);
	  if ((find2 == -1) || (find2 > find3)) find2 = find3;
	  pos = find2;
	  t2 = atoi(str.substr(find1,find2 - find1).c_str());
	  if (t2 > max)
	  {
	    Tree->max = t1;
	    max = t2;
	  }
	  Tree->count[t1] = t2;
	} while (str[pos] != '}');
	*/

	pos = str.find("}",find2);
	Data.currentNode = Tree;
	Data.root = Tree;
	if (str[pos + 1] == '[')
	{
	  AddChild(Tree,str,pos + 2, Data);
	}
      }
      else if (str[pos] == ']')
      {
	//if (Data.currentNode->parent) Data.currentNode = Data.currentNode->parent;
      }
      else if (str[pos] == ')')
      {
	if (Data.currentNode->parent) Data.currentNode = Data.currentNode->parent;
      }
      else if (str[pos] == ',')
      {
        AddChild(Data.currentNode,str,pos+1, Data);
      }
    }
    in.close();
  }
  else return false;         // File Error
  return true;
}

bool BOSS::Phimbl::Classify(const BOSS::t_SVector & str, string & res, const t_Data & Data) const
{
  int p, f;
  bool found;
  t_Node* node = Data.root;
  for (unsigned i = 0; i < Data.permutation.size(); i++)
  {
    found = false;
    p  = Data.permutation[i] - 1;
    for (unsigned j = 0; j < node->child.size(); j++)
    {
      stringstream s;
      f = node->child[j]->Feature - 1;
      s << Data.features[f];
      if (s.str() == str[p])
      {
	// cout << str[p] << " -> " << Data.classes[node->Class - 1] << " , ";
	node = node->child[j]->node;
	found = true;
	break;
      }
    }
    if (!found)
    {
      res = Data.classes[node->Class - 1];
      // cout << res << " !" << endl;
      return false;
    }
  }
  res = Data.classes[node->Class - 1];
  cout << res << endl;
  return true;
}


void BOSS::Phimbl::DeleteData(t_Data & Data)
{
  Data.classes.clear();
  Data.features.clear();
  Data.permutation.clear();
  DeleteNode(Data.root);
}

