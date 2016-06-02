# geoSurfDEM
C++ command-line application to determine intersection between 3D geosurfaces and DEMs.  
Input 3D geosurfaces are produced by the Python application as in the simSurf project:  
https://github.com/mauroalberti/simSurf  
Currrently in beta state.

------

The determination of the theoretical intersections between digital 3D geological surfaces and topography could be of potential help for studying the field attitudes of natural geological surfaces as seen in outcrops and from aerial and satellite images.

I have previously created a tool for QGIS, the qgSurf plugin, that allows to determine the intersections between a geological plane and the topography, as well as to infer the possible local attitude of geological surfaces given three or more points located on a DEM.

Since geological structures have geometries much more complex than planes, the analysis of the relationships between 3D surfaces and topography requires more evolved tools that can read and process 3D geological surfaces.

To this aim, I created a tool that aims at determining:
a) the theoretical intersections between a 3D surface and a topography
b) the local 3D attitude of that surface at each intersection point.

This can be considered a forward problem, for the solution of which I am currently unaware of any existing open source implementations. The inverse problem, i.e., given local intersections determine the probable geological attitudes, has been already investigated by other researchers and  I do not discuss it here.

Here I present a C++ implementation, that can be considered a working, beta pre-production release, solving the forward problem.
Why C++? Because it allows to create fast applications, much faster than others created with more-developer friendly tools such as Python, for instance. Considering that topography is generally represented by grids and geological surfaces by meshes, the product of these two types of data sources produces a data set to investigate that can be quite large, thus requiring a fast language in order to have acceptable run times.
The drawback is that C++ is complex and code production slower than with using Python.

How does it work?
Below you see the screenshot of a single run of the tool.

![alt tag](http://www.malg.eu/geosurfdem/images/appl_run.png)

What is important to note?
First of all, you have to input the name of a file, in the run example named "input_files.txt" (located in the same directory as the compiled application), that provides to the application the paths of three files:
1) DEM, in ESRI ASCII grid format
2) geological surface, in (old) VTK text format
3) output csv file storing for each row: x-y-z-dip direction-dip angle

An example of input text file is the following:

./publ_data/dem_malpi_aster_wgs84utm33.asc
./publ_data/geological_plane.vtk
./publ_data/intersections.csv

while examples of input data files (DEM ASCII grid, VTK geosurface file, CSV intersection result) can be found in the publ_data subdirectory.

Then the application outputs a few informative messages about the number of found features and hopefully at the last it prints out a number of found intersecting points greater than zero..

If you work with GIS, DEMs should have no secret for you. You can export a DEM in ESRI ASCII grid format with Saga GIS (in addition to ArcGIS).

And for the other input data source, i.e., the VTK text file storing the geological surface data?
One possibility is to use simSurf: https://github.com/mauroalberti/simSurf
With this Python tool (beware: 2.7, not 3.x), you can simulate analytical geological surfaces.
We will see a working example here, that will be used also as a validation example for geoSurfDEM output.

We create with simSurf an inclined plane with known attitude and location fitting the extent of a classical (for me..) Mt. Alpi DEM, derived from ASTER data.

simSurf is subdivide in two modules:
a) geosurface_simulation.py
b) geosurface_deformation.py

The module names are sufficiently clear to expose their purpose, I hope.

So we start creating a horizontal plane with the Geosurface simulation tool, "Analytical formula" part. See figure below.

![alt tag](http://www.malg.eu/geosurfdem/images/simSurf_analitical_surface.png)

The zero in the formula section is for the horizontal plane creation. You calculate the matrix and you can see the plane in three dimensions.

Then to the geographical parameters, that have to fit the DEM extent without creating a too large geological plane.

![alt tag](http://www.malg.eu/geosurfdem/images/simSurf_geog_params.png)

We create the simulated geosurface, optionally view it in three dimensions and then have to export it in the Geo Analytical Surface (GAS) format, i.e. a jason format..

We then pass to the Geosurface deformation tool, import the previously exported jason file and then apply a rotation to the plane around a N-S horizontal axis, by 35°.

![alt tag](http://www.malg.eu/geosurfdem/images/simSurf_rot_horiz_axis_35d.png)

Apply and then rotate by 45° around a vertical axis (plunge equal to 90°).

![alt tag](http://www.malg.eu/geosurfdem/images/simSurf_rot_vert_axis_45d.png)

Now we locate the rotated plane to a geographical position that broadly fits with the DEM. I choose to use my qgSurf plugin for QGIS for quickly locating a point at the center of the used DEM, while knowing also the z value.

![alt tag](http://www.malg.eu/geosurfdem/images/qgis_qgsurf_snap_point_dem.png)

You see to the right the coordinates (x-y-z) of the point at the DEM center, showed within QGIS.
I copied and pasted these values in the simSurf displacement tab, so that the plane is displaced by the given delta-x, delta-y and delta-z amounts.

![alt tag](http://www.malg.eu/geosurfdem/images/simSurf_displacement.png)

Done, after applying.
Save the geosurface as a VTK file and then you can use it in the geoSurfDEM application.
Before that we see how are the DEM and the VTK plane data in Paraview.
You can import the DEM when in x-y-z format (could create with Saga), then applying a Table to Point filter, while the VTK format is directly read.
Here a nadiral view.

![alt tag](http://www.malg.eu/geosurfdem/images/paraview_src_up.png)

And a lateral one.

![alt tag](http://www.malg.eu/geosurfdem/images/paraview_src_lateral.png)

At the end, what is the result of the geoSurfDEM application?
We see it displayed in Paraview, importing the resulting csv file.

![alt tag](http://www.malg.eu/geosurfdem/images/paraview_result_lateral.png)

The results are symbolized by blue dots. You see them following the visual intersection between the plane with dip direction 135° and dip angle 35° and the DEM.

Always in Paraview we see, for a few records, that the corresponding point attitudes calculated by geoSurfDEM are correct: 135°/35° for each point.

![alt tag](http://www.malg.eu/geosurfdem/images/paraview_result_table.png)

The code repository of geoSurfDEM is at:
[GitHub code repository](https://github.com/mauroalberti/geoSurfDEM)

Executables for both Linux and Windows will be soon available from:
[executables repository](http://malg.eu/geosurfdem.php) (note: page still to be created..)




