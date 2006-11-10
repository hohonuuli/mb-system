eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
                    & eval 'exec perl -S $0 $argv:q'
                         if 0;
#--------------------------------------------------------------------
#    The MB-system:	mbm_histplot.perl	3/20/2006
#    $Id: mbm_histplot.perl,v 5.0 2006-11-10 22:01:56 caress Exp $
#
#    Copyright (c) 2006 by 
#    D. W. Caress (caress@mbari.org)
#      Monterey Bay Aquarium Research Institute
#      Moss Landing, CA
#    and D. N. Chayes (dale@lamont.ldgo.columbia.edu)
#      Lamont-Doherty Earth Observatory
#      Palisades, NY  10964
#
#    See README file for copying and redistribution conditions.
#--------------------------------------------------------------------
#
# Command:
#   mbm_histplot
#
# Purpose:
#   Macro to generate a shellscript of GMT commands which, 
#   when executed, will generate a Postscript histogram plot
#   of a set of data values.
#   The plot will be scaled to fit on the specified page size 
#   or, if the scale is user defined, the page size will be 
#   chosen in accordance with the plot size. The primary purpose 
#   of this macro is to allow the simple, semi-automated
#   production of nice looking plots with a few command line
#   arguments. For users seeking more control over the plot
#   appearance, a number of additional optional arguments are
#   provided. Truly ambitious users may edit the plot shellscript 
#   to take advantage of GMT capabilites not supported by this 
#   macro.
#
# Basic Usage: 
#   mbm_histplot -Ifile [-Btickinfo -Gfill -H  
#            -Jprojection[/scale | width] 
#            -Ltitle -Oroot 
#            -Ppagesize -Uorientation -Q 
#            -Rw/e/s/n -V -Wpen -X]
#
# Author:
#   David W. Caress
#   Monterey Bay Aquarium Research Institute
#   Moss Landing CA
#   March 20, 2006
#
# Version:
#   $Id: mbm_histplot.perl,v 5.0 2006-11-10 22:01:56 caress Exp $
#
# Revisions:
#   $Log: not supported by cvs2svn $
#
#
$program_name = "mbm_histplot";

# use the Posix module
use POSIX;

# set page size database
@page_size_names = (  
	"a", "b", "c", "d", "e", "f", "e1",
	"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "a10", 
	"b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "b10", 
	"c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7");
%page_width_in = (  
	"a",     8.50,   "b",    11.00,   "c",    17.00,   "d",    22.00, 
	"e",    34.00,   "f",    28.00,   "e1",   44.00,   "a0",   33.11,
	"a1",   23.39,   "a2",   16.54,   "a3",   11.69,   "a4",    8.27,
	"a5",    5.83,   "a6",    4.13,   "a7",    2.91,   "a8",    2.05,
	"a9",    1.46,   "a10",   1.02,   "b0",   39.37,   "b1",   27.83,
	"b2",   19.68,   "b3",   13.90,   "b4",    9.84,   "b5",    6.93,
	"b6",    4.92,   "b7",    3.46,   "b8",    2.44,   "b9",    1.73,
	"b10",   1.22,   "c0",   36.00,   "c1",   25.60,   "c2",   18.00,
	"c3",   12.80,   "c4",    9.00,   "c5",    6.40,   "c6",    4.50,
	"c7",    3.20);
%page_height_in = ( 
	"a",    11.00,   "b",    17.00,   "c",    22.00,   "d",    34.00, 
	"e",    44.00,   "f",    40.00,   "e1",   68.00,   "a0",   46.81,
	"a1",   33.11,   "a2",   23.39,   "a3",   16.54,   "a4",   11.69,
	"a5",    8.27,   "a6",    5.83,   "a7",    4.13,   "a8",    2.91,
	"a9",    2.05,   "a10",   1.46,   "b0",   56.67,   "b1",   39.37,
	"b2",   27.83,   "b3",   19.68,   "b4",   13.90,   "b5",    9.84,
	"b6",    6.93,   "b7",    4.92,   "b8",    3.46,   "b9",    2.44,
	"b10",   1.73,   "c0",   51.20,   "c1",   36.00,   "c2",   25.60,
	"c3",   18.00,   "c4",   12.80,   "c5",    9.00,   "c6",    6.40,
	"c7",    4.50);
%page_anot_font = ( 
	"a",     8,   "b",    12,   "c",    16,   "d",    24,
	"e",    32,   "f",    32,   "e1",   32,   "a0",   32,
	"a1",   24,   "a2",   16,   "a3",   12,   "a4",    8,
	"a5",    6,   "a6",    6,   "a7",    6,   "a8",    4,
	"a9",    4,   "a10",   4,   "b0",   32,   "b1",   24,
	"b2",   16,   "b3",   16,   "b4",   12,   "b5",    8,
	"b6",    6,   "b7",    4,   "b8",    4,   "b9",    4,
	"b10",   4,   "c0",   32,   "c1",   24,   "c2",   16,
	"c3",   12,   "c4",    8,   "c5",    6,   "c6",    6,
	"c7",    6);
%page_header_font =(
	"a",    10,   "b",    15,   "c",    20,   "d",    30,
	"e",    40,   "f",    40,   "e1",   40,   "a0",   40,
	"a1",   30,   "a2",   20,   "a3",   15,   "a4",   10,
	"a5",    8,   "a6",    8,   "a7",    8,   "a8",    5,
	"a9",    5,   "a10",   5,   "b0",   40,   "b1",   30,
	"b2",   20,   "b3",   20,   "b4",   15,   "b5",   10,
	"b6",    8,   "b7",    5,   "b8",    5,   "b9",    5,
	"b10",   5,   "c0",   40,   "c1",   30,   "c2",   20,
	"c3",   15,   "c4",   10,   "c5",    8,   "c6",    8,
	"c7",    8);
%page_gmt_name =     (
	"a",     "archA",   "b",     "archB",   "c",     "archC",   "d",     "archD", 
	"e",     "archE",   "f",     "archE",   "e1",    "archE",   "a0",    "A0",
	"a1",    "A1",      "a2",    "A2",      "a3",    "A3",      "a4",    "A4",
	"a5",    "A5",      "a6",    "A6",      "a7",    "A7",      "a8",    "A8",
	"a9",    "A9",      "a10",   "A10",     "b0",    "B0",      "b1",    "B1",
	"b2",    "B2",      "b3",    "B3",      "b4",    "B4",      "b5",    "B5",
	"b6",    "A6",      "b7",    "A7",      "b8",    "A8",      "b9",    "A9",
	"b10",   "A10",     "c0",    "B0",      "c1",    "B1",      "c2",    "B2",
	"c3",    "B3",      "c4",    "B4",      "c5",    "B5",      "c6",    "B6",
	"c7",    "B7");
