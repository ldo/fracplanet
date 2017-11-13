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

#include "matrix33.h"

float Matrix33::cofactor(uint row,uint col) const
{
  const uint row0=(row==0 ? 1 : 0);
  const uint col0=(col==0 ? 1 : 0);

  const uint row1=(row==2 ? 1 : 2);
  const uint col1=(col==2 ? 1 : 2);

  return
    element(row0,col0)*element(row1,col1)
    -
    element(row0,col1)*element(row1,col0);
}

float Matrix33::determinant() const
{
  return 
    element(0,0)*cofactor(0,0)
    -element(0,1)*cofactor(0,1)
    +element(0,2)*cofactor(0,2);
}

const Matrix33 Matrix33::inverted() const
{
  Matrix33 ret;
  for (uint row=0;row<3;row++)
    for (uint col=0;col<3;col++)
      {
	const float cf=cofactor(row,col);
	ret.element(col,row)=(((row+col)&1) ? -cf : cf);  // NB Transpose is deliberate
      }

  return ret/determinant();
}
