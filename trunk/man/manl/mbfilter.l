.TH mbfilter l "9 August 2001" "MB-System 5.0" "MB-System 5.0"
.SH NAME
\fBmbfilter\fP - Apply some simple filter functions to sidescan, beam
amplitude, or bathymetry data from swath sonar data files.

.SH VERSION
Version 5.0

.SH SYNOPSIS
\fBmbfilter\fP [\fB-A\fIkind\fP 
\fB-B\fIyr/mo/da/hr/mn/sc 
\fB-C\fImode/xdim/ldim/iteration\fP 
\fB-D\fImode/xdim/ldim/iteration/offset\fP 
\fB-E\fIyr/mo/da/hr/mn/sc 
\fB-F\fIformat\fP \fB-I\fIinfile\fP \fB-N\fIbuffersize\fP
\fB-O\fIoutfile\fP \fB-R\fIwest/east/south/north\fP 
\fB-S\fImode/xdim/ldim/iteration\fP 
\fB-T\fIthreshold_lo/threshold_hi\fP
\fB-V -H\fP]

.SH DESCRIPTION
\fBmbfilter\fP applies one or more simple filters to the specified
swath data (sidescan, beam amplitude, and/or bathymetry). The filters
include:
  - boxcar mean for lo-pass filtering
  - gaussian mean for lo-pass filtering
  - boxcar median for lo-pass filtering
  - inverse gradient for lo-pass filtering
  - boxcar mean subtraction for hi-pass filtering
  - gaussian mean subtraction for hi-pass filtering
  - boxcar median subtraction for hi-pass filtering
  - edge detection for contrast enhancement
  - gradient magnitude subtraction for contrast enhancement

These filters are mostly intended for use with sidescan
data. In particular, the lo-pass or smoothing filters
can be used for first-order speckle reduction in sidescan
data, and the hi-pass filters can be used to emphasize
fine scale structure in the data. A combination of
lo-pass and hi-pass filtering can effectively perform
band-pass filtering. The contrast enhancing
filters can, under certain circumstances, sharpen sidescan
images of the seafloor. The lo-pass and contrast
enhancement filters are described
in the paper by Sauter and Parson (1994) listed below.

The filtering used here is designed and applied in the
same manner as spatial filters are applied to images.
The swath data is treated as an image, with the x and
y coordinates corresponding to pixel (or beam) number and
ping number, respectively. The filters consist of M x N
matrices which are convolved with the swath data image
(M is the filter dimension in the acrosstrack direction
and N is the filter dimension in the alongtrack direction).
Missing or flagged data are ignored in the filtering
process.

Users can apply up to three filters at a time (one each of
the lo-pass, hi-pass, and contrast enhancement filters).
If multiple filters are applied, the hi-pass will be
applied first, followed by the lo-pass, followed
finally by the contrast enhancement filter.
Users can apply as many iterations of each filter as
desired.

The user specifies the window size
used by the filters (3x3 or 5x5 are typical for using smoothing
filters iteratively to reduce speckle; larger windows like 3x20
are typical for hi-pass filtering). 

The boxcar mean, gaussian mean, and median filter all
achieve similar results in uniformly smoothing the data; 
the boxcar mean is faster to apply, the gaussian mean preserves
the frequency content of the data better, and the median filter
is least sensitive to spikes in the data. The inverse 
gradient filter applies averaging weights which depend on the
inverse gradient of the data. This approach causes the filter
to smooth regions without distinct edges much more than regions
with edges, thus tending to preserve the sharpness of features
more than the simpler smoothing filters. 

The median smoothing filter can be set by the \fB-T\fP
option to operate with low and high
ratio thresholds (the value is changed only if the original
value divided by the median value is less than the low
threshold or greater than the high threshold). This allows the
filter to preferentially despike the data. In particular, this
approach is useful for suppressing "stripes" or "bad" 
pings which have amplitude or sidescan values differing significantly
from surrounding pings.

The hi-pass filters are constructed by calculating a low-pass
filtered version of the data and then subtracting that from
the original data. An offset value is added to the hi-passed
data so that it is positive (negative values are considered
flagged as bad in some formats and not allowed in others). 
The hi-pass filters can be used to remove
long-wavelength variations in seafloor reflectivity in order
to emphasize fine-scale structure.

