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
  \brief Interface for class ControlTerrain.
*/

#ifndef _control_terrain_h_
#define _control_terrain_h_

#include "common.h"
#include "control.h"
#include "parameters_terrain.h"
#include "parameters_cloud.h"
#include "qtcommon.h"

class FracplanetMain;

//! Encapsulates controls for setting terrain generation parameters
/*! \todo: Way too much stuff in this classes .h file. Shift it to the .cpp!
 */
class ControlTerrain : public Control
{ 
 private:

  Q_OBJECT;

 public:

  ControlTerrain(FracplanetMain* tgt,ParametersTerrain* param_terrain,ParametersCloud* param_cloud);

  ~ControlTerrain()
    {}

  public slots:

  void setObjectType(int id)
    {
      parameters_terrain->object_type=static_cast<ParametersObject::ObjectType>(id);
      parameters_cloud->object_type=static_cast<ParametersObject::ObjectType>(id);
    }
  void setTerrainSeed(int v)
    {
      parameters_terrain->seed=v;
    }
  void setTerrainSubdivisions(int v)
    {
      parameters_terrain->subdivisions=v;
    }
  void setCloudSubdivisions(int v)
    {
      parameters_cloud->subdivisions=v;
    }
  void setSubdivisionsUnperturbed(int v)
    {
      parameters_terrain->subdivisions_unperturbed=v;
    }
  void setVariationVertical(int v)
    {
      parameters_terrain->variation.z=v/100.0;
    }
  void setVariationHorizontal(int v)
    {
      parameters_terrain->variation.x=v/100.0;
      parameters_terrain->variation.y=v/100.0;
    }
  void setNoiseTerms(int v)
    {
      parameters_terrain->noise.terms=v;
    }
  void setNoiseFrequency(int v)
    {
      parameters_terrain->noise.frequency=v/100.0;
    }
  void setNoiseAmplitude(int v)
    {
      parameters_terrain->noise.amplitude=v/100.0;
    }
  void setNoiseAmplitudeDecay(int v)
    {
      parameters_terrain->noise.amplitude_decay=v/100.0;
    }
  void setBaseHeight(int v)
    {
      parameters_terrain->base_height=v/100.0;
    }
  void setPowerLaw(int v)
    {
      parameters_terrain->power_law=v/100.0;
    }
  void setSnowlineEquator(int v)
    {
      parameters_terrain->snowline_equator=v/100.0;
    }
  void setSnowlinePole(int v)
    {
      parameters_terrain->snowline_pole=v/100.0;
    }
  void setSnowlinePowerLaw(int v)
    {
      parameters_terrain->snowline_power_law=v/100.0;
    }
  void setSnowlineSlopeEffect(int v)
    {
      parameters_terrain->snowline_slope_effect=v/100.0;
    }
  void setSnowlineGlacierEffect(int v)
    {
      parameters_terrain->snowline_glacier_effect=v/100.0;
    }
  void setRivers(int v)
    {
      parameters_terrain->rivers=v;
    }
  void setRiversSeed(int v)
    {
      parameters_terrain->rivers_seed=v;
    }
  void setLakeBecomesSea(int v)
    {
      parameters_terrain->lake_becomes_sea=v/100.0;
    }
  void setOceansAndRiversEmissive(int v)
    {
      parameters_terrain->oceans_and_rivers_emissive=v/100.0;
    }
  void pickColourOcean()
    {
      emit pickColour(colour_ocean_button,parameters_terrain->colour_ocean);
    }
  void pickColourShoreline()
    {
      emit pickColour(colour_shoreline_button,parameters_terrain->colour_shoreline);
    }
  void pickColourLow()
    {
      emit pickColour(colour_low_button,parameters_terrain->colour_low);
    }
  void pickColourRiver()
    {
      emit pickColour(colour_river_button,parameters_terrain->colour_river);
    }
  void pickColourSnow()
    {
      emit pickColour(colour_snow_button,parameters_terrain->colour_snow);
    }
  void pickColourHigh()
    {
      emit pickColour(colour_high_button,parameters_terrain->colour_high);
    }
  void pickColourCloud()
    {
      emit pickColour(colour_cloud_button,parameters_cloud->colour);
    }
  void setCloudsSubdivisionsUnlocked(bool f)
    {
      if (f)
	{
	  disconnect(
		     subdivisions_spinbox,SIGNAL(valueChanged(int)),
		     clouds_subdivisions_spinbox,SLOT(setValue(int))
		     );
	}
      else
	{
	  clouds_subdivisions_spinbox->setValue(parameters_terrain->subdivisions);
	  connect(
		  subdivisions_spinbox,SIGNAL(valueChanged(int)),
		  clouds_subdivisions_spinbox,SLOT(setValue(int))
		  );
		  
	}
    }

