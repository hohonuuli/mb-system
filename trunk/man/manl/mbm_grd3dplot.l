.TH mbm_grd3dplot l "4 January 2001" "MB-System 5.0" "MB-System 5.0"
.SH NAME
\fBmbm_grd3dplot\fP - Create an executable shellscript which will generate
a GMT 3D perspective plot of gridded data in a GMT grd file.

.SH VERSION
Version 5.0

.SH SYNOPSIS
\fBmbm_grd3dplot\fP \fB-I\fP\fIfile\fP 
[\fB-A\fP[\fImagnitude\fP[\fI/azimuth/elevation\fP] 
\fB-D\fP[\fIflipcolor/flipshade\fP]
\fB-E\fP[\fIview_az/view_el\fP] 
\fB-G\fP\fIcolor_mode\fP \fB-H\fP \fB-K\fP\fIintensity_file \fP
\fB-N\fP\fIdrape_file\fP
\fB-O\fP\fIroot\fP \fB-P\fP\fIpagesize \fP
\fB-S\fP\fI[color/shade]\fP \fB-U\fP\fIorientation\fP \fB-V\fP 
\fB-W\fP[\fIcolor_style\fP[\fI/pallette\fP[\fIncolors\fP]] | \fIcptfile\fP] ]

Additional Options:
.br
[\fB-B\fP\fItickinfo\fP
\fB-J\fP\fIprojection\fP[\fI/scale | width\fP]
\fB-L\fP\fItitle\fP[\fI:scale_label\fP] \fB-M\fP\fImisc\fP 
\fB-Q\fP \fB-R\fP\fIw/e/s/n\fP \fB-X\fP \fB-Y\fP 
\fB-Z\fP\fImin/max\fP ]

Miscellaneous Options:
.br
[\fB-MGD\fP\fIgmtdef/value\fP  \fB-MGF\fP\fIscale_loc\fP
\fB-MGL\fP[\fIf\fP][\fIx\fP]\fIlon0/lat0/slat/length\fP[\fIm\fP]
\fB-MGQ\fP\fIdpi\fP \fB-MGS\fP\fIscalefactor\fP
\fB-MGT\fP\fIx/y/size/angle/font/just/text\fP
\fB-MGU\fP\fP[\fI/dx/dy/\fP][\fIlabel\fP] 
\fB-MVM\fP\fImesh_pen\fP \fB-MVN\fP[\fInull[/r/g/b]\fP] ]


.SH DESCRIPTION
\fBmbm_grd3dplot\fP is a macro to generate a shellscript of GMT commands which, when 
executed, will generate a 3D perspective Postscript plot of  
gridded data.  Several styles of plots can be generated, 
including color fill views, color shaded relief views, mesh  
plot views, and text labels. Five different color schemes are 
included. The plot will be scaled to fit on the specified page  
size or, if the scale is user defined, the page size will be  
chosen in accordance with the plot size. The primary purpose 
of this macro is to allow the simple, semi-automated 
production of nice looking maps with a few command line 
arguments. For users seeking more control over the plot 
appearance, a number of additional optional arguments are 
provided. Truly ambitious users may edit the plot shellscript 
to take advantage of GMT capabilites not supported by this 
macro.

.SH AUTHORSHIP
David W. Caress (caress@mbari.org)
.br
  Monterey Bay Aquarium Research Institute
.br
Dale N. Chayes (dale@ldeo.columbia.edu)
.br
  Lamont-Doherty Earth Observatory

.SH SIMPLE DESCRIPTION OF BASIC OPTIONS
.TP
.B \-A
\fImagnitude[/azimuth/elevation]\fP
.br
Sets the parameters which control the
synthetic illumination of the gridded data (shaded relief).
The value \fImagnitude\fP
is an effective vertical exageration which modulates the intensity of
the shading; typical values are in the 0.1 to 10 range.  
The value \fIazimuth\fP
is the azimuth in degrees from north from which the data is illuminated.
The value \fIelevation\fP is the elevation of the illumination 
in degrees from horizontal.
Defaults: \fImagnitude\fP = 0.2; \fIazimuth\fP = 0.0; 
\fIelevation\fP = 30.0;
.TP
.B \-D
\fI[flipcolor/flipshade]\fP
.br
Normally, the color or grayscale tables used for color maps run from
cool colors (or dark grays) for low grid values 
to hot colors (or light grays) for high grid values.
This option reverses the color table so that cool colors (dark grays)
correspond to high values and hot colors (light grays) to low values.
If \fB-D\fP is given alone, it applies to the color table used
for color or gray fill plots,  shaded or unshaded. If the plot
is to be shaded,  either by synthetic illumination (\fB-G\fP\fI2\fP) 
or using an intensity file (\fB-K\fP and \fB-G\fP\fI3\fP options), then
setting \fIflipshade\fP = 1 will cause the shading convention
to be reversed (e.g. high intensities overlaid as light shading).
Using \fB-D\fP\fI0/1\fP will flip the shading convention
but leave the default color convention.
.TP
.B \-E
[\fIview_az/view_el\fP]
.br
Sets the azimuth and elevation in degrees of the
viewpoint for 3D perspective views.  An \fIview_az\fP of 0
represents a view from the north, and \fIview_az\fP = 90
represents a view from the east.  An \fIview_el\fP of 0
represents a horizontal view.  Default: \fIview_az\fP = 200.;
elevation = 40.
.TP
.B \-G
\fIcolor_mode\fP
.br
Turns on color fill plot and sets the style of the plot.
.br
        \fIcolor_mode\fP = 1: 3D color/gray fill.
        \fIcolor_mode\fP = 2: 3D color/gray fill shaded by 
                        synthetic illumination.
        \fIcolor_mode\fP = 3: 3D color/gray fill shaded by 
                        an intensity file. The \fB-K\fP 
                        option must be used to specify
 			the intensity file.
        \fIcolor_mode\fP = 4: 3D Color/gray fill of slope 
 			magnitude.
        \fIcolor_mode\fP = 5: 3D Color/gray fill shaded by 
                        slope magnitude.
        \fIcolor_mode\fP = 6: 3D mesh plot.
        \fIcolor_mode\fP = 7: 3D mesh plot with contours.
.TP
.B \-H
This "help" flag cause the program to print out a description
of its operation and then exit immediately.
.TP
.B \-I
\fIgrdfile\fP
.br
Sets the name of the gridded data file to be plotted.  
Alternatively, \fIgrdfile\fP may be a list of grid files
(one filename on each line) to be plotted together.
.TP
.B \-K
\fIintensity_file\fP
.br
Sets the name of the gridded data file containing
instensity values to be used for shading the map.  
.TP
.B \-N
\fIdrape_file\fP
.br
Sets the name of the gridded data file to be draped on
the relief provided by the file specified with the \fB-I\fP
option. If \fB-N\fP\fIdrape_file\fP is specified, then
all color control options will apply to the data in 
\\fIdrape_file\fP.
.TP
.B \-O
\fIroot\fP
.br
Sets the root used to construct the filename of the output shellscript
(\fIroot\fP.cmd) and names of files created when the shellscript is 
run.  Normally the 
name of the input grid file or grid file list is 
used as the \fIroot\fP. 
.TP
.B \-P
\fIpagesize\fP
.br
This option sets the size of the page the plot will be centered
on. If the user does not set the plot scale, the plot will be
sized as large as will fit on the designated page. If the user
sets the plot scale such that the plot will not fit on the
designated page, a larger page will be used.
The supported page sizes include ANSI A, B, C, D, E,
F, and E1, as well as most metric page sizes. See the
COMPLETE DESCRIPTION OF OPTIONS section 
below for a complete list of 
the supported page sizes. The default page size is A.
.TP
.B \-S
\fI[color/shade]\fP
.br
This option enables effective histogram equalization of the
color and/or shading of the gridded data. The equalization is
not achieved by changing the data values, but rather by
constructing the color or shading tables so that
the boundaries in the tables encompass equal fractions of
the datapoints. This serves to focus color or shading contrasts
in value ranges corresponding to the bulk of the data values.
If \fB-S\fP is given alone or with \fIcolor\fP = 1,
it enables equalization of the color table used
for color or gray fill plots,  shaded or unshaded. If the plot
is to be shaded,  either by synthetic illumination (\fB-G\fP\fI2\fP) 
or using an intensity file (\fB-K\fP and \fB-G\fP\fI3\fP options), then
setting \fIshade\fP = 1 will cause the shading to be equalized.
Using \fB-S\fP\fI0/1\fP will equalize the shading without
equalizing the color table.
.TP
.B \-U
\fIorientation\fP
.br
Normally the orientation of the plot (portrait or landscape)
is selected automatically so as to maximize the plot scale.
The \fB-U\fP option allows the user to set the plot orientation. If
\fIorientation\fP = 1, a portrait plot will be produced; if
\fIorientation\fP = 2, a landscape plot will be produced.
.TP
.B \-V
Causes \fBmbm_grd3dplot\fP to operate in "verbose" mode 
so that it outputs
more information than usual.
.TP
.B \-W
\fI[color_style\fP[\fI/pallette\fP[\fIncolors\fP]] | cptfile]
.br
This option controls the color scheme used for color
fill plots. 

If \fIcolor_style\fP = 1 [default], then
the color scheme used will be a continuous grading
of colors. If \fIcolor_style\fP = 2, the color scheme
will be a set of discrete color intervals. The color
pallette used is set using \fIpallette\fP. Five pallettes
are available:
 	\fIpallette\fP = 1:	Haxby colors [default]
 	\fIpallette\fP = 2:	high Intensity colors
 	\fIpallette\fP = 3:	low Intensity colors
 	\fIpallette\fP = 4:	grayscale
 	\fIpallette\fP = 5:	uniform grayscale
.br
A complete description of the color pallettes is given
in the COMPLETE DESCRIPTION OF OPTIONS section below.
.br
The \fIncolors\fP parameter sets the number of color
values used in plotting, whether the colors are 
represented in a continuous color scale or a
stepped, discrete color scale [default is 11].

If the option argument is the path to an existing \fBGMT\fP 
color pallette (CPT) file, then that CPT file and its
color scheme will be used for the plot

.SH COMPLETE DESCRIPTION OF OPTIONS
.TP
.B \-A
\fImagnitude[/azimuth]\fP
.br
Sets the parameters which control the
synthetic illumination of the gridded data (shaded relief).
The value \fImagnitude\fP
is an effective vertical exageration which modulates the intensity of
the shading; typical values are in the 0.1 to 0.5 range.  
The value \fIazimuth\fP
is the azimuth from which the data is illuminated.
Defaults: \fImagnitude\fP = 0.2; \fIazimuth\fP = 0.0;
.TP
.B \-B
\fItickinfo\fP
.br
Sets map boundary tickmark intervals. See the \fBpsbasemap\fP
manual page for details. By default the program chooses
basemap annotations based on the map boundaries.
.TP
.B \-D
\fI[flipcolor/flipshade]\fP
.br
Normally, the color or grayscale tables used for color maps run from
cool colors (or dark grays) for low grid values 
to hot colors (or light grays) for high grid values.
This option reverses the color table so that cool colors (dark grays)
correspond to high values and hot colors (light grays) to low values.
If \fB-D\fP is given alone, it applies to the color table used
for color or gray fill plots,  shaded or unshaded. If the plot
is to be shaded,  either by synthetic illumination (\fB-G\fP\fI2\fP) 
or using an intensity file (\fB-K\fP and \fB-G\fP\fI3\fP options), then
setting \fIflipshade\fP = 1 will cause the shading convention
to be reversed (e.g. high intensities overlaid as light shading).
Using \fB-D\fP\fI0/1\fP will flip the shading convention
but leave the default color convention.
.TP
.B \-E
[\fIview_az/view_el\fP]
.br
Sets the azimuth and elevation in degrees of the
viewpoint for 3D perspective views.  An \fIview_az\fP of 0
represents a view from the north, and \fIview_az\fP = 90
represents a view from the east.  An \fIview_el\fP of 0
represents a horizontal view.  Default: \fIview_az\fP = 200.;
elevation = 40.
.TP
.B \-G
\fIcolor_mode\fP
.br
Turns on color fill plot and sets the style of the plot.
.br
        \fIcolor_mode\fP = 1: 3D color/gray fill.
        \fIcolor_mode\fP = 2: 3D color/gray fill shaded by 
                        synthetic illumination.
        \fIcolor_mode\fP = 3: 3D color/gray fill shaded by 
                        an intensity file. The \fB-K\fP 
                        option must be used to specify
 			the intensity file.
        \fIcolor_mode\fP = 4: 3D Color/gray fill of slope 
 			magnitude.
        \fIcolor_mode\fP = 5: 3D Color/gray fill shaded by 
                        slope magnitude.
        \fIcolor_mode\fP = 6: 3D mesh plot.
        \fIcolor_mode\fP = 7: 3D mesh plot with contours.
.br
See the \fBgrdimage\fP manual page for information on shading
with intensity files
.TP
.B \-H
This "help" flag cause the program to print out a description
of its operation and then exit immediately.
.TP
.B \-I
\fIgrdfile\fP
.br
Sets the name of the gridded data file to be plotted.  
The data must be in a form acceptable to \fBGMT\fP version 3
programs (see the \fBGMT\fP Cookbook & Technical Reference).
.TP
.B \-J
\fIprojection\fP[\fI/scale | /width\fP]
.br
Selects the map projection. By default the map projection is
Mercator and the plot scale is chosen to fit on the selected
page size (see \fB-P\fP option). The user may specify a
different projection to be used, in which case the plot scale
is still automatically chosen to fit the page. The user may
also specify both the projection and the plot scale. If
the projection specifying character is upper case, a plot
width rather than a plot scale is used.
The scale values are specified in inch/degree or in 1:xxxxx
ratios. Plot widths are specified in inches. If the user
specifies a plot scale such that the plot will not fit
on the default A size page, a appropriately larger page
size will be chosen.
.br
.sp
\fBCYLINDRICAL PROJECTIONS:\fP
.br
.sp
\fB\-Jc\fP\fIlon0/lat0/scale\fP (Cassini)
.br
\fB\-Jm\fP\fIscale\fP (Mercator)
.br
\fB\-Joa\fP\fIlon0/lat0/azimuth/scale\fP (Oblique Mercator - point and azimuth)
.br
\fB\-Job\fP\fIlon0/lat0/lon1/lat1/scale\fP (Oblique Mercator - two points)
.br
\fB\-Joc\fP\fIlon0/lat0/lonp/latp/scale\fP (Oblique Mercator - point and pole)
.br
\fB\-Jq\fP\fIlon0/scale\fP (Equidistant Cylindrical Projection (Plate Carree))
.br
\fB\-Jt\fP\fIlon0/scale\fP (TM - Transverse Mercator)
.br
\fB\-Ju\fP\fIzone/scale\fP (UTM - Universal Transverse Mercator)
.br
\fB\-Jy\fP\fIlon0/lats/scale\fP (Basic Cylindrical Projection) 
.br
.sp
\fBAZIMUTHAL PROJECTIONS:\fP
.br
.sp
\fB\-Ja\fP\fIlon0/lat0/scale\fP (Lambert).
.br
\fB\-Je\fP\fIlon0/lat0/scale\fP (Equidistant).
.br
\fB\-Jg\fP\fIlon0/lat0/scale\fP (Orthographic).
.br
\fB\-Js\fP\fIlon0/lat0/scale\fP (General Stereographic)
.br
.sp
\fBCONIC PROJECTIONS:\fP
.br
.sp
\fB\-Jb\fP\fIlon0/lat0/lat1/lat2/scale\fP (Albers)
.br
\fB\-Jl\fP\fIlon0/lat0/lat1/lat2/scale\fP (Lambert)
.br
.sp
\fBMISCELLANEOUS PROJECTIONS:\fP
.br
.sp
\fB\-Jh\fP\fIlon0/scale\fP (Hammer)
.br
\fB\-Ji\fP\fIlon0/scale\fP (Sinusoidal)
.br
\fB\-Jk\fP\fIlon0/scale\fP (Eckert VI)
.br
\fB\-Jn\fP\fIlon0/scale\fP (Robinson)
.br
\fB\-Jr\fP\fIlon0/scale\fP (Winkel Tripel)
.br
\fB\-Jw\fP\fIlon0/scale\fP (Mollweide)
.br
.sp
\fBNON-GEOGRAPHICAL PROJECTIONS:\fP
.br
.sp
\fB\-Jp\fP\fIscale\fP (Linear projection for polar (theta,r) coordinates)
.br
\fB\-Jx\fP\fIx-scale\fP[\fBl|p\fP\fIpow\fP][\fI/y-scale\fP[\fBl|p\fP\fIpow\fP]] (Linear, log, and power scaling)
.br
More details can be found in the \fBpsbasemap\fP manpages.
.TP
.B \-K
\fIintensity_file\fP
.br
Sets the name of the gridded data file containing
instensity values to be used for shading the map.  
.TP
.B \-L
\fItitle:scalelabel\fP
.br
Sets the title and the label for the colorscale (if used) of
the plot. Note that a colon (:) rather than a slash (/) is
used to separate the labels. Colons cannot be used in the
labels themselves. If this option is not used, then a default title
and colorscale label are provided. If the title is supplied
alone, a default colorscale label will be provided. To force
no title use \fB-L\fP" "; to force no title or colorscale
label use \fB-L\fP" : ".
.TP
.B \-M
A series of "miscellaneous" options are provided which are
given as \fB-M\fP followed by a two character identifier, followed
by any other parameters associated with that option.
The \fB-M\fP options may be strung together separated by
colons, e.g. "-MGQ100:GU", which is equivalent to
"-MGQ -MGU". 
.TP
.B \-N
\fIdrape_file\fP
.br
Sets the name of the gridded data file to be draped on
the relief provided by the file specified with the \fB-I\fP
option. If \fB-N\fP\fIdrape_file\fP is specified, then
all color control options will apply to the data in 
\\fIdrape_file\fP.
.TP
.B \-MGD
\fIgmtdef/value\fP
.br
Allows the user to set the \fBGMT\fP default values used as
the plot is constructed. This command may be given repeatedly
to set as many \fBGMT\fP defaults as required. For example, to
set the basemap annotation font to Courier, use 
"-MGDANOT_FONT/Courier".
.TP
.B \-MGF
\fIscale_loc\fP
.br
Sets the location of the color scale. The possible values
of \fIscale_loc\fP are:
 	\fIscale_loc\fP = b:	bottom of plot
 	\fIscale_loc\fP = t:	top of plot
 	\fIscale_loc\fP = l:	left of plot
 	\fIscale_loc\fP = r:	right of plot
.br
[Default \fIscale_loc\fP = b]
.TP
.B \-MGL
[\fIf\fP][\fIx\fP]\fIlon0/lat0/slat/length\fP[\fIm\fP]
.br
Draws a simple map scale centered on \fIlon0/lat0\fP.  Use \fB\-Lx\fP to specify position in inch instead.
Scale is calculated at latitude \fIslat\fP, \fIlength\fP is in km [miles if m is appended].
Use \fB\-Lf\fP to get a "fancy" scale [Default is plain].
.TP
.B \-MGQ
\fIdpi\fP
.br
Sets the resolution in dots per inch of the raster image used
for color fill maps. Larger values of \fIdpi\fP produce larger
Postscript plot files. [Default is 100].
.TP
.B \-MGS
\fIscalefactor\fP
.br
The gridded data is multiplied by \fIscalefactor\fP.
This option is most often used flip the sign of the
data (\fIscalefactor\fP = -1). [Default no scaling]
.TP
.B \-MGT
\fIx/y/size/angle/font/just/text\fP
.br
Causes a text label to plotted on the map.
\fIsize\fP is text size in points, \fIangle\fP is 
measured in degrees counter-clockwise from horizontal, 
\fIfontno\fP sets the font type, \fIjustify\fP sets the alignment. 
If \fIfontno\fP starts with a leading hyphen, then 
the remainder of \fIfontno\fP is taken to be a 
textstring with the desired fontname.  See the 
\fBgmtdefaults\fP man page for names and numbers of available fonts 
(or run \fBpstext \-L\fP).  The alignment 
number refers to the part of the textstring that will be mapped 
onto the (\fIx,y\fP) point: 1 = Lower Left corner, 
2 = Lower Center, 3 = Lower Right, 5 = Mid Left, 6 = Mid Center, 
7 = Mid Right, 9 = Upper Left, 10 = Upper Center, 
11 = Upper Right. This option may be given as many times as
needed.
.TP
.B \-MGU
\fP[\fI/dx/dy/\fP][\fIlabel\fP]
.br
Draw Unix System time stamp on plot.  User may specify where the lower left corner
of the stamp should fall on the page relative to lower left corner of plot in inch [Default is (-0.75,-0.75)].  Optionally,
append a label, or \fBc\fP (which will plot the command string.)
.TP
.B \-MVM
\fImesh_pen\fP
.br
Set pen attributes for mesh plot.  The mesh plot must be specified
with the \fB-G\fP option. See chapter 4.12 in the
GMT Technical reference for a discussion of GMT pen values.  
[Defaults: width = 1, color = 0/0/0, 
texture = solid]. 
.TP
.B \-MVN
[\fInull[/r/g/b]\fP]
.br
Draws a plane at the level of the \fInull\fP value (with
respect to the data in the grid file specified with the \fB-I\fP
option. If the optional r/g/b is provided,
the frontal facade between the plane and the data perimeter is
filled with this color. If \fB-MVN\fP is given by
itself, then \fBmbm_grd3dplot\fP causes
the null plane to be drawn at the minimum z-level and the facade
to be colored a light gray (r=g=b=200).
.TP
.B \-O
\fIroot\fP
.br
Sets the root used to construct the filename of the output shellscript
(\fIroot\fP.cmd) and names of files created when the shellscript is 
run.  Normally the 
name of the input grid file or grid file list is 
used as the \fIroot\fP. 
.TP
.B \-P
\fIpagesize\fP
.br
This option sets the size of the page the plot will be centered
on. If the user does not set the plot scale, the plot will be
sized as large as will fit on the designated page. If the user
sets the plot scale such that the plot will not fit on the
designated page, a larger page will be used.
The supported page sizes are:

          American ANSI sizes:
          A     8.5 x 11.0 in.    ( 215.9 x  279.4 mm)
          B    11.0 x 17.0 in.    ( 279.4 x  431.8 mm)
          C    17.0 x 22.0 in.    ( 431.8 x  558.8 mm)
          D    22.0 x 34.0 in.    ( 558.8 x  863.6 mm)
          E    34.0 x 44.0 in.    ( 863.6 x 1117.6 mm)
          F    28.0 x 40.0 in.    ( 711.2 x 1016.0 mm)
          E1   44.0 x 68.0 in.    (1117.6 x 1727.2 mm)

          Metric ISO A sizes:
          A0   841.0 x 1189.0 mm  (33.11 x 46.81 in.)
          A1   594.0 x  841.0 mm  (23.39 x 33.11 in.)
          A2   420.0 x  594.0 mm  (16.54 x 23.39 in.)
          A3   297.0 x  420.0 mm  (11.69 x 16.54 in.)
          A4   210.0 x  297.0 mm  ( 8.27 x 11.69 in.)
          A5   148.0 x  210.0 mm  ( 5.83 x  8.27 in.)
          A6   105.0 x  148.0 mm  ( 4.13 x  5.83 in.)
          A7    74.0 x  105.0 mm  ( 2.91 x  4.13 in.)
          A8    52.0 x   74.0 mm  ( 2.05 x  2.91 in.)
          A9    37.0 x   52.0 mm  ( 1.46 x  2.05 in.)
          A10   26.0 x   37.0 mm  ( 1.02 x  1.46 in.)

          Metric ISO B sizes:
          B0   1000.0x 1414.0 mm  (39.37 x 55.67 in.)
          B1   707.0 x 1000.0 mm  (27.83 x 39.37 in.)
          B2   500.0 x  707.0 mm  (19.68 x 27.83 in.)
          B3   353.0 x  500.0 mm  (13.90 x 19.68 in.)
          B4   250.0 x  353.0 mm  ( 9.84 x 13.90 in.)
          B5   176.0 x  250.0 mm  ( 6.93 x  9.84 in.)
          B6   125.0 x  176.0 mm  ( 4.92 x  6.93 in.)
          B7    88.0 x  125.0 mm  ( 3.46 x  4.92 in.)
          B8    62.0 x   88.0 mm  ( 2.44 x  3.46 in.)
          B9    44.0 x   62.0 mm  ( 1.73 x  2.44 in.)
          B10   31.0 x   44.0 mm  ( 1.22 x  1.73 in.)

          Metric ISO C sizes:
          C0   914.4 x 1300.5 mm  (36.00 x 51.20 in.)
          C1   650.2 x  914.4 mm  (25.60 x 36.00 in.)
          C2   457.2 x  650.2 mm  (18.00 x 25.60 in.)
          C3   325.1 x  457.2 mm  (12.80 x 18.00 in.)
          C4   228.6 x  325.1 mm  ( 9.00 x 12.80 in.)
          C5   162.6 x  228.6 mm  ( 6.40 x  9.00 in.)
          C6   114.3 x  162.6 mm  ( 4.50 x  6.40 in.)
          C7    81.3 x  114.3 mm  ( 3.20 x  4.50 in.)

The default page size is A.
.TP
.B \-Q
Normally, the output plot generation shellscript 
includes lines which execute
a program to display the Postscript image on the screen.  
This option causes those lines to be commented out so
that executing the shellscript produces a Postscript plot
but does not attempt to display it on the screen.
The program
to be used to display the Postscript is set 
using \fBmbdefaults\fP;
the default value can be overridden by setting the environment
variable $MB_PS_VIEWER. 
.TP
.B \-R
\fIwest/east/south/north\fP
.br
\fIwest, east, south,\fP and \fInorth\fP specify the Region of interest.  To specify boundaries
in degrees and minutes [and seconds], use the dd:mm[:ss] format.  Append \fBr\fP if lower left and upper right
map coordinates are given instead of wesn.
You may ask for a larger \fIw/e/s/n\fP region to have more room between the image and the axes.
A smaller region than specified in the grdfile will result in a subset of the grid [Default is
region given by the grdfile].
.TP
.B \-S
\fI[color/shade]\fP
.br
This option enables effective histogram equalization of the
color and/or shading of the gridded data. The equalization is
not achieved by changing the data values, but rather by
constructing the color or shading tables so that
the boundaries in the tables encompass equal fractions of
the datapoints. This serves to focus color or shading contrasts
in value ranges corresponding to the bulk of the data values.
If \fB-S\fP is given alone or with \fIcolor\fP = 1,
it enables equalization of the color table used
for color or gray fill plots,  shaded or unshaded. If the plot
is to be shaded,  either by synthetic illumination (\fB-G\fP\fI2\fP) 
or using an intensity file (\fB-K\fP and \fB-G\fP\fI3\fP options), then
setting \fIshade\fP = 1 will cause the shading to be equalized.
Using \fB-S\fP\fI0/1\fP will equalize the shading without
equalizing the color table.
.TP
.B \-U
\fIorientation\fP
.br
Normally the orientation of the plot (portrait or landscape)
is selected automatically so as to maximize the plot scale.
The \fB-U\fP option allows the user to set the plot orientation. If
\fIorientation\fP = 1, a portrait plot will be produced; if
\fIorientation\fP = 2, a landscape plot will be produced.
.TP
.B \-W
\fI[color_style\fP[\fI/pallette\fP[\fIncolors\fP]] | cptfile]
.br
This option controls the color scheme used for color
fill plots. 

If \fIcolor_style\fP = 1 [default], then
the color scheme used will be a continuous grading
of colors. If \fIcolor_style\fP = 2, the color scheme
will be a set of discrete color intervals. The color
pallette used is set using \fIpallette\fP. Seven pallettes
are available:
 	\fIpallette\fP = 1:	Haxby colors [default]
 	\fIpallette\fP = 2:	high Intensity colors
 	\fIpallette\fP = 3:	low Intensity colors
 	\fIpallette\fP = 4:	grayscale
 	\fIpallette\fP = 5:	uniform grayscale
 	\fIpallette\fP = 6:	uniform black
 	\fIpallette\fP = 7:	uniform white

The RGB definitions of the color pallettes are:

color pallette 1 - Haxby Color Table
  red:   255 255 255 255 240 205 138 106  50  40  37
  green: 255 186 161 189 236 255 236 235 190 127  57
  blue:  255 133  68  87 121 162 174 255 255 251 175

color pallette 2 - High Intensity Colors
  red:   255 255 255 255 128   0   0   0   0 128 255
  green:   0  64 128 255 255 255 255 128   0   0   0
  blue:    0   0   0   0   0   0 255 255 255 255 255

color pallette 3 - Low Intensity Colors
  red:   200 194 179 141  90   0   0   0   0  90 141
  green:   0  49  90 141 179 200 141  90   0   0   0
  blue:    0   0   0   0   0   0 141 179 200 179 141

color pallette 4 - Grayscale
  red:   255 230 204 179 153 128 102  77  51  26   0
  green: 255 230 204 179 153 128 102  77  51  26   0
  blue:  255 230 204 179 153 128 102  77  51  26   0

color pallette 5 - Uniform Grayscale
  red:   128 128 128 128 128 128 128 128 128 128 128
  green: 128 128 128 128 128 128 128 128 128 128 128
  blue:  128 128 128 128 128 128 128 128 128 128 128
 
color pallette 6 - Uniform Black
  red:     0   0   0   0   0   0   0   0   0   0   0
  green:   0   0   0   0   0   0   0   0   0   0   0
  blue:    0   0   0   0   0   0   0   0   0   0   0

color pallette 7 - Uniform White
  red:   255 255 255 255 255 255 255 255 255 255 255
  green: 255 255 255 255 255 255 255 255 255 255 255
  blue:  255 255 255 255 255 255 255 255 255 255 255

The Haxby colors have been adapted from a pallette
developed by Dr. William Haxby of the Lamont-Doherty
Earth Observatory; this pallette is pleasing to the
eye and well suited for shading. The high intensity
colors describe linear paths through RGB space from
red to blue to green to purple; because the colors are high
intensity they are not well suited to shading.
The low intensity colors are similar to the high
intensity, but muted and thus well suited to shading.
The grayscale pallette runs linearly from white to
black and is commonly used for plots of sidescan and amplitude
data. The uniform grayscale is useful for non-color
shaded relief plots.
 
The \fIncolors\fP parameter sets the number of color
values used in plotting, whether the colors are 
represented in a continuous color scale or a
stepped, discrete color scale [default is 11].

If the option argument is the path to an existing \fBGMT\fP 
color pallette (CPT) file, then that CPT file and its
color scheme will be used for the plot
.TP
.B \-V
Causes \fBmbm_grd3dplot\fP to operate in "verbose" mode 
so that it outputs
more information than usual.
.TP
.B \-X
Normally, \fBmbm_grd3dplot\fP creates an executable shellscript and
then exits.  This option will cause the shellscript to be executed
in the background before \fBmbm_grd3dplot\fP exits.
.TP
.B \-Y
Normally, \fBmbm_grd3dplot\fP generates nicely rounded numbers
for the boundaries of the color pallette. Often, the resulting
color bounds extend well outside the range of the gridded data.
This option causes
the color boundaries to be uniformly distributed between the
minimum and maximum values of the grid.
.TP
.B \-Z
\fImin/max\fP
.br
This option overrides the minimum and maximum values of
the gridded data, affecting the color pallette and the
contour interval if those parameters are not specified
by the user.

.SH EXAMPLES
Suppose we have obtained two GRD files with dimensions of
127 by 194, one containing gridded bathymetry
(grd_sb2112_example_bath) and the other gridded sidescan
(grd_sb2112_example_ss). In order to generate a shellscript
which will in turn generate a 3D color shaded relief 
view of the bathymetry, we use the \fB-G\fP\fI2\fP option.
The grid file is in
bathymetry (positive down) rather in topography (positive
up), so the bathymetry needs to be rescaled by multiplying
by -1 (\fB-MGS\fP\fI-1\fP). We choose an illumination magnitude of 0.4
and an illumination azimuth of 45 degrees (\fB-A\fP\fI0.4/45\fP). We 
also choose a perspective azimuth of 250 degrees and an 
elevation of 30 degrees (\fB-E\fP\fI240/30\fP):
 
  	mbm_grd3dplot -Igrd_sb2112_example_bath \\
  	    -G2 -A0.4/45 -E250/30 -MGS-1 -X -V \\
  	    -Osb2112_example_bath3d
 
Now, to generate a 3D perspective view of the gridded
bathymetry shaded using the gridded sidescan data, we
use the \fB-G\fP\fI3\fP and \fB-K\fP options.
We want the sidescan data to
be histogram equalized, so we use \fB-S\fP\fI0/1\fP. We also want
the shading to be more prominent than the default
shading magnitude of 0.2 would produce, so we use \fB-A\fP\fI0.5\fP:
 
  	mbm_grd3dplot -Igrd_sb2112_example_bath \\
  	    -Kgrd_sb2112_example_ss \\
  	    -G3 -A0.5 -E250/30 -D0/1 \\
  	    -S0/1 -MGS-1 -X -V \\
  	    -Osb2112_example_bathss3d
 
The first example produces and executes a plot generation
shellscript called sb2112_example_bath3d.cmd and the second
produces a shellscript called sb2112_example_bathss3d.cmd.
As an example, the contents of the plotting shellscript 
"sb2112_example_bathss3d.cmd" are:
 
 #
 # Shellscript to create Postscript plot of data in grd file
 # Created by macro mbm_grd3dplot
 #
 # This shellscript created by following command line:
 # mbm_grd3dplot -Igrd_sb2112_example_bath \\
 #      -Kgrd_sb2112_example_ss -G3 -A0.5 \\
 #      -E250/30 -D0/1 -S0/1 -MGS-1 -X -V \\
 #      -Osb2112_example_bathss3d
 #
 # Save existing GMT defaults
 echo Saving GMT defaults...
 gmtdefaults -L > gmtdefaults$$
 #
 # Set new GMT defaults
 echo Setting new GMT defaults...
 gmtset ANOT_FONT Helvetica
 gmtset LABEL_FONT Helvetica
 gmtset HEADER_FONT Helvetica
 gmtset ANOT_FONT_SIZE 8
 gmtset LABEL_FONT_SIZE 8
 gmtset HEADER_FONT_SIZE 10
 gmtset FRAME_WIDTH 0.074999999999999997
 gmtset TICK_LENGTH 0.074999999999999997
 gmtset PAGE_ORIENTATION LANDSCAPE
 gmtset COLOR_BACKGROUND 0/0/0
 gmtset COLOR_FOREGROUND 255/255/255
 gmtset COLOR_NAN 255/255/255
 #
 # Make color pallette table file
 echo Making color pallette table file...
 echo  -4500  37  57 175  -4350  40 127 251 > \\
 	sb2112_example_bathss3d.cpt
 echo  -4350  40 127 251  -4200  50 190 255 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -4200  50 190 255  -4050 106 235 255 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -4050 106 235 255  -3900 138 236 174 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -3900 138 236 174  -3750 205 255 162 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -3750 205 255 162  -3600 240 236 121 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -3600 240 236 121  -3450 255 189  87 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -3450 255 189  87  -3300 255 161  68 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -3300 255 161  68  -3150 255 186 133 >> \\
 	sb2112_example_bathss3d.cpt
 echo  -3150 255 186 133  -3000 255 255 255 >> \\
 	sb2112_example_bathss3d.cpt
 #
 # Rescale data
 echo Rescaling data by -1...
 echo Running grdmath...
 grdmath grd_sb2112_example_bath -1 x = \\
 	grd_sb2112_example_bath.scale
 #
 # Get shading array
 echo Getting shading array...
 echo Running grdhisteq...
 grdhisteq grd_sb2112_example_ss \\
 	-Ggrd_sb2112_example_ss.eq -N
 echo Running grdmath...
 grdmath grd_sb2112_example_ss.eq -0.5 x \\
 	= grd_sb2112_example_ss.int
 rm -f grd_sb2112_example_ss.eq
 #
 # Make 3D view
 echo Running grdview...
 grdview grd_sb2112_example_bath.scale \\
 	-Jm13.678801784792578 \\
 	-Jz0.0014760910157720331 \\
 	-E250/30 \\
 	-R114.221/114.421/-31.9001/-31.6377 \\
 	-Csb2112_example_bathss3d.cpt \\
 	-N-4499.4399999999996/200/200/200 \\
 	-Igrd_sb2112_example_ss.int \\
 	-Qi \\
 	-P -X1.8081565710006675 -Y2 -K -V \\
 	> sb2112_example_bathss3d.ps
 #
 # Make color scale
 echo Running psscale...
 psscale -Csb2112_example_bathss3d.cpt \\
 	-D2.4418/-0.5000/4.8837/0.1500h \\
  	-B":.Data Values:" \\
 	-P -K -O -V >> sb2112_example_bathss3d.ps
 #
 # Make basemap
 echo Running psbasemap...
 psbasemap -Jm13.678801784792578 \\
 	-Jz0.0014760910157720331 \\
 	-E250/30 \\
 	-R114.221/114.421/-31.9001/-31.6377 \\
 	-B5m/5m:."Data File grd_sb2112_example_bath":WSZ \\
 	-P -O -V >> sb2112_example_bathss3d.ps
 #
 # Delete surplus files
 echo Deleting surplus files...
 rm -f sb2112_example_bathss3d.cpt
 rm -f grd_sb2112_example_bath.scale
 #
 # Reset GMT default fonts
 echo Resetting GMT fonts...
 mv gmtdefaults$$ .gmtdefaults
 #
 # Run xpsview
 echo Running xpsview in background...
 xpsview -ps a -maxp 4m sb2112_example_bathss3d.ps &
 #
 # All done!
 echo All done!

.SH SEE ALSO
\fBgrdimage\fP(l), \fBgrdcontour\fP(l), 
\fBmbdefaults\fP(l), \fBmbgrid\fP(l), \fBmbsystem\fP(l), 
\fBmbm_grd3dplot\fP(l), \fBmbm_plot\fP(l),
\fBpsbasemap\fP(l), \fBpstext\fP(l), \fBpsxy\fP(l)

.SH BUGS
By making this macro more useful, we have also made it
more complex. 