%xpsview_mem =     (
	"a",     "4m",   "b",     "6m",   "c",     "8m",   "d",    "12m", 
	"e",    "16m",   "f",    "16m",   "e1",   "16m",   "a0",   "16m",
	"a1",   "12m",   "a2",    "8m",   "a3",    "6m",   "a4",    "4m",
	"a5",    "4m",   "a6",    "4m",   "a7",    "4m",   "a8",    "4m",
	"a9",    "4m",   "a10",   "4m",   "b0",   "16m",   "b1",   "12m",
	"b2",    "8m",   "b3",    "8m",   "b4",    "6m",   "b5",    "4m",
	"b6",    "4m",   "b7",    "4m",   "b8",    "4m",   "b9",    "4m",
	"b10",   "4m",   "c0",   "16m",   "c1",   "12m",   "c2",    "8m",
	"c3",    "6m",   "c4",    "4m",   "c5",    "4m",   "c6",    "4m",
	"c7",    "4m");

# Determine the GMT version
@grdinfo = `grdinfo 2>&1`;
while (@grdinfo)
	{
	$line = shift @grdinfo;
	if ($line =~ 
		/^grdinfo\s+(\S+)\s+\S+/)
		{
		($gmt_version) = $line =~ 
			/^grdinfo\s+(\S+)\s+\S+/;
		}
	}

# Get the current working directory.
$current_working_dir = `pwd`;

# Deal with command line arguments
$command_line = "@ARGV";
&MBGetopts('B:b:C:c:G:g:HhI:i:J:j:L:l:O:o:P:p:QqR:r:U:u:VvW:w:Xx:');
$tick_info = 		($opt_B || $opt_b);
$cellwidth = 		($opt_C || $opt_c);
$fill = 		($opt_G || $opt_g || "gray");
$help =    		($opt_H || $opt_h);
$file_data =    	($opt_I || $opt_i);
$map_scale =    	($opt_J || $opt_j || "x");
$labels =    		($opt_L || $opt_l);
$root =    		($opt_O || $opt_o);
$pagesize = 		($opt_P || $opt_p);
$no_view_ps = 		($opt_Q || $opt_q);
$bounds = 		($opt_R || $opt_r);
$orientation = 		($opt_U || $opt_u);
$verbose = 		($opt_V || $opt_v);
$pen = 			($opt_W || $opt_w || "1p");
$execute = 		($opt_X || $opt_x);

# print out help message if required
if ($help)
	{
	print "\n$program_name:\n";
	print "\nMacro to generate a shellscript of GMT commands which, \n";
	print "when executed, will generate a Postscript histogram plot\n";
	print "of a set of data values.\n";
	print "The plot will be scaled to fit on the specified page size \n";
	print "or, if the scale is user defined, the page size will be \n";
	print "chosen in accordance with the plot size. The primary purpose \n";
	print "of this macro is to allow the simple, semi-automated\n";
	print "production of nice looking plots with a few command line\n";
	print "arguments. For users seeking more control over the plot\n";
	print "appearance, a number of additional optional arguments are\n";
	print "provided. Truly ambitious users may edit the plot shellscript \n";
	print "to take advantage of GMT capabilites not supported by this \n";
	print "macro.\n";
	print "\nBasic Usage: \n";
	print "\tmbm_histplot -Ifile [-Btickinfo -Gfill -H  \n";
	print "\t\t-Jprojection[/scale | width] \n";
	print "\t\t-Ltitle -Oroot \n";
	print "\t\t-Ppagesize -Uorientation -Q \n";
	print "\t\t-Rw/e/s/n -V -Wpen -X]\n";
	exit 0;
	}

# check for input file
if (!$file_data)
	{
	print "\a";
	die "\nNo input file specified!\n$program_name aborted\n";
	}

# set page size
if (!$pagesize)
{
    $pagesize = "a";
}
else
{
    $pagesize =~ tr/A-Z/a-z/;
    if (!$page_width_in{$pagesize})
    {
	$pagesize = "a";
    }
}

# get postscript viewer
# check environment variable
if ($ENV{"MB_PS_VIEWER"})
{
    $ps_viewer = $ENV{"MB_PS_VIEWER"};
}
# check for .mbio_defaults file
if (!$ps_viewer)
{
    $home = $ENV{"HOME"};
    $mbdef = "$home/.mbio_defaults";
    if (open(MBDEF,"<$mbdef"))
    {
	while (<MBDEF>)
	{
	    if (/ps viewer:\s+(\S+)/)
	    {
		($ps_viewer) = /ps viewer:\s+(\S+)/;
	    }
	}
	close MBDEF;
    }
}
# else just set it to ghostview
if (!$ps_viewer)
{
    $ps_viewer = "ghostview";
}

# check that file is ok
if (! -e $file_data)
	{
	    print "\a";
	die "\nInput file $file_data cannot be opened!\n$program_name aborted\n";
	}

# set output root if needed
if (!$root)
{
    $root = $file_data;
}

# use user defined bounds
if ($bounds)
	{
	if ($bounds =~ /^\S+\/\S+\/\S+\/\S+$/)
		{
		($xmin,$xmax,$ymin,$ymax) = $bounds =~ 
			/(\S+)\/(\S+)\/(\S+)\/(\S+)/;
		$bounds_plot = $bounds;
		}
	}

# get limits of file.
if (!$bounds)
	{
	# open and read the file for xmin xmax
	open(F,$file_data) || die "Cannot open input file: $file_data\n$program_name aborted.\n";
	$count = 0;
	while ($data=<F>) {
		if ($data < $xmin || $count == 0)
			{
			$xmin = $data;
			}
		if ($data > $xmax || $count == 0)
			{
			$xmax = $data;
			}
  		$count++;
		}
	close(F);
	if ($count <= 0)
		{
		die "Input file: $file_data appears to contain no data.\n$program_name aborted.\n";
		}
	
	# if needed set cell bounds
	if (!$cellwidth)
		{
		$cellwidth = ($xmax - $xmin) / 10;
		}
	
	# make cells
	$ncell = floor(($xmax - $xmin) / $cellwidth) + 4;
	$nxmin = floor($xmin / $cellwidth) - 1;
	$xmin = $nxmin * $cellwidth;
	$xmax = $xmin + $cellwidth * $ncell;
	for ($i=0;$i<$ncell;$i++)
		{
		push(@cellcount, 0);
		}
	
	# open and read the file for ymin ymax
	open(F,$file_data) || die "Cannot open input file: $file_data\n$program_name aborted.\n";
	while ($data=<F>) {
		$i = floor(($data - $xmin) / $cellwidth);
		@cellcount[$i]++;
		}
	close(F);
	$ymin = 0.0;
	$ymax = 0.0;
	for ($i=0;$i<$ncell;$i++)
		{
		@cellcount[$i] = 100 * @cellcount[$i] / $count;
		if (@cellcount[$i] > $ymax)
			{
			$ymax = @cellcount[$i];
			}
		}
	if ($ymax <= 0.0)
		{
		$ymax = 50.0;
		}
	else
		{
		$ymax = 1.1 * $ymax;
		}
	}

