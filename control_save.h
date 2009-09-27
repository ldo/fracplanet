// Source file for fracplanet
// Copyright (C) 2006 Tim Day
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*! \file
  \brief Interface for class ControlSave.
*/
#ifndef _control_save_h_
#define _control_save_h_

#include "control.h"
#include "parameters_save.h"

class FracplanetMain;

//! Encapsulates GUI elements for controlling save.
class ControlSave : public Control
{
 private:
  Q_OBJECT
 
 protected:
  //! The parameters set we control
  ParametersSave*const parameters;
  
 public:
  ControlSave(FracplanetMain* save_target,ParametersSave* param);
  virtual ~ControlSave();

  public slots:
   void setAtmosphere(int v);
   void setSeaSphere(int v);
   void setPerVertexAlpha(int v);
   void setTextureShaded(int v);
   void setTextureHeight(int v);
};



#endif
