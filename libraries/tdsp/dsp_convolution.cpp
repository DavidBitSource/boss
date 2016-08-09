/* dsp_convolution.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   A set of functions used for signal convolution.
   @author Karlheinz Stoeber
*/

#include <dsp_convolution.h>
#include <fstream>

template< class T > vector< T > ReadFilter(const string & name)
{
  ifstream in(name.c_str());
  if(!in) {
    cerr << "Can't open file " << name  << endl;
    exit (-1);
  }

  T tmp;
  vector< T > filter(100);
  unsigned i(0);
  while(in >> tmp) {
    filter[i++] = tmp;
    if(i+1 == filter.size())
      filter.resize(filter.size()+100);
  }
  filter.resize(i);
  return filter;
}

template< class T > vector<T> ConvHbDown(const vector<T> & signal, const vector<T> & filter)
{
  int   j, k;
  int   sl = signal.size();
  int   fl = filter.size();

  if(sl%2)
    j = sl / 2 + 1;
  else
    j = sl / 2;

  vector<T> res(j);
  T  sum;

  if(sl < 2*fl) {
    cerr << "Signal size should be greater than filter size" << endl;
    return 0;
  }

  // first case j-k<0
  for(j=0; j<fl; j+=2) {
    sum = signal[j] * filter[0];
    for(k=1; k<fl; k+=2) {
      if(j-k<0)
	sum += signal[j+k] * filter[k];
      else
	sum += (signal[j-k] + signal[j+k]) * filter[k];
    }
    res[j>>1] = sum;
  }

  // second case j-k>=0 und j+k<sl
  int l = sl - fl;
  for(; j<l; j+=2) {
    sum = signal[j] * filter[0];
    for(k=1; k<fl; k+=2)
      sum += (signal[j-k] + signal[j+k]) * filter[k];
    res[j>>1] = sum;
  }

  // third case j+k>sl
  for(; j<sl; j+=2) {
    sum = signal[j] * filter[0];
    for(k=1; k<fl; k+=2) {
      if(j+k>=sl)
	sum += signal[j-k] * filter[k];
      else
	sum += (signal[j-k] + signal[j+k]) * filter[k];
    }
    res[j>>1] = sum;
  }
  return res;
}

template< class T > vector<T> ConvHbUp(const vector<T> & signal, const vector<T> & filter)
{
  int   i, j, k, k2;
  int   sl = signal.size();
  int   fl = filter.size();

  vector<T> res(2*sl);
  T  sum;

  if(sl < 2*fl) {
    cerr << "Signal size should be greater than filter size" << endl;
    return res;
  }

  for(j=i=0; i<int(signal.size()); j++) {
    if(!(j%2)) {
      res[j] = (T)(2.0 * signal[j>>1] * filter[0]);
    }
    else {
      sum = (T)0;
      for(k=1; k<fl; k+=2) {
	k2 = k>>1;
	if(i-k2 < 0) {
	  sum += signal[i+k2+1] * filter[k];
	  continue;
	}
	if(i+k2+1 >= int(signal.size())) {
	  sum += signal[i-k2] * filter[k];
	  continue;
	}
	sum += (signal[i-k2] + signal[i+k2+1]) * filter[k];
      }
      i++;
      res[j] = (T)(2.0 * sum);
    }
  }

  return res;
}

template< class T > vector<T> ConvSym(const vector<T> & signal, const vector<T> & filter)
{
  int   j, k;
  int   sl = signal.size();
  int   fl = filter.size();

  vector<T> res(sl);
  T  sum;

  if(sl < 2*fl) {
    cerr << "Signal size should be greater than filter size" << endl;
    return 0;
  }

  // first case j-k<0
  for(j=0; j<fl; j++) {
    sum = signal[j] * filter[0];
    for(k=1; k<fl; k++) {
      if(j-k<0)
	sum += signal[j+k] * filter[k];
      else
	sum += (signal[j-k] + signal[j+k]) * filter[k];
    }
    res[j] = sum;
  }

  // second case j-k>=0 and j+k<sl
  int l = sl - fl;
  for(; j<l; j++) {
    sum = signal[j] * filter[0];
    for(k=1; k<fl; k++)
      sum += (signal[j-k] + signal[j+k]) * filter[k];
    res[j] = sum;
  }

  // third case j+k>sl
  for(; j<sl; j++) {
    sum = signal[j] * filter[0];
    for(k=1; k<fl; k++) {
      if(j+k>=sl)
	sum += signal[j-k] * filter[k];
      else
	sum += (signal[j-k] + signal[j+k]) * filter[k];
    }
    res[j] = sum;
  }
  return res;
}

template< class T > vector<T> Conv(const vector<T> & signal, const vector<T> & filter, const unsigned fzero)
{
  int   j, k;
  int   sl = signal.size();
  int   fl = filter.size();

  vector<T> res(sl);
  T  sum;

  if(sl < fl) {
    cerr << "Signal size should be greater than filter size" << endl;
    return 0;
  }

  for(j=0; j<sl; j++) {
    for(k=-fzero, sum=(T)0; k<fl-int(fzero); k++) {
      if(j-k<0 || j-k>=sl)
	continue;
      sum += signal[max(j-k, 0)] * filter[min(fzero+k, filter.size()-1)];
    }
    res[j] = sum;
  }
  return res;
}