# set bounds string for plotting if not already set
if (!$bounds_plot)
	{
	$bounds_plot = sprintf ("%1.8g/%1.8g/%1.8g/%1.8g",
		$xmin, $xmax, $ymin, $ymax);
	}

# set the relevent page width and height
&GetPageSize;

# get user constraints on map scale 
if ($map_scale)
	{
	# sets $plot_scale or $plot_width if possible
	&GetProjection;
	}

# set up for mapproject
if (($use_scale && $plot_scale) || ($use_width && $plot_width))
	{
	($projection) = $map_scale =~ /^(\w)/;
	($projection_pars) = $map_scale =~ /^$projection(\S+)/;
	}
elsif ($use_scale || $use_width)
	{
	($projection) = $map_scale =~ /^(\w)/;
	($projection_pars) = $map_scale =~ /^$projection(\S+)/;
	$projection_pars = "$projection_pars" . "$separator" . "$trial_value";
	}
else
	{
	$projection = "x";
	$projection_pars = "1/1";
	$use_scale = 1;
	}

# now find out the apparent size of the plot
`echo $xmin $ymin > tmp$$.dat`;
`echo $xmax $ymin >> tmp$$.dat`;
`echo $xmax $ymax >> tmp$$.dat`;
`echo $xmin $ymax >> tmp$$.dat`;
@projected = `mapproject tmp$$.dat -J$projection$projection_pars -R$bounds_plot 2>&1 `;
`/bin/rm -f tmp$$.dat`;
print "mapproject tmp$$.dat -J$projection$projection_pars -R$bounds_plot\n";

while (@projected)
{
    $line = shift @projected;
print $line;
    if(!$xxmin){
	($xxmin,$yymin) = $line =~ /(\S+)\s+(\S+)/;
	$xxmax = $xxmin;
	$yymax = $yymin;
    }
    else
    {
	($xx,$yy) = $line =~ /(\S+)\s+(\S+)/;
	$xxmin = ($xx < $xxmin ? $xx : $xxmin);
	$xxmax = ($xx > $xxmax ? $xx : $xxmax);
	$yymin = ($yy < $yymin ? $yy : $yymin);
	$yymax = ($yy > $yymax ? $yy : $yymax);
    }    
}
$dxx = $xxmax - $xxmin;
$dyy = $yymax - $yymin;


# check for valid scaling
if ($dxx == 0.0 && $dyy == 0.0)
	{
	print "\a";
	die "Invalid projection specified - $program_name aborted\n";
	}

# figure out scaling issues

if (($use_scale && $plot_scale) || ($use_width && $plot_width))
	{
	$plot_width = $dxx;
	$plot_height = $dyy;

	# decide which plot orientation to use
	if ($orientation == 1)
		{
		$portrait = 1;
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		$width_max = $width_max_portrait;
		$height_max = $height_max_portrait;
		}
	elsif ($orientation == 2)
		{
		$landscape = 1;
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		$width_max = $width_max_landscape;
		$height_max = $height_max_landscape;
		}
	elsif ($dxx > $dyy)
		{
		$landscape = 1;
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		$width_max = $width_max_landscape;
		$height_max = $height_max_landscape;
		}
	else
		{
		$portrait = 1;
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		$width_max = $width_max_portrait;
		$height_max = $height_max_portrait;
		}

	# check if plot fits on page
	if ($plot_width > $width_max || $plot_height > $height_max)
		{
		# try to find a sufficiently large pagesize
		$pagesize_save = $pagesize;
		foreach $elem (@page_size_names) {
			$pagesize = "$elem";
			&GetPageSize;
			if ($portrait)
				{
				$width_max = $width_max_portrait;
				$height_max = $height_max_portrait;
				}
			else
				{
				$width_max = $width_max_landscape;
				$height_max = $height_max_landscape;
				}
			if (!$good_page && 
				$plot_width <= $width_max 
				&& $plot_height <= $height_max)
				{
				$good_page = 1;
				$pagesize_save = $pagesize;
				}
			}

		# print out warning
		if ($pagesize eq $pagesize_save)
			{
			print "\nWarning: Unable to fit plot on any available page size!\n";
			print "\tThis plot will not be particularly useful!\n";
			print "\tTry using a different scale or allow the program to set the scale!\n";
			}

		# reset the page size
		$pagesize = $pagesize_save;
		&GetPageSize;
		if ($portrait)
			{
			$width = $page_width_in{$pagesize};
			$height = $page_height_in{$pagesize};
			}
		else
			{
			$width = $page_height_in{$pagesize};
			$height = $page_width_in{$pagesize};
			}
		}
	}
elsif ($use_scale && $projection =~ /^x.*/ && !$geographic)
	{
	# set size of plot according to orientation
	if ($orientation == 1)
		{
		$portrait = 1;
		$plot_scale_x = $width_max_portrait/$dxx;
		$plot_scale_y = $height_max_portrait/$dyy;
		$plot_scale = "$plot_scale_x/$plot_scale_y";
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		}
	else
		{
		$landscape = 1;
		$plot_scale_x = $width_max_landscape/$dxx;
		$plot_scale_y = $height_max_landscape/$dyy;
		$plot_scale = "$plot_scale_x/$plot_scale_y";
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		}

	# set plot width
	$plot_width = $dxx * $plot_scale_x;
	$plot_height = $dyy * $plot_scale_y;

	# construct plot scale parameters

	## These lines break things. $projection_pars was already properly
	## defined and $map_scale may not yet be defined. Moreover, 
	## $projection_pars should now contain JUST the scale values
	## with no leading projection type.  The single line added
	## is all that is required. 
	## Val Schmidt LDEO/Columbia OCTOBER 2003
	

	#($projection_pars) = $map_scale =~ /^$projection(\S+)/;
	#    $projection_pars = sprintf ("$projection_pars$separator%1.5g", 
	#				$plot_scale);
	$projection_pars="$plot_scale";

	}
