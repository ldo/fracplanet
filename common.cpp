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

#include "common.h"

void fatal_error(const char* msg)
{
  std::cerr
    << "\n*** Fatal error: "
    << msg
    << " ***\n";
  exit(1);
}

void fatal_internal_error(const char* src_file,uint src_line)
{
  std::cerr 
    << "\n*** Fatal internal error in "
    << src_file
    << " at line "
    << src_line
    << " ***\n";
  exit(1);
}

void constraint_violation(const char* test,const char* src_file,uint src_line)
{
  std::cerr 
    << "\n*** Constraint \""
    << test
    << "\" violated in file"
    << src_file
    << " at line "
    << src_line
    << " ***\n";
  exit(1);
}
