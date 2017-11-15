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
  \brief Interface for class ControlSave.
*/
#ifndef _control_save_h_
#define _control_save_h_

#include "common.h"
#include "control.h"
#include "parameters_save.h"
#include "qtcommon.h"

class FracplanetMain;

//! Encapsulates GUI elements for controlling save.
class ControlSave : public Control
{
 private:

  Q_OBJECT

 public:

  ControlSave(FracplanetMain* save_target,ParametersSave* param);

  ~ControlSave();

 public slots:

  void setAtmosphere(int v);
  void setSeaSphere(int v);
  void setPerVertexAlpha(int v);
  void setTextureShaded(int v);
  void setTextureHeight(int v);
 
 private:

  //! The parameters set we control
  ParametersSave*const parameters;  
};



#endif