elsif ($use_width && $projection =~ /^X.*/ && !$geographic)
	{
	# set size of plot according to orientation
	if ($orientation == 1)
		{
		$portrait = 1;
		$plot_width_x = $width_max_portrait;
		$plot_width_y = $height_max_portrait;
		$plot_width = "$plot_width_x/$plot_width_y";
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		}
	else
		{
		$landscape = 1;
		$plot_width_x = $width_max_landscape;
		$plot_width_y = $height_max_landscape;
		$plot_width = "$plot_width_x/$plot_width_y";
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		}

	# set plot width
	$plot_scale_x = $plot_width / $dxx;
	$plot_scale_y = $plot_height / $dyy;

	# construct plot scale parameters

	## These lines break things. $projection_pars was already properly
	## defined and $map_scale may not yet be defined. Moreover, 
	## $projection_pars should now contain JUST the scale values
	## with no leading projection type.  The single line added
	## is all that is required. 
	## Val Schmidt LDEO/Columbia OCTOBER 2003
	
	#($projection_pars) = $map_scale =~ /^$projection(\S+)/;
	#$projection_pars = sprintf ("$projection_pars$separator%1.5g", 
	#				$plot_width);
	$projection_pars="$plot_width";
	}
elsif ($use_scale)
	{
	# get landscape and portrait scales
	$plot_scale_landscape = $width_max_landscape/$dxx;
	if ($plot_scale_landscape*$dyy > $height_max_landscape)
		{
		$plot_scale_landscape = $height_max_landscape/$dyy;
		}
	$plot_scale_portrait = $width_max_portrait/$dxx;
	if ($plot_scale_portrait*$dyy > $height_max_portrait)
		{
		$plot_scale_portrait = $height_max_portrait/$dyy;
		}

	# decide which plot orientation to use
	if ($orientation == 1)
		{
		$portrait = 1;
		$plot_scale = $plot_scale_portrait;
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		}
	elsif ($orientation == 2)
		{
		$landscape = 1;
		$plot_scale = $plot_scale_landscape;
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		}
	elsif ($plot_scale_landscape > $plot_scale_portrait)
		{
		$landscape = 1;
		$plot_scale = $plot_scale_landscape;
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		}
	else
		{
		$portrait = 1;
		$plot_scale = $plot_scale_portrait;
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		}

	# set plot width
	$plot_width = $dxx * $plot_scale;
	$plot_height = $dyy * $plot_scale;

	# reset plot_scale if ratio required
	if ($use_ratio)
		{
		$top = int(1 / $plot_scale);
		$plot_scale = "1:" . "$top";
		}

	# construct plot scale parameters
	($projection_pars) = $map_scale =~ /^$projection(\S+)/;
	$projection_pars = sprintf ("$projection_pars$separator%1.5g", 
					$plot_scale);

	# handle special case for linear projections
	if ($geographic)
		{
		$projection_pars = "$projection_pars" . "d";
		}
	}
elsif ($use_width)
	{
	# get landscape and portrait heights
	$plot_width_landscape = $height_max_landscape * $dxx / $dyy;
	if ($plot_width_landscape > $width_max_landscape)
		{
		$plot_width_landscape = $width_max_landscape;
		}
	$plot_width_portrait = $height_max_portrait * $dxx / $dyy;
	if ($plot_width_portrait > $width_max_portrait)
		{
		$plot_width_portrait = $width_max_portrait;
		}

	# decide which plot orientation to use
	if ($orientation == 1)
		{
		$portrait = 1;
		$plot_width = $plot_width_portrait;
		$plot_height = $plot_width * $dyy / $dxx;
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		}
	elsif ($orientation == 2)
		{
		$landscape = 1;
		$plot_width = $plot_width_landscape;
		$plot_height = $plot_width * $dyy / $dxx;
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		}
	elsif ($plot_width_landscape > $plot_width_portrait)
		{
		$landscape = 1;
		$plot_width = $plot_width_landscape;
		$plot_height = $plot_width * $dyy / $dxx;
		$width = $page_height_in{$pagesize};
		$height = $page_width_in{$pagesize};
		}
	else
		{
		$portrait = 1;
		$plot_width = $plot_width_portrait;
		$plot_height = $plot_width * $dyy / $dxx;
		$width = $page_width_in{$pagesize};
		$height = $page_height_in{$pagesize};
		}

	# construct plot scale parameters
	($projection_pars) = $map_scale =~ /^$projection(\S+)/;
	$projection_pars = sprintf ("$projection_pars$separator%1.5g", 
					$plot_width);

	# handle special case for linear projections
	if ($geographic)
		{
		$projection_pars = "$projection_pars" . "d";
		}
	}

# place the origin so plot is more or less centered
$xoffset = ($width - $plot_width 
	- $space_left - $space_right) / 2 + $space_left;
$yoffset = ($height - $plot_height 
	- $space_bottom - $space_top) / 2 + $space_bottom;

# come up with the filenames
$cmdfile = "$root.cmd";
$psfile = "$root.ps";
$gmtfile = "gmtdefaults\$\$";

# set some gmtisms
$first = "-X\$X_OFFSET -Y\$Y_OFFSET -V >! \$PS_FILE";

# set macro gmt default settings
$gmt_def = "MEASURE_UNIT/inch";
push(@gmt_macro_defs, $gmt_def);
if ($gmt_version eq "3.0"
	|| $gmt_version eq "3.1")
	{
	$gmt_def = "PAPER_WIDTH/$page_width_in{$pagesize}";
	push(@gmt_macro_defs, $gmt_def);
	}
else
	{
	$gmt_def = "PAPER_MEDIA/$page_gmt_name{$pagesize}+";
	push(@gmt_macro_defs, $gmt_def);
	}
$gmt_def = "ANOT_FONT/Helvetica";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "LABEL_FONT/Helvetica";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "HEADER_FONT/Helvetica";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "ANOT_FONT_SIZE/$page_anot_font{$pagesize}";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "LABEL_FONT_SIZE/$page_anot_font{$pagesize}";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "HEADER_FONT_SIZE/$page_header_font{$pagesize}";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "FRAME_WIDTH/$frame_size";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "TICK_LENGTH/$tick_size";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "PAGE_ORIENTATION/LANDSCAPE";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "COLOR_BACKGROUND/0/0/0";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "COLOR_FOREGROUND/255/255/255";
push(@gmt_macro_defs, $gmt_def);
$gmt_def = "COLOR_NAN/255/255/255";
push(@gmt_macro_defs, $gmt_def);
if ($gmt_version eq "3.0"
	|| $gmt_version eq "3.1")
	{
	$gmt_def = "DEGREE_FORMAT/3";
	push(@gmt_macro_defs, $gmt_def);
	}
