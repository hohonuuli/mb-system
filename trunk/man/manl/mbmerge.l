.TH mbmerge l "13 July 2001" "MB-System 5.0" "MB-System 5.0"
.SH NAME
\fBmbmerge\fP - Merges swath sonar data with new navigation.

.SH VERSION
Version 5.0

.SH SYNOPSIS
\fBmbmerge\fP [\fB-A\fIheading_offset\fP \fB-B\fP \fB-F\fIformat 
\fB-L\fIlonflip\fP 
\fB-I\fIinfile \fB-O\fIoutfile \fB-M\fInavformat\fP 
\fB-N\fInavfile\fP \fB-Z -V -H\fP]

.SH DESCRIPTION
\fBmbmerge\fP is a utility for merging navigation with swath sonar
echosounder data.  The input navigation can be in one of several
formats, including the L-DEO shipboard processing format. 

Many data formats include the navigation information in the
same data records as the ping or survey data. In this case,
\fBmbmerge\fP replaces the position values in the file
by interpolation of the values in the navigation file.

Some data formats use asynchronous navigation, which means that
the navigation is contained in data records separate from the
data records containing sonar ping data. The time stamps and
frequency of the navigation records are in general different
from those of the ping data records. Examples include all
of the Simrad vendor data formats and all of the "UNB" formats
for Reson SeaBat and Elac Bottomchart sonars. For data formats
with asynchronous navigation, \fBMB-System\fP programs calculate
the positions of ping data by interpolation and/or 
extrapolation of the navigation data. One frequently encountered
problem is having the first ping record in a file occur
before the first navigation record so that the position of
that first record is unknown to \fBMB-System\fP programs. 
In the case of asynchronous navigation, \fBmbmerge\fP replaces
the position values in the navigation records. In the special
case of a ping record before any navigation records, \fBmbmerge\fP
inserts a new navigation record just before the first ping
record to eliminate the unknown position problem.

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
.B \-A
\fIheading_offset\fP
.br
Adds this value to all heading values. This option
is useful for correcting simple, constant deviations
in heading values.
.TP
.B \-B
This option causes the navigation to be interpolated using a simple
linear interpolation. Normally, a spline interpolation is
used. Linear interpolation may provide better results when
the navigation is sparse relative to the swath data or when
the navigation contains significant gaps.
Default: spline interpolation.
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
.B \-L
\fIlonflip\fP
.br
Sets the range of the longitude values used.
If \fIlonflip\fP=-1 then the longitude values will be in
the range from -360 to 0 degrees. If \fIlonflip\fP=0 
then the longitude values will be in
the range from -180 to 180 degrees. If \fIlonflip\fP=1 
then the longitude values will be in
the range from 0 to 360 degrees.
Default: \fIlonflip\fP = 0.
.TP
.B \-M
\fInavformat\fP
.br
Defines the format of the navigation data to be read.  Currently,
eight formats are allowed:

\fInavformat\fP = 1:  
.br
This format consists of ascii lines of the form:
 	time_d lon lat
.br
where time_d is decimal unix time in seconds since
1/1/1970 00:00:00.000 and lon and lat are in decimal degrees.
This form can be obtained from swath sonar data files using \fBmblist\fP
with the \fB-O\fP\fIMXY\fP option. Some representative lines are:
 5440574.017	-129.505875	 46.445255
 5440574.100	-129.506045	 46.445083
 5440574.183	-129.506208	 46.445083
 5440574.267	-129.506542	 46.444913

\fInavformat\fP = 2:  
.br
This format consists of ascii lines of the form:
 	year mon day hour min sec lon lat
.br
where lon and lat are in decimal degrees.
This form can be obtained from swath sonar data files using \fBmblist\fP
with the \fB-O\fP\fItXY\fP option. Some representative lines are:
 1981 05 06 04 14 01	-129.505875	 46.445255
 1981 05 06 04 14 06	-129.506045	 46.445083
 1981 05 06 04 14 11	-129.506208	 46.445083
 1981 05 06 04 14 16	-129.506542	 46.444913

\fInavformat\fP = 3:  
.br
This format consists of ascii lines of the form:
 	year jday hour min sec lon lat
.br
where jday is the julian day of the year and 
lon and lat are in decimal degrees.
This form can be obtained from swath sonar data files using \fBmblist\fP
with the \fB-O\fP\fIJXY\fP option. Some representative lines are:
 1981 126 04 14 01	-129.505875	 46.445255
 1981 126 04 14 06	-129.506045	 46.445083
 1981 126 04 14 11	-129.506208	 46.445083
 1981 126 04 14 16	-129.506542	 46.444913

\fInavformat\fP = 4:  
.br
This format consists of ascii lines of the form:
 	year jday daymin sec lon lat
.br
where jday is the julian day of the year, daymin is the minute
of the day, and lon and lat are in decimal degrees.
This form can be obtained from swath sonar data files using \fBmblist\fP
with the \fB-O\fP\fIjXY\fP option. Some representative lines are:
 1981 126 0254 01	-129.505875	 46.445255
 1981 126 0254 06	-129.506045	 46.445083
 1981 126 0254 11	-129.506208	 46.445083
 1981 126 0254 16	-129.506542	 46.444913

