.TH mbedit l "10 November 2001" "MB-System 5.0" "MB-System 5.0"
.SH NAME
\fBmbedit\fP - Interactive editor used to flag bad bathymetry values in swath sonar bathymetry data.

.SH VERSION
Version 5.0

.SH SYNOPSIS
\fBmbedit\fP [\fB-B\fIyr/mo/da/hr/mn/sc\fP 
\fB-D\fP \fB-E\fIyr/mo/da/hr/mn/sc\fP 
\fB-F\fIformat\fP \fB -G -I\fIinfile\fP \fB-S -X -V -H\fP]

.SH DESCRIPTION
\fBMBedit\fP is an interactive editor 
used to identify and flag artifacts in swath sonar 
bathymetry data. Once a file has been
read in, \fBMBedit\fP displays the bathymetry profiles
from several pings, allowing the user to identify and flag anomalous
beams. In extreme circumstances,
entire pings may be nulled (see "KEYBOARD ACTIONS"). 

In the default mode the edit events are output to 
an "edit save" file. The program can also be operated 
in a "browse" mode where no edit events are output.
If saved, the edit events can be applied 
to the data using the program \fBmbprocess\fP, which
outputs a processed swath data file. 
The \fBmbprocess\fP program is also used to merge
edited navigation, recalculate bathymetry, and apply
other corrections to swath bathymetry data. Users
may cause \fBmbprocess\fP to be run automatically by
specifying the \fB-X\fP option when starting \fBmbedit\fP.

In previous  versions of \fBMB-System\fP 
(version 4.6.10 or earlier), \fBmbedit\fP
produced output swath data files directly by reading and
storing all of the swath data information in memory (including
non-survey data records). This approach limited the 
amount of data that could be handled
at once, frequently requiring users to process files in
small pieces. The current version only stores the bathymetry
data and only outputs changes to the beam flag status
(good data or bad data) of bathymetry soundings.
The previous form of \fBmbedit\fP is now distributed
as \fBmbeditold\fP. The old version will continue to
be supported for \fBMB-System\fP version 5.0. However,
no improvements will be made to the old program, and
it may be dropped from later \fBMB-System\fP distributions.

The \fBmbedit\fP display consists of several 
bathymetry profiles plotted within
a box with annotation showing the scaling of the x 
(acrosstrack distance) and y (depth) axes. The number of pings displayed,
and the width, vertical exageration, and annotation of the plot 
are all set by the user. Unflagged bathymetry points
are shown as small filled black squares, and flagged bathymtetry points
are shown as small unfilled red or blue squares. Soundings 
flagged manually are shown in red, and those flagged by filter
algorithms are shown in green. When a sounding is selected in the
info mode, it is displayed as a large, filled, blue square.
The unflagged or good bathymetry
points are connected by black line segments to show the acrosstrack
bathymetry profiles for each ping. If the \fBShow Flagged Profile\fP
toggle is set on, then red line segments connect the flagged depth
values in the acrosstrack bathymetry profile.
Each of the displayed pings has a label giving the record number
in the data file, the ping time, and the center beam depth. 

The editing is driven by the left mouse button and 
involves four basic edit modes and one information mode. In toggle
mode each mouse pick toggles the nearest bathymetry point between
flagged and unflagged states. In pick mode each mouse pick flags
the nearest bathymetry point. In erase mode the left mouse button
is held down as the cursor is dragged over the data; all bathymetry
points touched by the mouse are flagged. Restore mode is just like
erase mode, except that the affected bathymetry values are unflagged.
Finally, info mode acts like pick mode, except that instead of flagging
the nearest sounding, the program displays information about the
selected ping and sounding above the bathymetry profile box.
A few keyboard macros described below add additional flexibility
to the editing process.