else
	{
	$gmt_def = "PLOT_DEGREE_FORMAT/ddd:mm";
	push(@gmt_macro_defs, $gmt_def);
	}

# open the shellscript file
if (!open(FCMD,">$cmdfile"))
	{
	print "\a";
	die "Cannot open output file $cmdfile\nMacro $program_name aborted.\n";
	}

# write the shellscript header
print FCMD "#! /bin/csh -f\n";
print FCMD "#\n# Shellscript to create Postscript plot of data in grd file\n";
print FCMD "# Created by macro $program_name\n";
print FCMD "#\n# This shellscript created by following command line:\n";
print FCMD "# $program_name $command_line\n";

# Define shell variables
print FCMD "#\n# Define shell variables used in this script:\n";
print FCMD "set PS_FILE         = $psfile\n";
print FCMD "set CPT_FILE        = $cptfile\n";
print FCMD "set MAP_PROJECTION  = $projection\n";
print FCMD "set MAP_SCALE       = $projection_pars\n";
print FCMD "set MAP_REGION      = $bounds_plot\n";
printf FCMD "set X_OFFSET        = %1.5g\n", $xoffset;
printf FCMD "set Y_OFFSET        = %1.5g\n", $yoffset;

# Reset GMT defaults, saving old defaults
print FCMD "#\n# Save existing GMT defaults\n";
print FCMD "echo Saving GMT defaults...\n";
print FCMD "gmtdefaults -L >! $gmtfile\n";
print FCMD "#\n# Set new GMT defaults\n";
print FCMD "echo Setting new GMT defaults...\n";
foreach $gmt_def (@gmt_macro_defs) {
	($gmt_par, $gmt_var) = $gmt_def =~ /^([^\/]+)\/(.+)/;
	print FCMD "gmtset $gmt_par $gmt_var\n";
	}

# Reset GMT defaults as per user commands
if (@gmt_defs)
	{
	print FCMD "#\n# Set user defined GMT defaults\n";
	print FCMD "echo Setting user defined GMT defaults...\n";
	foreach $gmt_def (@gmt_defs) {
		($gmt_par, $gmt_var) = $gmt_def =~ /^([^\/]+)\/(.+)/;
		print FCMD "gmtset $gmt_par $gmt_var\n";
		}
	}

# figure out labels
$nlabels = 0;
if ($labels)
	{
	@labels_split = split(/:/, $labels);
	$nlabels = @labels_split;
	if ($nlabels > 0)
		{
		$tlabel = shift(@labels_split);
		$tlabel =~ s/\\/\//g;
		}
	if ($nlabels > 1)
		{
		$xlabel = shift(@labels_split);
		$xlabel =~ s/\\/\//g;
		}
	if ($nlabels > 2)
		{
		$ylabel = shift(@labels_split);
		$ylabel =~ s/\\/\//g;
		}
	}
if ($nlabels < 1)
	{
	$tlabel = "Frequency Histogram of $file_data";
	}
if ($nlabels < 2)
	{
	$xlabel = " ";
	}
if ($nlabels < 3)
	{
	$ylabel = "Frequency %";
	}

# set basemap axes annotation
if ($tick_info)
	{
	$axes = $tick_info;
	if (!($tick_info =~ /.*:\..*/))
		{
		$axes = "$axes:.\"$tlabel\":";
		}
	}
elsif ($projection =~ /^[Xx].*/ && !$geographic)
	{
	# figure out some reasonable tick intervals for the basemap
	&GetBaseTickLinear;
	$axes = "$base_tick_x:\"$xlabel\":/$base_tick_y:\"$ylabel\"::.\"$tlabel\":";
	}
else
	{
	# figure out some reasonable tick intervals for the basemap
	&GetBaseTick;
	$axes = "$base_tick:\"$xlabel\":/$base_tick:\"$ylabel\"::.\"$tlabel\":";
	}

# do histogram plot
printf FCMD "#\n# Make histogram\n";
printf FCMD "echo Running pshistogram...\n";
printf FCMD "pshistogram $file_data \\\n\t";
printf FCMD "-J\$MAP_PROJECTION\$MAP_SCALE \\\n\t";
printf FCMD "-R\$MAP_REGION \\\n\t";
printf FCMD "-B$axes \\\n\t";
printf FCMD "-W$cellwidth -Z1 \\\n\t";
if ($pen)
	{
	printf FCMD "-L$pen \\\n\t";
	}
if ($fill && $fill ne "N")
	{
	printf FCMD "-G$fill \\\n\t";
	}
if ($unix_stamp_on && $unix_stamp)
	{
	printf FCMD "-U$unix_stamp \\\n\t";
	}
elsif ($unix_stamp_on)
	{
	printf FCMD "-U \\\n\t";
	}
if ($portrait)
	{
	printf FCMD "-P ";
	}
printf FCMD "$first\n";

# delete surplus files
print FCMD "#\n# Delete surplus files\n";
print FCMD "echo Deleting surplus files...\n";
if (!$file_cpt)
	{
	print FCMD "/bin/rm -f \$CPT_FILE\n";
	}
if ($data_scale)
	{
	printf FCMD "/bin/rm -f $file_use\n";
	}

if (! $save_temp_files) {
    foreach $file (@xyfiles) {
	printf FCMD "/bin/rm -f $file \n";
    }
}


# reset GMT defaults
print FCMD "#\n# Reset GMT default fonts\n";
print FCMD "echo Resetting GMT fonts...\n";
print FCMD "/bin/mv $gmtfile .gmtdefaults\n";

# display image on screen if desired
print FCMD "#\n# Run $ps_viewer\n";
if ($ps_viewer eq "xpsview")
	{
	if ($portrait)
		{
		$view_pageflag = "-ps $pagesize -maxp $xpsview_mem{$pagesize}";
		}
	elsif ($landscape)
		{
		$view_pageflag = "-ps $pagesize -or landscape -maxp $xpsview_mem{$pagesize}";
		}
	}
elsif ($ps_viewer eq "pageview")
	{
	if ($portrait)
		{
		$view_pageflag = "-w $page_width_in{$pagesize} -h $page_height_in{$pagesize}";
		}
	elsif ($landscape)
		{
		$view_pageflag = "-w $page_height_in{$pagesize} -h $page_width_in{$pagesize}";
		}
	}
elsif ($ps_viewer eq "ghostview")
	{
	if ($portrait)
		{
		$view_pageflag = "-portrait -media BBox";
		}
	elsif ($landscape)
		{
		$view_pageflag = "-landscape -media BBox";
		}
	}
elsif ($ps_viewer eq "gv")
	{
	if ($portrait)
		{
		$view_pageflag = "--orientation=portrait --media=BBox";
		}
	elsif ($landscape)
		{
		$view_pageflag = "--orientation=landscape --media=BBox";
		}
	}
