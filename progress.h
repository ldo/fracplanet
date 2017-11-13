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
  \brief Interface for class Progress.
*/

#ifndef _progress_h_
#define _progress_h_

#include "common.h"

//! Mix-in class for call-backs from long operations.
/*! Use concrete implementations of these methods to drive progress bars.
 */
class Progress
{
 public:
  
  virtual ~Progress();

  virtual void progress_start(uint steps,const std::string& info)
    =0;

  // This will change the dialog text to indicate that progress is stalled for some reason.
  // It resets to the original text when progress_step is next stalled.
  // (Used by river generation).
  virtual void progress_stall(const std::string& reason)
    =0;

  virtual void progress_step(uint step)
    =0;

  virtual void progress_complete(const std::string& info)
    =0;
};

//! Helper class to facilitate driving progress bar
class ProgressScope
{
 public:

  ProgressScope(uint steps,const std::string& info,Progress* tgt);

  ~ProgressScope();

  void step();

 private:

  const uint _steps;

  const std::string _info;

  Progress*const _target;

  uint _step;
};

#endif
