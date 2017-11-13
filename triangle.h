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
  \brief Interface for class Triangle.
 */

#ifndef _triangle_h_
#define _triangle_h_

#include "common.h"
#include "rgb.h"
#include "xyz.h"

//! Class to store triangle state.
/*! There is no direct access to members.
  Should probably be a protected member class of TriangleMesh.
 */
class Triangle
{
 public:

  Triangle()
    {}

  Triangle(uint v0,uint v1,uint v2)
    {
      _vertex[0]=v0;
      _vertex[1]=v1;
      _vertex[2]=v2;
    }

  Triangle(const Triangle& t)
    {
      _vertex[0]=t.vertex(0);
      _vertex[1]=t.vertex(1);
      _vertex[2]=t.vertex(2);
    }
  
  //! Return a reference to the vertex because GL will want it's address
  const uint& vertex(uint i) const
    {
      return _vertex[i];
    }

 protected:

  uint _vertex[3];
};

#endif
