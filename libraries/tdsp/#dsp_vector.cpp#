/* dsp_vector.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 1997-2001 University of Bonn.
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
   A class simulating mathematical vector operations.
   @author Karlheinz Stoeber
*/

#include "dsp_vector.h"
#include <string.h>

using namespace dsp;

template< class T > ostream & operator << (ostream & out, const vector< T > & v)
{
  if(!v.Size)
    return out;
  unsigned i;
  for(i=0; i<v.Size-1; i++)
    out << v.Data[i] << " ";
  return out << v.Data[i];
}

template< class T > void vector< T >::emptyData()
{
  if(Data != 0)
    delete [] Data;
  Data = 0;
  Size = 0;
}

template< class T > void vector< T >::alloccp(const T * V, const unsigned & s, const unsigned & l, const unsigned & r) throw(IndexError, AllocationError)
{
  static const string F_Name("void vector<T>::alloccp(const T *, const unsigned &, const unsigned &, const unsigned &)");

  if(l > r || r > s) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Size", s));
    kvlist.push_back(TKVPair("left", l));
    kvlist.push_back(TKVPair("right", r));
    throw IndexError(F_Name, kvlist);
  }
  if(l-r == 0) {
    emptyData();
    return;
  }
  if(Size == r-l) {
    memcpy(Data, V+l, Size*sizeof(T));    
  }
  else {
    emptyData();
    Size = r - l;
    if(Size > 0) {
      alloc(false);
      memcpy(Data, V+l, Size*sizeof(T));
    }
  }
}                                                                           

template< class T > void vector< T >::alloccp(const vector & V, const unsigned & l, const unsigned & r)
{ alloccp(V.Data, V.Size, l, r); }

template< class T > void vector< T >::alloc(const bool flag) throw(AllocationError) 
{
  static const string F_Name("void vector< T >::alloc()");
  if((Data = new T [Size]) == 0) {
    Size = 0;
    throw AllocationError(F_Name, Size);
  }
  if(flag)
    memset(Data, 0, Size*sizeof(T));
}
                           
template< class T > vector< T >::vector() : Size(0), Data(0) {}

template< class T > vector< T >::vector(const unsigned & s) : Size(s), Data(0) 
{ alloc(); };

template< class T > vector< T >::vector(const vector & V) : Size(0), Data(0)
{ alloccp(V, unsigned(0), V.Size); }

template< class T > vector< T >::vector(const vector & V, const unsigned s) : Size(0), Data(0)
{ alloccp(V, unsigned(0), s); }

template< class T > vector< T >::vector(const T * V, const unsigned s) : Size(s), Data(0)
{ alloccp(V, Size, unsigned(0), s); }

template< class T > vector< T >::vector(const vector & V, const unsigned l, const unsigned r) : Size(0), Data(0)
{ alloccp(V, l, r); }                                                           

template< class T > vector< T >::vector(const T * V, const unsigned l, const unsigned r) : Size(0), Data(0)
{ alloccp(V, l, r); }                                                           

template< class T > vector< T >::vector(istream & in) throw(StreamError) : Size(0), Data(0)
{
  static const string F_Name("vector< T >::vector(istream &)");
  unsigned pos = in.tellg();
  in.seekg(0, ios::end);
  Size = in.tellg() - pos;
  in.seekg(pos, ios::beg);
  if(Size % sizeof(T)) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("FileSize", Size));
    kvlist.push_back(TKVPair("TypeSize", sizeof(T)));
    throw IndexError(F_Name, kvlist);
  }
  Size /= sizeof(T);
  alloc(false);
  if(!(in.read((char *)Data, sizeof(T) * Size)))
    throw StreamError(F_Name, StreamError::read);
}

template< class T > vector< T >::vector(istream & in, const unsigned & s)  throw(StreamError, IndexError) : Size(0), Data(0)
{
  static const string F_Name("vector< T >::vector(istream &, const unsigned &)");
  streampos pos = in.tellg();
  in.seekg(0, ios::end);
  Size = in.tellg() - pos;
  in.seekg(pos, ios::beg);
  Size /= sizeof(T);
  if(Size < s) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("FileSize", Size));
    kvlist.push_back(TKVPair("RequSize", s));
    throw IndexError(F_Name, kvlist);
  }
  Size = s;
  alloc(false);
  if(!(in.read((char *)Data, sizeof(T) * Size)))
    throw StreamError(F_Name, StreamError::read);
}

