eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
                    & eval 'exec perl -S $0 $argv:q'
                         if 0;
#--------------------------------------------------------------------
#    The MB-system:	mbm_fmtvel.perl	6/18/93
#    $Id: mbm_fmtvel.pl 1891 2011-05-04 23:46:30Z caress $
#
#    Copyright (c) 1993-2011 by 
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
#   mbm_fmtvel
#
# Purpose:
#   Perl shellscript to generate a list of the cmean (mean water 
#   velocity) and ckeel (surface water velocity) used by Hydrosweep
#   in its internal processing.  This macro executes the program
#   hsdump and then scans the output for the desired values.
#   The only allowed formats are 21 (raw Hydrosweep data) and 24
#   (L-DEO in-house binary Hydrosweep data).
#
# Usage:
#   mbm_fmtvel -Fformat -Ifile
#
# Author:
#   Dale N. Chayes
#   David W. Caress
#   Lamont-Doherty Earth Observatory
#   Palisades, NY  10964
#   May 19, 1993
#
# Version:
#   $Id: mbm_fmtvel.pl 1891 2011-05-04 23:46:30Z caress $
#
# Revisions:
#   $Log: mbm_fmtvel.perl,v $
#   Revision 5.1  2003/04/17 20:42:48  caress
#   Release 5.0.beta30
#
#   Revision 5.0  2000/12/01 22:58:01  caress
#   First cut at Version 5.0.
#
# Revision 4.4  2000/10/03  21:42:17  caress
# Snapshot for Dale.
#
# Revision 4.3  1997/04/21  16:54:41  caress
# MB-System 4.5 Beta Release.
#
# Revision 4.2  1995/05/12  17:43:23  caress
# Made exit status values consistent with Unix convention.
# 0: ok  nonzero: error
#
# Revision 4.2  1995/05/12  17:43:23  caress
# Made exit status values consistent with Unix convention.
# 0: ok  nonzero: error
#
# Revision 4.1  1994/10/21  11:36:58  caress
# Release V4.0
#
# Revision 4.0  1994/03/05  23:52:40  caress
# First cut at version 4.0
#
# Revision 4.1  1994/03/03  04:11:13  caress
# Fixed copyright message.
#
# Revision 4.0  1994/02/26  19:37:57  caress
# First cut at new version.
#
# Revision 1.4  1993/08/17  16:58:36  caress
# Set location of perl to /usr/local/bin/perl
#
# Revision 1.3  1993/08/16  23:03:15  caress
# I'm not sure what the changes are - I'm just checking in the
# current version.
#
# Revision 1.2  1993/06/19  15:53:55  caress
# Streamlined the Perl code. Now uses hsdump to scan the
# Hydrosweep file, parses the output without using a
# temporary file, and writes the output to stdout.
#
#
# Revision 1.1  1993/05/20  04:03:13  dale
# Initial revision
#
#
###################################################
$program_name = "mbm_fmtvel";

# Deal with command line arguments
&Getopts('F:f:I:i:');
$file =    ($opt_I || $opt_i);
$format =  ($opt_F || $opt_f);

# print out help message if required
if ($help)
	{
	print "\n$program_name:\n";
	print "\nVersion: $Id: mbm_fmtvel.pl 1891 2011-05-04 23:46:30Z caress $\n";
	print "\nPerl shellscript to generate a list of the C-mean (mean water velocity) \nand C-keel (surface water velocity) used by Hydrosweepin its internal \nprocessing.  This macro executes the programhsdump and then scans the \noutput for the desired values.The only allowed formats are 5 (raw \nHydrosweep data) and 8(L-DEO in-house binary Hydrosweep data).\n";
	print "\nUsage: $program_name -Fformat -Ifile\n";
	exit 0;
	}

# check for defined parameters
if (!$file)
	{
	die "No input file specified - $program_name aborted\n";
	}
if (!$format) 
	{
	$format = "24";
	}

# get user id and time
$me = `/usr/bin/id`;
@me = split(' ',$me);
$now = &ctime(time);