elsif ($ps_viewer eq "ggv")
	{
	if ($portrait)
		{
		$view_pageflag = "--geometry=portrait";
		}
	elsif ($landscape)
		{
		$view_pageflag = "--geometry=landscape";
		}
	}
if ($no_view_ps)
	{
	print FCMD "#echo Running $ps_viewer in background...\n";
	print FCMD "#$ps_viewer $view_pageflag \$PS_FILE &\n";
	}
else
	{
	print FCMD "echo Running $ps_viewer in background...\n";
	print FCMD "$ps_viewer $view_pageflag \$PS_FILE &\n";
	}

# claim it's all over
print FCMD "#\n# All done!\n";
print FCMD "echo All done!\n";

# now close the shellscript and make it executable
close FCMD;
chmod 0775, $cmdfile;

# tell program status
if ($verbose)
	{
	print "\nProgram Status:\n";
	print "--------------\n";
	print "\n  GMT Version:\n";
	print "    Version $gmt_version\n";
	print "\n  Plot Style:\n";
	if (@xyfiles)
		{
		print "    XY Plots of ", scalar(@xyfiles), " Datasets\n";
		}
	if ($coast_control)
		{
		print "    Coastline\n";
		}
	if (@text)
		{
		print "    ", scalar(@text), " Text labels\n";
		}
	if ($length_scale)
		{
		print "    Map distance scale\n";
		}
	if ($unix_stamp_on && $unix_stamp)
		{
		print "    Unix time stamp: $unix_stamp\n";
		}
	elsif ($unix_stamp_on)
		{
		print "    Unix time stamp\n";
		}
	print "\n  Input Files:\n";
	foreach $xyfile (@xyfiles) {
		print "    XY Data File:             $xyfile\n";
	}
	print "\n  Output Files:\n";
	print "    Output plot name root:    $root\n";
	print "    Plotting shellscript:     $cmdfile\n";
	print "    Plot file:                $psfile\n";
	print "\n  Plot Attributes:\n";
	printf "    Plot width:               %.4f\n", $plot_width;
	printf "    Plot height:              %.4f\n", $plot_height;
	print "    Page size:                $pagesize\n";
	print "    Page width:               $width\n";
	print "    Page height:              $height\n";
	print "    Projection:               -J$projection$projection_pars\n";
	print "    Axes annotation:          $axes\n";
	if ($portrait)
		{
		print "    Orientation:              portrait\n";
		}
	else
		{
		print "    Orientation:              landscape\n";
		}
	print "\n  XY Data Attributes:\n";
	printf "    X min max:                %9.4g  %9.4g\n", 
		$xmin, $xmax;
	printf "    Y min max:                %9.4g  %9.4g\n", 
		$ymin, $ymax;
	print "\n  GMT Default Values Reset in Script:\n";
	foreach $gmt_def (@gmt_macro_defs) {
		($gmt_par, $gmt_var) = $gmt_def =~ /^([^\/]+)\/(.+)/;
		printf "    %-25s %s\n", $gmt_par, $gmt_var;
		}
	foreach $gmt_def (@gmt_defs) {
		($gmt_par, $gmt_var) = $gmt_def =~ /^([^\/]+)\/(.+)/;
		printf "    $gmt_par : $gmt_var\n";
		}
	print "\n--------------\n";
	}

# print out final notes
print "\nPlot generation shellscript <$cmdfile> created.\n";
print "\nInstructions:\n";
print "  Execute <$cmdfile> to generate Postscript plot <$psfile>.\n";
if (!$no_view_ps)
	{
	print "  Executing <$cmdfile> also invokes $ps_viewer ";
	print "to view the plot on the screen.\n";
	}
if ($verbose)
	{
	print "\n--------------\n\n";
	}

# execute shellscript if desired
if ($execute)
	{
	if ($verbose)
		{
		print "Executing shellscript $cmdfile...\n";
		}
	system "$cmdfile &";
	}

exit 0;

