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
  \brief Interface for class ControlRender.
*/

#ifndef _control_render_h_
#define _control_render_h_

#include "common.h"
#include "control.h"
#include "parameters_render.h"

//! Encapsulates GUI elements for controlling OpenGL rendering.
class ControlRender : public Control
{
 private:

  Q_OBJECT;

 public:

  ControlRender(ParametersRender* param);

  ~ControlRender();

 public slots:
    
  void setWireframe(int v);
  void setDisplayList(int v);
  void setJoystickMouse(int v);
  void setAmbient(int v);
  void setIlluminationAzimuth(int v);
  void setIlluminationElevation(int v);

  void pickBackgroundColourLow();
  void pickBackgroundColourHigh();

 private:

  //! The parameter set being controlled.
  ParametersRender*const parameters;

  QCheckBox* wireframe;
  QCheckBox* display_list;
  QCheckBox* joystick_mouse;
  QSlider* ambient;
  QSlider* illumination_azimuth;
  QSlider* illumination_elevation;

  QPushButton* background_colour_low_button;
  QPushButton* background_colour_high_button;
};

#endif
