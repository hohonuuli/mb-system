.TH mbtide l "13 July 2001" "MB-System 5.0" "MB-System 5.0"
.SH NAME
\fBmbtide\fP - MBTIDE corrects swath sonar bathymetry data for tides.

.SH VERSION
Version 5.0

.SH SYNOPSIS
\fBmbtide\fP [\fB-F\fIformat \fB-I\fIinfile \fB-O\fIoutfile 
\fB-M\fItideformat\fP \fB-T\fItidefile\fP \fB-V -H\fP]

.SH DESCRIPTION
\fBmbtide\fP is a utility for correcting swath bathymetry for
tides.  The input tide data can be in one of several ascii table
formats. A simple spline interpolation of the tide values is
used to calculate the tide correction for each ping. The tide
values are interpreted in meters above zero tide, where zero
tide is also referred to as the "chart datum" on hydrographic
charts. The effect of the tide correction is to make the
bathymetry values correspond to the "chart datum" reference.
The default input and output streams are stdin and stdout.

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
.B \-F
\fIformat\fP
.br
Sets the format for the input and output swath sonar data using 
\fBMBIO\fP integer format identifiers. 
This program uses the \fBMBIO\fP library and will read or write any swath sonar
format supported by \fBMBIO\fP. A list of the swath sonar data formats
currently supported by \fBMBIO\fP and their identifier values
is given in the \fBMBIO\fP manual page.
Default: \fIformat\fP = 11.
.TP
.B \-H
This "help" flag causes the program to print out a description
of its operation and then exit immediately.
.TP
.B \-I
\fIinfile\fP
.br
Data file from which the input data will be read. If
no input file is specified, the input will be read
from stdin. Default: \fIinfile\fP = stdin.
.TP
.B \-M
\fItideformat\fP
.br
Defines the format of the tide data to be read.  Currently,
four formats are allowed, all consisting of ascii records:

\fItideformat\fP = 1:  
.br
This format consists for lines of the form:
 	time_d tide
.br
where time_d is decimal unix time in seconds since
1/1/1970 00:00:00.000 and tide values are in meters
above zero tide.
Some representative lines are:
 804164520.0	1.25
 804164940.0	2.25
 804165660.0	3.25
 804166800.0	4.00

\fItideformat\fP = 2:  
.br
This format consists for lines of the form:
 	year mon day hour min sec tide
.br
where tide is in meters above zero tide.
Some representative lines are:
 1995 06 26 11 04 00 2.91
 1995 06 26 11 11 00 2.98
 1995 06 26 11 23 00 3.21
 1995 06 26 11 42 00 3.59

\fItideformat\fP = 3:  
.br
This format consists for lines of the form:
 	year jday hour min sec tide
.br
where jday is the julian day of the year and 
tide is in meters above zero tide.
Some representative lines are:
 1995 177 11 04 00 2.91
 1995 177 11 11 00 2.98
 1995 177 11 23 00 3.21
 1995 177 11 42 00 3.59

\fItideformat\fP = 4:  
.br
This format consists for lines of the form:
 	year jday daymin sec tide
.br
where jday is the julian day of the year, daymin is the minute
of the day, and tide is in meters above zero tide.
Some representative lines are:
 1995 177 664 00 2.91
 1995 177 671 00 2.98
 1995 177 683 00 3.21
 1995 177 702 00 3.59

Default: \fItideformat\fP = 2.
.TP
.B \-O
\fIoutfile\fP
.br
Data file to which the output data will be written. If
no output file is specified, the output will be written to
stdout. Default: \fIoutfile\fP = stdout.
.TP
.B \-N
\fItidefile\fP
.br
Data file from which the tide data will be read.
.TP
.B \-V
Normally, \fBmbtide\fP works "silently" without outputting
anything to the stderr stream.  If the
\fB-V\fP flag is given, then \fBmbtide\fP works in a "verbose" mode and
outputs the program version being used and all error status messages.

.SH EXAMPLES
Suppose one wishes to correct the bathymetry in the Reson SeaBat 9002
data file ref2001.mb81 (format 81) for the tide variations 
contained in the file tide_950626.dat. The following will
suffice:
 	mbtide -F81 -M2 -Ttide_950626.dat 
 		-Iref2001.mb81 -Oref2001_t.mb81

.SH BUGS
The wide variety of data formats makes the application of
tides in a uniform manner problematic.
Bathymetry data collected in intertidal zones can end up
negative (above zero tide or chart datum) after correction
for tides. Many formats support flagging of bad bathymetry
only with negative depth values. Others actually have 
separate tidal correction values have to be applied to
the bathymetry as the data is read. We have yet to
develop a completely satisfactory scheme, and it is
likely that the functionality of the program will change.
Please let us know if you find any other problems.