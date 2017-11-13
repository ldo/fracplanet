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
  \brief Interface for class Matrix33.
*/

#ifndef _matrix33_h_
#define _matrix33_h_

#include "common.h"
#include "xyz.h"

//! Class to hold 3x3 matrics
class Matrix33
{
 public:

  //! Column vectors of matrix
  XYZ basis[3];

  //! Null constructor
  /*! NB Doesn't set up identity or anything useful
   */
  Matrix33()
    {}

  //! Construct from column vectors
  Matrix33(const XYZ& rx,const XYZ& ry,const XYZ& rz)
    {
      basis[0]=rx;
      basis[1]=ry;
      basis[2]=rz;
    }

  //! Destructor.
  ~Matrix33()
    {}

  void assign(const Matrix33& m)
    {
      basis[0]=m.basis[0];
      basis[1]=m.basis[1];
      basis[2]=m.basis[2];
    }

  //! Access a given element
  const float& element(uint row,uint col) const
    {
      return basis[col].element(row);
    }

  //! Access a given element
  float& element(uint row,uint col)
    {
      return basis[col].element(row);
    }

  //! Extract copy of the first row of matrix
  const XYZ row0() const
    {
      return XYZ(basis[0].x,basis[1].x,basis[2].x);
    }

  //! Extract copy of the second row of matrix
  const XYZ row1() const
    {
      return XYZ(basis[0].y,basis[1].y,basis[2].y);
    }

  //! Extract copy of the third row of matrix
  const XYZ row2() const
    {
      return XYZ(basis[0].z,basis[1].z,basis[2].z);
    }

  //! Cofactor of an element
  float cofactor(uint row,uint col) const;

  //! Determinant of matrix 
  float determinant() const;

  //! Return inverse of matrix
  const Matrix33 inverted() const; 
};

//! Multiplication by scalar
inline const Matrix33 operator*(float k,const Matrix33& m)
{
  return Matrix33(k*m.basis[0],k*m.basis[1],k*m.basis[2]);
}

//! Multiplication by scalar
inline const Matrix33 operator*(const Matrix33& m,float k)
{
  return Matrix33(m.basis[0]*k,m.basis[1]*k,m.basis[2]*k);
}

//! Division by scalar
inline const Matrix33 operator/(const Matrix33& m,float k)
{
  return Matrix33(m.basis[0]/k,m.basis[1]/k,m.basis[2]/k);
}

//! Apply matrix to vector
inline const XYZ operator*(const Matrix33& m,const XYZ& v)
{
  return XYZ
    (
     m.row0()%v,
     m.row1()%v,
     m.row2()%v
     );
}

//! Matrix multiplication
inline const Matrix33 operator*(const Matrix33& a,const Matrix33& b)
{
  return Matrix33
    (
     a*b.basis[0],
     a*b.basis[1],
     a*b.basis[2]
     );
}

//! 3x3 Identity matrix
class Matrix33Identity : public Matrix33
{
 public:

  Matrix33Identity()
    {
      basis[0]=XYZ(1.0f,0.0f,0.0f);
      basis[1]=XYZ(0.0f,1.0f,0.0f);
      basis[2]=XYZ(0.0f,0.0f,1.0f);
    }
};

class Matrix33RotateAboutX : public Matrix33
{
 public:

  Matrix33RotateAboutX(float angle)
    {
      const float ca=cos(angle);
      const float sa=sin(angle);
      basis[0]=XYZ(1.0f,0.0f,0.0f);
      basis[1]=XYZ(0.0f, ca,  sa);
      basis[2]=XYZ(0.0f,-sa,  ca);
    }
};

class Matrix33RotateAboutY : public Matrix33
{
 public:

  Matrix33RotateAboutY(float angle)
    {
      const float ca=cos(angle);
      const float sa=sin(angle);
      basis[0]=XYZ(ca,  0.0f,-sa);
      basis[1]=XYZ(0.0f,1.0f,0.0f);
      basis[2]=XYZ(sa  ,0.0f,ca);
    }
};

class Matrix33RotateAboutZ : public Matrix33
{
 public:

  Matrix33RotateAboutZ(float angle)
    {
      const float ca=cos(angle);
      const float sa=sin(angle);
      basis[0]=XYZ( ca,sa,0.0f);
      basis[1]=XYZ(-sa,ca,0.0f);
      basis[2]=XYZ(0.0f,0.0f,1.0f);
    }
};

class Matrix33RotateAboutAxis : public Matrix33
{
 public:

  //! NB Axis must be normalized.
  Matrix33RotateAboutAxis(const XYZ& axis,float angle)
    {
      // Want 2 vectors perpendicular to axis TODO: Check for degenerate cases
      const XYZ axis_ortho0((axis*XYZ(1.0,0.0,0.0)).normalised());
      const XYZ axis_ortho1(axis*axis_ortho0);

      // The matrix which rotates identity basis to axis&orthos.  z axis goes to passed in axis
      const Matrix33 xyz_to_axis
	(
	 axis_ortho0,
	 axis_ortho1,
	 axis
	 );

      const Matrix33 axis_to_xyz(xyz_to_axis.inverted());

      assign(xyz_to_axis*Matrix33RotateAboutZ(angle)*axis_to_xyz);
    }
};

#endif
