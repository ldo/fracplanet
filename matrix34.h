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
  \brief Interface for class Matrix34.
*/

#ifndef _matrix34_h_
#define _matrix34_h_

#include "common.h"
#include "matrix33.h"
#include "xyz.h"

//! Class to hold 3x4 matrices
/*! Rotation below actually means any sort of origin preserving transform; 
  it's just easier to visualize rotations and most of the time that's what they are.
 */
class Matrix34
{
 public:

  //! 3x3 rotational component
  Matrix33 rotate;

  //! Translational component
  XYZ translate;
  //! Null constructor.
  /*! NB The components are not cleared to zero. 
   */
  Matrix34()
    {}

  //! Construct from a rotation and translation
  Matrix34(const Matrix33& r,const XYZ& t)
    :rotate(r)
    ,translate(t)
    {}

  //! Construct from column vectors
  Matrix34(const XYZ& rx,const XYZ& ry,const XYZ& rz,const XYZ& tr)
    :rotate(rx,ry,rz)
    ,translate(tr)
    {}

  //! Destructor.
  ~Matrix34()
    {}

  void assign(const Matrix34& m)
    {
      rotate=m.rotate;
      translate=m.translate;
    }
};

inline const XYZ operator*(const Matrix34& m,const XYZ& v)
{
  return m.rotate*v+m.translate;
}

inline const Matrix34 operator*(const Matrix34& a,const Matrix34& b)
{
  return Matrix34
    (
     a.rotate*b.rotate,
     a.rotate*b.translate+a.translate
     );
}

class Matrix34Identity : public Matrix34
{
 public:
  Matrix34Identity()
    :Matrix34(Matrix33Identity(),XYZ(0.0f,0.0f,0.0f))
    {}
};

class Matrix34Translate : public Matrix34
{
 public:
  Matrix34Translate(const XYZ& t)
    :Matrix34(Matrix33Identity(),t)
    {}
};

class Matrix34RotateAboutAxisThrough : public Matrix34
{
 public:
  //! Axis must be normalized
  Matrix34RotateAboutAxisThrough(const XYZ& axis,float angle,const XYZ& pt);

  //! Axis size gives rotation size
  Matrix34RotateAboutAxisThrough(const XYZ& axis,const XYZ& pt);
};

#endif





