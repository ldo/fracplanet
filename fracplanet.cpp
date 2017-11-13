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

/*! \mainpage Fracplanet : fractal terrain generator

  \author Tim Day 

  \section introduction Introduction
  "Fracplanet" is an interactive tool for generating fractal planets and terrains.
  It can output the generated meshes to files suitable for use by POV-Ray.

  \todo For new features to be added, see the TODO file.
 */

#include "fracplanet_main.h"

//! Application code
/*! Currently this simply creates a TriangleMesh object of some sort,
  then passes it to a viewer.
 */
int main(int argc,char* argv[])
{
  QApplication app(argc,argv);

  boost::program_options::variables_map opts;
  try
    {
      boost::program_options::options_description opt_desc
	("Recognised options (besides Qt standards):");

      opt_desc.add_options()
	("help,h","show list of recognised options")
	("verbose,v","verbose output to stderr")
	;
	
      opt_desc.add(ParametersRender::options());

      boost::program_options::store
	(
	 boost::program_options::parse_command_line(argc,argv,opt_desc),
	 opts
	 );
      boost::program_options::notify(opts);

      if (opts.count("help"))
	{
	  std::cerr << opt_desc << std::endl;
	  return 1;
	}
    }
  catch (boost::program_options::error& e)
    {
      std::cerr << "Bad command line: " << e.what() << std::endl;
      std::cerr << "Use -h or --help to list recognised options" << std::endl;
      return 1;
    }

  const bool verbose=opts.count("verbose");

  if (verbose)
    std::cerr << "Setting up...\n";

  FracplanetMain*const main_widget=new FracplanetMain(0,&app,opts,verbose);

  if (verbose)
    std::cerr << "...setup completed\n";

  main_widget->show();

  if (verbose)
    {
      std::cerr << "Fracplanet:" << std::endl;
      std::cerr << "  sizeof(ByteRGBA) is " << sizeof(ByteRGBA) << " (4 is good)" << std::endl;  
      std::cerr << "  sizeof(Vertex)   is " << sizeof(Vertex) << " (32 is good)" << std::endl;
      std::cerr << "  sizeof(Triangle) is " << sizeof(Triangle) << " (12 is good)" << std::endl;
    }
  
  main_widget->regenerate();
  
  return app.exec();
}
