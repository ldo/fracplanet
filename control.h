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
  \brief Interface for class Control.
*/

#ifndef _control_h_
#define _control_h_

#include "common.h"
#include "rgb.h"
#include "qtcommon.h"

//! Base class for other controls; useful for shared stuff.
class Control : public QWidget
{
 public:

  Control();

  ~Control();

  //! Use Qt's colour-picking dialog to replace the referenced colour
  void pickColour(QPushButton* button,FloatRGBA& colour);

 protected:

  //! Utility function to build a small Qt icon of the specified colour.
  static QIcon build_icon_of_colour(const FloatRGBA& col);
};

#endif
