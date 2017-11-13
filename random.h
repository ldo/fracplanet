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
  \brief Interface for class Random and derived classes.
*/

#ifndef _random_h_
#define _random_h_

#include "common.h"

//! Generates random numbers in the range [0,1).
class Random01
{
public:

  // Constructor.  Argument is seed value.
  Random01(uint s=0);
  
  //! Destructor.
  ~Random01();
  
  // Return random number in 0-1 (don't think we care whether open interval or not).
  double operator()()
    {
      return _gen();
    }
  
 private:

  boost::mt19937 _rng;

  boost::uniform_real<> _dist;

  boost::variate_generator<boost::mt19937,boost::uniform_real<> > _gen;
};

#endif