#-----------------------------------------------------------------------
sub min {

	# make local variables
	local ($min);
	
	# get the minimum of the arguments
	if ($_[0] < $_[1])
		{
		$min = $_[0];
		}
	else
		{
		$min = $_[1];
		}
	$min;
}
#-----------------------------------------------------------------------
sub max {

	# make local variables
	local ($max);
	
	# get the minimum of the arguments
	if ($_[0] > $_[1])
		{
		$max = $_[0];
		}
	else
		{
		$max = $_[1];
		}
	$max;
}
#-----------------------------------------------------------------------
sub GetDecimalDegrees {

	# make local variables
	local ($dec_degrees, $degrees, $minutes, $seconds);

	# deal with dd:mm:ss format
	if ($_[0] =~ /^\S+:\S+:\S+$/)
		{
		($degrees, $minutes, $seconds) 
			= $_[0] =~ /^(\S+):(\S+):(\S+)$/;
		if ($degrees =~ /^-\S+/)
			{
			$dec_degrees = $degrees 
				- $minutes / 60.0 
				- $seconds / 3600.0;
			}
		else
			{
			$dec_degrees = $degrees 
				+ $minutes / 60.0 
				+ $seconds / 3600.0;
			}
		}
	# deal with dd:mm format
	elsif ($_[0] =~ /^\S+:\S+$/)
		{
		($degrees, $minutes) 
			= $_[0] =~ /^(\S+):(\S+)$/;
		if ($degrees =~ /^-\S+/)
			{
			$dec_degrees = $degrees - $minutes / 60.0;
			}
		else
			{
			$dec_degrees = $degrees + $minutes / 60.0;
			}		}

	# value already in decimal units of some sort
	else
		{
		$dec_degrees = $_[0];
		}

	# return decimal degrees;
	$dec_degrees;
}
#-----------------------------------------------------------------------
sub GetPageSize {

# get space around edge of plot
	$space_top =    1.25 * $page_height_in{$pagesize} 
			    / $page_height_in{"a"};
	$space_bottom = 1.50 * $page_height_in{$pagesize} 
			    / $page_height_in{"a"};
	$space_left =   1.00 * $page_height_in{$pagesize} 
			    / $page_height_in{"a"};
	$space_right =  1.00 * $page_height_in{$pagesize} 
			    / $page_height_in{"a"};

# set the relevent page width and height
$width_max_landscape = $page_height_in{$pagesize}
		- $space_left - $space_right;
$height_max_landscape = $page_width_in{$pagesize}
		- $space_bottom - $space_top;
$width_max_portrait = $page_width_in{$pagesize}
		- $space_left - $space_right;
$height_max_portrait = $page_height_in{$pagesize}
		- $space_bottom - $space_top;
$frame_size = 0.075;
if (($frame_size / $height_max_portrait) > 0.01)
	{
	$frame_size = 0.01 * $height_max_portrait;
	}
$tick_size = 0.075;
if (($tick_size / $height_max_portrait) > 0.01)
	{
	$tick_size = 0.01 * $height_max_portrait;
	}
}
#-----------------------------------------------------------------------
sub GetProjection {

	# get the map projection flag
	($projection) = $map_scale =~ /^(\w)/;
	($projection_pars) = $map_scale =~ /^$projection(\S+)/;

	# see if plot scale or plot width defined 
	$use_scale = 0;
	$use_width = 0;
	$separator = "/";
	$trial_value = 1.0;

	# Cassini Projection
	if ($projection eq "c")
		{
		($plot_scale) = $map_scale =~ /^c\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "C")
		{
		($plot_width) = $map_scale =~ /^C\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Mercator Projection
	elsif ($projection eq "m")
		{
		($plot_scale) = $map_scale =~ /^m(\S+)$/; 
		$use_scale = 1;
		$separator = "";
		}
	elsif ($projection eq "M")
		{
		($plot_width) = $map_scale =~ /^M(\S+)$/; 
		$use_width = 1;
		$separator = "";
		}

	# Oblique Mercator Projection
	elsif ($projection eq "o")
		{
		if ($map_scale =~ /^oa\S+/)
			{
			($plot_scale) = $map_scale =~ /^oa\S+\/\S+\/\S+\/(\S+)$/;
			}
		elsif ($map_scale =~ /^ob\S+/)
			{
			($plot_scale) = $map_scale =~ /^ob\S+\/\S+\/\S+\/\S+\/(\S+)$/;
			}
		elsif ($map_scale =~ /^oc\S+/)
			{
			($plot_scale) = $map_scale =~ /^oc\S+\/\S+\/\S+\/\S+\/(\S+)$/;
			}
		$use_scale = 1;
		}
	elsif ($projection eq "O")
		{
		if ($map_scale =~ /^Oa\S+/)
			{
			($plot_width) = $map_scale =~ /^Oa\S+\/\S+\/\S+\/(\S+)$/;
			}
		elsif ($map_scale =~ /^Ob\S+/)
			{
			($plot_width) = $map_scale =~ /^Ob\S+\/\S+\/\S+\/\S+\/(\S+)$/;
			}
		elsif ($map_scale =~ /^Oc\S+/)
			{
			($plot_width) = $map_scale =~ /^Oc\S+\/\S+\/\S+\/\S+\/(\S+)$/;
			}
		$use_width = 1;
		}

	# Equidistant Cylindrical Projection
	elsif ($projection eq "q")
		{
		($plot_scale) = $map_scale =~ /^q\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "Q")
		{
		($plot_width) = $map_scale =~ /^Q\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Transverse Mercator Projection
	elsif ($projection eq "t")
		{
		($plot_scale) = $map_scale =~ /^t\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "T")
		{
		($plot_width) = $map_scale =~ /^T\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Universal Transverse Mercator Projection
	elsif ($projection eq "u")
		{
		($plot_scale) = $map_scale =~ /^u\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "U")
		{
		($plot_width) = $map_scale =~ /^U\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Basic Cylindrical Projection
	elsif ($projection eq "y")
		{
		($plot_scale) = $map_scale =~ /^y\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "Y")
		{
		($plot_width) = $map_scale =~ /^Y\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Lambert Azimuthal Projection
	elsif ($projection eq "a")
		{
		($plot_scale) = $map_scale =~ /^a\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		$trial_value = "1:1";
		$use_ratio = 1;
		}
	elsif ($projection eq "A")
		{
		($plot_width) = $map_scale =~ /^A\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Equidistant Projection
	elsif ($projection eq "e")
		{
		($plot_scale) = $map_scale =~ /^e\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		$trial_value = "1:1";
		$use_ratio = 1;
		}
	elsif ($projection eq "E")
		{
		($plot_width) = $map_scale =~ /^E\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Orthographic Projection
	elsif ($projection eq "g")
		{
		($plot_scale) = $map_scale =~ /^g\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		$trial_value = "1:1";
		$use_ratio = 1;
		}
	elsif ($projection eq "G")
		{
		($plot_width) = $map_scale =~ /^G\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# General Sterographic Projection
	elsif ($projection eq "s")
		{
		($plot_scale) = $map_scale =~ /^s\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		$trial_value = "1:1";
		$use_ratio = 1;
		}
	elsif ($projection eq "S")
		{
		($plot_width) = $map_scale =~ /^S\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Albers Projection
	elsif ($projection eq "b")
		{
		($plot_scale) = $map_scale =~ /^b\S+\/\S+\/\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "B")
		{
		($plot_width) = $map_scale =~ /^B\S+\/\S+\/\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Lambert Conic Projection
	elsif ($projection eq "l")
		{
		($plot_scale) = $map_scale =~ /^l\S+\/\S+\/\S+\/\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "L")
		{
		($plot_width) = $map_scale =~ /^L\S+\/\S+\/\S+\/\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Hammer Projection
	elsif ($projection eq "h")
		{
		($plot_scale) = $map_scale =~ /^h\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "H")
		{
		($plot_width) = $map_scale =~ /^H\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Sinusoidal Projection
	elsif ($projection eq "i")
		{
		($plot_scale) = $map_scale =~ /^i\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "I")
		{
		($plot_width) = $map_scale =~ /^I\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Eckert VI Projection
	elsif ($projection eq "k")
		{
		($plot_scale) = $map_scale =~ /^k\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "K")
		{
		($plot_width) = $map_scale =~ /^K\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Robinson Projection
	elsif ($projection eq "n")
		{
		($plot_scale) = $map_scale =~ /^n\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "N")
		{
		($plot_width) = $map_scale =~ /^N\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Winkel Tripel Projection
	elsif ($projection eq "r")
		{
		($plot_scale) = $map_scale =~ /^r\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "R")
		{
		($plot_width) = $map_scale =~ /^R\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Mollweide Projection
	elsif ($projection eq "w")
		{
		($plot_scale) = $map_scale =~ /^w\S+\/(\S+)$/; 
		$use_scale = 1;
		}
	elsif ($projection eq "W")
		{
		($plot_width) = $map_scale =~ /^W\S+\/(\S+)$/; 
		$use_width = 1;
		}

	# Linear Polar Projection
	elsif ($projection eq "p")
		{
		($plot_scale) = $map_scale =~ /^p(\S+)$/; 
		$use_scale = 1;
		$separator = "";
		}
	elsif ($projection eq "P")
		{
		($plot_width) = $map_scale =~ /^P(\S+)$/; 
		$use_width = 1;
		$separator = "";
		}

	# Linear Projection
	elsif ($projection eq "x")
		{
		if ($map_scale =~ /^xd$/)
			{
			$geographic = 1;
			chop($map_scale);
			}
		else
			{
			($plot_scale) = $map_scale =~ /^x(\S+)$/; 
			}
		$use_scale = 1;
		$separator = "";
		}
	elsif ($projection eq "X")
		{
		if ($map_scale =~ /^Xd$/)
			{
			$geographic = 1;
			chop($map_scale);
			}
		else
			{
			($plot_width) = $map_scale =~ /^X(\S+)$/; 
			}
		$use_width = 1;
		$separator = "";
		}
}
#-----------------------------------------------------------------------
sub GetBaseTickLinear {

	# figure out some reasonable tick intervals for the basemap
	$dxxx = $xmax - $xmin;
	if ($dxxx > 0)
		{
		$base = int((log($dxxx) / log(10.)) + 0.5);
		$base_tick_x = (10 ** $base) / 5;
		if ($dxxx / $base_tick_x < 5)
			{
			$base_tick_x = $base_tick_x / 4;
			}
		elsif ($dxxx / $base_tick_x < 10)
			{
			$base_tick_x = $base_tick_x / 2;
			}
		}
	$dyyy = $ymax - $ymin;
	if ($dyyy > 0)
		{
		$base = int((log($dyyy) / log(10.)) + 0.5);
		$base_tick_y = (10 ** $base) / 5;
		if ($dyyy / $base_tick_y < 5)
			{
			$base_tick_y = $base_tick_y / 4;
			}
		elsif ($dyyy / $base_tick_y < 10)
			{
			$base_tick_y = $base_tick_y / 2;
			}
		}

}
#-----------------------------------------------------------------------
sub GetBaseTick {

	# figure out some reasonable tick intervals for the basemap
	$base_tick_x = ($xmax - $xmin) / 5;
	$base_tick_y = ($ymax - $ymin) / 5;
	$base_tick = &min($base_tick_x, $base_tick_y);
	if ($base_tick < 0.0002777777)
		{
		$base_tick = "1c";
		}
	elsif ($base_tick < 0.0005555555)
		{
		$base_tick = "2c";
		}
	elsif ($base_tick < 0.0013888889)
		{
		$base_tick = "5c";
		}
	elsif ($base_tick < 0.0027777778)
		{
		$base_tick = "10c";
		}
	elsif ($base_tick < 0.0041666667)
		{
		$base_tick = "15c";
		}
	elsif ($base_tick < 0.0083333333)
		{
		$base_tick = "30c";
		}
	elsif ($base_tick < 0.0166667)
		{
		$base_tick = "1m";
		}
	elsif ($base_tick < 0.0333333)
		{
		$base_tick = "2m";
		}
	elsif ($base_tick < 0.0833333)
		{
		$base_tick = "5m";
		}
	elsif ($base_tick < 0.1666667)
		{
		$base_tick = "10m";
		}
	elsif ($base_tick < 0.25)
		{
		$base_tick = "15m";
		}
	elsif ($base_tick < 0.5)
		{
		$base_tick = "30m";
		}
	elsif ($base_tick < 1.0)
		{
		$base_tick = "1";
		}
	elsif ($base_tick < 2.0)
		{
		$base_tick = "2";
		}
	elsif ($base_tick < 5.0)
		{
		$base_tick = "5";
		}
	elsif ($base_tick < 10.0)
		{
		$base_tick = "10";
		}
	elsif ($base_tick < 15.0)
		{
		$base_tick = "15";
		}
	elsif ($base_tick < 30.0)
		{
		$base_tick = "30";
		}
	elsif ($base_tick < 30.0)
		{
		$base_tick = "30";
		}
	elsif ($base_tick < 360.0)
		{
		$base_tick = "60";
		}

}
#-----------------------------------------------------------------------
# This version of Getopts has been augmented to support multiple
# calls to the same option. If an arg in argumentative is followed
# by "+" rather than ":",  then the corresponding scalar will
# be concatenated rather than overwritten by multiple calls to
# the same arg.
#
# Usage:
#      do Getopts('a:b+c'); # -a takes arg, -b concatenates args,  
#			    # -c does not take arg. Sets opt_* as a
#                           # side effect.

sub MBGetopts {
    local($argumentative) = @_;
    local(@args,$_,$first,$rest);
    local($errs) = 0;
    local($[) = 0;

    @args = split( / */, $argumentative );
    while(@ARGV && ($_ = $ARGV[0]) =~ /^-(.)(.*)/) {
	($first,$rest) = ($1,$2);
	$pos = index($argumentative,$first);
	if($pos >= $[) {
	    if($args[$pos+1] eq ':') {
		shift(@ARGV);
		if($rest eq '') {
		    ++$errs unless @ARGV;
		    $rest = shift(@ARGV);
		}
		eval "\$opt_$first = \$rest;";
		eval "\$flg_$first = 1;";
	    }
	    elsif($args[$pos+1] eq '+') {
		shift(@ARGV);
		if($rest eq '') {
		    ++$errs unless @ARGV;
		    $rest = shift(@ARGV);
		}
		if (eval "\$opt_$first") {
		    eval "\$opt_$first = \$opt_$first 
				. \":::::::\" . \$rest;";
		}
		else {
		    eval "\$opt_$first = \$rest;";
		}
		eval "\$flg_$first = 1;";
	    }
	    elsif($args[$pos+1] eq '%') {
		shift(@ARGV);
		if($rest ne '') {
		    eval "\$opt_$first = \$rest;";
		}
		else {
		    $rest = $ARGV[0];
		    ($one) = $rest =~ /^-(.).*/;
		    $pos = index($argumentative,$one);
		    if(!$one || $pos < $[) {
			eval "\$opt_$first = \$rest;";
			shift(@ARGV);
		    }
		}
		eval "\$flg_$first = 1;";
	    }
	    else {
		eval "\$opt_$first = 1";
		eval "\$flg_$first = 1;";
		if($rest eq '') {
		    shift(@ARGV);
		}
		else {
		    $ARGV[0] = "-$rest";
		}
	    }
	}
	else {
	    print STDERR "Unknown option: $first\n";
	    ++$errs;
	    if($rest ne '') {
		$ARGV[0] = "-$rest";
	    }
	    else {
		shift(@ARGV);
	    }
	}
    }
    $errs == 0;
}
#-----------------------------------------------------------------------
