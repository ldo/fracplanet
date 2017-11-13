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

#include "control_render.h"

ControlRender::ControlRender(ParametersRender* param)
  :Control()
  ,parameters(param)
{
  wireframe=new QCheckBox("Wireframe");
  layout()->addWidget(wireframe);
  wireframe->setChecked(parameters->wireframe);
  wireframe->setToolTip("Selects wireframe OpenGL rendering");
  connect(
	  wireframe,SIGNAL(stateChanged(int)),
	  this,SLOT(setWireframe(int))
	  );

  joystick_mouse=new QCheckBox("Joystick mouse-Y (fly mode)");
  layout()->addWidget(joystick_mouse);
  joystick_mouse->setChecked(parameters->joystick_mouse);
  joystick_mouse->setToolTip("Mouse y-axis functions as joystick in fly mode:\nmouse moved down/pulled-back pitches up.");
  connect(
	  joystick_mouse,SIGNAL(stateChanged(int)),
	  this,SLOT(setJoystickMouse(int))
	  );

  display_list=new QCheckBox("Display list");
  layout()->addWidget(display_list);
  display_list->setChecked(parameters->display_list);
  display_list->setToolTip("Use OpenGL display lists; CAUTION: unstable on many platforms");
  connect(
	  display_list,SIGNAL(stateChanged(int)),
	  this,SLOT(setDisplayList(int))
	  );

  background_colour_low_button=new QPushButton(build_icon_of_colour(parameters->background_colour_low),"Background colour (low altitude)");
  layout()->addWidget(background_colour_low_button);
  background_colour_low_button->setToolTip("Colour used in display area when the camera is low.");
  connect(background_colour_low_button,SIGNAL(clicked()),
	  this,SLOT(pickBackgroundColourLow())
	  );

  background_colour_high_button=new QPushButton(build_icon_of_colour(parameters->background_colour_high),"Background colour (high altitude)");
  layout()->addWidget(background_colour_high_button);
  background_colour_high_button->setToolTip("Colour used in display area when the camera is high.");
  connect(background_colour_high_button,SIGNAL(clicked()),
	  this,SLOT(pickBackgroundColourHigh())
	  );
	  
  QGroupBox* ambient_box=new QGroupBox("Ambient");
  layout()->addWidget(ambient_box);
  ambient_box->setLayout(new QHBoxLayout());
  
  ambient_box->layout()->addWidget(new QLabel("0.0"));

  ambient=new QSlider(Qt::Horizontal);
  ambient_box->layout()->addWidget(ambient);
  ambient->setMinimum(0);
  ambient->setValue(10);   //! \todo Should be obtained from somewhere ?
  ambient->setMaximum(100);
  ambient->setTickInterval(10);
  ambient->setTickPosition(QSlider::TicksBothSides);
  ambient->setTracking(true);

  ambient_box->layout()->addWidget(new QLabel("1.0"));

  connect(
	  ambient,SIGNAL(valueChanged(int)),
	  this,SLOT(setAmbient(int))
	  );

  QGroupBox* illumination_box=new QGroupBox("Illumination azimuth/elevation");
  layout()->addWidget(illumination_box);
  illumination_box->setLayout(new QHBoxLayout());  // TODO: Need a grid here, not an hbox
  
  illumination_box->layout()->addWidget(new QLabel("-180"));

  illumination_azimuth=new QSlider(Qt::Horizontal);
  illumination_box->layout()->addWidget(illumination_azimuth);
  illumination_azimuth->setMinimum(-180);
  illumination_azimuth->setValue(static_cast<int>(parameters->illumination_azimuth*180/M_PI));
  illumination_azimuth->setMaximum(180);
  illumination_azimuth->setTickInterval(10);
  illumination_azimuth->setTickPosition(QSlider::TicksBothSides);
  illumination_azimuth->setTracking(true);
  
  illumination_box->layout()->addWidget(new QLabel("180"));
  
  illumination_box->layout()->addWidget(new QLabel("-90"));
  
  illumination_elevation=new QSlider(Qt::Horizontal);
  illumination_box->layout()->addWidget(illumination_elevation);
  illumination_elevation->setMinimum(-90);
  illumination_elevation->setValue(static_cast<int>(parameters->illumination_elevation*180/M_PI));
  illumination_elevation->setMaximum(90);
  illumination_elevation->setTickInterval(10);
  illumination_elevation->setTickPosition(QSlider::TicksBothSides);
  illumination_elevation->setTracking(true);
  
  illumination_box->layout()->addWidget(new QLabel("90"));

  connect(
	  illumination_azimuth,SIGNAL(valueChanged(int)),
	  this,SLOT(setIlluminationAzimuth(int))
	  );
  connect(
	  illumination_elevation,SIGNAL(valueChanged(int)),
	  this,SLOT(setIlluminationElevation(int))
	  );
  
  QWidget*const padding=new QWidget();
  layout()->addWidget(padding);
  padding->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding));  // Expanding vertically
}

ControlRender::~ControlRender()
{}

void ControlRender::setWireframe(int v)
{
  parameters->wireframe=(v==2);
}

void ControlRender::setDisplayList(int v)
{
  parameters->display_list=(v==2);
}

void ControlRender::setJoystickMouse(int v)
{
  parameters->joystick_mouse=(v==2);
}

void ControlRender::setAmbient(int v)
{
  parameters->ambient=v/100.0f;
}

void ControlRender::setIlluminationAzimuth(int v)
{
  parameters->illumination_azimuth=v*M_PI/180;
}

void ControlRender::setIlluminationElevation(int v)
{
  parameters->illumination_elevation=v*M_PI/180;
}

void ControlRender::pickBackgroundColourLow()
{
  emit pickColour(background_colour_low_button,parameters->background_colour_low);
}

void ControlRender::pickBackgroundColourHigh()
{
  emit pickColour(background_colour_high_button,parameters->background_colour_high);
}
