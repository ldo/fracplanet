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

#include "control_terrain.h"

#include "fracplanet_main.h"
#include "spinbox.h"

/*! Lots of tedious code to instantiate controls and wire things up.
 */
ControlTerrain::ControlTerrain(FracplanetMain* tgt,ParametersTerrain* param_terrain,ParametersCloud* param_cloud)
  :Control()
  ,parameters_terrain(param_terrain)
  ,parameters_cloud(param_cloud)
  ,regenerate_target(tgt)
{  
  QTabWidget* tabs=new QTabWidget();      // Top level tab widget
  layout()->addWidget(tabs);
  tabs->setMinimumWidth(512);

  QTabWidget* tab_terrain=new QTabWidget(this);  
  tabs->addTab(tab_terrain,"Terrain");

  QWidget*const tab_terrain_basics=new QWidget();
  tab_terrain->addTab(tab_terrain_basics,"Basics");

  QWidget*const tab_terrain_subdivision=new QWidget();
  tab_terrain->addTab(tab_terrain_subdivision,"Subdivision");

  QWidget*const tab_terrain_noise=new QWidget();
  tab_terrain->addTab(tab_terrain_noise,"Noise");

  QWidget*const tab_snow=new QWidget();
  tabs->addTab(tab_snow,"Snow");

  QWidget*const tab_rivers=new QWidget();
  tabs->addTab(tab_rivers,"Rivers");

  QWidget*const tab_colours=new QWidget();
  tabs->addTab(tab_colours,"Colours");

  QWidget* tab_clouds=new QWidget();
  tabs->addTab(tab_clouds,"Clouds");

  QGridLayout*const grid_terrain_basics=new QGridLayout();
  tab_terrain_basics->setLayout(grid_terrain_basics);
  QGridLayout*const grid_terrain_subdivision=new QGridLayout();
  tab_terrain_subdivision->setLayout(grid_terrain_subdivision);
  QGridLayout*const grid_terrain_noise=new QGridLayout();
  tab_terrain_noise->setLayout(grid_terrain_noise);

  grid_terrain_basics->addWidget(new QLabel("Generate:"),0,0);

  QComboBox* object_type_combo_box=new QComboBox();
  grid_terrain_basics->addWidget(object_type_combo_box,0,1);
  object_type_combo_box->insertItem(ParametersObject::ObjectTypePlanet,"Planet");
  object_type_combo_box->insertItem(ParametersObject::ObjectTypeFlatHexagon,"Hexagonal Area");
  object_type_combo_box->insertItem(ParametersObject::ObjectTypeFlatTriangle,"Triangular Area");
  object_type_combo_box->insertItem(ParametersObject::ObjectTypeFlatSquare,"Square Area");

  object_type_combo_box->setCurrentIndex(parameters_terrain->object_type);

  connect(
	  object_type_combo_box,SIGNAL(activated(int)),
	  this,SLOT(setObjectType(int))
	  );
  
  base_height_label=new QLabel("Base land height (%):");
  grid_terrain_basics->addWidget(base_height_label,1,0);
  base_height_spinbox=new SpinBox(-100,100,10);
  grid_terrain_basics->addWidget(base_height_spinbox,1,1);
  base_height_spinbox->setValue((uint)(100.0*parameters_terrain->base_height));
  connect(
	  base_height_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setBaseHeight(int))
	  );
  base_height_spinbox->setToolTip("The initial height of land relative to sea-level");

  terrain_seed_label=new QLabel("Terrain random seed:");
  grid_terrain_basics->addWidget(terrain_seed_label,2,0);
  terrain_seed_spinbox=new SpinBox(0xffffffff,0x7fffffff,1);
  grid_terrain_basics->addWidget(terrain_seed_spinbox,2,1);
  terrain_seed_spinbox->setValue(parameters_terrain->seed);
  connect(
	  terrain_seed_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setTerrainSeed(int))
	  );
  terrain_seed_spinbox->setToolTip("The random seed for subdivision and noise.");

  subdivisions_label=new QLabel("Subdivisions:");
  grid_terrain_subdivision->addWidget(subdivisions_label,0,0);
  subdivisions_spinbox=new SpinBox(0,16,1);
  grid_terrain_subdivision->addWidget(subdivisions_spinbox,0,1);
  subdivisions_spinbox->setValue(parameters_terrain->subdivisions);
  connect(
	  subdivisions_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setTerrainSubdivisions(int))
	  );
  subdivisions_spinbox->setToolTip("The number of times the initial structure will be subdivided.\nWARNING: EACH STEP QUADRUPLES THE MEMORY REQUIREMENT!");

  subdivisions_unperturbed_label=new QLabel("Unperturbed subdivisions:");
  grid_terrain_subdivision->addWidget(subdivisions_unperturbed_label,1,0);
  subdivisions_unperturbed_spinbox=new SpinBox(0,16,1);
  grid_terrain_subdivision->addWidget(subdivisions_unperturbed_spinbox,1,1);
  subdivisions_unperturbed_spinbox->setValue(parameters_terrain->subdivisions_unperturbed);
  connect(
	  subdivisions_unperturbed_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setSubdivisionsUnperturbed(int))
	  );
  subdivisions_unperturbed_spinbox->setToolTip("The number of subdivisions which will be performed without perturbing vertices");

  variation_vertical_label=new QLabel("Vertical perturbation :");
  grid_terrain_subdivision->addWidget(variation_vertical_label,2,0);
  variation_vertical_spinbox=new SpinBox(0,50,1);
  grid_terrain_subdivision->addWidget(variation_vertical_spinbox,2,1);
  variation_vertical_spinbox->setValue((int)(100*parameters_terrain->variation.z));
  connect(
	  variation_vertical_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setVariationVertical(int))
	  );
  variation_vertical_spinbox->setToolTip("The magnitude of random height perturbations");

  variation_horizontal_label=new QLabel("Horizontal perturbation:");
  grid_terrain_subdivision->addWidget(variation_horizontal_label,3,0);
  variation_horizontal_spinbox=new SpinBox(0,25,1);
  grid_terrain_subdivision->addWidget(variation_horizontal_spinbox,3,1);
  variation_horizontal_spinbox->setValue((int)(100*parameters_terrain->variation.x));
  connect(
	  variation_horizontal_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setVariationHorizontal(int))
	  );
  variation_horizontal_spinbox->setToolTip("The magnitude of random horizontal perturbations");

  noise_terms_label=new QLabel("Noise terms");
  grid_terrain_noise->addWidget(noise_terms_label,0,0);
  noise_terms_spinbox=new SpinBox(0,10,1);
  grid_terrain_noise->addWidget(noise_terms_spinbox,0,1);
  noise_terms_spinbox->setValue(parameters_terrain->noise.terms);
  connect(
	  noise_terms_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setNoiseTerms(int))
	  );
  noise_terms_spinbox->setToolTip("Number of terms in added Perlin noise");

  noise_frequency_label=new QLabel("Noise frequency");
  grid_terrain_noise->addWidget(noise_frequency_label,1,0);
  noise_frequency_spinbox=new SpinBox(0,10000,10);
  grid_terrain_noise->addWidget(noise_frequency_spinbox,1,1);
  noise_frequency_spinbox->setValue(static_cast<int>(100*parameters_terrain->noise.frequency));
  connect(
	  noise_frequency_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setNoiseFrequency(int))
	  );
  noise_frequency_spinbox->setToolTip("Frequency for Perlin noise 1st term");

  noise_amplitude_label=new QLabel("Noise amplitude");
  grid_terrain_noise->addWidget(noise_amplitude_label,2,0);
  noise_amplitude_spinbox=new SpinBox(0,100,1);
  grid_terrain_noise->addWidget(noise_amplitude_spinbox,2,1);
  noise_amplitude_spinbox->setValue(static_cast<int>(100*parameters_terrain->noise.amplitude));  
  connect(
	  noise_amplitude_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setNoiseAmplitude(int))
	  );
  noise_amplitude_spinbox->setToolTip("Amplitude for Perlin noise 1st term");

  noise_amplitude_decay_label=new QLabel("Noise amplitude decay rate");
  grid_terrain_noise->addWidget(noise_amplitude_decay_label,3,0);
  noise_amplitude_decay_spinbox=new SpinBox(0,100,10);
  grid_terrain_noise->addWidget(noise_amplitude_decay_spinbox,3,1);
  noise_amplitude_decay_spinbox->setValue(static_cast<int>(100*parameters_terrain->noise.amplitude_decay));  
  connect(
	  noise_amplitude_decay_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setNoiseAmplitudeDecay(int))
	  );
  noise_amplitude_decay_spinbox->setToolTip("Amplitude decay rate for subsequent Perlin noise terms\n(subsequent terms are this percentage amplitude of the previous term)");

  power_law_label=new QLabel("Power law:");
  grid_terrain_basics->addWidget(power_law_label,3,0);
  power_law_spinbox=new SpinBox(1,10000,10);
  grid_terrain_basics->addWidget(power_law_spinbox,3,1);
  power_law_spinbox->setValue((int)(100*parameters_terrain->power_law));
  connect(
	  power_law_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setPowerLaw(int))
	  );
  power_law_spinbox->setToolTip("The power-law to be applied to elevations.");

  QGridLayout*const grid_snow=new QGridLayout();
  tab_snow->setLayout(grid_snow);

  snowline_equator_label=new QLabel("Snowline at equator");
  grid_snow->addWidget(snowline_equator_label,0,0);
  snowline_equator_spinbox=new SpinBox(-100,200,10);
  grid_snow->addWidget(snowline_equator_spinbox,0,1);
  snowline_equator_spinbox->setValue((int)(100*parameters_terrain->snowline_equator));
  connect(
	  snowline_equator_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setSnowlineEquator(int))
	  );
  snowline_equator_spinbox->setToolTip("Snowline on the equator (as a % of the maximum height)");

  snowline_pole_label=new QLabel("Snowline at pole");
  grid_snow->addWidget(snowline_pole_label,1,0);
  snowline_pole_spinbox=new SpinBox(-100,200,10);
  grid_snow->addWidget(snowline_pole_spinbox,1,1);
  snowline_pole_spinbox->setValue((int)(100*parameters_terrain->snowline_pole));
  connect(
	  snowline_pole_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setSnowlinePole(int))
	  );
  snowline_pole_spinbox->setToolTip("Snowline at the poles (as a % of the maximum height)");

  snowline_power_law_label=new QLabel("Snowline power law");
  grid_snow->addWidget(snowline_power_law_label,2,0);
  snowline_power_law_spinbox=new SpinBox(1,1000,10);
  grid_snow->addWidget(snowline_power_law_spinbox,2,1);
  snowline_power_law_spinbox->setValue((int)(100*parameters_terrain->snowline_power_law));
  connect(
	  snowline_power_law_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setSnowlinePowerLaw(int))
	  );
  snowline_power_law_spinbox->setToolTip("Power law applied to snowline elevation.");

  snowline_slope_effect_label=new QLabel("Snowline slope suppression");
  grid_snow->addWidget(snowline_slope_effect_label,3,0);
  snowline_slope_effect_spinbox=new SpinBox(0,10000,5);
  grid_snow->addWidget(snowline_slope_effect_spinbox,3,1);
  snowline_slope_effect_spinbox->setValue((int)(100*parameters_terrain->snowline_slope_effect));
  connect(
	  snowline_slope_effect_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setSnowlineSlopeEffect(int))
	  );
  snowline_slope_effect_spinbox->setToolTip("Snow suppression on slopes.");

  snowline_glacier_effect_label=new QLabel("Snowline glacier effect");
  grid_snow->addWidget(snowline_glacier_effect_label,4,0);
  snowline_glacier_effect_spinbox=new SpinBox(-1000,1000,5);
  grid_snow->addWidget(snowline_glacier_effect_spinbox,4,1);
  snowline_glacier_effect_spinbox->setValue((int)(100*parameters_terrain->snowline_glacier_effect));
  connect(
	  snowline_glacier_effect_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setSnowlineGlacierEffect(int))
	  );
  snowline_glacier_effect_spinbox->setToolTip("Converts rivers to glaciers.");

  QGridLayout*const grid_rivers=new QGridLayout();
  tab_rivers->setLayout(grid_rivers);

  rivers_label=new QLabel("Rivers:");
  grid_rivers->addWidget(rivers_label,0,0);
  rivers_spinbox=new SpinBox(0,1000000,100);
  grid_rivers->addWidget(rivers_spinbox,0,1);
  rivers_spinbox->setValue(parameters_terrain->rivers);
  connect(
	  rivers_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setRivers(int))
	  );
  rivers_spinbox->setToolTip("The number of rivers to be attempted to be generated");

  rivers_seed_label=new QLabel("Rivers seed:");
  grid_rivers->addWidget(rivers_seed_label,1,0);
  rivers_seed_spinbox=new SpinBox(0xffffffff,0x7fffffff,1);
  grid_rivers->addWidget(rivers_seed_spinbox,1,1);
  rivers_seed_spinbox->setValue(parameters_terrain->rivers_seed);
  connect(
	  rivers_seed_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setRiversSeed(int))
	  );
  rivers_seed_spinbox->setToolTip("The random seed for river generation.");

  lake_becomes_sea_label=new QLabel("Lake becomes sea:");
  grid_rivers->addWidget(lake_becomes_sea_label,2,0);
  lake_becomes_sea_spinbox=new SpinBox(1,100,1);
  grid_rivers->addWidget(lake_becomes_sea_spinbox,2,1);
  lake_becomes_sea_spinbox->setValue((uint)(100.0*parameters_terrain->lake_becomes_sea));
  connect(
	  lake_becomes_sea_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setLakeBecomesSea(int))
	  );
  lake_becomes_sea_spinbox->setToolTip("The percentage of planetary surface which must be covered by a lake for it to be considered a sea");

  QGridLayout*const grid_colours=new QGridLayout();
  tab_colours->setLayout(grid_colours);
  
  colour_ocean_button=new QPushButton(build_icon_of_colour(parameters_terrain->colour_ocean),"Ocean");
  grid_colours->addWidget(colour_ocean_button,0,0);
  connect(
	  colour_ocean_button,SIGNAL(clicked()),
	  this,SLOT(pickColourOcean())
	  );
  colour_shoreline_button=new QPushButton(build_icon_of_colour(parameters_terrain->colour_shoreline),"Shore");
  grid_colours->addWidget(colour_shoreline_button,0,1);
  connect(
	  colour_shoreline_button,SIGNAL(clicked()),
	  this,SLOT(pickColourShoreline())
	  );
  colour_river_button=new QPushButton(build_icon_of_colour(parameters_terrain->colour_river),"River");
  grid_colours->addWidget(colour_river_button,1,0);
  connect(
	  colour_river_button,SIGNAL(clicked()),
	  this,SLOT(pickColourRiver())
	  );
  colour_low_button=new QPushButton(build_icon_of_colour(parameters_terrain->colour_low),"Low");
  grid_colours->addWidget(colour_low_button,1,1);
  connect(
	  colour_low_button,SIGNAL(clicked()),
	  this,SLOT(pickColourLow())
	  );
  colour_snow_button=new QPushButton(build_icon_of_colour(parameters_terrain->colour_snow),"Snow");
  grid_colours->addWidget(colour_snow_button,2,0);
  connect(
	  colour_snow_button,SIGNAL(clicked()),
	  this,SLOT(pickColourSnow())
	  );
  colour_high_button=new QPushButton(build_icon_of_colour(parameters_terrain->colour_high),"High");
  grid_colours->addWidget(colour_high_button,2,1);
  connect(
	  colour_high_button,SIGNAL(clicked()),
	  this,SLOT(pickColourHigh())
	  );

  grid_colours->addWidget(new QLabel("Oceans & rivers emissive"),3,0);
  oceans_and_rivers_emissive_spinbox=new SpinBox(0,100,10);
  grid_colours->addWidget(oceans_and_rivers_emissive_spinbox,3,1);
  oceans_and_rivers_emissive_spinbox->setValue((uint)(100.0*parameters_terrain->oceans_and_rivers_emissive));
  connect(
	  oceans_and_rivers_emissive_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setOceansAndRiversEmissive(int))
	  );
  oceans_and_rivers_emissive_spinbox->setToolTip("Percentage of ocean and river colour which is emissive");

  tab_clouds->setLayout(new QVBoxLayout());

  QCheckBox*const clouds_checkbox=new QCheckBox("Clouds enabled");
  tab_clouds->layout()->addWidget(clouds_checkbox);

  clouds_checkbox->setToolTip("Switch on cloud generation");
  clouds_checkbox->setChecked(parameters_cloud->enabled);
  connect(
	  clouds_checkbox,SIGNAL(toggled(bool)),
	  this,SLOT(setCloudsEnabled(bool))
	  );

  QWidget*const clouds_widget=new QWidget();
  tab_clouds->layout()->addWidget(clouds_widget);
  QGridLayout*const grid_clouds=new QGridLayout();
  clouds_widget->setLayout(grid_clouds);

  clouds_subdivisions_unlock_checkbox=new QCheckBox("Subdivisions ");
  grid_clouds->addWidget(clouds_subdivisions_unlock_checkbox,1,0);
  clouds_subdivisions_unlock_checkbox->setChecked(false);
  clouds_subdivisions_unlock_checkbox->setToolTip("Enable explicit control of cloud subdivisons.\nWhen disabled, the cloud mesh will be subdivided by the same amount as the terrain");
  connect(clouds_subdivisions_unlock_checkbox,SIGNAL(toggled(bool)),
	  this,SLOT(setCloudsSubdivisionsUnlocked(bool))
	  );

  clouds_subdivisions_spinbox=new SpinBox(0,16,1);
  grid_clouds->addWidget(clouds_subdivisions_spinbox,1,1);
  clouds_subdivisions_spinbox->setValue(parameters_cloud->subdivisions);
  connect(
	  clouds_subdivisions_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setCloudSubdivisions(int))
	  );
  clouds_subdivisions_spinbox->setToolTip("The number of times the initial structure will be subdivided.\nWARNING: EACH STEP QUADRUPLES THE MEMORY REQUIREMENT!");
  clouds_subdivisions_spinbox->setEnabled(clouds_subdivisions_unlock_checkbox->isChecked());
  connect(
	  clouds_subdivisions_unlock_checkbox,SIGNAL(toggled(bool)),
	  clouds_subdivisions_spinbox,SLOT(setEnabled(bool))
	  );
  setCloudsSubdivisionsUnlocked(clouds_subdivisions_unlock_checkbox->isChecked());

  clouds_seed_label=new QLabel("Clouds seed:");
  grid_clouds->addWidget(clouds_seed_label,2,0);
  clouds_seed_spinbox=new SpinBox(0xffffffff,0x7fffffff,1);
  grid_clouds->addWidget(clouds_seed_spinbox,2,1);
  clouds_seed_spinbox->setValue(parameters_cloud->seed);
  clouds_seed_spinbox->setToolTip("Random seed for cloud generation");

  grid_clouds->addWidget(new QLabel("Cloud height"),3,0);
  QSpinBox*const clouds_height_spinbox=new SpinBox(1,100,1);
  grid_clouds->addWidget(clouds_height_spinbox,3,1);
  clouds_height_spinbox->setValue(static_cast<int>(parameters_cloud->cloudbase*100.0f));
  clouds_height_spinbox->setToolTip("Altitude of cloud layer");
  connect(
	  clouds_height_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setCloudbase(int))
	  );

  grid_clouds->addWidget(new QLabel("Weather systems"),4,0);
  QSpinBox*const clouds_weather_systems_spinbox=new SpinBox(0,1000,10);
  grid_clouds->addWidget(clouds_weather_systems_spinbox,4,1);
  clouds_weather_systems_spinbox->setValue(parameters_cloud->weather_systems);
  clouds_weather_systems_spinbox->setToolTip("Number of cyclonic features in clouds");
  connect(
	  clouds_weather_systems_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setWeatherSystems(int))
	  );
  
  colour_cloud_button=new QPushButton(build_icon_of_colour(parameters_cloud->colour),"Cloud colour");
  grid_clouds->addWidget(colour_cloud_button,5,0,1,2);
  connect(
	  colour_cloud_button,SIGNAL(clicked()),
	  this,SLOT(pickColourCloud())
	  );

  // Wiring to disable all cloud controls 
  clouds_widget->setEnabled(parameters_cloud->enabled);
  connect(
	  clouds_checkbox,SIGNAL(toggled(bool)),
	  clouds_widget,SLOT(setEnabled(bool))
	  );

  QWidget*const regenerate_widget=new QWidget();
  layout()->addWidget(regenerate_widget);
  QGridLayout*const regenerate_grid=new QGridLayout();
  regenerate_widget->setLayout(regenerate_grid);
  
  regenerate_button=new QPushButton("Generate");
  regenerate_grid->addWidget(regenerate_button,0,0,1,3);
  connect(
	  regenerate_button,SIGNAL(clicked()),
	  regenerate_target,SLOT(regenerate())
	  );

  regenerate_with_new_terrain_seed_button=new QPushButton("...new terrain seed");
  regenerate_grid->addWidget(regenerate_with_new_terrain_seed_button,1,0);
  connect(
	  regenerate_with_new_terrain_seed_button,SIGNAL(clicked()),
	  this,SLOT(regenerate_with_new_terrain_seed())
	  );

  regenerate_with_new_rivers_seed_button=new QPushButton("...new rivers seed");
  regenerate_grid->addWidget(regenerate_with_new_rivers_seed_button,1,1);
  connect(
	  regenerate_with_new_rivers_seed_button,SIGNAL(clicked()),
	  this,SLOT(regenerate_with_new_rivers_seed())
	  );

  regenerate_with_new_clouds_seed_button=new QPushButton("...new clouds seed");
  regenerate_grid->addWidget(regenerate_with_new_clouds_seed_button,1,2);
  connect(
	  regenerate_with_new_clouds_seed_button,SIGNAL(clicked()),
	  this,SLOT(regenerate_with_new_clouds_seed())
	  );
  regenerate_with_new_clouds_seed_button->setEnabled(parameters_cloud->enabled);
  connect(
	  clouds_checkbox,SIGNAL(toggled(bool)),
	  regenerate_with_new_clouds_seed_button,SLOT(setEnabled(bool))
	  );
}

void ControlTerrain::regenerate_with_new_terrain_seed()
{
  parameters_terrain->seed++;
  terrain_seed_spinbox->setValue(parameters_terrain->seed);

  regenerate_target->regenerate();
}

void ControlTerrain::regenerate_with_new_rivers_seed()
{
  parameters_terrain->rivers_seed++;
  rivers_seed_spinbox->setValue(parameters_terrain->rivers_seed);

  regenerate_target->regenerate();
}

void ControlTerrain::regenerate_with_new_clouds_seed()
{
  parameters_cloud->seed++;
  clouds_seed_spinbox->setValue(parameters_cloud->seed);

  regenerate_target->regenerate();
}

