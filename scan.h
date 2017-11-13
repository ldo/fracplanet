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

/*! \file
  \brief Interface for classes for scan conversion.
*/

#ifndef _scan_h_
#define _scan_h_

#include "common.h"
#include "xyz.h"

//! Encapsulates information needed for scan conversion.
/*! We want to be independent of what quantity the client is going to interpolate over,
  so the best we can do is identify (by index in given triangle) the vertices delimiting an edge and give the weights.
*/
class ScanEdge
{
 public:

  ScanEdge()
    {}

  ScanEdge(float vx,uint v0,uint v1,float l)
    :x(vx)
    ,vertex0(v0)
    ,vertex1(v1)
    ,lambda(l)
    {}

  float x;
  uint vertex0;
  uint vertex1;
  float lambda;
};

class ScanConvertBackend;

class ScanConverter
{
 public:

  ScanConverter()
    {}

  virtual ~ScanConverter()
    {}

  //! Set-up for scan conversion of given vertices to the given map.
  /* Scan conversion output is a series of [,) open intervals with vertex identifiers and weightings for each end.
   */
  virtual void scan_convert
    (
     const boost::array<XYZ,3>& v,
     const ScanConvertBackend&
     ) const
    =0;
};

class ScanConvertBackend
{
 public:

  ScanConvertBackend(int w,int h)
    :_width(w)
    ,_height(h)
    {}

  virtual ~ScanConvertBackend()
    {}

  int width() const
    {
      return _width;
    }

  int height() const
    {
      return _height;
    }

  virtual void scan_convert_backend(uint y,const ScanEdge& edge0,const ScanEdge& edge1) const
    =0;

  virtual void subdivide(const boost::array<XYZ,3>&,const XYZ&,const ScanConverter&) const
    =0;

 private:

  const int _width;

  const int _height;
};

#endif