The contrast enhancement filters are generally only successful
when applied after smoothing because of their tendency to
amplify noise. The edge detection filter enhances contrast by
tending to shift values on either side of a boundary away
from the average value across the boundary. A 5 X 5 filter or
larger is generally required for success with the edge detection
algorithm. The gradient filter increases contrast by subtracting
twice the local gradient magnitude from each value.

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
\fIkind\fP
.br
Determines whether bathymetry (\fIkind\fP = 0),
beam amplitude (\fIkind\fP = 1), 
or sidescan (\fIkind\fP = 2) data will be processed. 
Default: \fIkind\fP = 2.
.TP
.B \-B
\fIyr/mo/da/hr/mn/sc\fP
.br
This option sets the starting time for data allowed in the input data.
The \fB-E\fP option sets the ending time for data. If the 
starting time is before the ending time, then any data
with a time stamp before the starting time or after the
ending time is ignored. If instead the starting time is
after the ending time, then any data between the ending
and starting time will be ignored. This scheme allows time
windowing both inside and outside a specified interval.
Default: \fIyr/mo/da/hr/mn/sc\fP = 1962/2/21/10/30/0.
.TP
.B \-C
\fImode/xdim/ldim/iteration\fP
.br
.br
Turns on contrast enhancement filtering and sets the 
filter parameters to be 
used. Here \fImode\fP specifies the filter type:
 	\fImode\fP = 1 : Edge Detection Filter
 	\fImode\fP = 2 : Gradient Magnitude Subtraction Filter
.br
The dimensions of the data window used are set using \fIxdim\fP
(acrosstrack dimension) and \fIldim\fP (alongtrack dimension).
Values of \fIxdim\fP = 5 and \fIldim\fP = 5 are typical, but
larger or smaller dimensions can be used. 
The \fIiteration\fP value specifies
the number of times the filter is applied; there is no limit to
this value. 
Default: contrast enhancement off, \fIxdim\fP = 5, \fIldim\fP = 5, 
\fIiteration\fP = 1.
.TP
.B \-D
\fImode/xdim/ldim/iteration/offset\fP
.br
Turns on hi-pass filtering and sets the filter parameters to be 
used. Here \fImode\fP specifies the filter type:
 	\fImode\fP = 1 : Boxcar Mean Subtraction Filter
 	\fImode\fP = 2 : Gaussian Mean Subtraction Filter
 	\fImode\fP = 3 : Boxcar Median Subtraction Filter
.br
The dimensions of the data window used are set using \fIxdim\fP
(acrosstrack dimension) and \fIldim\fP (alongtrack dimension).
Values of \fIxdim\fP = 3 and \fIldim\fP = 3 are typical, but
larger dimensions can be used. The \fIiteration\fP value specifies
the number of times the filter is applied; there is no limit to
this value, but high-pass filters are generally only applied
once. The \fIoffset\fP value is added to each hi-pass filtered
value to force the results to be positive; this value should be 
chosen according to the range of values allowed in the
data type and data formats being used (e.g. for SeaBeam 2100
sidescan as represented in format 41, the sidescan values
can range from 1 to 65535, so an \fIoffset\fP = 1000 is appropriate).
Default: hi-pass filtering off, \fIxdim\fP = 3, \fIldim\fP = 10, 
\fIiteration\fP = 1, \fIoffset\fP = 1000.
.TP
.B \-E
\fIyr/mo/da/hr/mn/sc\fP
.br
This option sets the ending time for data allowed in the input data.
The \fB-B\fP option sets the starting time for data. If the 
starting time is before the ending time, then any data
with a time stamp before the starting time or after the
ending time is ignored. If instead the starting time is
after the ending time, then any data between the ending
and starting time will be ignored. This scheme allows time
windowing both inside and outside a specified interval.
Default: \fIyr/mo/da/hr/mn/sc\fP = 2062/2/21/10/30/0.
.TP
.B \-F
\fIformat\fP
.br
Sets the data format used in reading the input from stdin
or from a file. This program uses the \fBMBIO\fP library 
and will read or write any swath sonar
format supported by \fBMBIO\fP. A list of the swath sonar data formats
currently supported by \fBMBIO\fP and their identifier values
is given in the \fBMBIO\fP manual page. Default: \fIformat\fP = 11.
.TP
.B \-H
This "help" flag causes the program to print out a description
of its operation and then exit immediately.
.TP
.B \-I
\fIinfile\fP
.br
Sets the input swath sonar data filename. 
This program uses the \fBMBIO\fP library and will read or write any swath sonar
format supported by \fBMBIO\fP. A list of the swath sonar data formats
currently supported by \fBMBIO\fP and their identifier values
is given in the \fBMBIO\fP manual page. Default: \fIinfile\fP = "stdin".
.TP
.B \-N
\fIbuffersize\fP
.br
Sets the maximum number of data records which can be
read into the buffer. In general, data records may be
of several different types (e.g. parameter, position, 
comment) in addition to survey data records. Many data 
formats include many more position data records than
survey data records. Thus, a large buffer may be required
to access a reasonable number of survey data records. 
However, on memory limited machines large buffer sizes
can lead to poor performance due to memory swapping. 
The default value of \fIbuffersize\fP = 500 is appropriate 
for most cases, but users can set the buffer size as required.
The absolute maximum buffer size is 5000. 
Default: \fIbuffersize\fP = 500.
.TP
.B \-O
\fIoutfile\fP
.br
Data file to which the output data will be written. The \fBMBIO\fP
format id used is the same as for the input data. If
no output file is specified, the output will be written to
stdout. Default: \fIoutfile\fP = stdout.
.TP
.B \-R
\fIwest/east/south/north\fP
.br
Sets the longitude and latitude bounds within which swath sonar 
data will be read. Only the data which lies within these bounds will
be copied. 
Default: \fIwest\fP=-360, east\fI=360\fP, \fIsouth\fP=-90, \fInorth\fP=90.
.TP
.B \-S
\fImode/xdim/ldim/iteration\fP
.br
Turns on lo-pass smoothing filtering and sets the filter 
parameters to be used to 
smooth the data. Here \fImode\fP specifies the filter type:
 	\fImode\fP = 1 : Boxcar Mean Filter
 	\fImode\fP = 2 : Gaussian Mean Filter
 	\fImode\fP = 3 : Boxcar Median Filter
 	\fImode\fP = 4 : Boxcar Inverse Gradient Filter
