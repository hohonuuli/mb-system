.TH mbdefaults l "20 November 2001" "MB-System 5.0" "MB-System 5.0"
.SH NAME
\fBmbdefaults\fP - Set or list default mbio parameters for 
reading and writing swath sonar data

.SH VERSION
Version 5.0

.SH SYNOPSIS
\fBmbdefaults\fP [\fB-D\fP\fIpsdisplay\fP \fB-I\fP\fIimagedisplay\fP 
\fB-L\fP\fIlonflip\fP \fB-T\fP\fItimegap\fP \fB-W\fP\fIproject\fP \fB-V -H\fP]

.SH DESCRIPTION
\fBmbdefaults\fP is a utility for setting or listing the default read
parameters used by some \fBMBIO\fP utilities such as \fBMBCOPY\fP.  
If a user wishes to set default
parameters different from those set by the \fBMBIO\fP library, then
these specialized default values must be stored
in the file .mbio_defaults in the users home directory.  If any option
flag is given in invoking \fBmbdefaults\fP, then a new .mbio_defaults
file will be written incorporating the newly set parameters along with
any old parameters which have not been reset. If no option flag
is given, then \fBmbdefaults\fP will list the current default values
(set by \fBmbio\fP if no .mbio_defaults file exists).

.SH CHANGES FROM VERSION 4.6
Note that prior to version 5.0, the \fBMB-System\fP defaults
set by \fBmbdefaults\fP included the format id, a control for
ping averaging, longitude and latitude bounds for windowing
by area, and begin and end times for windowing in time. These
values are no longer set in the .mbio_defaults file or controlled
by \fBmbdefaults\fP.

.SH AUTHORSHIP
David W. Caress (caress@mbari.org)
.br
  Monterey Bay Aquarium Research Institute
.br
Dale N. Chayes (dale@ldeo.columbia.edu)
.br
  Lamont-Doherty Earth Observatory

.SH OPTIONS
.TP
.B \-D
\fIpsdisplay\fP
.br
Sets the program used for displaying Postscript graphics files
on a screen; this facility is used by shellscripts created through
macros like \fBmbm_plot\fP. The default Postscript display
utility is \fBghostview\fP. Other common Postscript display utilities
are \fBpageview\fP (for Sun workstations) and \fBxpsview\fP 
(for Silicon Graphics workstations.
.TP
.B \-H
This "help" flag cause the program to print out a description
of its operation and then exit immediately.
.TP
.B \-I
\fIimagedisplay\fP
.br
Sets the program used for displaying Tiff image files
on a screen; this facility is used by shellscripts created through
macros like \fBmbm_grdtiff\fP. The default image display
utility is \fBxv\fP. Other common image display utilities
are \fBgimp\fP (GNU image processing) and \fBgqview\fP.
.TP
.B \-L
\fIlonflip\fP
.br
Sets the range of the longitude values returned.
If \fIlonflip\fP=-1 then the longitude values will be in
the range from -360 to 0 degrees. If \fIlonflip\fP=0 
then the longitude values will be in
the range from -180 to 180 degrees. If \fIlonflip\fP=1 
then the longitude values will be in
the range from 0 to 360 degrees.
Default: \fIlonflip\fP = 0.
.TP
.B \-T
\fItimegap\fP
.br
Sets the maximum time gap in minutes between adjacent pings allowed before
the data is considered to have a gap. Default: \fItimegap\fP = 1.
.TP
.B \-V
Normally, \fBmbdefaults\fP outputs only a list of the default values 
to the stdout stream.  If the
\fB-V\fP flag is given, then \fBmbdefaults\fP works in a "verbose" mode and
also outputs the program version being used.
.TP
.B \-W
\fIproject\fP
.br
Sets the name of the default project. This will be used by
MB-System database management utilities yet to be created.
Default: \fIproject\fP = "none".

.SH EXAMPLES
Suppose that one wishes the default Postscript viewer to
be xpsview rather than ghostview. 
The following will suffice to create (or rewrite)
a .mbio_defaults file in the users home directory:
	mbdefaults -Dxpsview

The output will be:

 New MBIO Default Control Parameters:
 lonflip:  0
 timegap:  1.000000
 ps viewer:  xspsview
 img viewer: xv
 project:    none

Suppose that one just wishes to see what the current default
parameters are.  The following will suffice:
	mbdefaults

The output will be

 Current MBIO Default Control Parameters:
 lonflip:  0
 timegap:  1.000000
 ps viewer:  ghostview
 img viewer: xv
 project:    none

.SH SEE ALSO
\fBmbsystem\fP(l), \fBmbio\fP(l), \fBmbcontour\fP(l), 
\fBmbswath\fP(l), \fBmbgrdtiff\fP(l), 
\fBmbm_plot\fP(l), \fBmbm_grdplot\fP(l), 
\fBmbm_grd3dplot\fP(l), \fBmbm_xyplot\fP(l), \fBmbm_grdtiff\fP(l)

.SH BUGS
Too simple to have any really interesting bugs.

Well, ok, this program occassionally runs wild, destroying filesystems
and eating young children. But don't worry, it could never
happen to you...