\fBMBedit\fP has some limited automatic flagging and unflagging
options. The \fBUnflag View\fP button allows users to unflag all of
the currently visible soundings, and the \fBUnflag Forward\fP
button unflags all soundings from the current position forward
to the end of the data held in memory. The \fBControls\fP menu
includes a \fBFilters...\fP button that brings up a dialog
which sets the automatic flagging functions. At present, these
include a median filter algorithm, a "wrong side filter", "data 
cutting" by beam number, and "data cutting" by acrosstrack
distance. The "wrong side" filter
flags soundings positioned on the "wrong" side of the
swath (port soundings incorrectly located on the starboard side,
or vice versa). The two "data cutting" algorithms flag
beams inside or outside of specified beam number ranges
or acrosstrack distance ranges, respectively. 
Additional automatic filters will be added
in later versions.

If the ping includes flagged depths outside the plotting box, the ping label
is underlain by a green box. If
the ping includes unflagged depths outside the plotting box, the label
is underlain by a red box, and a small black box appears to the left
of the label. These colored labels help users identify
bad bathymetry points which lie outside the box defined by the current
vertical exageration and plot width values. In the case where unflagged
depths lie outside the plotting box (red label), clicking on the small
black box to the left of the ping label automatically flags all of the
depth values outside the plotting box. Alternatively, 
users can decrease the
vertical exageration and/or increase the plot width to bring the
offending bathymetry points into view. Users should be aware that
extreme bathymetric slopes or the use of high vertical exagerations
may cause good depth values to lie outside the plotting box for
the first and last pings in view.

The middle and right mouse buttons allow the user to step forward
and backward, respectively, through the data file. The \fBForward\fP
and \fBReverse\fP buttons provide a duplicate stepping capability. 

\fBMBedit\fP can hold up to 25000 swath bathymetry 
profiles at a time in memory. In the case that
a data file contains more survey data records than can be held in memory,
users will find it necessary to step through multiple buffers of
data. The \fBNext Buffer\fP button will cause \fBMBedit\fP to
dump the current buffer contents and read in a new set of data.
The handling of buffered data is set 
using the \fBBuffer Controls\fP dialog accessed by pulling down the
\fBBuffer Controls...\fP menu item from the \fBControls\fP button.
The \fBBuffer Controls\fP dialog includes two slider controls, one
entitled \fBData Buffer Size\fP and the other \fBBuffer Retain Size\fP.
Users on memory limited machines may find it necessary to set the
maximum buffer size to a smaller number using the
\fBData Buffer Size\fP slider.  The \fBBuffer Retain Size\fP 
slider sets the number of data records
retained from the old buffer when more data are loaded.

The user can bring up a \fBGo To Specified Time\fP dialog
by pulling down the \fBGo to a specified time...\fP menu item from the
\fBControls\fP button. This dialog
allows the user to specify the time of a particular ping to be viewed.
The first ping with a time tag later than or equal to the specified time
is then displayed, providing such a ping is available. If an appropriate
ping is not available in the current buffer, \fBMBedit\fP will dump
and load buffers of data until such a ping is found or the end of the
file is reached. Thus, specifying an incorrect "go to" time may cause
\fBMBedit\fP to close the file. Caution is advised in using this feature.

\fBMBedit\fP creates "edit save" files containing a list of each
edit command executed during an editing session. These files are
given names consisting of the input filename appended with ".esf".
The program also modifies the \fBmbprocess\fP parameter file
for the input file by specifying the edit save file and setting
the \fBmbprocess\fP mode to apply the edits when generating a
processed swath output file. If no parameter file named "ifile.par"
exists, \fBmbedit\fP creates one.