\fInavformat\fP = 5:  
.br
This is the L-DEO shipboard processing navigation format, which consists of
ascii records.  Some representative lines of the format are:
 91+300:21:00:00.000 S 31 56.5282 E 179 21.3922   gp1  25.7   0.4
 91+300:21:01:00.000 S 31 56.5746 E 179 21.5991   gp1  25.7   0.4
 91+300:21:02:00.000 S 31 56.6215 E 179 21.8051   gp1  25.7   0.4
 91+300:21:03:00.000 S 31 56.6679 E 179 22.0111   gp1  25.7   0.4
 |year               |north or south              |nav source 
   |julian day         |latitude degrees               |quality1
        |hour             |latitude minutes                   |quality2
           |minute                |east or west
              |second               |longitude degrees
                                        |longitude minutes

\fInavformat\fP = 6 or 7:  
.br
These two formats are subsets of the NMEA 0183 marine interface standard. 
The raw data stream includes ASCII "sentences"
beginning with the '$' character and ending with <CR><LF>
characters; the sentences recognized by \fBmbmerge\fP include
time (starting with "$--ZDA" or "$--UNX" where the "--" can
be any two characters) and navigation (starting with 
"$--GLL" or "$--GGA").
The time tags for each navigation point are taken from the last 
ZDA or UNX sentence read prior to the GLL or GGA sentence.
The GLL sentences used when \fInavformat\fP = 6 record 
generic position data, and the GGA sentences used when 
\fInavformat\fP = 7 record Global Positioning System (GPS)
fixes.
The sentence definitions are:
 
ZDA (Time and Date):
    $--ZDA,hhmmss.ss,dd,mm,yyyy<CR><LF>
           |     |   |  |  |
           |     |   |  |  +- Year
           |     |   |  +---- Month
           |     |   +------- Day
           |     +----------- Optional fractional 
           |                  seconds 
           +----------------- Hour, minute, and 
                              second
 
UNX (Nonstandard Time and Date derived from Unix host):
    $--UNX,hhmmss.ss,dd,mm,yyyy<CR><LF>
           |     |   |  |  |
           |     |   |  |  +- Year
           |     |   |  +---- Month
           |     |   +------- Day
           |     +----------- Optional fractional 
           |                  seconds 
           +----------------- Hour, minute, and 
                              second
 
GLL (Generic Position):
    $--GLL,ddmm.mm,N,dddmm.mm,E,hhmmss.ss,AXX<CR><LF>
           | |     | |  |     | |
           | |     | |  |     | +- UTC time as 
           | |     | |  |     |    in ZDA sentence
           | |     | |  |     +- E/W East or West
           | |     | |  +- Longitude decimal minutes
           | |     | +- Longitude degrees
           | |     +- N/S North or South
           | +- Latitude decimal minutes 
           +- Latitude degrees
 
GGA (GPS Position):
    $--GLL,hhmmss.ss,ddmm.mm,N,dddmm.mm,E,.....<CR><LF>
           |         | |     | |  |     |
           |         | |     | |  |     +- E/W East
           |         | |     | |  |        or West
           |         | |     | |  +- Longitude decimal 
           |         | |     | |     minutes
           |         | |     | +- Longitude degrees
           |         | |     +- N/S North or South
           |         | +- Latitude decimal minutes
           |         +- Latitude degrees 
           +- UTC time as in ZDA sentence

\fInavformat\fP = 8:  
.br
This is the Simrad 90 Position datagram format. This
type of navigation typically derives from the *.pos
files created by the Simrad Mermaid or Merlin data
logging packages (when run in the parallel file mode).
 
 Default: \fInavformat\fP = 5.
.TP
.B \-N
\fInavfile\fP
.br
Data file from which the navigation data will be read.
.TP
.B \-O
\fIoutfile\fP
.br
Data file to which the output data will be written. If
no output file is specified, the output will be written to
stdout. Default: \fIoutfile\fP = stdout.
.TP
.B \-V
Normally, \fBmbmerge\fP works "silently" without outputting
anything to the stderr stream.  If the
\fB-V\fP flag is given, then \fBmbmerge\fP works in a "verbose" mode and
outputs the program version being used and all error status messages.
.TP
.B \-Z
This option causes the heading values to be set to 
"course-made-good" as calculated from the navigation. This
option is useful when the original heading values are
irretrievably bad.

.SH EXAMPLES
Suppose one wishes to merge the good navigation in file navhs with the
raw Hydrosweep data (format 21) file hsraw, which currently 
contains real-time navigation.
The following will suffice:
 	mbmerge -F21 -Nnavhs -Ihsraw -Ohsgood

.SH SEE ALSO
\fBmbsystem\fP(l), \fBmblist\fP(l)

.SH BUGS
You tell me.