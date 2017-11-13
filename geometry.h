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
  \brief Interface for class Geometry and derived classes.
*/

#ifndef _geometry_h_
#define _geometry_h_

#include "common.h"
#include "scan.h"
#include "vertex.h"
#include "xyz.h"

//! Class to provide abstract interface to different world geometries.
/*! This is an abstract base class providing methods which will differ between world geometries.
  For example, the direction of "up" at a given point will vary depending on whether we are generating a flat world or a spherical one.
  \todo Most of these methods should have their implementation moved geometry.cpp
 */
class Geometry : public ScanConverter
{
public:

  //! Constructor.
  Geometry(uint seed)
    :_r01(seed)
    {}

  //! Destructor.
  virtual ~Geometry()
    {}

  //! Return the height of the given point.
  virtual float height(const XYZ& p) const
    =0;

  //! Move the specified point vertically until.
  virtual void set_height(XYZ& p,float v) const
    =0;

  //! Return a point halfway between the two given points.
  virtual const XYZ midpoint(const XYZ& v0,const XYZ& v1) const
    =0;

  //! Really only meaningful for spherical geometries.
  virtual float normalised_latitude(const XYZ&p) const
    =0;

  //! Return the direction of "up" at the specified point.
  virtual const XYZ up(const XYZ& p) const
    =0;
  //! Return the direction of "north" at the specified point.
  virtual const XYZ north(const XYZ& p) const
    =0;
  //! Return the direction of "east" at the specified point.
  virtual const XYZ east(const XYZ& p) const
    =0;

  //! Add a random variation to a point
  virtual const XYZ perturb(const XYZ& v,const XYZ& variation) const
    =0;

  //! Nasty hack to work around height setting possibly not being exact.
  /*! In some geometries (e.g spherical, but not flat) modifying a point to be at a particular height does not guarantee that exact value will be returned on a susequent height query.
    If this is the case, a non-zero epsilon value can be returned and used as an error tolerence when comparing two heights for equivalence.
   */
  virtual float epsilon() const
    =0;

  //! Multiplier for width of a scan-converted image.
  virtual uint scan_convert_image_aspect_ratio() const
    {
      return 1;
    }

 protected:

  //! Common scan-converter code
  static void scan_convert_common
    (
     const boost::array<XYZ,3>& v,
     const ScanConvertBackend& backend
     );

  //! Random number generator used for perturbations and the like.
  /*! Declared mutable so it can be used in const methods.
    \todo Perhaps theres a better place for the geometry random number generator to live: having it here creates the anomaly of having to pass random seeds into apparently non-random objects like icosahedrons etc.
   */
  mutable Random01 _r01;
};

//! Concrete class providing a flat geometry (in the XY-plane, with Z up).
class GeometryFlat : public Geometry
{
 public:

  GeometryFlat(uint seed)
    :Geometry(seed)
    {}

  ~GeometryFlat()
    {}

  //! Height is just the z co-ordinate of a point.
  float height(const XYZ& p) const
    {
      return p.z;
    }
  
  //! Setting a height is simply assigning to the z-coordinate.
  void set_height(XYZ& p,float v) const
    {
      p.z=v;
    }
  
  //! The mid-point between two points is simply their average.
  const XYZ midpoint(const XYZ& v0,const XYZ& v1) const
    {
      return 0.5f*(v0+v1);
    }

  //! This doesn't really mean anything here, so return zero, which would correspond to the equator of a spherical geometry.
  float normalised_latitude(const XYZ&) const
    {
      return 0.0f;
    }
  
  //! Returns unit z vector.  (Up is the same everywhere in this geometry).
  const XYZ up(const XYZ&) const
    {
      return XYZ(0.0f,0.0f,1.0f);
    }

  //! Returns unit y vector.  (North is the same everywhere in this geometry).
  const XYZ north(const XYZ&) const
    {
      return XYZ(0.0f,1.0f,0.0f);
    }

  //! Returns unit x vector.  (East is the same everywhere in this geometry).
  const XYZ east(const XYZ&) const
    {
      return XYZ(1.0f,0.0f,0.0f);
    }

