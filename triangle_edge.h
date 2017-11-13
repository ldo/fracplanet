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
  \brief Interface for class TriangleEdge.
*/

#ifndef _triangle_edge_h_
#define _triangle_edge_h_

#include "common.h"

//! Class to store triangle edges.
/*! An edge is described by two vertices.
 These are ordered internally for more efficient sorting and comparison.
 This class is useful for, for example, discovering adjacent triangles through edges they have in common.
NB There is no void constructor because the const vertices wouldn't be set.
 */
class TriangleEdge
{
 public:

  //! Constructor.  Sorts arguments to ensure _vertex0<_vertex1
  TriangleEdge(uint v0,uint v1)
    :_vertex0(v0<v1 ? v0 : v1)
    ,_vertex1(v0>v1 ? v0 : v1)
    {}

  //! Copy constructor.
  TriangleEdge(const TriangleEdge& e)
    :_vertex0(e._vertex0)
    ,_vertex1(e._vertex1)
    {}

  //! Destructor.
  ~TriangleEdge()
    {}

  //! Accessor.
  uint vertex0() const
    {return _vertex0;}

  //! Accessor.
  uint vertex1() const
    {return _vertex1;}

 protected:

  //! One vertex of the edge.  This should always be the lesser valued index.
  const uint _vertex0;

  //! The other vertex of the edge.  This should always be the greater valued index.
  const uint _vertex1;
};

//! Comparison operator, required to build ordered STL data-structures.
inline bool operator<(const TriangleEdge& e0,const TriangleEdge& e1)
{
  return 
    (
     e0.vertex0()<e1.vertex0()
     ||
     (
      e0.vertex0()==e1.vertex0()
      &&
      e0.vertex1()<e1.vertex1()
      )
     );
}

#endif
