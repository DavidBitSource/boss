/* dsp_melcep.cpp    $Revision: 1.3 $ $Date: 2005/12/17 16:06:11 $
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
   A class computing the MelCepstrum coefficients for a vector.
   @author Karlheinz Stoeber
*/

#include "dsp_melcep.h"

#define sqr(x) ((x)*(x))

template < class T >
T Cabs(const T & a, const T & b)
{
  double x,y,ans,temp;
  x=fabs(a);
  y=fabs(b);
  if (x == 0.0)
    ans=y;
  else if (y == 0.0)
    ans=x;
  else if (x > y) {
    temp=y/x;
    ans=x*sqrt(1.0+temp*temp);
  } else {
    temp=x/y;
    ans=y*sqrt(1.0+temp*temp);
  }
  return ans;
}

template< class T > TriagFilter< T >::TriagFilter() : l(0), m(0), r(0) { ; }

template< class T > TriagFilter< T >::TriagFilter(const T & L, const T & M, const T & R) :
  l(L),
  m(M),
  r(R)
  {
    trl = dsp::line<T>(typename dsp::line<T>::point(L, 0.0), typename dsp::line<T>::point(M, 1.0));
    trr = dsp::line<T>(typename dsp::line<T>::point(M, 1.0), typename dsp::line<T>::point(R, 0.0));
  }

template< class T > T TriagFilter< T >::operator () (const T & f_hz) const
{
  if(f_hz<=l || f_hz>=r) return 0;
  if(f_hz<m)             return trl(f_hz);
  return trr(f_hz);
}

template< class T > T MelCepstrum< T >::HzToMel(const T & x) const
{ return 2595 * log10(1 + x / 700); }

template< class T > T MelCepstrum< T >::MelToHz(const T & x) const
{ return (pow(10.0, x / 2595) - 1) * 700; }

template< class T > void MelCepstrum< T >::createFilter(const T & l_hz, const T & r_hz)
{
  T l_mel     = HzToMel(l_hz);
  T step_mel  = (HzToMel(r_hz) - l_mel) / (triags.size()+1);
  T l;
  for(unsigned i=0; i<triags.size(); i++) {
    l = l_mel + i * step_mel;
    triags[i] = TriagFilter< T >(MelToHz(l), MelToHz(l+step_mel), MelToHz(l+2*step_mel));
  }				
}

template< class T > MelCepstrum< T >::MelCepstrum(const unsigned nrbands, const T l_hz, const T r_hz,
						  const unsigned ffts, const unsigned sr, const unsigned cc) :
  triags(nrbands), ft(ffts), cepcoeff(cc)
{ createFilter(l_hz, r_hz); delta = T(sr)/T(ffts); }

template< class T > vector< T > MelCepstrum< T >::operator () (const vector< T > & d, const bool energy) const
{
  // Apply the weighting window and do zero padding
  vector< T > f(ft.size());
  int i, d2 = d.size() / 2, k = (f.size() - d.size()) / 2;
  for(i=0; i<int(d.size()); i++)
    f[i+k] = d[i] * hamming(i-d2, d.size());

  // Fourier transform
  f = ft(f, fft<T>::FORWARD);

  // Magnitude
  T t = f[1];
  f[1] = 0;
  for(i=0; i<int(f.size()); i+=2)
    f[i/2] = sqrt(sqr(f[i]) + sqr(f[i+1]));
  f[i/2] = sqrt(sqr(t));
  f.resize(i/2+1);

  // Triag weighted Fourier coefficients
  vector< T > w(triags.size());
  unsigned l, r;
  for(i=0; i<int(triags.size()); i++) {
    l = unsigned(ceil(triags[i].l / delta));
    r = unsigned(ceil(triags[i].r / delta));
    w[i] = 0.0;
    for(unsigned j=l; j<r; j++)
      w[i] += triags[i](j*delta) * f[j];
    w[i] = log(max(w[i], T(1.0)));
  }

  // Inverse cosine transform
  vector< T > c(cepcoeff);
  T Q = M_PI / T(w.size());
  unsigned n;
  for(n=1; n<=cepcoeff; n++) {
    for(k=1; k<=int(w.size()); k++)
      c[n-1] += w[k-1] * cos(T(n) * (T(k) - 0.5) * Q);
  }

  if(energy) {
    if(w[0] > 1)
      c[cepcoeff-1] = log(w[0]);
    else
      c[cepcoeff-1] = 0;
  }
      

  return c;
}