If a user seeks to open a swath sonar data 
file and an associated edit save file already exists, the user is
given an option to apply the saved edits to the data as it is loaded.
If the user chooses not to apply the saved edits, those edits will
be lost as a new edit save file is created.
If (as is usually the case) the saved edits are to be 
applied, \fBMBedit\fP copies the edit
save file to a file named "mbedit_tmp.esf" and reads the saved
edits from that second file. In this way, the use of 
edit save files and \fBmbprocess\fP allows users
to edit a swath data file multiple times without generating
multiple output files. Subsequent editing sessions serve to
update the processed data rather than generate additional
edited data files. 

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
.B \-B
\fIyr/mo/da/hr/mn/sc\fP
.br
Sets the starting time for data allowed in the input data; pings
with times before the starting time will be ignored. 
Default: \fIyr/mo/da/hr/mn/sc\fP = 1962/2/21/10/30/0.
.TP
.B \-D
Starts up the program in "browse" mode. If a file is opened
in browse mode (either at startup or later), none
of the edited data will be output to a file. The default
is to output the edited data to a file.
.TP
.B \-E
\fIyr/mo/da/hr/mn/sc\fP
.br
Sets the ending time for data allowed in the input data; pings
with times after the ending time will be ignored. 
Default: \fIyr/mo/da/hr/mn/sc\fP = 2062/2/21/10/30/0.
.TP
.B \-F
\fIformat\fP
.br
Sets the format at startup for the input and output swath sonar data using 
\fBMBIO\fP integer format identifiers. This value can also be set
interactively when specifying the input file.
This program uses the \fBMBIO\fP library and will read any swath sonar
format supported by \fBMBIO\fP. A list of the swath sonar data formats
currently supported by \fBMBIO\fP and their identifier values
is given in the \fBMBIO\fP manual page.
Default: \fIformat\fP = 11.
.TP
.B \-G
This flag causes the program to treat the \fBDone\fP button as
equivalent to the \fBQuit\fP button. This option is used when
\fBMBedit\fP is started automatically by some other process and
only a single file is to edited.
.TP
.B \-H
This "help" flag cause the program to print out a description
of its operation and then exit immediately.
.TP
.B \-I
\fIinfile\fP
.br
Sets the data file from which the input data will be read at startup.
This option is usually used only when \fBMBedit\fP is started
automatically from some other process. The \fB-F\fP option should
also be used to set the data format id. If the \fB-B\fP option is
not used to specify browse mode, then the edit save output file is
automatically set as \fIinfile\fP with ".esf" appended.
.TP
.B \-S
This flag modifies how the program handles an input data file
read at startup, as specified with the \fB-I\fP option. If the
\fB-S\fP flag is given and an edit save file exists for the
startup input data file, the edit save file will be used. 
.TP
.B \-X
This option causes \fBmbprocess\fP to run automatically
on an input swath data file when the \fBmbedit\fP editing session
is completed. The \fB-X\fP option effectively means that the
corresponding processed swath data is generated or updated immediately.
The program \fBmbprocess\fP will not be invoked if the
editing session is in the browse mode.
.TP
.B \-V
Normally, \fBMBedit\fP outputs information to the stderr stream
regarding the number of records loaded and dumped.  If the
\fB-V\fP flag is given, then \fBMBedit\fP works in a "verbose" mode and
outputs the program version being used, all error status messages, 
and a large amount of other information including all of the
beams flagged or zeroed.