# print output header
print "\n# Hydrosweep C-mean and C-keel values extracted using \n# macro $program_name\n";
print "# Run by user @me[0] on $now";
print "#\n# Date\t\t  Time\t\t Longitude\t Latitude\tC-mean\tC-keel\n";

# Run hsdump
@hsdump = `hsdump -F$format -I$file -O4`;
while (@hsdump)
	{
	$line = shift @hsdump;
	if ($line =~ /^  Time: (.*)/)
		{ 
		@result = split(' ',$line);
		print  @result[1],"\t", @result[2],"\t" ;
		}
	if ($line =~ /Longitude: (.*)/)
		{ 
		@result = split(' ',$line);
		print  @result[1],"\t";
		}
	if ($line =~ /Latitude: (.*)/)
		{ 
		@result = split(' ',$line);
		print  @result[1],"\t";
		}
	if ($line =~ /Mean velocity: (.*)/)
		{ 
		@result = split(' ',$line);
		printf  "%6.1f\t", @result[2];
		}
	if ($line =~ /Keel velocity: (.*)/)
		{ 
		@result = split(' ',$line);
		printf  "%6.1f\n", @result[2];
		}
 	}

# end it all
exit 0;

#-----------------------------------------------------------------------
# This should be loaded from the library but the shipboard installation
# of Perl is screwed up so....
#
;# getopts.pl - a better getopt.pl

;# Usage:
;#      do Getopts('a:bc');  # -a takes arg. -b & -c not. Sets opt_* as a
;#                           #  side effect.

sub Getopts {
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
	    }
	    else {
		eval "\$opt_$first = 1";
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

1;

#-----------------------------------------------------------------------
# This should be loaded from the library but the shipboard installation
# of Perl is screwed up so....
#

;# ctime.pl is a simple Perl emulation for the well known ctime(3C) function.
;#
;# Waldemar Kebsch, Federal Republic of Germany, November 1988
;# kebsch.pad@nixpbe.UUCP
;# Modified March 1990, Feb 1991 to properly handle timezones
;#  $RCSfile: mbm_fmtvel.perl,v $$Revision: 5.1 $$Date: 2003/04/17 20:42:48 $
;#   Marion Hakanson (hakanson@cse.ogi.edu)
;#   Oregon Graduate Institute of Science and Technology
;#
;# usage:
;#
;#     #include <ctime.pl>          # see the -P and -I option in perl.man
;#     $Date = &ctime(time);

CONFIG: {
    package ctime;

    @DoW = ('Sun','Mon','Tue','Wed','Thu','Fri','Sat');
    @MoY = ('Jan','Feb','Mar','Apr','May','Jun',
	    'Jul','Aug','Sep','Oct','Nov','Dec');
}

sub ctime {
    package ctime;

    local($time) = @_;
    local($[) = 0;
    local($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst);

    # Determine what time zone is in effect.
    # Use GMT if TZ is defined as null, local time if TZ undefined.
    # There's no portable way to find the system default timezone.

    $TZ = defined($ENV{'TZ'}) ? ( $ENV{'TZ'} ? $ENV{'TZ'} : 'GMT' ) : '';
    ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) =
        ($TZ eq 'GMT') ? gmtime($time) : localtime($time);

    # Hack to deal with 'PST8PDT' format of TZ
    # Note that this can't deal with all the esoteric forms, but it
    # does recognize the most common: [:]STDoff[DST[off][,rule]]

    if($TZ=~/^([^:\d+\-,]{3,})([+-]?\d{1,2}(:\d{1,2}){0,2})([^\d+\-,]{3,})?/){
        $TZ = $isdst ? $4 : $1;
    }
    $TZ .= ' ' unless $TZ eq '';

    $year += ($year < 70) ? 2000 : 1900;
    sprintf("%s %s %2d %2d:%02d:%02d %s%4d\n",
      $DoW[$wday], $MoY[$mon], $mday, $hour, $min, $sec, $TZ, $year);
}
1;