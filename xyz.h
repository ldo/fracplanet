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
  \brief Interface for class XYZ.
*/

#ifndef _xyz_h_
#define _xyz_h_

#include "common.h"
#include "random.h"

//! Class to hold vectors in 3D cartesian co-ordinates.
/*! Direct access to the x,y,z members is permitted.
    There is a general assumption that the co-ordinate system will be right handed,
    and that for terrain type applications x and y will be plan position and z will be height.
 */
class XYZ
{
 public:
  
  float x;
  float y;
  float z;

  //! Null constructor.
  /*! NB The components are not cleared to zero. 
   */
  XYZ()
    {}

  //! Copy constructor.
  XYZ(const XYZ& v)
    :x(v.x),y(v.y),z(v.z){}

  //! Initialise from separate components.
  XYZ(float vx,float vy,float vz)
    :x(vx),y(vy),z(vz){}

  //! Destructor.
  ~XYZ()
    {}

  typedef float XYZ::* ElementPtr;
  static ElementPtr element_table[3];

  //! Access by number
  const float& element(uint e) const
    {
      return this->*(element_table[e]);
    }

  //! Access by number
  float& element(uint e)
    {
      return this->*(element_table[e]);
    }

  //! Multiply by scalar.
  void operator*=(float k)
    {
      x*=k;
      y*=k;
      z*=k;
    }

  //! Divide by scalar.
  /*! Implemented assuming one divide and three multiplies is faster than three divides.
   */
  void operator/=(float k)
    {
      const float ik(1.0/k);
      x*=ik;
      y*=ik;
      z*=ik;
    }

  //! Vector addition.
  void operator+=(const XYZ& v)
    {
      x+=v.x;
      y+=v.y;
      z+=v.z;
    }

  //! Vector subtraction.
  void operator-=(const XYZ& v)
    {
      x-=v.x;
      y-=v.y;
      z-=v.z;
    }

  //! Assignment. 
  void assign(const XYZ& v)
    {
      x=v.x;
      y=v.y;
      z=v.z;
    }

  //! Negation.
  const XYZ operator-() const
    {
      return XYZ(-x,-y,-z);
    }

  //! Return the square of the magnitude.
  float magnitude2() const
    {
      return x*x+y*y+z*z;
    }

  //! Return the magnitude.
  float magnitude() const
    {
      return sqrt(magnitude2());
    }

  //! Return the vector normalised.
  const XYZ normalised() const;

  //! Normalise this vector.
  void normalise();

  //! Write the vector (spaces as separators).
  std::ostream& write(std::ostream&) const;

  //! Alternate formatting.
  const std::string format_comma() const;

  //! Alternate formatting.
  const std::string format_blender() const;

  //! Alternate formatting.
  const std::string format_pov() const;
};

//! Cross product.
inline const XYZ operator*(const XYZ& a,const XYZ& b)
{
  return XYZ(
	     a.y*b.z-a.z*b.y,
	     a.z*b.x-a.x*b.z,
	     a.x*b.y-a.y*b.x
	     );
} 

//! Dot product.
/*! Perhaps a curious choice of operator but it works for me.
 */
inline float operator%(const XYZ& a,const XYZ& b)
{
  return a.x*b.x+a.y*b.y+a.z*b.z;
} 

//! Vector addition.
inline const XYZ operator+(const XYZ& a,const XYZ& b)
{
  return XYZ(a.x+b.x,a.y+b.y,a.z+b.z);
}

//! Vector subtraction.
inline const XYZ operator-(const XYZ& a,const XYZ& b)
{
  return XYZ(a.x-b.x,a.y-b.y,a.z-b.z);
}

//! Multiplication by scalar.
inline const XYZ operator*(float k,const XYZ& v)
{  
  return XYZ(k*v.x,k*v.y,k*v.z);
}

//! Multiplication by scalar.
inline const XYZ operator*(const XYZ& v,float k)
{
  return XYZ(k*v.x,k*v.y,k*v.z);
}

//! Division by scalar.
inline const XYZ operator/(const XYZ& v,float k)
{
  return (1.0/k)*v;
}

//! Equality operator.
inline bool operator==(const XYZ& a,const XYZ& b)
{
  return (a.x==b.x && a.y==b.y && a.z==b.z);
}

//! Inequality operator.
inline bool operator!=(const XYZ& a,const XYZ& b)
{
  return (a.x!=b.x || a.y!=b.y || a.z!=b.z);
}

/*! Will fail assertion if the co-ordinate has zero magnitude.
 */
inline const XYZ XYZ::normalised() const
{
  const float m=magnitude();
  assert(m!=0.0);
  return (*this)/m;
}

/*! Will fail assertion if the co-ordinate has zero magnitude.
 */
inline void XYZ::normalise()
{
  (*this)=normalised();
}

//! Stream output operator.
inline std::ostream& operator<<(std::ostream& out,const XYZ& v)
{
  return v.write(out);
}

//! Generates a random point in the cube bounded by (0,0,0) and (1.0,1.0,1.0)
class RandomXYZInUnitCube : public XYZ
{
 public:
  RandomXYZInUnitCube(Random01&);
};

//! Generates random points in a recnangular box centred on the origin
class RandomXYZInBox : public XYZ
{
 public:
  RandomXYZInBox(Random01& rng,const XYZ& bounds);
};

//! Generates a random point in or on a unit-radius sphere centred on the origin.
class RandomXYZInSphere : public XYZ
{
 public:
  RandomXYZInSphere(Random01& rng,float radius);
};

//! Generates a random point in or on an origin-centred ellipsoid with semi-axes of the specified size.
class RandomXYZInEllipsoid : public XYZ
{
 public:
  RandomXYZInEllipsoid(Random01& rng,const XYZ& axes);
};

//! Generates a random point on the surface of a unit-radius sphere
class RandomXYZSphereNormal : public XYZ
{
 public:
  RandomXYZSphereNormal(Random01& rng);
};

#endif
