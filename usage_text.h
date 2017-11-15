"<qt title=\"Fracplanet user manual\">\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"<h1>\n"
"  Fracplanet user manual\n"
"</h1>\n"
"<p>Version: 0.5.1dev</p>\n"
"\n"
"<h2>Introduction</h2>\n"
"\n"
"<p>\n"
"  Fracplanet is an application to generate simple random planets and terrain \n"
"  with oceans, mountains, icecaps and rivers.\n"
"  Parameters are specified interactively and the results displayed using OpenGL.\n"
"  The generated objects can be exported in formats directly usable by POV-Ray or Blender,\n"
"  or as more generally useful texture images.\n"
"</p>\n"
"\n"
"<h2>Command line arguments</h2>\n"
"\n"
"<p>\n"
"  Some command line arguments (not listed here) are intercepted and interpreted by Qt.\n"
"  These are pretty much what you'd expect most X11 applications to handle \n"
"  e.g <code>-geometry <em>X</em>x<em>Y</em></code>.\n"
"  Fracplanet's own command line arguments are:\n"
"</p>\n"
"\n"
"<dl>\n"
"  <dt>--help, -h</dt>\n"
"  <dd>\n"
"    Display a list of recognised arguments.\n"
"  </dd>\n"
"  <dt>--verbose, -v</dt>\n"
"  <dd>\n"
"    Output information about fracplanet execution (and OpenGL) to stderr.\n"
"  </dd>\n"
"  <dt>--display-list, -d</dt>\n"
"  <dd>\n"
"    Start up the application with rendering in display list mode by default\n"
"    (c.f immediate mode).\n"
"    This is particularly useful when the application is running remotely as the\n"
"    generated meshes are then only sent to the local OpenGL display hardware once.\n"
"  </dd>\n"
"  <dt>--invert-mouse-y, -y</dt>\n"
"  <dd>\n"
"    Start application with mouse in non-joystick mode for flight.\n"
"  </dd>\n"
"  <dt>--wireframe, -w</dt>\n"
"  <dd>\n"
"    Start application with rendering set to wireframe mode.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<h2>GUI</h2>\n"
"\n"
"<p>\n"
"  The main part of the fracplanet GUI is a tabbed control.\n"
"  The terrain generated is shown in a separate window (which disappears while regenerating).\n"
"  A progress dialog is generally displayed while generating terrain (except during application startup).\n"
"</p>\n"
"\n"
"<h3>Create tab</h3>\n"
"\n"
"<p>\n"
"  Note that none of the parameters adjustable on this tab have any effect on the displayed model until one of \n"
"  the \"Regenerate\" buttons is pressed.  This tab is further subdivided into sub-tabs\n"
"  (\"Terrain\", \"Snow\", \"Rivers\", \"Colours\", \"Clouds\").\n"
"</p>\n"
"\n"
"<h4>Terrain</h4>\n"
"  \n"
"<p>\n"
"  This tab is actually even further subdivided \n"
"  (\"Basics\", \"Subdivision\" and \"Noise\" tabs),\n"
"  but the list below doesn't distinguish.\n"
"</p>\n"
"\n"
"<dl>\n"
"  <dt>Starting geometry: Generate...</dt>\n"
"  <dd>\n"
"    A pull-down menu (or \"combo-box\") selects the initial geometry which will be \"fractalized\".\n"
"    Planet creates planets by subdividing an icosahedron.\n"
"    The other terrain area type create planar terrain areas by subdividing one or more triangles.\n"
"    Note that the square type doesn't produce very good results because the triangles it uses aren't equilateral.\n"
"  </dd>\n"
"  <dt>Base land height</dt>\n"
"  <dd>\n"
"    This expresses the initial height of the terrain (relative to the sea level) as a percentage of the vertical maximum perturbation size.\n"
"    Negative values produce (on average) more ocean than land, positive values produce more land than ocean. \n"
"  </dd>\n"
"  <dt>Terrain random seed</dt>\n"
"  <dd>\n"
"    Specifies the random number generator seed used when creating terrain.\n"
"    Regenerating without changing this value will produce the same terrain, allowing the user to play with subdivision levels, colours etc while still retaining the same basic pattern of oceans and continents. \n"
"    The value is initially set to the system time on application start-up.\n"
"  </dd>\n"
"  <dt>Power law</dt>\n"
"  <dd>\n"
"    A power law applied to all above-sea-level heights after terrain generation.\n"
"    (This consists of normalising the height relative to the maximum height in the terrain model and raising it to the power of this number divided by 100).\n"
"    Values above 100 flatten low terrain, tending to produce smooth plains surrounding spiky mountains.\n"
"    Values below 100 flatten high terrain, tending to produce smooth highlands surrounded by steep cliffs.\n"
"  </dd>\n"
"  <dt>Subdivisions</dt>\n"
"  <dd>\n"
"    The number of subdivisions of the initial structure.\n"
"    Each successive level of subdivision increases the number of triangles by a factor of four, so users should increase this parameter with caution.\n"
"    This has a major impact on the amount of memory consumed, the frame rate and the responsiveness of the application.\n"
"  </dd>\n"
"  <dt>Unperturbed subdivisions</dt>\n"
"  <dd>\n"
"    Specifies the number of the subdivisions which will be performed <em>without</em> random perturbation of the vertices.\n"
"    Lower numbers (0, 1) produce a few large continents.\n"
"    Higher values produce many small islands.\n"
"  </dd>\n"
"  <dt>Vertical perturbation</dt>\n"
"  <dd>\n"
"    Specifies the maximum size of vertical perturbations at the first level of subdivision.\n"
"    The maximum perturbation size is then halved at each subsequent subdivision.\n"
"    Planets and terrain areas both have a nominal radius of 1.0, \n"
"    and the number here is divided by 100 so if you specify a vertical variation of 12 you could get mountains which are on the order of 12 percent of the planet's radius high, or larger if subsequent perturbations accumulate upwards at a point (of course suppressing initial large perturbations using the \"Unperturbed\" parameter will tend to reduce this).\n"
"    This is of course a ridiculous height for mountains on anything but an asteroid, but using realistic values will just produce very boring looking planets. \n"
"  </dd>\n"
"  <dt>Horizontal perturbation</dt>\n"
"  <dd>\n"
"    Specifies the maximum size of horizontal perturbations at the first level of subdivision.\n"
"    The maximum perturbation size is then halved at each subsequent subdivision.\n"
"    Beware of making this value too large as it can produce overhanging/self-intersecting terrain,\n"
"    on the other hand small values can be useful for breaking up obvious artifacts of the initial geometry.\n"
"  </dd>\n"
"  <dt>Noise terms</dt>\n"
"  <dd>\n"
"    Number of terms in a Perlin noise function added to the terrain heights.\n"
"    Each subsequent term doubles the frequency.\n"
"    The best way to get an appreciation of the qualitative differences between Perlin noise and\n"
"    subdivision-perturbation is to reduce \"Vertical perturbation\" (on the \"Subdivision\" tab)\n"
"    to zero and increase the number of noise terms.\n"
"  </dd>\n"
"  <dt>Noise frequency<dt>\n"
"  </dt>\n"
"</dt>\n"
"<dd>\n"
"    Frequency of 1st noise term.\n"
"    100 gives a scale on the order of the terrain radius.\n"
"    Subsequent terms double the frequency (and therefore halve their length scale).\n"
"  </dd>\n"
"  <dt>Noise amplitude<dt>\n"
"  </dt>\n"
"</dt>\n"
"<dd>\n"
"    Amplitude of 1st noise term, as a percentage of the nominal object radius.\n"
"  </dd>\n"
"  <dt>Noise amplitude decay rate<dt>\n"
"  </dt>\n"
"</dt>\n"
"<dd>\n"
"    The amplitude of subsequent noise terms decays to this percentage of the amplitude of the previous term.\n"
"    Fifty percent does the classic fractal thing of scaling perturbation size with length scale.\n"
"    Increasing it slightly (to e.g sixty percent) has the interesting effect of making rivers meander more\n"
"    as small-scale variations now have a bigger effect on local gradients than large scale features.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<h4>Snow</h4>\n"
"\n"
"<dl>\n"
"  <dt>Snowline at equator</dt>\n"
"  <dd>\n"
"    The nominal height of the snowline at the equator of a planet (and everywhere on a flat-based terrain area),\n"
"    expressed as a percentage of the maximum height of the terrain.\n"
"  </dd>\n"
"  <dt>Snowline at pole</dt>\n"
"  <dd>\n"
"    The nominal height of the snowline at the poles of a planet (unused for a flat-based terrain area),\n"
"    expressed as a percentage of the maximum height of the terrain.\n"
"  </dd>\n"
"  <dt>Snowline power law</dt>\n"
"  <dd>\n"
"    Tweaking this parameter lets you control whether the snowline remains high up and only plunges\n"
"    downward towards the poles, or whether it only quickly rises near the equator.\n"
"    Experiment, it has a fairly subtle effect.\n"
"  </dd>\n"
"  <dt>Snowline slope suppression</dt>\n"
"  <dd>\n"
"    The larger this value is, the harder it is for snow to stick to steep slopes \n"
"    (you can see this effect for real on any mountain range).\n"
"    This breaks up the snowline quite nicely, as it otherwise tends to stop at \n"
"    an artificially uniform height (reduce this parameter to zero to see what I mean).\n"
"  </dd>\n"
"  <dt>Snowline glacier effect</dt>\n"
"  <dd>\n"
"    If this parameter is positive, rivers tend to form glaciers and you \n"
"    will see e.g white lines running out of snowy areas, and frozen lakes.\n"
"    If this parameter is negative rivers find it harder to freeze and \n"
"    you will see them running through snowy areas and forming un-frozen lakes.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<h4>Rivers</h4>\n"
"\n"
"<dl>  \n"
"  <dt>Rivers</dt>\n"
"  <dd>\n"
"    Specifies the number of rivers to be generated.\n"
"    Note that rivers starting in the sea are immediately abandoned, but still count against this number\n"
"    (this is so the relative proportions of land/ocean can be tweaked without affecting the river source density).\n"
"    Rivers run from vertex to vertex along triangle edges, and are then rendered by blending from the\n"
"    river vertex colour to the surrounding terrain colour.\n"
"    This isn't ideal as they aren't very sharply defined.\n"
"    A previous version of the software flowed rivers from triangle to triangle\n"
"    which produced nice solid edged rivers (like the oceans are)\n"
"    but since they weren't flat it wasn't ideal either.\n"
"  </dd>\n"
"  <dt>Rivers seed</dt>\n"
"  <dd>\n"
"    Random number seed for river generation.\n"
"    If you change this, but not the perturbation seed, you can get a different river network on the same terrain.\n"
"  </dd>\n"
"  <dt>Lake becomes sea</dt>\n"
"  <dd>\n"
"    As a river is flowed across a terrain, it will sometimes form a lake as the water\n"
"    level rises sufficiently to overcome a terrain barrier.\n"
"    If the lake becomes sufficiently large (greater than the percentage of available surface area specified here) \n"
"    then it is considered to have become an inland sea and it is no longer necessary for the lake to rise until \n"
"    an outflow to be discovered (in the real world, surface evaporation replaces the outflow). \n"
"    Increasing this number may result in larger lakes, but the process of river generation can \n"
"    take considerably longer.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<h4>Colours</h4>\n"
"\n"
"<dl>\n"
"  <dt>Change colours</dt>\n"
"  <dd>\n"
"    Click on these buttons to bring up a colour-picking dialog and change the colour for the selected class of terrain.\n"
"    Each button displays the colour currently selected for that class.\n"
"    Obvious things you might want to do are to change the shoreline colour to \n"
"    the same colour as the low level terrain (those beaches are pretty absurdly <em>wide</em> otherwise), \n"
"    change the orange highlands to a more mountainous grey, or perhaps you preferred\n"
"    an apocalyptic \"lava world\" with red rivers and oceans (use emission to make them glow), \n"
"    black shorelines and ash-grey terrain.\n"
"  </dd>\n"
"  <dt>Oceans and rivers emissive</dt>\n"
"  <dd>\n"
"    Sets the emission (also called \"glow\") of lakes and rivers, on a scale of 0-100.\n"
"    This is mainly to facilitate glowing lava planets.\n"
"    Note that for terrain generated with emission non-zero, \n"
"    an alternative OpenGL rendering path must be used which may be a little slower.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<h4>Clouds</h4>\n"
"\n"
"<p>\n"
"  The cloud layer, when generated, is a triangle mesh with variable\n"
"  translucency at each vertex.\n"
"  Note that it's <em>not</em> a texture in the normal sense.\n"
"</p>\n"
"\n"
"<dl>\n"
"  <dt>Clouds enabled</dt>\n"
"  <dd>\n"
"    Select the check box to trigger creation of a cloud layer.\n"
"  </dd>\n"
"  <dt>Subdivisions</dt>\n"
"  <dd>\n"
"    When left unchecked, the cloud layer will be subdivided\n"
"    to the same degree as the terrain.\n"
"    Check the box for explicit control of cloud layer subdivision.\n"
"  </dd>\n"
"  <dt>Clouds seed.</dt>\n"
"  <dd>\n"
"    Change this to get a different cloud pattern.\n"
"  </dd>\n"
"  <dt>Cloud height</dt>\n"
"  <dd>\n"
"    Change this to get adjust the height of the clouds above the terrain.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<h4>Other \"Create\" tab controls</h4>  \n"
"  \n"
"<dl>\n"
"  <dt>Generate</dt>\n"
"  <dd>\n"
"    Click this button to regenerate the planet/terrain area <em>without changing the random seeds</em>.\n"
"  </dd>\n"
"  <dt>...new rivers seed</dt>\n"
"  <dd>\n"
"    Click this button to increment the river generation seed by one and regenerate.\n"
"    This gets you the same landscape with a different river network.\n"
"  </dd>\n"
"  <dt>...new terrain seed</dt>\n"
"  <dd>\n"
"    Click this button increment the terrain seed by one and regenerate.\n"
"    This will get you a completely different terrain.\n"
"    (The river network will be different too, although it's seed won't have changed, as the rivers flow over the terrain differently).\n"
"  </dd>\n"
"  <dt>...new clouds seed</dt>\n"
"  <dd>\n"
"    Click this button increment the clouds seed by one and regenerate.\n"
"    This will get you a different cloud layer.\n"
"  </dd>\n"
"</dl>  \n"
"\n"
"<h3>Render tab</h3>\n"
"\n"
"<p>\n"
"  Options controlling OpenGL rendering appear on this tab.\n"
"  Generally none of these makes any difference to the state of the rendered mesh.\n"
"  However, some of the parameters do affect some export methods; see the individual item descriptions.\n"
"</p>\n"
"\n"
"<dl>\n"
"  <dt>Wireframe checkbox</dt>\n"
"  <dd>\n"
"    Selects OpenGL wireframe rendering mode.\n"
"  </dd>\n"
"  <dt>Joystick mouse checkbox</dt>\n"
"  <dd>\n"
"    This simply affects whether, when in flight mode, pulling the mouse down the screen flies you in that\n"
"    direction, or whether it's more like an aircraft joystick and pitches you up instead.\n"
"  </dd>\n"
"  <dt>Display list</dt>\n"
"  <dd>\n"
"    Selects OpenGL display list rendering.\n"
"    Display list rendering is generally but not always faster\n"
"    (depends on your OpenGL implementation and graphics drivers).\n"
"    The main reason display list rendering is not enabled by default is that\n"
"    the application may pause for a long time while OpenGL processes the\n"
"    list when it is first rendered; the amount of additional memory\n"
"    consumed by the display list is also a problem for very large meshes.  \n"
"  </dd>\n"
"  <dt>Background colour</dt>\n"
"  <dd>\n"
"    Colour picking buttons to control the colour used for the viewing area background.\n"
"    The low altitude colour is also used when exporting a cloud mesh to blender.\n"
"  </dd>\n"
"  <dt>Ambient</dt>\n"
"  <dd>\n"
"    The amount of ambient illumination.\n"
"    This is also used when exporting shaded textures.\n"
"  </dd>\n"
"  <dt>Illumination azimuth and elevation</dt>\n"
"  <dd>\n"
"    Control the illumination direction.\n"
"    These are also used when exporting shaded textures.\n"
"  </dd>\n"
"  <dt>Status</dt>\n"
"  <dd>\n"
"    A text field displaying information about the rendered mesh,\n"
"    and rendering performance (recent average frames-per-second).\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<h3>Save tab</h3>\n"
"\n"
"<p>\n"
"  Strictly speaking it's Export which is provided, not save, as fracplanet's state cannot be restored (yet).\n"
"  There are currently three ways of exporting models from fracplanet for other uses: to POV-Ray, to Blender and as textures.\n"
"</p>\n"
"\n"
"<h4>POV-Ray</h4>\n"
"\n"
"<dl>\n"
"  <dt>Add atmosphere</dt>\n"
"  <dd>\n"
"    This tick-box causes additional POV-Ray directives to be emitted to render a thin layer of atmosphere.\n"
"  </dd>\n"
"  <dt>Sea object</dt>\n"
"  <dd>\n"
"    This tick-box causes a single sphere or infinite plane to generated \n"
"    for the oceans <em>instead</em> of numerous individual triangles.\n"
"  </dd>\n"
"  <dt>Base filename</dt>\n"
"  <dd>\n"
"    Enter the filename root to be used here.\n"
"    Two files will be generated, one with \".pov\" appended, and one with \".inc\" appended.\n"
"    The .inc file actually contains the object (using POV-Rays's <code>mesh2</code> format),\n"
"    plus any other objects generated due to the options above.\n"
"    The .pov just includes the .inc and adds a camera and light source and was primarily intended for testing:\n"
"    the expectation is that users will generally include the .inc into their own scenes, probably wrapping it\n"
"    in a POV-Ray <code>union</code> with embedded translate/scale/rotate directives).\n"
"  </dd>\n"
"  <dt>Save (POV-Ray)</dt>\n"
"  <dd>\n"
"    Click this button to create the POV-Ray files.\n"
"    This can take quite a long time so a progress bar is used to track the completed percentage.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<p>\n"
"  There are additional comments on usage with POV-Ray <a href=\"#pov\">below</a>.\n"
"</p>\n"
"\n"
"<h4>Blender</h4>\n"
"\n"
"<dl>\n"
"  <dt>Per vertex alpha</dt>\n"
"  <dd>\n"
"    This check box selects Fracplanet's preferred method for outputting clouds:\n"
"    by specifying per-vertex alpha components.\n"
"    However, Blender seems to ignore these so a workround is used and an opaque cloud layer is output\n"
"    (which should be ok if you fly underneath it and light it sensibly).\n"
"    You probably don't want to switch this on.\n"
"  </dd>\n"
"  <dt>Save (Blender)</dt>\n"
"  <dd>\n"
"    Click this button to create a file (a file dialog will appear) which can be imported into Blender.\n"
"    Note that the save file takes the form of a Python script (traditionally a <code>.py</code> file extension)\n"
"    which can be executed by Blender to reconstruct the fracplanet model.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<p>\n"
"  There are additional comments on usage with Blender <a href=\"#blender\">below</a>.\n"
"</p>\n"
"\n"
"<h4>Texture</h4>\n"
"\n"
"<p>\n"
"  Saving as texture(s) gives you a set of 2D images which can be used by other rendering or modelling applications,\n"
"  if not immediately then perhaps after a little massaging with the ImageMagick or NetPBM tools.\n"
"  On saving you'll be prompted for a base <i>filename.ppm</i>.  A number of files are then saved:\n"
"</p>\n"
"\n"
"<dl>\n"
"  <dt>filename.ppm</dt>\n"
"  <dd>\n"
"    The basic texture.\n"
"    This will be embedded in black pixels if the terrain isn't square e.g for the hexagonal terrain area.\n"
"    Planets produce a cylindrically projected latitude-longitude \"spheremap\".\n"
"  </dd>\n"
"  <dt>filename_dem.pgm</dt>\n"
"  <dd>\n"
"    The height field (a \"DEM\" is a Digital Elevation Model).\n"
"    This is output as a PGM image, with heights from 0.0 to 1.0 scaling to 0 to 65535.\n"
"    If the DEM contains a value greater than or equal to 256\n"
"    (highly likely, except for exceptionally flat terrains),\n"
"    then it will be output as a 16-bit PGM which, while part of the PGM\n"
"    \"standard\"\n"
"        (see http://netpbm.sourceforge.net/doc/pgm.html)\n"
"      \n"
"    isn't well supported by many common graphics tools which appear to otherwise offer good PPM support\n"
"    (e.g The Gimp doesn't like it).\n"
"    The NetPBM tools are your best bet to convert this to something else (e.g pnmdepth).\n"
"    Imagemagick's simple \"display\" tool also handles them well, scaling the maximum value to white.\n"
"  </dd>\n"
"  <dt>filename_norm.pgm</dt>\n"
"  <dd>\n"
"    The surface normals; XYZ components are mapped to RGB; [-1, 1] is scaled to [0,255].\n"
"    It is thought this should be useful for bump-mapping renderers (e.g\n"
"    Celestia\n"
"        (see http://www.shatters.net/celestia/)\n"
"      ) where the\n"
"    illusion of surface relief is produced by normal perturbation.\n"
"    If you intend to use this file, you almost certainly want to disable shading (see below),\n"
"    as your renderer will compute it later.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<p>\n"
"  Other texture save options:\n"
"</p>\n"
"<p>\n"
"\n"
"</p>\n"
"<dl>\n"
"  <dt>Shaded texture</dt>\n"
"  <dd>\n"
"    Controls whether the texture map includes the effect of lighting.\n"
"    If you intend to use the information in the _norm.ppm or the _dem.pgm files to subsequently compute lighting,\n"
"    then you almost certainly do <em>not</em> want the output texture to be shaded.\n"
"    The amount of ambient lighting is taken from the slider on the render tab.\n"
"    (Disabling shading is the same effect as setting ambient to 1.0).\n"
"    The lighting direction is controlled by the render tab's illumination azimuth and elevation controls.\n"
"    If you want to use the OpenGL window to preview the lighting, you should set zero spin and tilt\n"
"    by hitting the \"reset\"button and dragging the tilt to the centre (unfortunately, this\n"
"    gives you an edge-on view of flat terrain areas so it may be necessary to use the \"fly\" mode).\n"
"  </dd>\n"
"  <dt>Texture height</dt>\n"
"  <dd>\n"
"    The number of pixels high the saved texture images are.\n"
"    The width is implicitly determined by the height.\n"
"    For flat terrain the width is the same as the height.\n"
"    For planets the width is twice the height because the height spans [-90,+90] degrees latitude,\n"
"    and the width spans [-180,180] degrees longitude.\n"
"  </dd>\n"
"</dl>\n"
"\n"
"<p>\n"
"  There are additional comments exported texture usage <a href=\"#texture\">below</a>.\n"
"</p>\n"
"\n"
"\n"
"<h3>About tab</h3>\n"
"\n"
"<p>\n"
"  This tab displays information about the software (in particular the version number) and its license.\n"
"  There is a button to show a dialog containing user documentation,\n"
"  and another button to display information about the Qt toolkit.\n"
"</p>\n"
"\n"
"<h3>Display window</h3>\n"
"\n"
"<p>\n"
"  The display window, when shown, shows the current\n"
"  (most recently generated) terrain model.\n"
"  It is hidden when terrain is being regenerated or saved.\n"
"  The (badly named) \"tilt\" slider controls the latitude of the camera (when viewing a sphere-based object), \n"
"  and the elevation (as in azimuth-elevation) of the camera when viewing a planar based terrain area.  \n"
"  Note that in the latter case the bottom half of the slider places the camera below base ground level/sea level, \n"
"  which results in little being seen due to back-face culling.\n"
"  The \"spin rate\" controls the rate at which the object is rotated.\n"
"  The display window update rate is clamped to 60Hz, although this will typically\n"
"  only be reached at the lower levels of subdivision.\n"
"</p>\n"
"\n"
"<p>\n"
"  Hitting the \"Fly\" button puts you into free-flight mode.\n"
"  Pitch and yaw are controlled by the mouse position relative to the window centre\n"
"  (if the pitch control feels backwards, invert mouse-Y behaviour on the render tab).\n"
"  Roll is controlled by left/right mouse buttons, or the keyboard left/right arrow keys.\n"
"  Speed is controlled by the mouse wheel or the keyboard up/down arrow keys.\n"
"  Hit Esc to return to the normal viewing mode.\n"
"</p>\n"
"\n"
"<h2>Fracplanet exports: usage notes</h2>\n"
"\n"
"<a name=\"pov\"><h3>POV-Ray</h3></a>\n"
"\n"
"<p>\n"
"  To ray-trace a saved terrain (saved as terrain.pov and terrain.inc, say) do:<br>\n"
"  <code>povray -Q9 -geometry 768x576 terrain.pov</code><br>\n"
"  Expect to get MANY \"determinant too small\" messages, \n"
"  especially when using high degrees of subdivision.\n"
"  It used to take POV-Ray a LOT longer to read large models than\n"
"  to render them, but it seems to load much faster these days.\n"
"</p>\n"
"\n"
"<p>\n"
"  Note that clouds, if generated, are output using POV-Ray's <code>double_illuminate</code>\n"
"  and <code>no_shadow</code> declarations.\n"
"  <code>double_illuminate</code> means that both sides of clouds are the same colour\n"
"  (rather than the underside being black).\n"
"  <code>no_shadow</code> means that the clouds cast no shadow on the ground\n"
"  (where the clouds are solid, the shadows are completely black, which looks more unnatural than no shadows).\n"
"  If desired, the declarations can be found at the end of the saved .inc file and simply edited out.\n"
"</p>\n"
"\n"
"<p>\n"
"  (The author currently uses POV-Ray 3.6).\n"
"</p>\n"
"\n"
"<a name=\"blender\"><h3>Blender</h3></a>\n"
"\n"
"<p>\n"
"  The easiest way to load mesh saved for blender is with blender's <code>-P &lt;filename.py&gt;</code>\n"
"  command line argument on startup.\n"
"  Note that you'll have to remove blender's default initially created object to see much of the fracplanet object.\n"
"  Also, its shading won't look much like it did in fracplanet until you change blender's viewport shading mode to\n"
"  something better than the \"solid\" default.\n"
"</p>\n"
"\n"
"<p>\n"
"  Blender consumes an alarming amount of memory.\n"
"  Even loading a planet generated with fracplanet's default five levels of subdivision\n"
"  consumes a quarter of a gigabyte of RAM.\n"
"  It would probably help if fracplanet made some effort to optimise its meshes,\n"
"  and/or made use of textures rather than trying to do everything using geometry, but it doesn't yet.\n"
"</p>\n"
"\n"
"<p>\n"
"  Blender's per-vertex-colours capability seems to ignore the alpha component of per-vertex colours,\n"
"  with the result that the cloud layer is rendered as a solid opaque colour.\n"
"  <em>If anyone has a solution to this it would be gratefully received.</em>\n"
"  Fracplanet's workround is to output an opaque surface with the blending\n"
"  (with the \"low altitude\" colour from the render tab controls) already done.\n"
"  The \"per-vertex alpha\" checkbox switches to the preferred behaviour in case it ever gets fixed.\n"
"</p>\n"
"\n"
"<p>\n"
"  Each mesh is output with 2 materials.\n"
"  The clouds just use material 0.\n"
"  For the terrain, material 0 is for land facets, material 1 for the sea.\n"
"  This may be useful for users wanting to give them different shader properties.\n"
"</p>\n"
"\n"
"<p>\n"
"  In Blender, the terrain mesh is named \"fracplanet.terrain\".\n"
"  There will also be a \"fracplanet.cloud\" if clouds were enabled.\n"
"</p>\n"
"\n"
"<p>\n"
"  Emissive terrain isn't supported.\n"
"</p>\n"
"\n"
"<p>\n"
"  If you want to manipulate the terrain, and know a bit of Python,\n"
"  an easy way might be to edit the <code>.py</code> file;\n"
"  at the start there are two functions defined: \n"
"  <code>v</code> called to create each vertex\n"
"  and <code>f</code> called to create each face.\n"
"</p>\n"
"\n"
"<p>\n"
"  (The author currently uses Blender 2.46).\n"
"</p>\n"
"\n"
"<a name=\"texture\"><h3>Texture Maps</h3></a>\n"
"\n"
"<p>\n"
"  It is hoped that the texture export option will be of use to those building planets for\n"
"  Celestia\n"
"        (see http://www.shatters.net/celestia/)\n"
"      .\n"
"  (The author hasn't tried it himself yet; at time of writing Celestia\n"
"  is waiting to make it's way from Debian Unstable to Testing).\n"
"  Celestia's texture formats are well documented at the \n"
"  Celestia Motherlode\n"
"        (see http://www.celestiamotherlode.net/catalog/documentation.html)\n"
"      ,\n"
"  in particular by the Virtual Surface Textures\n"
"        (see http://www.lns.cornell.edu/~seb/celestia/textures.html)\n"
"      \n"
"  document.\n"
"  The main issues for anyone doing this would seem to be the need to reduce the height map from 16 to 8 bit\n"
"  (easily changed by pnmdepth) and the possibility that fracplanet's normal map (if used, alternative Celestia\n"
"  can compute normal maps from the height map) needs it's components exchanging or reflecting.\n"
"  Celestia can also take a specular map, generally used to indicating the presence of reflective water (or snow);\n"
"  fracplanet doesn't output this (yet) but it should be possible to fake something by processing the height or texture map.\n"
"</p>\n"
"\n"
"<p>\n"
"  As a simple demonstration of how to use a saved planet texture, here's how to use it as a spheremap in POV-Ray.\n"
"  Save an unshaded <code>spheremap.ppm</code> from fracplanet, and do:<br>\n"
"  <code>\n"
"    convert spheremap.ppm spheremap.png<br>\n"
"    convert -depth 8 spheremap_dem.pgm spheremap_dem.png<br>\n"
"    cat &lt;&lt;EOF &gt; spheremap.pov<br>\n"
"    camera {perspective location &lt;0,1,-4.5&gt; look_at &lt;0,0,0&gt; angle 45}<br>\n"
"    light_source {&lt;100,100,-100&gt; color rgb&lt;1,1,1&gt;}<br>\n"
"    sphere<br>\n"
"    {<br>\n"
"    &lt;0,0,0&gt;,1<br>\n"
"    pigment { image_map {png \"spheremap.png\" map_type 1} }<br>\n"
"    normal { bump_map {png \"spheremap_dem.png\" map_type 1 bump_size 20.0} }<br>\n"
"    rotate &lt;0,clock*360,0.0&gt;<br>\n"
"    }<br>\n"
"    EOF<br>\n"
"    povray spheremap.pov +KFI1 +KFF100 +W640 +H480 +Of.png<br>\n"
"    animate f???.png<br>\n"
"  </code>\n"
"</p>\n"
"\n"
"<p>\n"
"  <code>convert</code> is one of the ImageMagick utilities.\n"
"  You could omit the line beginning \"normal\" but you will get a rather flat looking planet.\n"
"  The factor of 20.0 at the end of the line is a rather arbitrary parameter.\n"
"  Raise or lower it to adjust the apparent roughness of the planet.\n"
"</p>\n"
"\n"
"<h2>License</h2>\n"
"\n"
"<p>\n"
"  This program is free software; you can redistribute it and/or\n"
"  modify it under the terms of the GNU General Public License\n"
"  as published by the Free Software Foundation; either version 2\n"
"  of the License, or (at your option) any later version.\n"
"  \n"
"  This program is distributed in the hope that it will be useful,\n"
"  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"  GNU General Public License for more details.\n"
"  \n"
"  You should have received a copy of the GNU General Public License\n"
"  along with this program; if not, write to the Free Software\n"
"  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n"
"</p>\n"
"\n"
"<p>\n"
"  The full license can be also viewed on fracplanet's \"About\" tab.\n"
"</p>\n"
"\n"
"\n"
"</qt>\n"
