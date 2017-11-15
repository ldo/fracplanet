/**************************************************************************/
/*  Copyright 2009 Tim Day                                                */
/*                                                                        */
/*  This file is part of Fracplanet                                       */
/*                                                                        */
/*  Fracplanet is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Fracplanet is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Fracplanet.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

#ifndef _common_h_
#define _common_h_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/random.hpp>
#include <boost/range.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>

#define stringify(S) __STRING(S)

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

template <class T> inline const T maximum(T a,T b) {return (a>b ? a : b);}
template <class T> inline const T minimum(T a,T b) {return (a<b ? a : b);}

template <class T> inline const T maximum(T a,T b,T c) {return maximum(a,maximum(b,c));}
template <class T> inline const T minimum(T a,T b,T c) {return minimum(a,minimum(b,c));}

template <class T> inline const T maximum(T a,T b,T c,T d) {return maximum(maximum(a,b),maximum(c,d));}
template <class T> inline const T minimum(T a,T b,T c,T d) {return minimum(minimum(a,b),minimum(c,d));}

template <class T> inline const T sqr(T a) {return a*a;}

template <class T> inline const T clamped(T v,T lo,T hi) {return (v<lo ? lo : (v>hi ? hi : v));}

template <class T> inline void clamp(T& v,T lo,T hi) {v=(v<lo ? lo : (v>hi ? hi : v));}

template <class T> inline void exchange(T& a,T& b) {const T x(a);a=b;b=x;}

extern void fatal_error(const char*);

inline void fatal_error(const std::string& s)
{
  fatal_error(s.c_str());
}

extern void fatal_internal_error(const char* src_file,uint src_line);

extern void constraint_violation(const char* test,const char* src_file,uint src_line);
#define constraint(TEST) {if (!TEST) {constraint_violation(#TEST,__FILE__,__LINE__);}}

#endif
