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

#include "xyz.h"

/*! Table so we can look up by element number.
 */
XYZ::ElementPtr XYZ::element_table[3]={&XYZ::x,&XYZ::y,&XYZ::z};

std::ostream& XYZ::write(std::ostream& out) const
{
  return out << x << " " << y << " " << z;
}

const std::string XYZ::format_comma() const
{
  std::ostringstream s;
  s << x << "," << y << "," << z;
  return s.str();
}

const std::string XYZ::format_blender() const
{
  std::ostringstream s;
  s << x << ", " << y << ", " << z;
  return s.str();
}

/*! Also transposes y and z co-ordinates because of POV-Rays idea of up.
 */
const std::string XYZ::format_pov() const
{
  std::ostringstream s;
  s << "<" << x << "," << z << "," << y << ">";
  return s.str();
}

RandomXYZInUnitCube::RandomXYZInUnitCube(Random01& rng)
:XYZ()
{
  x=rng();
  y=rng();
  z=rng();
}

RandomXYZInBox::RandomXYZInBox(Random01& rng,const XYZ& bounds)
:XYZ()
{
  x=-bounds.x+2.0*bounds.x*rng();
  y=-bounds.y+2.0*bounds.y*rng();
  z=-bounds.z+2.0*bounds.z*rng();
}

RandomXYZInSphere::RandomXYZInSphere(Random01& rng,float radius)
:XYZ()
{
  do
    {
      x=2.0*rng()-1.0;
      y=2.0*rng()-1.0;
      z=2.0*rng()-1.0;
    }
  while (magnitude2()>1.0);
  (*this)*=radius;
}

/*! Can handle case of individual axes being zero.
 */
RandomXYZInEllipsoid::RandomXYZInEllipsoid(Random01& rng,const XYZ& axes)
:XYZ()
{
  do
    {
      assign(RandomXYZInBox(rng,axes));
    }
  while (
	  (axes.x==0.0 ? 0.0 : sqr(x/axes.x))
	 +(axes.y==0.0 ? 0.0 : sqr(y/axes.y))
	 +(axes.z==0.0 ? 0.0 : sqr(z/axes.z))
	 >1.0
	 );
}

RandomXYZSphereNormal::RandomXYZSphereNormal(Random01& rng)
:XYZ(0.0,0.0,0.0)
{
  float m2;
  do
    {
      assign(RandomXYZInSphere(rng,1.0));
      m2=magnitude2();
    }
  while (m2==0.0);

  (*this)/=sqrtf(m2);
}