  void setCloudsEnabled(bool f)
    {
      parameters_cloud->enabled=f;
    }
  void setCloudbase(int v)
    {
      parameters_cloud->cloudbase=v/100.0f;
    }
  void setWeatherSystems(int v)
    {
      parameters_cloud->weather_systems=v;
    }

  void regenerate_with_new_terrain_seed();
  void regenerate_with_new_rivers_seed();
  void regenerate_with_new_clouds_seed();

 private:

  //! Pointer to the terrain parameters we control.
  ParametersTerrain*const parameters_terrain;

  //! Pointer to the cloud parameters we control.
  ParametersCloud*const parameters_cloud;

  //! Requests to regenerate terrain are forwarded to the top level GUI widget.
  FracplanetMain* regenerate_target;

  QComboBox* object_type_combo_box;

  QLabel* base_height_label;
  QSpinBox* base_height_spinbox;
  
  QLabel* terrain_seed_label;
  QSpinBox* terrain_seed_spinbox;
  QLabel* subdivisions_label;
  QSpinBox* subdivisions_spinbox;
  QLabel* subdivisions_unperturbed_label;
  QSpinBox* subdivisions_unperturbed_spinbox;

  QLabel* variation_vertical_label;
  QSpinBox* variation_vertical_spinbox;
  QLabel* variation_horizontal_label;
  QSpinBox* variation_horizontal_spinbox;

  QLabel* noise_terms_label;
  QSpinBox* noise_terms_spinbox;
  QLabel* noise_frequency_label;
  QSpinBox* noise_frequency_spinbox;
  QLabel* noise_amplitude_label;
  QSpinBox* noise_amplitude_spinbox;
  QLabel* noise_amplitude_decay_label;
  QSpinBox* noise_amplitude_decay_spinbox;

  QLabel* power_law_label;
  QSpinBox* power_law_spinbox;

  QLabel* snowline_equator_label;
  QSpinBox* snowline_equator_spinbox;
  QLabel* snowline_pole_label;
  QSpinBox* snowline_pole_spinbox;
  QLabel* snowline_power_law_label;
  QSpinBox* snowline_power_law_spinbox;
  QLabel* snowline_slope_effect_label;
  QSpinBox* snowline_slope_effect_spinbox;
  QLabel* snowline_glacier_effect_label;
  QSpinBox* snowline_glacier_effect_spinbox;
  
  QLabel* rivers_label;
  QSpinBox* rivers_spinbox;
  QLabel* rivers_seed_label;
  QSpinBox* rivers_seed_spinbox;
  QLabel* lake_becomes_sea_label;
  QSpinBox* lake_becomes_sea_spinbox;

  QSpinBox* oceans_and_rivers_emissive_spinbox;

  QLabel* clouds_seed_label;
  QSpinBox* clouds_seed_spinbox;

  QLabel* colour_label;
  QPushButton* colour_ocean_button;
  QPushButton* colour_shoreline_button;
  QPushButton* colour_low_button;
  QPushButton* colour_river_button;
  QPushButton* colour_snow_button;
  QPushButton* colour_high_button;
  QPushButton* colour_cloud_button;

  QCheckBox* clouds_subdivisions_unlock_checkbox;
  QSpinBox* clouds_subdivisions_spinbox;

  QPushButton* regenerate_button;
  QPushButton* regenerate_with_new_terrain_seed_button;
  QPushButton* regenerate_with_new_rivers_seed_button;
  QPushButton* regenerate_with_new_clouds_seed_button;

  QWidget* padding;
};

#endif