.SH INTERACTIVE CONTROLS
.TP
.B File
This button brings up a popup window which allows the user to
specify the input swath sonar bathymetry data file, its \fBMBIO\fP
format id, the output mode, and the output filename. 
This program uses the \fBMBIO\fP library and 
will read or write any swath sonar
format supported by \fBMBIO\fP. A list of the swath sonar data
formats currently supported by \fBMBIO\fP and their
identifier values is given in the \fBMBIO\fP manual page. 
If the swath sonar data file is named using the \fBMB-System\fP
suffix convention (format 11 files end with ".mb11", format
41 files end with ".mb41", etc.), then the program will
automatically use the appropriate format id; otherwise the
format must be set by the user.
The popup window also allows the output mode to be set to "browse"
so that no edit events are output. 
When a valid file is specified and the \fBOK\fP button
is clicked, as much data as will fit
into the data buffer are read and several pings are displayed as 
stacked bathymetry profiles.
.TP
.B Controls
This button accesses a pulldown menu with four items: 
\fBGo To Specified Time...\fP, \fBBuffer Controls...\fP, 
\fBAnnotation...\fP, and \fBAnnotation...\fP. 
Each of these items brings up a dialog of
the same name. These dialogs are discussed below.
.TP
.B Controls->Go To Specified Time...
This menu item brings up a dialog which allows the user to
specify the time of a particular ping to be displayed.  Once the
year, month, day, hour, minute, and second values are entered, clicking
the \fBApply\fP button causes \fBmbedit\fP to seek the specified
target time.  If the current data buffer begins after the target
time, an error is returned.  If the target time is later than the
end of the current data buffer, then \fBmbedit\fP will dump and
load buffers until the target time is reached or the data file ends.
If the end of the file is reached during the search, the file will
be closed.
.TP
.B Controls->Buffer Controls...
This menu item brings up a dialog which allows the user
to set the data buffer handling
through two sliders discussed immediately below.
.TP
.B Controls->Buffer Controls->Data Buffer Size
This slider on the \fBBuffer Controls\fP dialog
sets the number of data records which can be held
in the data buffer.  Any change becomes effective the next time
that a data file is read in.
.TP
.B Controls->Buffer Controls->Buffer Retain Size
This slider on the \fBBuffer Controls\fP dialog
sets the number of data records which are held over in
the buffer each time the old buffer is written out.
.TP
.B Controls->Annotation...
This menu item brings up a dialog which
allows the user to set the annotation intervals for
the across track distance and depth axes through the two sliders
discussed immediately below.
.TP
.B Controls->Annotation->X Axis Tick Interval
This slider on the \fBAnnotation\fP dialog
sets the tick interval in m for the across track
distance axis.
If a particular value is desired which cannot be
obtained by dragging the slider, the slider
can be changed by increments of 1 by clicking with the left button
inside the slider area, but not on the slider itself.
.TP
.B Controls->Annotation->Y Axis Tick Interval
This slider on the \fBAnnotation\fP dialog
sets the tick interval in m for the depth axis.
If a particular value is desired which cannot be
obtained by dragging the slider, the slider
can be changed by increments of 1 by clicking with the left button
inside the slider area, but not on the slider itself.
.TP
.B Controls->Filters
This menu item brings up a dialog which
allows the user to turn automatic bathymetry filtering algorithms
on and off and to set the filter parameters 
through the widgets discussed immediately below.
Clicking on the \fBApply\fP button first removes any previous
automatic filter flags and then applies the current
filter settings to the swath bathymetry data. These actions
are applied only from the current position 
to the end of the data currently
in memory. The filtering is not applied (or unapplied) to
pings earlier in the data file than the current position.
The \fBReset\fP button restores the filter settings that
were applied last.
.TP
.B Controls->Filters->Median Spike Fllter
This toggle button turns automatic median filtering of
bathymetry profiles on and off.
When median filtering is enabled, the median depth is
calculated for each bathymetry ping. Each sounding in the
ping is compared with the median value; any sounding that
differs from the median by a percentage 
greater than a threshold percentage
is flagged as bad data.
.TP
.B Controls->Filters->% Median Depth Threshold
This slider sets the threshold of the median depth
filter in units of percentage of the median depth.
.TP
.B Controls->Filters->Wrong Side Fllter
This toggle button turns automatic wrong side filtering of
bathymetry profiles on and off. This filter expects
bathymetry beams to be numbered from port to 
starboard. Any beam numbered less than the 
center beam with a positive
(starboard) acrosstrack distance is considered to
be on the "wrong side", and any beam numbered higher than
the center beam with a negative (port) acrosstrack distance
is similarly regarded. For most swath data, the center
beam occurs near the midway point in the beam list. This
algorithm ignores apparent wrong side beams close to
the midway beam, where close is defined as being within
a threshold number of the midway beam.
.TP
.B Controls->Filters->Beams from Center Threshold
This slider sets the threshold of beam locations
checked by the wrong side filter. 
.TP
.B Controls->Filters->Flag by Beam Number
This toggle button turns automatic beam flagging by
beam number on and off. The flagging is controlled by
start and end beam number values (set by the two
sliders discussed immediately below). If the start beam
number is less than or equal to the end beam number, then
all beams between and including the start and end are
flagged, producing a flagged zone within the swath. If
the start beam number is greater than the end beam number,
then all beams before and including the end beam are flagged,
and all beams including and after the start beam are flagged.
This produces flagging of both swath edges.
.TP
.B Controls->Filters->Start Flagging Beam Number
This slider sets the beam number at which flagging by beam number starts. 
.TP
.B Controls->Filters->End Flagging Beam Number
This slider sets the beam number at which flagging by beam number ends. 
.TP
.B Controls->Filters->Flag by Distance
This toggle button turns automatic beam flagging by
acrosstrack distance on and off. The flagging is controlled by
start and end distance values (set by the two
sliders discussed immediately below). If the start distance
is less than or equal to the end distance, then
all beams with acrosstrack distances between 
the start and end distances are
flagged, producing a flagged zone within the swath. If
the start distance is greater than the end distance,
then all beams with acrosstrack distance less than 
the end distance are flagged,
and all beams with acrosstrack distance greater than 
the start distance are flagged.
This produces flagging of both swath edges.
.TP
.B Controls->Filters->Start Flagging Distance
This slider sets the beam number at which flagging 
by acrosstrack distance starts. 
.TP
.B Controls->Filters->End Flagging Distance
This slider sets the beam number at which flagging 
by acrosstrack distance ends. 
.TP
.B Next Buffer
This button causes the program to write out the data from the
current buffer and then read in and display the next buffer.
If there is no more data to be read in after the current buffer
has been written out, then the input and output files are closed.
.TP
.B Done
This button causes the program to write out all of the data from the
input file and then close the input and output files.
.TP
.B Forward
This button causes the set of displayed pings to step \fInstep\fP 
pings forward in the current buffer.  The right mouse button causes
the same action.
.TP
.B Reverse
This button causes the set of displayed pings to step \fInstep\fP 
pings backward in the current buffer.  The middle mouse button causes
the same action.
.TP
.B Quit
This button causes the program to exit gracefully.  If a data file
has been read, all of the data will be written to the output file
before exiting.
.TP
.B About
This button causes the program to bring up a dialog showing the
program's name, version, and authors.
.TP
.B Acrosstrack Width
This slider sets the width of the plot in meters; in general this
value should be slightly larger than the swath width of the data
being edited. If a particular value is desired which cannot be
obtained by dragging the slider (e.g., the user wants a plot
width of 10 meters but the slider jumps from 1 to 47), the slider
can be changed by increments of 1 by clicking with the left button
inside the slider area, but not on the slider itself.
.TP
.B Vertical Exageration
This slider sets the depth scale in terms of vertical exageration.
The depth scale will change as the cross track distance scale is
changed to maintain the same vertical exageration.
If a particular value is desired which cannot be
obtained by dragging the slider, the slider
can be changed by increments of 0.01 by clicking with the left button
inside the slider area, but not on the slider itself.
.TP
.B Mode
This toggle allows the user to specify the edit mode.  If mode
is set to \fIToggle\fP, then clicking the left mouse button will cause
the nearest beam to toggle between flagged and unflagged.  If mode
is set to \fIPick\fP, then clicking the left mouse button will cause
the nearest unflagged beam to be flagged.  If mode
is set to \fIErase\fP, then the cursor will change to an erasor and
any beam with the cursor while the left mouse button is held down
will be flagged.  If mode is set to \fIRestore\fP, the behavior will
be the same as for \fIErase\fP except that the affected beams will be
unflagged instead of flagged. Info mode acts like pick mode, 
except that instead of flagging
the nearest sounding, the program displays information about the
selected ping and sounding above the bathymetry profile box. 
The edit mode can also be set using
key macros (see the keyboard action section):  
 	Toggle:		'Q', 'q', 'U', 'u' 
 	Pick:		'W', 'w', 'I', 'i' 
 	Erase:		'E', 'e', 'O', 'o' 
 	Restore:	'R', 'r', 'P', 'p' 
 	Info:		'T', 't', '{', '[' 