  //! Add a random variation to a point.
  const XYZ perturb(const XYZ& p,const XYZ& variation) const
    {
      // The correct thing to do would be to return p+RandomXYZInEllipsoid(_r01,variation);
      // however, this uses a variable number of random number calls which means small parameter changes can have big effects on generated terrain.

      // This, on the other hand, always uses the same number of random numbers, but isn't statistically equivalent:
      return p+RandomXYZInBox(_r01,variation);
    }

  //! Returns zero.  Heights are stored exactly once assigned so no need for non-zero epsilon.
  float epsilon() const
    {
      return 0.0f;  // No need 'cos heights are stored exactly
    }

  virtual void scan_convert
    (
     const boost::array<XYZ,3>& v,
     const ScanConvertBackend&
     ) const;
};

//! Concrete class providing a flat geometry (a sphere with nominal radius 1, equator in the XY-plane, Z axis through the poles).
class GeometrySpherical : public Geometry
{
 public:
  //! Constructor.
  GeometrySpherical(uint seed)
    :Geometry(seed)
    {}

  //! Destructor.
  ~GeometrySpherical()
    {}

  //! Height is relative to the surface of the unit radius sphere.
  float height(const XYZ& p) const
    {
      return p.magnitude()-1.0f;
    }

  //! The height set is relative to the surface of the unit radius sphere.
  void set_height(XYZ& p,float h) const
    {
      const float m=p.magnitude();
      p*=((1.0f+h)/m);
    }

  //! Don't just take the mid-point of the straight-line path through the sphere's surface: must work relative to the sphere's surface.
  const XYZ midpoint(const XYZ& v0,const XYZ& v1) const
    {
      const float h0=v0.magnitude();
      const float h1=v1.magnitude();
      const float h_av=0.5f*(h0+h1);

      const XYZ m(0.5f*(v0+v1));
      return (h_av/m.magnitude())*m;
    } 
  
  //! Normalised latitude is 1.0 at the north pole, -1.0 at the south pole
  float normalised_latitude(const XYZ& p) const
    {
      return p.z;
    }

  //! Up is normal to the sphere.
  const XYZ up(const XYZ& p) const
    {
      return p.normalised();
    }

  //! North is perpendicular to "up" and "east"
  /*! \warning Returns zero vector at the poles
   */
  const XYZ north(const XYZ& p) const
    {
      if (p.x==0.0f && p.y==0.0f)
	return XYZ(0.0f,0.0f,0.0f);
      else
	return (up(p)*east(p)).normalised();
    }  

  //! East is perpendicular to "up" and the polar vector.
  /*! \warning Returns zero vector at the poles
   */
  const XYZ east(const XYZ& p) const
    {
      if (p.x==0.0f && p.y==0.0f)
	return XYZ(0.0f,0.0f,0.0f);
      else
	return (XYZ(0.0f,0.0f,1.0f)*up(p)).normalised();
    }

  //! Add a random variation to a point.
  /*! In spherical geometry, the variation ellipsoid tracks the surface (ie z corresponds to up, north to y)
   */
  const XYZ perturb(const XYZ& p,const XYZ& variation) const
    {
      // The correct thing to do would be to use const RandomXYZInEllipsoid v(_r01,variation);
      // however, this uses a variable number of random number calls which means small parameter changes can have big effects on generated terrain.

      // This, on the other hand, always uses the same number of random numbers, but isn't statistically equivalent:
      const RandomXYZInBox v(_r01,variation);
      return p+v.x*east(p)+v.y*north(p)+v.z*up(p);
    }

  //! This needs to return something small for the lake flooding algorithm to work.
  float epsilon() const
    {
      return 0.000001f;
    }

  void scan_convert
    (
     const boost::array<XYZ,3>& v,
     const ScanConvertBackend&
     ) const;

  //! Return 2.0 for spheres because vertical range is +/- pi/2, horizontal is +/- pi
  uint scan_convert_image_aspect_ratio() const
    {
      return 2;
    }
};

#endif