template< class T > vector< T >::vector(istream & in, const unsigned & l, const unsigned & r) throw(StreamError, IndexError) : Size(0), Data(0)
{
  static const string F_Name("vector< T >::vector(istream &, const unsigned &, const unsigned &)");
  Size = r - l;
  in.seekg(l*sizeof(T), ios::beg);
  if(in.tellg()*sizeof(T) < l) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("FileSize", in.tellg()));
    kvlist.push_back(TKVPair("RequSize", Size));
    kvlist.push_back(TKVPair("left", l));
    kvlist.push_back(TKVPair("right", r));
    throw IndexError(F_Name, kvlist);
  }
  in.seekg(r*sizeof(T), ios::beg);
  if(in.tellg()*sizeof(T) < r) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("FileSize", in.tellg()));
    kvlist.push_back(TKVPair("RequSize", Size));
    kvlist.push_back(TKVPair("left", l));
    kvlist.push_back(TKVPair("right", r));
    throw IndexError(F_Name, kvlist);
  }
  in.seekg(l*sizeof(T), ios::beg);
  alloc(false);
  if(!(in.read((char *)Data, sizeof(T) * Size)))
    throw StreamError(F_Name, StreamError::read);
}

template< class T > vector< T >::~vector()
{ emptyData(); }

template< class T > vector<T> & vector< T >::operator = (const vector & V)
{ alloccp(V, unsigned(0), V.Size);  return *this; }

template< class T > T & vector< T >::operator [] (const unsigned & i) throw(IndexError)
{
  static const string F_Name("T & vector< T >::operator [] (const unsigned &)");
  if(i>=Size) { 
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Size", Size));
    kvlist.push_back(TKVPair("Pos", i));
    throw IndexError(F_Name, kvlist);
  }
  return Data[i];
}

template< class T > T vector< T >::operator [] (const unsigned & i) const throw(IndexError)
{
  static const string F_Name("T vector< T >::operator [] (const unsigned &) const");
  if(i>=Size) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Size", Size));
    kvlist.push_back(TKVPair("Pos", i));
    throw IndexError(F_Name, kvlist);
  }
  return Data[i];
}                                         

template< class T > vector< T > vector< T >::operator + (const vector< T > & v) const throw(IndexError)
{
  static const string F_Name("T vector< T >::operator + (const vector< T > & v) const");
  if(v.Size!=Size) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Size1", Size));
    kvlist.push_back(TKVPair("Size2", v.Size));
    throw IndexError(F_Name, kvlist);
  }
  vector< T > res(Size);
  for(unsigned i=0; i<Size; i++)
    res.Data[i] = Data[i] + v.Data[i];
  return res;
}                                         

template< class T > vector< T > vector< T >::operator * (const T & v) const
{
  vector< T > res(*this);
  for(unsigned i=0; i<Size; i++)
    res.Data[i] *= v;
  return res;
}

template< class T > ostream & vector< T >::write(ostream & out)
{
  if(!Size)
    return out;
  
  return out.write((char*)Data, sizeof(T)*Size);
}

template< class T > void vector< T >::write(const string & file) throw (StreamError)
{
  static const string F_Name("void vector< T >::write(const string &)");
  if(!Size)
    throw StreamError(F_Name, StreamError::empty);
  ofstream out(file.c_str());
  if(!out)
    throw StreamError(F_Name, StreamError::open);
  out.write(Data, sizeof(T)*Size);
}

template< class T > void vector< T >::cat(const vector & v)
{
  if(v.Size==0)
    return;
  resize(Size+v.Size);
  memcpy(Data+(Size-v.Size), v.Data,  v.Size*sizeof(T));
}
                                                                               
template< class T > void vector< T >::clear()
{  emptyData(); }

template< class T > void vector< T >::resize(const unsigned & s) throw (AllocationError)
{  
  static const string F_Name("void vector< T >::resize(const unsigned &)");

  if(s == 0) {
    clear();
    return;
  }
  if(s == Size)
    return;
  if(s < Size) {
    if((Data = (T *)realloc(Data, sizeof(T)*(s)))==0)
      throw AllocationError(F_Name, Size);
    Size = s;
    return;
  }
  if(s > Size) {
    if((Data = (T *)realloc(Data, sizeof(T)*(s)))==0)
      throw AllocationError(F_Name, Size);
    memset(Data+Size, 0, (s-Size)*sizeof(T));
    Size = s;
  }
}