.TP
.B Unflag View
This button unflags all flagged beams among the currently
displayed pings. Pings in the buffer before or after the current 
display are unaffected.
.TP
.B Unflag Forward
This button  unflags all flagged beams among all pings from
the start of the current display to the end of the current
data buffer. Pings before the start of the current display are
unaffected.
.TP
.B Number of pings shown
This slider sets the number of pings shown at a time.
.TP
.B Number of pings to step
This slider sets the number of pings to step when the \fBForward\fP
or \fBReverse\fP buttons are pushed.
.TP
.B Show Flagged Profile
This toggle allows the user to specify whether the acrosstrack
bathymetry profile includes only the unflagged or "good" bathymetry
(toggle set to "On") or
whether the profile also includes the flagged or "bad" bathymetry
(toggle set to "Off"). In
the latter case, red line segments show the portion of the profile
associated with the flagged depth points.

.SH MOUSE ACTIONS
.TP
.B Left Mouse Button
The left mouse button is used to pick beams.  Good beams are
shown as filled black squares and bad (flagged) beams as unfilled red
or green squares.  The 
result of picking a particular beam depends on the current edit mode,
as set by the \fBMode\fP button or keyboard macros defined below. The
last picked beam (and ping) is remembered for use with 
some of the keyboard actions described below.
.TP
.B Middle Mouse Button
The middle mouse button causes the set of displayed pings to step \fInstep\fP 
pings backward in the current buffer.  The control button \fBReverse\fP causes
the same action.
.TP
.B Right Mouse Button
The right mouse button causes the set of displayed pings to step \fInstep\fP 
pings forward in the current buffer.  The control button \fBForward\fP causes
the same action.