.br
The dimensions of the data window used are set using \fIxdim\fP
(acrosstrack dimension) and \fIldim\fP (alongtrack dimension).
Values of \fIxdim\fP = 3 and \fIldim\fP = 3 are typical, but
larger dimensions can be used. The \fIiteration\fP value specifies
the number of times the filter is applied; there is no limit to
this value.
Default: \fImode\fP = 1, \fIxdim\fP = 3, \fIldim\fP = 3, 
\fIiteration\fP = 1.
.TP
.B \-T
\fIthreshold_lo/threshold_hi\fP
.br
This option causes the boxcar median smoothing filter to
operate with low and high
ratio thresholds (the value is changed only if the original
value divided by the median value is less than \fIthreshold_lo\fP
or greater than \fIthreshold_hi\fP). This allows the
filter to preferentially despike the data. In particular, this
approach is useful for suppressing "stripes" or "bad" 
pings which have amplitude or sidescan values differing significantly
from surrounding pings. This option only works with the median
smoothing filter.
.TP
.B \-V
Normally, \fBmbfilter\fP works "silently" without outputting
anything to the stderr stream.  If the
\fB-V\fP flag is given, then \fBmbfilter\fP works in a "verbose" 
mode and outputs the program version being used, the values
of some important control parameters, and 
all error status messages.

.SH EXAMPLES
Suppose one has a SeaBeam 2100 data file called test.mb41
which contains bathymetry (121 beams in a 120 degree swath), 
beam amplitude (121 beams coincident with bathymetry),
and sidescan data (2000 pixels, roughly a 150 degree swath)
which has been corrected for the amplitude vs grazing angle
variation using the program \fBmbanglecorrect\fP.
Plots of the corrected sidescan often show a large amount
of speckle that was suppressed in plots of the raw data
by the large contrast between the specular and non-specular
regions of the swath. To reduce the speckle and make coherent
features of the data clearer, the user can apply any of
the smoothing filters available in \fBmbfilter\fP. One iteration
of the boxcar mean filter can be applied as follows:
 	mbfilter -F41 -Itest.mb41 -Otest_mean.mb41 \
 		-S1/3/3/1
.br
Five iterations of the inverse gradient filter can be applied as follows:
 	mbfilter -F41 -Itest.mb41 -Otest_igrad.mb41 \
 		-S4/3/3/5
.br
To remove large scale variations in seafloor reflectivity, one can
apply a hi-pass filter to the data:
 	mbfilter -F41 -Itest.mb41 -Otest_hipass.mb41 \
 		-D1/3/10/1/10000
.br
To first apply a hi-pass filter to emphasize fine-scale structure
and then apply a lo-pass filter to reduce speckle:
 	mbfilter -F41 -Itest.mb41 -Otest_hipass.mb41 \
 		-D1/3/10/1/10000 -S4/3/3/5
.br
To first reduce speckle by smoothing the data with Gaussian
mean filter and apply an edge detection contrast enhancement
filter:
 	mbfilter -F41 -Itest.mb41 -Otest_hipass.mb41 \
 		-S3/7/7/1 -C1/5/5/1

.SH SEE ALSO
\fBmbsystem\fP(l), \fBmbbackangle\fP(l), \fBmbanglecorrect\fP(l)

.SH REFERENCES
Sauter, D., and L. Parson, Spatial filtering for speckle reduction,
contrast enchancement, and texture analysis of GLORIA images,
\fIIEEE J. Ocean. Eng.\fP, \fB19\fP, 563-576, 1994.

.SH BUGS
All the filtering in the world won't make bad data good.