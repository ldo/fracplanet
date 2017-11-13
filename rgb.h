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

/* Copyright (C) 1998,1999 T Day */

/*! \file
  \brief Interface for class ByteRGBA and class FloatRGBA.
*/

#ifndef _image_rgb_h_
#define _image_rgb_h_

#include "common.h"

/*! Direct access to class members is permitted.
  The uchar version is intended as a minimal class for efficient storage of colours.
  (However, padding means there is a wasted byte if we just want RGB so we
  might as well have an alpha channel).
 */
template <typename T> class RGBA
{
 public:
  //@{
  //! Colour component.
  T r;
  T g;
  T b;
  T a;
  //@}

  //! Null constructor.
  /* NB There are no default values.
   */
  RGBA()
    {}

  //! Copy constructor.
  RGBA(const RGBA& c)
    :r(c.r)
    ,g(c.g)
    ,b(c.b)
    ,a(c.a)
    {}

  //! Initialise from separate components.
  RGBA(T vr,T vg,T vb,T va)
    :r(vr)
    ,g(vg)
    ,b(vb)
    ,a(va)
    {}

  //! Colour addition.
  void operator+=(const RGBA& v)
    {
      r+=v.r;
      g+=v.g;
      b+=v.b;
      a+=v.a;
    }
  
  //! Colour subtraction.
  void operator-=(const RGBA& v)
    {
      r-=v.r;
      g-=v.g;
      b-=v.b;
      a-=v.a;
    }
};

//! Colour equality operator.
template <typename T> inline bool operator==(const RGBA<T>& a,const RGBA<T>& b)
{
  return (a.r==b.r && a.g==b.g && a.b==b.b && a.a==b.a);
}

//! Colour inequality operator.
template <typename T> inline bool operator!=(const RGBA<T>& a,const RGBA<T>& b)
{
  return (a.r!=b.r || a.g!=b.g || a.b!=b.b || a.a!=b.a);
}

//! Colour addition operator.
template <typename T> inline RGBA<T> operator+(const RGBA<T>& a,const RGBA<T>& b)
{
  return RGBA<T>
    (
     a.r+b.r,
     a.g+b.g,
     a.b+b.b,
     a.a+b.a
     );
}

//! Colour negation operator.
template <typename T> inline RGBA<T> operator-(const RGBA<T>& c)
{
  return RGBA<T>
    (
     -c.r,
     -c.g,
     -c.b,
     -c.a
     );
}

//! Colour subtraction operator.
template <typename T> inline RGBA<T> operator-(const RGBA<T>& a,const RGBA<T>& b)
{
  return RGBA<T>
    (
     a.r-b.r,
     a.g-b.g,
     a.b-b.b,
     a.a-b.a
     );
}


//! Class to represent red-green-blue-alpha colours stored with 8-bit resolution.
class ByteRGBA : public RGBA<uchar>
{
 public:

  //! Null constructor.  NB There are no default values.
  ByteRGBA()
    :RGBA<uchar>()
    {}

  //! Copy constructor.
  ByteRGBA(const RGBA<uchar>& c)
    :RGBA<uchar>(c)
    {}

  //! Componentwise initialization.
  ByteRGBA(uchar vr,uchar vg,uchar vb,uchar va)
    :RGBA<uchar>(vr,vg,vb,va)
    {}
  
  //! Construct ByteRGBA from float RGBAs.
  /*! Components in the range [0.0,1.0] are scaled to [0,255].
   */
  explicit ByteRGBA(const RGBA<float>& c)
    :RGBA<uchar>
    (
     static_cast<uchar>(255.0*clamped(c.r,0.0f,1.0f)),
     static_cast<uchar>(255.0*clamped(c.g,0.0f,1.0f)),
     static_cast<uchar>(255.0*clamped(c.b,0.0f,1.0f)),
     static_cast<uchar>(255.0*clamped(c.a,0.0f,1.0f))
     )
    {}
  
  std::ostream& write(std::ostream&) const;
  const std::string format_comma() const;
};

//! Class to represent red-green-blue-alpha colours stored to floating point accuracy.
class FloatRGBA : public RGBA<float>
{
public:

  //! Null constructor.
  /* NB There are no default values.
   */
  FloatRGBA()
    {}

  //! Copy constructor.
  FloatRGBA(const RGBA<float>& c)
    :RGBA<float>(c)
    {}
  
  //! Initialise from separate components.
  FloatRGBA(float vr,float vg,float vb,float va)
    :RGBA<float>(vr,vg,vb,va)
    {}

  //! Initialise from ByteRGBA.
  /*! Byte values [0,255] are normalised to [0.0,1.0]
   */
  explicit FloatRGBA(const RGBA<uchar>& c)
    :RGBA<float>(c.r/255.0f,c.g/255.0f,c.b/255.0,c.a/255.0)
    {}
  
  //! Output method.
  std::ostream& write(std::ostream&) const;

  const std::string format_pov_rgb() const;

  const std::string format_pov_rgbf() const;
};

//! Colour multiplication-by-scalar operator.
inline FloatRGBA operator*(float k,const FloatRGBA& c)
{
  return FloatRGBA
    (
     k*c.r,
     k*c.g,
     k*c.b,
     k*c.a
     );
}

//! Colour multiplication-by-scalar operator.
inline FloatRGBA operator*(const FloatRGBA& c,float k)
{
  return FloatRGBA
    (
     k*c.r,
     k*c.g,
     k*c.b,
     k*c.a
     );
}

//! Colour division-by-scalar operator.
inline FloatRGBA operator/(const FloatRGBA& c,float k)
{
  return FloatRGBA
    (
     c.r/k,
     c.g/k,
     c.b/k,
     c.a/k
     );
}


//! Colour multiplication operator.
/*! Componentwise multiplication.
 */
inline FloatRGBA operator*(const FloatRGBA& a,const FloatRGBA& b)
{
  return FloatRGBA
    (
     a.r*b.r,
     a.g*b.g,
     a.b*b.b,
     a.a*b.a
     );
}

inline std::ostream& operator<<(std::ostream& out,const FloatRGBA& c)
{
  return c.write(out);
}

#endif