.SH KEYBOARD ACTIONS
.TP
.B 'Z', 'z', 'M', or 'm'
.B Bad Ping:
Pressing one of these keys causes all of the beams in the last picked
ping to be flagged as bad.
.TP
.B 'S', 's', 'K', or 'k'
.B Good Ping:
Pressing one of these keys causes all of the beams in the last picked
ping to be unflagged as good.
.TP
.B 'A', 'a', 'J', or 'j'
.B Left:
Pressing one of these keys causes all of the beams including and to 
the left of the last picked beam to be flagged as bad.
.TP
.B 'D', 'd', 'L', or 'l'
.B Right:
Pressing one of these keys causes all of the beams including and to 
the right of the last picked beam to be flagged as bad.
.TP
.B '!'
.B Zero Ping:
Pressing this key causes all of the beams in the ping 
associated with the last picked beam to be zeroed. This
should be used only for completely ridiculous values, as
the values are not recoverable.
.TP
.B 'Q', 'q', 'U', or 'u'
.B Toggle Mode:
Pressing one of these keys sets the edit mode to "toggle" so that
clicking the left mouse button will cause
the nearest beam to toggle between flagged and unflagged.  
The edit mode can also be set using the \fBMode\fP toggle buttons.
.TP
.B 'W', 'w', 'I', or 'i'
.B Pick Mode:
Pressing one of these keys sets the edit mode to "pick" so that
clicking the left mouse button will cause
the nearest unflagged beam to be flagged.  
The edit mode can also be set using the \fBMode\fP toggle buttons.
.TP
.B 'E', 'e', 'O', or 'o'
.B Erase Mode:
Pressing one of these keys sets the edit mode to "erase" so that
clicking the left mouse button will cause
any beam under the cursor while the left mouse button is held down
to be flagged.
The edit mode can also be set using the \fBMode\fP toggle buttons.
.TP
.B 'R', 'r', 'P', or 'p'
.B Restore Mode:
Pressing one of these keys sets the edit mode to "restore" so that
clicking the left mouse button will cause any beam under the cursor 
while the left mouse button is held down to be unflagged.
The edit mode can also be set using the \fBMode\fP toggle buttons.
.TP
.B 'T', 't', '{', or '['
.B Info Mode:
Pressing one of these keys sets the edit mode to "info" so that
clicking the left mouse button will cause information about the
nearest beam to be displayed above the bathymetry profile box.
The edit mode can also be set using the \fBMode\fP toggle buttons.

.SH SEE ALSO
\fBmbsystem\fP(l), \fBmbprocess\fP(l), \fBmbset\fP(l), 
\fBmbclean\fP(l), \fBmbunclean\fP(l), \fBmbinfo\fP(l)

.SH BUGS
This program is only fun when the data are good. The main
window for \fBmbedit\fP is too large for use on screens
smaller than 1024 X 768 pixels.