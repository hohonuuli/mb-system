/*--------------------------------------------------------------------
 *    The MB-system:    mbvelocitytool.c        6/6/93
 *    $Id: mbvelocity_prog.c,v 4.23 2000-10-11 01:06:03 caress Exp $ 
 *
 *    Copyright (c) 1993, 1994, 2000 by
 *    David W. Caress (caress@mbari.org)
 *      Monterey Bay Aquarium Research Institute
 *      Moss Landing, CA 95039
 *    and Dale N. Chayes (dale@ldeo.columbia.edu)
 *      Lamont-Doherty Earth Observatory
 *      Palisades, NY 10964
 * 
 *    See README file for copying and redistribution conditions.
 *--------------------------------------------------------------------*/
/*
 * MBVELOCITYTOOL is an interactive water velocity profile editor 
 * used to examine multiple water velocity profiles and to create 
 * new water velocity profiles which can be used for the processing 
 * of multibeam sonar data.  In general, this tool is used to examine 
 * water velocity profiles obtained from XBTs, CTDs, or databases, 
 * and to construct new profiles consistent with these various 
 * sources of information.  
 * 
 * Author:      D. W. Caress 
 * Date:        June 6, 1993 
 * 
 * $Log: not supported by cvs2svn $
 * Revision 4.22  2000/09/30  07:05:18  caress
 * Snapshot for Dale.
 *
 * Revision 4.21  1999/02/04  23:54:54  caress
 * MB-System version 4.6beta7
 *
 * Revision 4.20  1998/10/09  18:46:59  caress
 * MB-System version 4.6beta
 *
 * Revision 4.19  1997/09/16  21:44:24  caress
 * Removed draft option as it is no longer needed.
 *
 * Revision 4.18  1997/09/15  19:10:50  caress
 * Real Version 4.5
 *
 * Revision 4.17  1997/07/25  14:27:30  caress
 * Version 4.5beta2.
 *
 * Revision 4.16  1997/04/21  17:09:54  caress
 * MB-System 4.5 Beta Release.
 *
 * Revision 4.16  1997/04/16  21:35:31  caress
 * Complete rewrite of mbvelocitytool without uid file.
 *
 * Revision 4.15  1996/08/26  17:34:23  caress
 * Release 4.4 revision.
 * 
 * Revision 4.14  1996/04/22 13:22:47  caress 
 * Now have DTR and MIN/MAX defines in mb_define.h 
 * 
 * Revision 4.13  1996/02/16  18:27:11  caress 
 * Changed labels from Water Velocity to Water Sound Velocity.  
 * 
 * Revision 4.12 1996/02/12  18:09:25  caress 
 * Added command line arguments to specify input files at startup time.  
 * 
 * Revision 4.11  1996/01/26  21:25:34 caress 
 * Version 4.3 distribution.  
 * 
 * Revision 4.10  1995/10/02 22:25:20  caress 
 * Added -D option.  
 * 
 * Revision 4.9  1995/09/28 18:03:58  caress 
 * Improved handling of .mbxxx file suffix convention.  
 * 
 * Revision 4.8  1995/06/06  12:57:17  caress 
 * Fixed mb_close() call.  
 * 
 * Revision 4.7  1995/05/12  17:27:40  caress 
 * Made exit status values consistent with Unix convention.  
 * 0: ok nonzero: error 
 * 
 * Revision 4.6  1995/03/17  15:23:00  caress 
 * Changed size of new editable profiles to 
 * 30 nodes.  
 * 
 * Revision 4.5  1995/03/06  19:41:22  caress 
 * Changed include strings.h to string.h for POSIX compliance.  
 * 
 * Revision 4.4  1995/02/14 18:26:46  caress 
 * Moved the widgets around, made the format defaults work, and made 
 * the program recognize the MB-System file suffix convention.  
 * 
 * Revision 4.3  1994/11/24  01:54:08  caress 
 * Some fixes related to gradient raytracing version.  
 * 
 * Revision 4.2 1994/11/18  18:58:19  caress 
 * First gradient raytracing version.  
 * 
 * Revision 4.1  1994/11/10  01:16:07  caress 
 * Set program to do raytracing for every ping rather than once at beginning.  
 * 
 * Revision 4.0  1994/10/21  12:43:44  caress 
 * Release V4.0 
 * 
 * Revision 4.2 1994/04/12  01:13:24  caress 
 * First cut at translation from hsvelocitytool. The new program 
 * mbvelocitytool will deal with all supported multibeam data 
 * including travel time observations.  
 * 
 * Revision 4.1  1994/03/12  01:50:30  caress 
 * Added declarations of ctime and/or getenv for compatability 
 * with SGI compilers.  
 * 
 * Revision 4.0  1994/03/05  23:49:05  caress 
 * First cut at version 4.0 
 * 
 * Revision 4.1  1994/03/03  03:53:26  caress 
 * Fixed copyright message.  
 * 
 * Revision 4.0  1994/02/27  00:17:23  caress 
 * First cut at new version.  
 * 
 * Revision 1.2  1993/11/05  16:21:57  caress 
 * The graphical representation of the editable velocity profile 
 * now shows the layered model actually used for the raytracing.  
 * 
 * Revision 1.1 1993/08/16  23:28:30  caress 
 * Initial revision 
 * 
 * 
 */

/*--------------------------------------------------------------------*/

/* standard include files */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

/* MBIO include files */
#include "../../include/mb_format.h"
#include "../../include/mb_status.h"
#include "../../include/mb_define.h"
#include "../../include/mb_io.h"

/* velocity profile structure definition */
struct profile
	{
	int	n;
	char	name[128];
	double	*depth;
	double	*velocity;
	};
#define	MB_SSV_NO_USE	    0
#define	MB_SSV_CORRECT	    1
#define	MB_SSV_INCORRECT    2

/* id variables */
static char rcs_id[] = "$Id: mbvelocity_prog.c,v 4.23 2000-10-11 01:06:03 caress Exp $";
static char program_name[] = "MBVELOCITYTOOL";
static char help_message[] = "MBVELOCITYTOOL is an interactive water velocity profile editor  \nused to examine multiple water velocity profiles and to create  \nnew water velocity profiles which can be used for the processing  \nof multibeam sonar data.  In general, this tool is used to  \nexamine water velocity profiles obtained from XBTs, CTDs, or  \ndatabases, and to construct new profiles consistent with these  \nvarious sources of information.";
static char usage_message[] = "mbvelocitytool [-Byr/mo/da/hr/mn/sc -Eyr/mo/da/hr/mn/sc \n\t-Fformat -Ifile -Ssvpfile -Wsvpfile -V -H]";

/* status variables */
int	error = MB_ERROR_NO_ERROR;
int	verbose = 0;
char	*message = NULL;

/* mbvelocitytool control variables */
#define	MAX_PROFILES	10
#define	PICK_DISTANCE	50
struct profile	profile_display[MAX_PROFILES];
struct profile	profile_edit;
int	*edit_x = NULL;
int	*edit_y = NULL;
char	editfile[128];
int	edit = 0;
int	ndisplay = 0;
int	mbvt_xgid;
int	borders[4];
double	maxdepth = 3000.0;
double	velrange = 500.0;
double	resrange = 200.0;
double	ssv_start = 0.0;

/* plotting variables */
int	xmin, xmax, ymin, ymax;
double	xminimum, xmaximum, yminimum, ymaximum;
double	xscale, yscale;
int	xrmin, xrmax, yrmin, yrmax;
double	xrminimum, xrmaximum, yrminimum, yrmaximum;
double	xrscale, yrscale;
int	xpmin, xpmax, ypmin, ypmax;
double	xpminimum, xpmaximum, ypminimum, ypmaximum;
double	xpscale, ypscale;
int	active = -1;

/* default edit profile */
int	numedit = 15;
int	depthedit[15] = {   0,100, 300, 500, 
			  750, 1000, 
			 1500,2000,
			 2500, 3000,
			 4000, 5000, 
			 7000,9000, 
			 12000};
int	veledit[30] = { 1500, 1500, 1500, 1500, 1500,
			1500, 1500, 1500, 1500, 1500,
			1500, 1500, 1500, 1500, 1500 };
/*int	depthedit[30] = {   0, 50, 100, 200, 300, 400, 500, 
#			  600, 700, 800, 900, 1000, 
#			 1250, 1500, 1750, 2000,
#			 2250, 2500, 2750, 3000,
#			 3500, 4000, 4500, 5000, 
#			 6000, 7000, 8000, 9000, 
#			 10000, 12000};
#int	veledit[30] = { 1500, 1500, 1500, 1500, 1500,
#			1500, 1500, 1500, 1500, 1500,
#			1500, 1500, 1500, 1500, 1500,
#			1500, 1500, 1500, 1500, 1500,
#			1500, 1500, 1500, 1500, 1500,
#			1500, 1500, 1500, 1500, 1500 };*/

/* MBIO control parameters */
int	format;
int	pings;
int	lonflip;
double	bounds[4];
int	btime_i[7];
int	etime_i[7];
double	btime_d;
double	etime_d;
double	speedmin;
double	timegap;
int	beams_bath;
int	beams_amp;
int	pixels_ss;
char	*mbio_ptr;

/* mbio read and write values */
char	*store_ptr = NULL;
int	kind;
int	id;
int	nbeams;

/* buffer control variables */
#define	MBVT_BUFFER_SIZE	1000
char	*buff_ptr = NULL;
int	buffer_size = MBVT_BUFFER_SIZE;
int	nbuffer;
int	nload;

/* survey ping raytracing arrays */
int	time_i[7];
double	time_d;
double	navlon, navlat;
double	speed, heading;
int	nbath, namp, nss;
char	*beamflag = NULL;
double	*bath = NULL;
double	*amp = NULL;
double	*bathacrosstrack = NULL;
double	*bathalongtrack = NULL;
double	*ss = NULL;
double	*ssacrosstrack = NULL;
double	*ssalongtrack = NULL;
char	*comment = NULL;
double	*ttimes = NULL;
double	*angles = NULL;
double	*angles_forward = NULL;
double	*angles_null = NULL;
double	*heave = NULL;
double	*alongtrack_offset = NULL;
double	draft;
double	*p = NULL;
int	nraypathmax;
int	*nraypath;
double	**raypathx;
double	**raypathy;
double	*depth = NULL;
double	*acrosstrack = NULL;

/* depth range variables */
double	bath_min = 0.0;
double	bath_max = 0.0;

/* residual variables */
double	*angle;
double	*residual;
double	*res_sd;
int	*nresidual;

/* beam range variables */
int	beam_first = 0;
int	beam_last = 100;

/* color control values */
#define	WHITE	0	
#define	BLACK	1	
#define RED	2
#define GREEN	3
#define BLUE	4
#define CORAL	5
#define	XG_SOLIDLINE	0
#define	XG_DASHLINE	1
int	ncolors;
int	pixel_values[256];

/* system function declarations */
char	*ctime();
char	*getenv();

/*--------------------------------------------------------------------*/
/* Initialize the 'mbio' struct                                       */
/* Called by:                                                         */
/*                  main                                              */
/* Functions called:                                                  */
/*                  mb_defaults                                       */
/* Function returns:                                                  */
/*                  int status                                        */
/*--------------------------------------------------------------------*/
int mbvt_init(int argc, char **argv)
{
	/* local variables */
	char	*function_name = "mbvt_init";
	int	status = MB_SUCCESS;
	char	ifile[256], sfile[256], wfile[256];
	int	i;

	/* parsing variables */
	extern char *optarg;
	extern int optkind;
	int	errflg = 0;
	int	c;
	int	help = 0;
	int	flag = 0;

	/* set default values */
	status = mb_defaults(verbose,&format,&pings,&lonflip,bounds,
		btime_i,etime_i,&speedmin,&timegap);
	pings = 1;
	lonflip = 0;
	bounds[0] = -360.;
	bounds[1] = 360.;
	bounds[2] = -90.;
	bounds[3] = 90.;
	btime_i[0] = 1962;
	btime_i[1] = 2;
	btime_i[2] = 21;
	btime_i[3] = 10;
	btime_i[4] = 30;
	btime_i[5] = 0;
	etime_i[0] = 2062;
	etime_i[1] = 2;
	etime_i[2] = 21;
	etime_i[3] = 10;
	etime_i[4] = 30;
	etime_i[5] = 0;
	speedmin = 0.0;
	timegap = 1000000000.0;
	nbeams = 16;
	strcpy(ifile, "\0");
	strcpy(sfile, "\0");
	strcpy(wfile, "\0");

	/* process argument list */
	while ((c = getopt(argc, argv, "B:b:E:e:F:f:I:i:S:s:W:w:VvHh")) != -1)
	  switch (c) 
		{
		case 'H':
		case 'h':
			help++;
			break;
		case 'V':
		case 'v':
			verbose++;
			break;
		case 'B':
		case 'b':
			sscanf (optarg,"%d/%d/%d/%d/%d/%d",
				&btime_i[0],&btime_i[1],&btime_i[2],
				&btime_i[3],&btime_i[4],&btime_i[5]);
			btime_i[6] = 0;
			flag++;
			break;
		case 'E':
		case 'e':
			sscanf (optarg,"%d/%d/%d/%d/%d/%d",
				&etime_i[0],&etime_i[1],&etime_i[2],
				&etime_i[3],&etime_i[4],&etime_i[5]);
			etime_i[6] = 0;
			flag++;
			break;
		case 'F':
		case 'f':
			sscanf (optarg,"%d", &format);
			flag++;
			break;
		case 'I':
		case 'i':
			sscanf (optarg,"%s", ifile);
			flag++;
			break;
		case 'S':
		case 's':
			sscanf (optarg,"%s", sfile);
			flag++;
			break;
		case 'W':
		case 'w':
			sscanf (optarg,"%s", wfile);
			flag++;
			break;
		case '?':
			errflg++;
		}

	/* if error flagged then print it and exit */
	if (errflg)
		{
		fprintf(stderr,"usage: %s\n", usage_message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		error = MB_ERROR_BAD_USAGE;
		exit(error);
		}

	/* print starting message */
	if (verbose == 1 || help)
		{
		fprintf(stderr,"\nProgram %s\n",program_name);
		fprintf(stderr,"Version %s\n",rcs_id);
		fprintf(stderr,"MB-system Version %s\n",MB_VERSION);
		}

	/* print starting debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  Program <%s>\n",program_name);
		fprintf(stderr,"dbg2  Version %s\n",rcs_id);
		fprintf(stderr,"dbg2  MB-system Version %s\n",MB_VERSION);
		fprintf(stderr,"dbg2  Control Parameters:\n");
		fprintf(stderr,"dbg2       verbose:            %d\n",verbose);
		fprintf(stderr,"dbg2       help:               %d\n",help);
		fprintf(stderr,"dbg2       format:             %d\n",format);
		fprintf(stderr,"dbg2       input file:         %s\n",ifile);
		fprintf(stderr,"dbg2       display svp file:   %s\n",sfile);
		fprintf(stderr,"dbg2       edit svp file:      %s\n",wfile);
		}

	/* if help desired then print it and exit */
	if (help)
		{
		fprintf(stderr,"\n%s\n",help_message);
		fprintf(stderr,"\nusage: %s\n", usage_message);
		exit(error);
		}

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       argc:      %d\n",argc);
		for (i=0;i<argc;i++)
			fprintf(stderr,"dbg2       argv[%d]:    %s\n",
				i,argv[i]);
		}

	/* if files specified then use them at startup */
	if (strlen(wfile) > 0)
		{
		status = mbvt_open_edit_profile(wfile);
		}
	if (strlen(sfile) > 0)
		{
		status = mbvt_open_display_profile(sfile);
		}
	if (strlen(ifile) > 0)
		{
		status = mbvt_open_swath_file(ifile,format);
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:  %d\n",status);
		}

	/* return */
	return(status);
}

/*--------------------------------------------------------------------*/
/* exits the program - from "QUIT" on menu bar.                       */
/* Called by:                                                         */
/*                  mbvelocity_callbacks.c                            */
/* Functions called:                                                  */
/*                  mb_free                                           */
/*                  mb_memory_list                                    */
/* Function returns:                                                  */
/*                  int status                                        */
/*--------------------------------------------------------------------*/
int mbvt_quit()
{
	/* local variables */
	char	*function_name = "mbvt_quit";
	int	status;
	int	i;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		}

	/* deallocate previously loaded data, if any */
	if (nbuffer > 0)
		{
		mb_buffer_close(verbose,&buff_ptr,mbio_ptr,&error);
		mb_close(verbose,&mbio_ptr,&error);
		mb_free(verbose,&beamflag,&error);
		mb_free(verbose,&bath,&error);
		mb_free(verbose,&amp,&error);
		mb_free(verbose,&bathacrosstrack,&error);
		mb_free(verbose,&bathalongtrack,&error);
		mb_free(verbose,&ss,&error);
		mb_free(verbose,&ssacrosstrack,&error);
		mb_free(verbose,&ssalongtrack,&error);
		mb_free(verbose,&ttimes,&error);
		mb_free(verbose,&angles,&error);
		mb_free(verbose,&angles_forward,&error);
		mb_free(verbose,&angles_null,&error);
		mb_free(verbose,&heave,&error);
		mb_free(verbose,&alongtrack_offset,&error);
		mb_free(verbose,&nraypath,&error);
		for (i=0;i<beams_bath;i++)
			{
			mb_free(verbose,&raypathx[i],&error);
			mb_free(verbose,&raypathy[i],&error);
			}
		mb_free(verbose,&raypathx,&error);
		mb_free(verbose,&raypathy,&error);
		mb_free(verbose,&depth,&error);
		mb_free(verbose,&acrosstrack,&error);
		mb_free(verbose,&angle,&error);
		mb_free(verbose,&residual,&error);
		mb_free(verbose,&res_sd,&error);
		mb_free(verbose,&nresidual,&error);
		}

	/* check allocated memory */
	status = mb_memory_list(verbose,&error);

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:  %d\n",status);
		}

	/* return */
	return(status);
}

/*--------------------------------------------------------------------*/
/* mbvt_set_graphics - sets mbvt_xgrid to a pointer to the display    */
/*                     and sets borders.                              */
/* Called by:                                                         */
/*                  main                                              */
/* Functions called:                                                  */
/*                  none                                              */
/* Function returns:                                                  */
/*                  int status                                        */
/*--------------------------------------------------------------------*/
int mbvt_set_graphics(int xgid, int *brdr, int ncol, int *pixels)
{
	/* local variables */
	char	*function_name = "mbvt_set_graphics";
	int	status = MB_SUCCESS;
	int	i;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       xgid:         %d\n",xgid);
		for (i=0;i<4;i++)
			fprintf(stderr,"dbg2       borders[%d]:   %d\n",
				i,brdr[i]);
		fprintf(stderr,"dbg2       ncolors:      %d\n",ncol);
		for (i=0;i<ncol;i++)
			fprintf(stderr,"dbg2       pixel[%d]:     %d\n",
				i, pixels[i]);
		}

	/* set graphics id */
	mbvt_xgid = xgid;

	/* set graphics bounds */
	for (i=0;i<4;i++)
		borders[i] = brdr[i];

	/* set colors */
	ncolors = ncol;
	for (i=0;i<ncolors;i++)
		pixel_values[i] = pixels[i];

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:       %d\n",error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* Sets some of the mbio variables                                    */
/* Called by:                                                         */
/*                  main                                              */
/* Functions called:                                                  */
/*                  none                                              */
/* Function returns:                                                  */
/*                  int status                                        */
/*--------------------------------------------------------------------*/
int mbvt_get_values(int *s_edit, int *s_ndisplay, double *s_maxdepth,
	double *s_velrange, double *s_resrange, int *s_format)
{
	/* local variables */
	char	*function_name = "mbvt_get_values";
	int	status = MB_SUCCESS;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		}

	/* set values */
	*s_edit = edit;
	*s_ndisplay = ndisplay;
	*s_maxdepth = maxdepth;
	*s_velrange = velrange;
	*s_resrange = resrange;
	*s_format = format;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       s_edit:      %d\n",*s_edit);
		fprintf(stderr,"dbg2       s_ndisplay:  %d\n",*s_ndisplay);
		fprintf(stderr,"dbg2       s_maxdepth:  %f\n",*s_maxdepth);
		fprintf(stderr,"dbg2       s_velrange:  %f\n",*s_velrange);
		fprintf(stderr,"dbg2       s_resrange:  %f\n",*s_resrange);
		fprintf(stderr,"dbg2       s_format:    %d\n",*s_format);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}

/*--------------------------------------------------------------------*/
/* Sets some of the mbio variables.                                   */
/* Called by:                                                         */
/*                  action_maxdepth - mbvelocity_callbacks - slider   */
/*                  action_velrange - mbvelocity_callbacks - slider   */
/*                  action_residual_range - mbvelocity_callbacks      */
/* Functions called:                                                  */
/*                  none                                              */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_set_values(int s_edit, int s_ndisplay, 
		double s_maxdepth, double s_velrange, double s_resrange)
{
	/* local variables */
	char	*function_name = "mbvt_set_values";
	int	status = MB_SUCCESS;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       s_edit:      %d\n",s_edit);
		fprintf(stderr,"dbg2       s_ndisplay:  %d\n",s_ndisplay);
		fprintf(stderr,"dbg2       s_maxdepth:  %f\n",s_maxdepth);
		fprintf(stderr,"dbg2       s_velrange:  %f\n",s_velrange);
		fprintf(stderr,"dbg2       s_resrange:  %f\n",s_resrange);
		}

	/* set values */
	edit = s_edit;
	ndisplay = s_ndisplay;
	maxdepth = s_maxdepth;
	velrange = s_velrange;
	resrange = s_resrange;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function attempts to open a file as editable.                 */
/* Called by:                                                         */
/*                 open_file_ok in mbvelocity_callbacks.c. This is called */
/*		     by selecting the "OK" button in the file         */
/*		     selection widget for an editable file.           */
/* Functions called:                                                  */
/*                  mb_free                                           */
/*                  mb_malloc                                         */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_open_edit_profile(char *file)
{
	/* local variables */
	char	*function_name = "mbvt_open_edit_profile";
	int	status = MB_SUCCESS;
	int	size;
	char	buffer[128];
	char	*result;
	struct profile *profile;
	FILE	*fp;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       file:        %s\n",file);
		}

	/* get profile pointer */
	profile = &profile_edit;

	/* clear out old velocity data */
	if (edit == MB_YES)
		{
		edit = MB_NO;
		profile->n = 0;
		strcpy(profile->name,"\0");
		mb_free(verbose,&edit_x,&error);
		mb_free(verbose,&edit_y,&error);
		mb_free(verbose,&profile->depth,&error);
		mb_free(verbose,&profile->velocity,&error);
		}

	/* open the file if possible and count the velocity points */
	profile->n = 0;
	if ((fp = fopen(file, "r")) == NULL) 
		{
		status = MB_FAILURE;
		fprintf(stderr,"\nUnable to Open Velocity Profile File <%s> for reading\n",file);
		do_error_dialog("Unable to open input SVP file.", 
				"File may not exist or you may not have", 
				"read permission in this directory!");
		return(status);
		}
	while ((result = fgets(buffer,128,fp)) == buffer)
		if (buffer[0] != '#')
			profile->n++;
	fclose(fp);

	/* allocate space for the velocity profile and raytracing tables */
	size = profile->n*sizeof(int);
	status = mb_malloc(verbose,size,&edit_x,&error);
	status = mb_malloc(verbose,size,&edit_y,&error);
	size = profile->n*sizeof(double);
	status = mb_malloc(verbose,size,&(profile->depth),&error);
	status = mb_malloc(verbose,size,&(profile->velocity),&error);

	/* if error initializing memory then quit */
	if (error != MB_ERROR_NO_ERROR)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error allocating data arrays:\n%s\n",message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}

	/* open the file if possible and read the velocity points */
	profile->n = 0;
	strcpy(profile->name,file);
	if ((fp = fopen(file, "r")) == NULL) 
		{
		status = MB_FAILURE;
		fprintf(stderr,"\nUnable to Open Velocity Profile File <%s> for reading\n",file);
		return(status);
		}
	strncpy(buffer,"\0",sizeof(buffer));
	while ((result = fgets(buffer,128,fp)) == buffer)
		{
		if (buffer[0] != '#')
			{
			sscanf(buffer,"%lf %lf",
				&(profile->depth[profile->n]),
				&(profile->velocity[profile->n]));

			/* output some debug values */
			if (verbose >= 5)
				{
				fprintf(stderr,"\ndbg5  New velocity value read in program <%s>\n",program_name);
				fprintf(stderr,"dbg5       dep[%d]: %f  vel[%d]: %f\n",
					profile->n,
					profile->depth[profile->n],
					profile->n,
					profile->velocity[profile->n]);
				}
			profile->n++;
			}
		strncpy(buffer,"\0",sizeof(buffer));
		}
	fclose(fp);

	/* assume success */
	edit = MB_YES;
	status = MB_SUCCESS;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function displays a new editable profile.                     */
/* Called by:                                                         */
/*                  action_new_profile in mbvelocity_callbacks.c      */
/*		      which is called by selecting the "NEW EDITABLE  */
/*                    PROFILE" from the "FILE" pulldown menu.         */
/*		    - also called from mbvt_process_multibeam()	      */
/*		      if no editable profile already exists.	      */
/* Functions called:                                                  */
/*                  mb_free                                           */
/*		    mb_malloc                                         */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_new_edit_profile()
{
	/* local variables */
	char	*function_name = "mbvt_new_edit_profile";
	int	status = MB_SUCCESS;
	struct profile *profile;
	int	size;
	double	dz;
	int	i;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		}

	/* get profile pointer */
	profile = &profile_edit;

	/* clear out old velocity data */
	if (edit == MB_YES)
		{
		edit = MB_NO;
		profile->n = 0;
		strcpy(profile->name,"\0");
		mb_free(verbose,&edit_x,&error);
		mb_free(verbose,&edit_y,&error);
		mb_free(verbose,&profile->depth,&error);
		mb_free(verbose,&profile->velocity,&error);
		}

	/* allocate space for the velocity profile and raytracing tables */
	profile->n = numedit;
	size = profile->n*sizeof(int);
	status = mb_malloc(verbose,size,&edit_x,&error);
	status = mb_malloc(verbose,size,&edit_y,&error);
	size = profile->n*sizeof(double);
	profile->depth = NULL;
	profile->velocity = NULL;
	status = mb_malloc(verbose,size,&(profile->depth),&error);
	status = mb_malloc(verbose,size,&(profile->velocity),&error);

	/* if error initializing memory then quit */
	if (error != MB_ERROR_NO_ERROR)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error allocating data arrays:\n%s\n",message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}

	/* copy the default values */
	strcpy(profile->name,"new");
	if (bath_max > bath_min && bath_max < 2000)
		{
		dz = 1.25 * bath_max / (profile->n - 2);
		for (i=0;i<profile->n-1;i++)
			{
			profile->depth[i] = i * dz;
			profile->velocity[i] = veledit[i];
			}
		profile->depth[profile->n-1] 
			= depthedit[profile->n-1];
		profile->velocity[profile->n-1] 
			= veledit[profile->n-1];
		}
	else
		{
		for (i=0;i<profile->n;i++)
			{
			profile->depth[i] = depthedit[i];
			profile->velocity[i] = veledit[i];
			}
		}

	/* assume success */
	edit = MB_YES;
	status = MB_SUCCESS;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function saves the editable profile.                          */
/* Called by:                                                         */
/*                  controls_save_file in mbvelocity_callbacks.c. It is   */
/*                    called when the user selects "OK" from the      */
/*                    save editable file widget.                      */
/* Functions called:                                                  */
/*                  none                                              */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_save_edit_profile(char *file)
{
	/* local variables */
	char	*function_name = "mbvt_save_edit_profile";
	int	status = MB_SUCCESS;
	struct profile *profile;
	FILE	*fp;
	int	i;

	/* time, user, host variables */
	time_t	right_now;
	char	date[25], user[128], *user_ptr, host[128];

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       file:        %s\n",file);
		}

	/* get profile pointer */
	profile = &profile_edit;

	/* open the file if possible */
	if ((fp = fopen(file, "w")) == NULL) 
		{
		status = MB_FAILURE;
		fprintf(stderr,"\nUnable to Open Output Velocity Profile File <%s> for writing\n",file);
		do_error_dialog("Unable to open output file.", 
				"You may not have write", 
				"permission in this directory!");
		return(status);
		}

	/* write comments */
	fprintf(fp,"# Water velocity profile created by program %s\n",
		program_name);
	fprintf(fp,"# Version %s\n",rcs_id);
	strncpy(date,"\0",25);
	right_now = time((time_t *)0);
	strncpy(date,ctime(&right_now),24);
	if ((user_ptr = getenv("USER")) == NULL)
		user_ptr = getenv("LOGNAME");
	if (user_ptr != NULL)
		strcpy(user,user_ptr);
	else
		strcpy(user, "unknown");
	gethostname(host,128);
	fprintf(fp,"# Run by user <%s> on cpu <%s> at <%s>\n",
		user,host,date);

	/* write the profile */
	for (i=0;i<profile->n;i++)
		fprintf(fp,"%f %f\n",profile->depth[i],
			profile->velocity[i]);

	/* close the file */
	fclose(fp);

	/* assume success */
	edit = MB_YES;
	status = MB_SUCCESS;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function reads the data in the requested display file.        */
/* Called by:                                                         */
/*                  open_file_ok in mbvelocity_callbacks.c                */
/* Functions called:                                                  */
/*                  mb_malloc                                         */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_open_display_profile(char *file)
{
	/* local variables */
	char	*function_name = "mbvt_open_display_profile";
	int	status = MB_SUCCESS;
	char	buffer[128];
	char	*result;
	struct profile *profile;
	FILE	*fp;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       file:        %s\n",file);
		}

	/* check that there is room for this data */
	if (ndisplay >= MAX_PROFILES)
		{
		status = MB_FAILURE;
		fprintf(stderr,"\nNo room for another display velocity profile\n");
		do_error_dialog("Unable to open input SVP file.", 
				"There is no room for another", 
				"display SVP!");
		return(status);
		}

	/* get profile pointer */
	profile = &profile_display[ndisplay];

	/* open the file if possible and count the velocity points */
	profile->n = 0;
	if ((fp = fopen(file, "r")) == NULL) 
		{
		status = MB_FAILURE;
		fprintf(stderr,"\nUnable to Open Velocity Profile File <%s> for reading\n",file);
		do_error_dialog("Unable to open input SVP file.", 
				"File may not exist or you may not have", 
				"read permission in this directory!");
		return(status);
		}
	while ((result = fgets(buffer,128,fp)) == buffer)
		if (buffer[0] != '#')
			profile->n++;
	fclose(fp);

	/* allocate space for the velocity profile and raytracing tables */
	profile->depth = NULL;
	profile->velocity = NULL;
	status = mb_malloc(verbose,profile->n*sizeof(double),
		&(profile->depth),&error);
	status = mb_malloc(verbose,profile->n*sizeof(double),
		&(profile->velocity),&error);

	/* if error initializing memory then quit */
	if (error != MB_ERROR_NO_ERROR)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error allocating data arrays:\n%s\n",message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}

	/* open the file if possible and read the velocity points */
	profile->n = 0;
	strcpy(profile->name,file);
	if ((fp = fopen(file, "r")) == NULL) 
		{
		status = MB_FAILURE;
		fprintf(stderr,"\nUnable to Open Velocity Profile File <%s> for reading\n",file);
		return(status);
		}
	strncpy(buffer,"\0",sizeof(buffer));
	while ((result = fgets(buffer,128,fp)) == buffer)
		{
		if (buffer[0] != '#')
			{
			sscanf(buffer,"%lf %lf",
				&(profile->depth[profile->n]),
				&(profile->velocity[profile->n]));

			/* output some debug values */
			if (verbose >= 5)
				{
				fprintf(stderr,"\ndbg5  New velocity value read in program <%s>\n",program_name);
				fprintf(stderr,"dbg5       dep[%d]: %f  vel[%d]: %f\n",
					profile->n,
					profile->depth[profile->n],
					profile->n,
					profile->velocity[profile->n]);
				}
			profile->n++;
			}
		strncpy(buffer,"\0",sizeof(buffer));
		}
	fclose(fp);

	/* assume success */
	ndisplay++;
	status = MB_SUCCESS;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function is used to keep track of the display profiles that   */
/*   are being displayed so the user can remove unwanted displayed.   */
/*   This feature is not functional at this time.                     */
/* Called by:                                                         */
/*                  mbvelocitytool_set_menu which is not being used   */
/*		      at this time.                                   */
/* Functions called:                                                  */
/*                  none                                              */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_get_display_names(int *nlist, char *list[MAX_PROFILES])
{
	/* local variables */
	char	*function_name = "mbvt_get_display_names";
	int	status = MB_SUCCESS;
	int	i;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       list:        %d\n",list);
		}

	/* set values */
	*nlist = ndisplay;
	for (i=0;i<*nlist;i++)
		list[i] = profile_display[i].name;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       nlist:       %d\n",*nlist);
		for (i=0;i<*nlist;i++)
			fprintf(stderr,"dbg2       name[%d]: %s\n",i,list[i]);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function is used to delete an unwanted display profile from   */
/*   the screen. This feature is not in use at this time.             */
/* Called by:                                                         */
/*                  none - not in use                                 */
/* Functions called:                                                  */
/*                  mb_free                                           */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_delete_display_profile(int select)
{
	/* local variables */
	char	*function_name = "mbvt_delete_display_profile";
	struct profile *profile;
	int	status = MB_SUCCESS;
	int	i, j;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       select:      %d\n",select);
		}

	/* check if reasonable */
	if (select < ndisplay)
		{
		/* remove selected profile */
		profile = &profile_display[select];
		profile->n = 0;
		strcpy(profile->name,"\0");
		mb_free(verbose,&profile->depth,&error);
		mb_free(verbose,&profile->velocity,&error);

		/* reorder remaining profiles */
		for (i=select+1;i<ndisplay;i++)
			{
			profile_display[i-1].n = profile_display[i].n;
			strcpy(profile_display[i-1].name,
				profile_display[i].name);
			for (j=0;j<profile_display[i-1].n;j++)
				{
				profile_display[i-1].depth[j] 
					= profile_display[i].depth[j];
				profile_display[i-1].velocity[j] 
					= profile_display[i].velocity[j];
				}
			}
		ndisplay--;
		}
	else
		status = MB_FAILURE;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This is the main plotting function. It does all the drawing in the */
/*   canvas part of the screen.                                       */
/* Called by:                                                         */
/*                  main                                              */
/*                  display_menu                                      */
/*                  action_new_profile                                */
/*                  controls_save_file                                */
/*                  open_file_ok                                      */
/*                  action_maxdepth                                   */
/*                  action_menu_close_profile                         */
/*                  action_velrange                                   */
/*                  action_canvas_event                               */
/*                  action_residual_range                             */
/*                  action_process_mb                                 */
/*                  mbvt_open_swath_file                         */
/* Functions called:                                                  */
/*                  xg_setclip                                        */
/*                  xg_fillrectangle                                  */
/*                  xg_drawline                                       */
/* 		    xg_justify                                        */
/* 		    xg_drawstring                                     */
/* 		    xg_drawrectangle                                  */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_plot()
{
	/* local variables */
	char	*function_name = "mbvt_plot";
	struct profile *profile;
	int	status = MB_SUCCESS;

	/* plotting variables */
	int	margin;
	int	xcen, ycen;
	double	deltax, deltay;
	int	nx_int, ny_int;
	int	x_int, y_int;
	int	xrcen, yrcen;
	double	deltaxr, deltayr;
	int	nxr_int, nyr_int;
	int	xr_int, yr_int;
	int	xpcen, ypcen;
	double	deltaxp, deltayp;
	int	nxp_int, nyp_int;
	int	xp_int, yp_int;
	int	xx, yy;
	int	yyl, yyu;
	double	vx, vy;
	int	xxo, yyo;
	int	swidth, sascent, sdescent;
	char	string[128];
	char	format_str[10];
	int	color;
	int	i, j;

	struct xg_graphic *graphic;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr," borders[0] = %d\n", borders[0]);
		fprintf(stderr," borders[1] = %d\n", borders[1]);
		fprintf(stderr," borders[2] = %d\n", borders[2]);
		fprintf(stderr," borders[3] = %d\n", borders[3]);
		fprintf(stderr," mbvt_xgid = %d\n", mbvt_xgid);
		}

	/* turn clipp mask back to whole canvas */
        /* set clip rectangle */

	xg_setclip(mbvt_xgid,borders[0],borders[2],
		borders[1]-borders[0],borders[3]-borders[2]);

	/* clear screen */
	xg_fillrectangle(mbvt_xgid,borders[0],borders[2],
		borders[1]-borders[0],borders[3]-borders[2],
		pixel_values[WHITE],XG_SOLIDLINE);

	/* set scaling */
	margin = (borders[3] - borders[2])/15;
	xmin = 2.25*margin;
	xmax = 0.5*(borders[1] - borders[0]) - margin;
	ymin = margin;
	ymax = 0.5*(borders[3] - borders[2]);
	xcen = xmin + (xmax - xmin)/2;
	ycen = ymin + (ymax - ymin)/2;
	xminimum = 1490.0 - velrange/2;
	xmaximum = 1490.0 + velrange/2;
	deltax = 0.15*(xmaximum - xminimum);
	xscale = (xmax - xmin)/(xmaximum - xminimum);
	x_int = deltax*xscale;
	nx_int = (xmaximum - xminimum)/deltax + 1;
	yminimum = 0.0;
	ymaximum = maxdepth;
	deltay = 0.1*(ymaximum - yminimum);
	yscale = (ymax - ymin)/(ymaximum - yminimum);	
	y_int = deltay*yscale;
	ny_int = (ymaximum - yminimum)/deltay + 1;

	/* plot grid */
	xg_drawline(mbvt_xgid,xmin,ymin,xmin,ymax,
		pixel_values[BLACK],XG_SOLIDLINE);
	xg_drawline(mbvt_xgid,xmax,ymin,xmax,ymax,
		pixel_values[BLACK],XG_SOLIDLINE);
	for (i=0;i<nx_int;i++)
		{
		xx = xmin + i*x_int;
		vx = xminimum + i*deltax;
		xg_drawline(mbvt_xgid,xx,ymin,xx,ymax,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,"%.1f",vx);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xx-swidth/2,
			ymax+sascent+5,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		}
	xg_drawline(mbvt_xgid,xmin,ymin,xmax,ymin,
			pixel_values[BLACK],XG_SOLIDLINE);
	xg_drawline(mbvt_xgid,xmin,ymax,xmax,ymax,
			pixel_values[BLACK],XG_SOLIDLINE);
	for (i=0;i<ny_int;i++)
		{
		yy = ymin + i*y_int;
		vy = yminimum + i*deltay;
		xg_drawline(mbvt_xgid,xmin,yy,xmax,yy,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,"%.1f",vy);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xmin-swidth-5,
			yy+sascent/2,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		}
	strcpy(string,"Water Sound Velocity Profiles");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xcen-swidth/2,
		ymin-2*sascent+10,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"Water Sound Velocity (m/s)");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xcen-swidth/2,
		ymax+2*sascent+10,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"Depth");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xmin-2*swidth-10,
		ycen-sascent,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"(m)");
	xg_drawstring(mbvt_xgid,xmin-2*swidth,
		ycen+sascent,string,
		pixel_values[BLACK],XG_SOLIDLINE);

	/* turn clipping on */

	xg_setclip(mbvt_xgid,xmin,ymin,(xmax-xmin),(ymax-ymin));

	/* plot display profiles */
	for (i=0;i<ndisplay;i++)
	  {
	  color = i%3 + 2;
	  for (j=0;j<profile_display[i].n;j++)
		{
		xx = xmin + (profile_display[i].velocity[j] - xminimum)*xscale;
		yy = ymin + (profile_display[i].depth[j] - yminimum)*yscale;
/*		xg_drawrectangle(mbvt_xgid, xx-2, yy-2, 4, 4,
			pixel_values[color],XG_SOLIDLINE);*/
		if (j > 0)
			xg_drawline(mbvt_xgid,xxo,yyo,xx,yy,
				pixel_values[color],XG_SOLIDLINE);
		xxo = xx;
		yyo = yy;
		}
	  }

	/* plot edit profile */
	if (edit == MB_YES)
	  {
	  for (j=0;j<profile_edit.n;j++)
		{
		xx = xmin + (profile_edit.velocity[j] - xminimum)*xscale;
		yy = ymin + (profile_edit.depth[j] - yminimum)*yscale;
		xg_fillrectangle(mbvt_xgid, xx-2, yy-2, 4, 4, 
			pixel_values[BLACK],XG_SOLIDLINE);
		if (j > 0)
			{
			xg_drawline(mbvt_xgid,xxo,yyo,xx,yy,
				pixel_values[BLACK],XG_SOLIDLINE);
			}
		xxo = xx;
		yyo = yy;
		edit_x[j] = xx;
		edit_y[j] = yy;
		}
	  }

	/* now plot grid for Bathymetry Residuals */
	/* turn clip mask back to whole canvas */

	xg_setclip(mbvt_xgid,borders[0],borders[2],
		borders[1]-borders[0],borders[3]-borders[2]);

	/* set scaling */
	xrmin = 0.5*(borders[1] - borders[0]) + 2*margin;
	xrmax = borders[1] - 0.5*margin;
	yrmin = margin;
	yrmax = 0.5*(borders[3] - borders[2]);
	xrcen = xrmin + (xrmax - xrmin)/2;
	yrcen = yrmin + (yrmax - yrmin)/2;
	xrminimum = beam_first - 1.0;
	xrmaximum = beam_last + 1.0;
	deltaxr = (int) (0.1*(xrmaximum - xrminimum));
	xrscale = (xrmax - xrmin)/(xrmaximum - xrminimum);
	xr_int = deltaxr*xrscale;
	nxr_int = (xrmaximum - xrminimum)/deltaxr + 1;
	yrminimum = -resrange;
	yrmaximum = resrange;
	deltayr = 0.1*(yrmaximum - yrminimum);
	yrscale = (yrmax - yrmin)/(yrmaximum - yrminimum);	
	yr_int = deltayr*yrscale;
	nyr_int = (yrmaximum - yrminimum)/deltayr/2 + 1;

	/* plot grid */
	xg_drawline(mbvt_xgid,xrmin,yrmin,xrmin,yrmax,
		pixel_values[BLACK],XG_SOLIDLINE);
	xg_drawline(mbvt_xgid,xrmax,yrmin,xrmax,yrmax,
		pixel_values[BLACK],XG_SOLIDLINE);
	for (i=0;i<nxr_int;i++)
		{
		xx = xrmin + i*xr_int;
		vx = xrminimum + i*deltaxr;
		xg_drawline(mbvt_xgid,xx,yrmin,xx,yrmax,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,"%.0f",vx);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xx-swidth/2,
			yrmax+sascent+5,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		}
	xg_drawline(mbvt_xgid,xrmin,yrmin,xrmax,yrmin,
		pixel_values[BLACK],XG_SOLIDLINE);
	xg_drawline(mbvt_xgid,xrmin,yrmax,xrmax,yrmax,
		pixel_values[BLACK],XG_SOLIDLINE);
	if (resrange > 100.0)
		sprintf(format_str, "%s", "%.0f");
	else if (resrange > 10.0)
		sprintf(format_str, "%s", "%.1f");
	else
		sprintf(format_str, "%s", "%.2f");
	for (i=0;i<nyr_int;i++)
		{
		yy = yrcen + i*yr_int;
		vy = i*deltayr;
		xg_drawline(mbvt_xgid,xrmin,yy,xrmax,yy,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,format_str,vy);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xrmin-swidth-5,
			yy+sascent/2,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		yy = yrcen - i*yr_int;
		vy = -i*deltayr;
		xg_drawline(mbvt_xgid,xrmin,yy,xrmax,yy,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,format_str,vy);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xrmin-swidth-5,
			yy+sascent/2,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		}
	if (nbuffer > 0)
		{
		sprintf(string,"Depth Range:  minimum: %5.0f m   maximum: %5.0f m",
			bath_min, bath_max);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,
			borders[1]/2-swidth/2,
			yrmin-4*sascent+14,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		}
	strcpy(string,"Swath Bathymetry Beam Residuals");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xrcen-swidth/2,
		yrmin-2*sascent+10,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"Bathymetry Beam Number");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xrcen-swidth/2,
		yrmax+2*sascent+10,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"Residual");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xrmin-swidth-30,
		yrcen-sascent,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"(m)");
	xg_drawstring(mbvt_xgid,xrmin-swidth-10,
		yrcen+sascent,string,
		pixel_values[BLACK],XG_SOLIDLINE);

	/* turn clipping on for residual plot box */
	xg_setclip(mbvt_xgid,xrmin,yrmin,(xrmax-xrmin),(yrmax-yrmin));

	/* plot residuals */
	if (nbuffer > 0)
	  for (i=0;i<nbeams;i++)
	    {
	    if (nresidual[i] > 0)
		{
		xx = xrmin + (i - xrminimum)*xrscale;
		yy = yrmin + (residual[i] - yrminimum) * yrscale;
		yyl = yrmin + (residual[i] - res_sd[i] - yrminimum) 
				* yrscale;
		yyu = yrmin + (residual[i] + res_sd[i] - yrminimum) 
				* yrscale;
		xg_fillrectangle(mbvt_xgid, xx-2, yy-2, 4, 4,
			pixel_values[BLACK],XG_SOLIDLINE);
		xg_drawline(mbvt_xgid,xx,yyl,xx,yyu,
			pixel_values[BLACK],XG_SOLIDLINE);
		if (i > 0 && nresidual[i-1] > 0)
			xg_drawline(mbvt_xgid,xxo,yyo,xx,yy,
				pixel_values[BLACK],XG_SOLIDLINE);
		xxo = xx;
		yyo = yy;
		}
	     }

	/* now plot grid for raypaths */
	/* turn clip mask back to whole canvas */

	xg_setclip(mbvt_xgid,borders[0],borders[2],
		borders[1]-borders[0],borders[3]-borders[2]);

	/* set scaling */
	xpmin = 2.25*margin;
	xpmax = borders[1] - 0.5*margin;
	ypmin = 0.5*(borders[3] - borders[2]) + 1.5*margin;
	ypmax = ypmin + (xpmax - xpmin)/5.0;
	xpcen = xpmin + (xpmax - xpmin)/2;
	ypcen = ypmin + (ypmax - ypmin)/2;
	xpminimum = -2.25*maxdepth;
	xpmaximum = 2.25*maxdepth;
	deltaxp = 0.4*maxdepth;
	xpscale = (xpmax - xpmin)/(xpmaximum - xpminimum);
	xp_int = deltaxp*xpscale;
	nxp_int = (xpmaximum - xpminimum)/deltaxp/2 + 1;
	ypminimum = 0.0;
	ypmaximum = maxdepth;
	deltayp = 0.2*(ypmaximum - ypminimum);
	ypscale = (ypmax - ypmin)/(ypmaximum - ypminimum);	
	yp_int = deltayp*ypscale;
	nyp_int = (ypmaximum - ypminimum)/deltayp + 1;

	/* plot grid */
	xg_drawline(mbvt_xgid,xpmin,ypmin,xpmin,ypmax,
		pixel_values[BLACK],XG_SOLIDLINE);
	xg_drawline(mbvt_xgid,xpmax,ypmin,xpmax,ypmax,
		pixel_values[BLACK],XG_SOLIDLINE);
	for (i=0;i<nxp_int;i++)
		{
		xx = xpcen + i*xp_int;
		vx = i*deltaxp;
		xg_drawline(mbvt_xgid,xx,ypmin,xx,ypmax,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,"%.1f",vx);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xx-swidth/2,
			ypmax+sascent+5,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		xx = xpcen - i*xp_int;
		vx = -i*deltaxp;
		xg_drawline(mbvt_xgid,xx,ypmin,xx,ypmax,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,"%.1f",vx);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xx-swidth/2,
			ypmax+sascent+5,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		}
	xg_drawline(mbvt_xgid,xpmin,ypmin,xpmax,ypmin,
		pixel_values[BLACK],XG_SOLIDLINE);
	xg_drawline(mbvt_xgid,xpmin,ypmax,xpmax,ypmax,
		pixel_values[BLACK],XG_SOLIDLINE);
	for (i=0;i<nyp_int;i++)
		{
		yy = ypmin + i*yp_int;
		vy = ypminimum + i*deltayp;
		xg_drawline(mbvt_xgid,xpmin,yy,xpmax,yy,
			pixel_values[BLACK],XG_DASHLINE);
		sprintf(string,"%.1f",vy);
		xg_justify(mbvt_xgid,string,&swidth,
			&sascent,&sdescent);
		xg_drawstring(mbvt_xgid,xpmin-swidth-5,
			yy+sascent/2,string,
			pixel_values[BLACK],XG_SOLIDLINE);
		}
	strcpy(string,"Raypaths");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xpcen-swidth/2,
		ypmin-2*sascent+10,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"Acrosstrack Distance (m)");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xpcen-swidth/2,
		ypmax+2*sascent+10,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"Depth");
	xg_justify(mbvt_xgid,string,&swidth,
		&sascent,&sdescent);
	xg_drawstring(mbvt_xgid,xpmin-2*swidth-10,
		ypcen-sascent,string,
		pixel_values[BLACK],XG_SOLIDLINE);
	strcpy(string,"(m)");
	xg_drawstring(mbvt_xgid,xpmin-2*swidth,
		ypcen+sascent,string,
		pixel_values[BLACK],XG_SOLIDLINE);

	/* turn clipping on for raypath plot box */
	xg_setclip(mbvt_xgid,xpmin,ypmin,(xpmax-xpmin),(ypmax-ypmin));

	/* plot raypaths */
	if (nbuffer > 0)
	  for (i=0;i<nbeams;i++)
	    {
	    if (nraypath[i] > 0)
		{
		xxo = xpmin + (raypathx[i][0] - xpminimum)*xpscale;
		yyo = ypmin + (raypathy[i][0] - ypminimum)*ypscale;
		for (j=1;j<nraypath[i];j++)
			{
			xx = xpmin + (raypathx[i][j] - xpminimum)*xpscale;
			yy = ypmin + (raypathy[i][j] - ypminimum)*ypscale;
			xg_drawline(mbvt_xgid,xxo,yyo,xx,yy,
				pixel_values[BLACK],XG_SOLIDLINE);
			xxo = xx;
			yyo = yy;
			}
		xg_fillrectangle(mbvt_xgid, xx-2, yy-2, 4, 4,
			pixel_values[RED],XG_SOLIDLINE);
		}
	     }

	/* turn clipping on for velocity profile box */
	xg_setclip(mbvt_xgid,xmin,ymin,(xmax-xmin),(ymax-ymin));

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function is called when the right mouse button is pressed     */
/*   when it is in the canvas area. It finds the mouse location so    */
/*   the program knows which editable point to move.                  */
/* Called by:                                                         */
/*                  action_canvas_event                               */
/* Functions called:                                                  */
/*                  none                                              */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_action_mouse_down(int x, int y)
{
	/* local variables */
	char	*function_name = "mbvt_action_mouse_down";
	int	status = MB_SUCCESS;
	double	distance, distance_min;
	int	i;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       x:            %d\n",x);
		fprintf(stderr,"dbg2       y:            %d\n",y);
		}

	/* select node if possible */
	if (x >= xmin && x <= xmax && y >= ymin && y <= ymax)
		{

		/* find the closest node */
		distance_min = 20000.0;
		active = -1;
		for (i=0;i<profile_edit.n;i++)
			{
			distance = (edit_x[i] - x)*(edit_x[i] - x) 
				+ (edit_y[i] - y)*(edit_y[i] - y);
			if (distance < distance_min)
				{
				distance_min = distance;
				active = i;
				}
			}
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function is called when the right mouse button is released.   */
/*   The only thing it really does is set the active flag to -1.      */
/* Called by:                                                         */
/*                  action_canvas_event                               */
/* Functions called:                                                  */
/*                  none                                              */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_action_mouse_up(int x, int y)
{
	/* local variables */
	char	*function_name = "mbvt_action_mouse_up";
	int	status = MB_SUCCESS;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       x:            %d\n",x);
		fprintf(stderr,"dbg2       y:            %d\n",y);
		}

	/* relocate and deselect node if selected */
	if (active > 0)
		{
		active = -1;
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function is continuously called as long as the left mouse is  */
/*   is depressed. It moves the selected point with elastic lines     */
/*   untill the button is released.                                   */
/* Called by:                                                         */
/*                  action_canvas_event                               */
/* Functions called:                                                  */
/*                  xg_fillrectangle                                  */
/*                  xg_drawline                                       */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_action_mouse_drag(int x, int y)
{
	/* local variables */
	char	*function_name = "mbvt_action_mouse_drag";
	int	status = MB_SUCCESS;
	int	ylim_min, ylim_max;
	int     xh, yh, xp, yp, yh2, yp2, x_prev, y_prev;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input values:\n");
		fprintf(stderr,"dbg2       x:            %d\n",x);
		fprintf(stderr,"dbg2       y:            %d\n",y);
		}

	/* relocate node if selected */
	if (active > -1 && 
		x >= xmin && x <= xmax && y >= ymin && y <= ymax)
		{
		/* find upper and lower bounds for current node */
		if (active == 0)
			ylim_min = ymin;
		else
			ylim_min = edit_y[active - 1];
		if (active == profile_edit.n - 1)
			ylim_max = ymax;
		else
			ylim_max = edit_y[active + 1];

		/* get new location */
		if (x <= xmin)
			x = xmin + 1;
		if (x >= xmax)
			x = xmax - 1;
		if (y <= ylim_min)
			y = ylim_min + 1;
		if (y >= ylim_max)
			y = ylim_max;
		if (active == 0)
			y = ymin;

		/* unplot the current ping */
		xg_fillrectangle(mbvt_xgid, edit_x[active]-2, 
			edit_y[active]-2, 4, 4,
			pixel_values[WHITE],XG_SOLIDLINE);
		if (active > 0)
			{
			xg_drawline(mbvt_xgid,
				edit_x[active-1],edit_y[active-1],
				edit_x[active],edit_y[active],
				pixel_values[WHITE],XG_SOLIDLINE);
			}
		if (active < profile_edit.n - 1)
			{
			xg_drawline(mbvt_xgid,
				edit_x[active],edit_y[active],
				edit_x[active+1],edit_y[active+1],
				pixel_values[WHITE],XG_SOLIDLINE);
			}

		/* get new location and velocity values */
		edit_x[active] = x;
		edit_y[active] = y;
		profile_edit.velocity[active] = (x - xmin)/xscale + xminimum;
		profile_edit.depth[active] = (y - ymin)/yscale + yminimum;

		/* replot the current svp */
		if (active > 0)
			{
			xg_drawline(mbvt_xgid,
				edit_x[active-1],edit_y[active-1],
				edit_x[active],edit_y[active],
				pixel_values[BLACK],XG_SOLIDLINE);
			}
		if (active < profile_edit.n - 1)
			{
			xg_drawline(mbvt_xgid,
				edit_x[active],edit_y[active],
				edit_x[active+1],edit_y[active+1],
				pixel_values[BLACK],XG_SOLIDLINE);
			}
		if (active > 0)
			xg_fillrectangle(mbvt_xgid, edit_x[active-1]-2, 
				edit_y[active-1]-2, 4, 4, 
				pixel_values[BLACK],XG_SOLIDLINE);
		xg_fillrectangle(mbvt_xgid, edit_x[active]-2, 
			edit_y[active]-2, 4, 4, 
			pixel_values[BLACK],XG_SOLIDLINE);
		if (active < profile_edit.n - 1)
			xg_fillrectangle(mbvt_xgid, edit_x[active+1]-2, 
				edit_y[active+1]-2, 4, 4, 
				pixel_values[BLACK],XG_SOLIDLINE);
		}
	else
		status = MB_FAILURE;

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
/* This function reads the data from the multibeam file and makes    */
/*   the calls to display it.                                         */
/* Called by:                                                         */
/*                  action_process_mb                                 */
/*                  open_file_ok                                      */
/* Functions called:                                                  */
/*                  mb_malloc                                         */
/*                  mb_format                                         */
/*                  mb_buffer_close                                   */
/*                  mb_close                                          */
/*                  mb_free                                           */
/*                  mb_read_init                                      */
/*                  mb_error                                          */
/*                  mb_buffer_init                                    */
/*                  mb_buffer_load                                    */
/*                  mbvt_setup_raytracing                             */
/*                  mbvt_process_multibeam                           */
/*                  mbvt_plot                                         */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_open_swath_file(char *file, int form)
{
	/* local variables */
	char	*function_name = "mbvt_open_swath_file";
	int	status = MB_SUCCESS;
	int	format_num;
	struct mb_buffer_struct *buff;
	int	kind;
	double	navlon_levitus, navlat_levitus;
	double	ssv;
	char	command[64];
	int	i, j, k;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       file:        %s\n",file);
		fprintf(stderr,"dbg2       format:      %d\n",form);
		}

	/* check for format with travel time data */
	format = form;
	status = mb_format(verbose,&format,&format_num,&error);
	if (mb_traveltime_table[format_num] != MB_YES)
		{
		fprintf(stderr,"\nProgram <%s> requires travel time data.\n",program_name);
		fprintf(stderr,"Format %d is unacceptable because it does not include travel time data.\n",format);
		fprintf(stderr,"\nSwath Sonar File <%s> not initialized for reading\n",file);
		status = MB_FAILURE;
		do_error_dialog("Data loading aborted.", 
				"The specified swath data format does", 
				"not include travel time data!");
		return(status);
		}

	/* deallocate previously loaded data, if any */
	if (nbuffer > 0)
		{
		mb_buffer_close(verbose,&buff_ptr,mbio_ptr,&error);
		mb_close(verbose,&mbio_ptr,&error);
		mb_free(verbose,&beamflag,&error);
		mb_free(verbose,&bath,&error);
		mb_free(verbose,&amp,&error);
		mb_free(verbose,&bathacrosstrack,&error);
		mb_free(verbose,&bathalongtrack,&error);
		mb_free(verbose,&ss,&error);
		mb_free(verbose,&ssacrosstrack,&error);
		mb_free(verbose,&ssalongtrack,&error);
		mb_free(verbose,&ttimes,&error);
		mb_free(verbose,&angles,&error);
		mb_free(verbose,&angles_forward,&error);
		mb_free(verbose,&angles_null,&error);
		mb_free(verbose,&heave,&error);
		mb_free(verbose,&alongtrack_offset,&error);
		mb_free(verbose,&nraypath,&error);
		for (i=0;i<beams_bath;i++)
			{
			mb_free(verbose,&raypathx[i],&error);
			mb_free(verbose,&raypathy[i],&error);
			}
		mb_free(verbose,&raypathx,&error);
		mb_free(verbose,&raypathy,&error);
		mb_free(verbose,&depth,&error);
		mb_free(verbose,&acrosstrack,&error);
		mb_free(verbose,&angle,&error);
		mb_free(verbose,&residual,&error);
		mb_free(verbose,&res_sd,&error);
		mb_free(verbose,&nresidual,&error);
		}

	/* initialize reading the input multibeam file */
	format = form;
	if ((status = mb_read_init(
		verbose,file,format,pings,lonflip,bounds,
		btime_i,etime_i,speedmin,timegap,
		&mbio_ptr,&btime_d,&etime_d,
		&beams_bath,&beams_amp,&pixels_ss,&error)) != MB_SUCCESS)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error returned from function <mb_read_init>:\n%s\n",message);
		fprintf(stderr,"\nSwath Sonar File <%s> not initialized for reading\n",file);
		status = MB_FAILURE;
		do_error_dialog("Unable to open input file.", 
				"File may not exist or you may not have", 
				"read permission in this directory!");
		return(status);
		}
		
	/* turn message on */
	do_message_on("MBvelocitytool is loading data...");

	/* set beam_first and beam_last */
	beam_first = 0;
	beam_last = beams_bath;

	/* allocate memory for data arrays */
	status = mb_malloc(verbose,beams_bath*sizeof(char),&beamflag,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&bath,&error);
	status = mb_malloc(verbose,beams_amp*sizeof(double),&amp,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&bathacrosstrack,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&bathalongtrack,&error);
	status = mb_malloc(verbose,pixels_ss*sizeof(double),&ss,&error);
	status = mb_malloc(verbose,pixels_ss*sizeof(double),&ssacrosstrack,&error);
	status = mb_malloc(verbose,pixels_ss*sizeof(double),&ssalongtrack,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&ttimes,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&angles,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&angles_forward,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&angles_null,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&heave,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&alongtrack_offset,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&depth,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&acrosstrack,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&angle,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&residual,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double),&res_sd,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(int),&nresidual,&error);

	/* if error initializing memory then quit */
	if (error != MB_ERROR_NO_ERROR)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error allocating data arrays:\n%s\n",message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}

	/* initialize the buffer */
	status = mb_buffer_init(verbose,&buff_ptr,&error);
	nbuffer = 0;

	/* load data into buffer */
	status = mb_buffer_load(verbose,buff_ptr,mbio_ptr,buffer_size,
			&nload,&nbuffer,&error);

	/* get buffer structure */
	buff = (struct mb_buffer_struct *) buff_ptr;
	
	/* set error message */
	if (nbuffer <= 0)
		do_error_dialog("No data were read from the input", 
				"file. You may have specified an", 
				"incorrect MB-System format id!");

	/* loop over the data records to find the first surface velocity */
	ssv_start = 0.0;
	bath_min = 10000.0;
	bath_max = 0.0;
	navlon_levitus = 0.0;
	navlat_levitus = 0.0;
	for (k=0;k<nbuffer;k++)
		{
		if (buff->buffer_kind[k] == MB_DATA_DATA)
			{
			/* extract bathymetry - just to get flags */
			mb_buffer_extract(verbose,buff_ptr,mbio_ptr,
				k, &kind,
				time_i,&time_d,&navlon,&navlat,
				&speed,&heading,
				&nbath,&namp,&nss,
				beamflag,bath,amp,
				bathacrosstrack,bathalongtrack,
				ss,ssacrosstrack,ssalongtrack,
				comment,&error);
			
			/* extract travel times */
			status = mb_ttimes(verbose,mbio_ptr,
				buff->buffer[k],&kind,&nbeams,
				ttimes,angles,
				angles_forward,angles_null,
				heave,alongtrack_offset,
				&draft,&ssv,&error);
			
			/* get first nav */
			if (navlon_levitus == 0.0 
			    && navlat_levitus == 0.0)
			    {
			    navlon_levitus = navlon;
			    navlat_levitus = navlat;
			    }
			
			/* check for first nonzero ssv */
			if (ssv > 0.0 && ssv_start == 0.0)
				ssv_start = ssv;
				
			/* get approximate min max depths */
			for (i=0;i<nbeams;i++)
			    if (mb_beam_ok(beamflag[i]))
				{
				depth[i] = 750 * ttimes[i] 
					* cos(DTR * angles[i]);

				/* get min max depths */
				if (depth[i] < bath_min)
				    bath_min = depth[i];
				if (depth[i] > bath_max)
				    bath_max = depth[i];
				}
			}
		}
	if (ssv_start <= 0.0)
		ssv_start = 1500.0;
		
	/* set maxdepth and apply */
	if (bath_max > 0.0 && bath_max < 13000.0)
		{
		maxdepth = 1.25 * bath_max;
		resrange = 0.02 * bath_max;
		if (resrange > 200.0)
			resrange = 200.0;
		else if (resrange < 0.1)
			resrange = 0.1;
		do_set_controls();
		}

	/* output info */
	if (verbose >= 0)
		{
		if (status == MB_SUCCESS)
			fprintf(stderr,"\nSwath Sonar File <%s> read\n",file);
		else
			fprintf(stderr,"\nSwath Sonar File <%s> not read\n",file);
		fprintf(stderr,"Swath Sonar Data Format ID:   %d\n",format);
		fprintf(stderr,"Records loaded into buffer: %d\n",nload);
		fprintf(stderr,"Records in buffer:          %d\n",nbuffer);
		}
		
	/* turn message off */
	do_message_off();
	
	/* get editable svp if needed */
	if (edit != MB_YES)
	    mbvt_new_edit_profile();
	    
	/* add Levitus display profile if nav available */
	if (navlon_levitus != 0.0 || navlat_levitus != 0.0)
		{
		sprintf(command, 
			"mblevitus -R%f/%f -Ombvt_levitus_tmp.svp\n", 
			navlon_levitus, navlat_levitus);
		system(command);
		mbvt_open_display_profile("mbvt_levitus_tmp.svp");
		system("rm -f mbvt_levitus_tmp.svp");
		}

	/* allocate memory for raytracing arrays */
	status = mb_malloc(verbose,beams_bath*sizeof(int),&nraypath,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double *),
				&raypathx,&error);
	status = mb_malloc(verbose,beams_bath*sizeof(double *),
				&raypathy,&error);
	nraypathmax = 100*profile_edit.n;
	for (i=0;i<beams_bath;i++)
		{
		status = mb_malloc(verbose,nraypathmax*sizeof(double),
				&raypathx[i],&error);
		status = mb_malloc(verbose,nraypathmax*sizeof(double),
				&raypathy[i],&error);
		}

	/* if error initializing memory then quit */
	if (error != MB_ERROR_NO_ERROR)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error allocating data arrays:\n%s\n",message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}

	/* process the data */
	if (status == MB_SUCCESS && edit == MB_YES)
		status = mbvt_process_multibeam();

	/* plot everything */
	mbvt_plot();

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	/* return */
	return(status);
}
/*--------------------------------------------------------------------*/
/* This function is called when the "PROCESS MULTIBEAM"               */
/*   selection is made from the menu bar.                             */
/* Called by:                                                         */
/*                  action_process_mb                                 */
/* Functions called:                                                  */
/*                  mb_ttimes                                         */
/* Function returns:                                                  */
/*                  status                                            */
/*--------------------------------------------------------------------*/
int mbvt_process_multibeam()
{
	/* local variables */
	char	*function_name = "mbvt_process_multibeam";
	int	status = MB_SUCCESS;
	struct mb_buffer_struct *buff;
	double	*dep;
	double	*vel;
	int	nvel;
	char	*rt_svp = NULL;
	int	first;
	double	ttime;
	int	ray_stat;
	int	ssv_mode = MB_SSV_CORRECT;
	double	ssv;
	double	factor;
	double	sx, sy, sxx, sxy;
	double	delta, a, b;
	int	ns;
	double	depth_predict, res;
	double	dr, dx;
	int	icenter;
	int	i, j, k;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		}

	/* check for data and velocity profile */
	if (profile_edit.n <= 0)
		{
		fprintf(stderr,"\nNo edit velocity profile available - bathymetry processing aborted.\n");
		status = MB_FAILURE;
		return(status);
		}
	if (nbuffer <= 0)
		{
		fprintf(stderr,"\nNo swath sonar data available - bathymetry processing aborted.\n");
		status = MB_FAILURE;
		return(status);
		}
		
	/* turn message on */
	do_message_on("MBvelocitytool is processing data...");

	/* initialize residuals and raypaths */
	for (i=0;i<beams_bath;i++)
		{
		angle[i] = 0.0;
		residual[i] = 0.0;
		res_sd[i] = 0.0;
		nresidual[i] = 0;
		nraypath[i] = 0;
		}

	/* initialize min-max variables */
	bath_min = 10000.0;
	bath_max = 0.0;

	/* get buffer structure */
	buff = (struct mb_buffer_struct *) buff_ptr;

	/* set up raytracing */
	nvel = profile_edit.n;
	vel = profile_edit.velocity;
	dep = profile_edit.depth;
	status = mb_rt_init(verbose, nvel, dep, vel, &rt_svp, &error);
	first = MB_YES;

	/* loop over the data records */
	for (k=0;k<nbuffer;k++)
		{
		/* initialize linear fit variables */
		sx = 0.0;
		sy = 0.0;
		sxx = 0.0;
		sxy = 0.0;
		ns = 0;
		depth_predict = 0.0;
		res = 0.0;

		/* output some debug values */
		if (verbose >= 5)
			{
			fprintf(stderr,"\ndbg5  Data record used in program <%s>:\n",program_name);
			fprintf(stderr,"dbg5       record %d  kind: %d\n",
				k,buff->buffer_kind[k]);
			}

		/* extract travel times */
		if (buff->buffer_kind[k] == MB_DATA_DATA)
			{
			
			/* extract bathymetry - just to get flags */
			mb_buffer_extract(verbose,buff_ptr,mbio_ptr,
				k, &kind,
				time_i,&time_d,&navlon,&navlat,
				&speed,&heading,
				&nbath,&namp,&nss,
				beamflag,bath,amp,
				bathacrosstrack,bathalongtrack,
				ss,ssacrosstrack,ssalongtrack,
				comment,&error);

			status = mb_ttimes(verbose,mbio_ptr,
				buff->buffer[k],&kind,&nbeams,
				ttimes,angles,
				angles_forward,angles_null,
				heave,alongtrack_offset,
				&draft,&ssv,&error);
			
			/* set surface sound speed to default if needed */
			if (ssv <= 0.0)
				ssv = ssv_start;
			else
				ssv_start = ssv;
			}

		/* loop over the beams */
		if (buff->buffer_kind[k] == MB_DATA_DATA)
		  for (i=0;i<nbeams;i++)
		    {
		    /* trace the ray */
		    if (mb_beam_ok(beamflag[i]))
			{
			/* get factor relating lateral distance to
			    acrosstrack distance */
			factor = cos(DTR*angles_forward[i]);

			/* trace rays */
			if (first == MB_NO)
			    {
			    /* call raytracing without keeping
				plotting list */
			    status = mb_rt(verbose, 
				    rt_svp, 0.0, 
				    angles[i], 0.5*ttimes[i],
				    ssv_mode, ssv, angles_null[i], 
				    0, NULL, NULL, NULL, 
				    &acrosstrack[i], &depth[i], 
				    &ttime, &ray_stat, &error);
			    }
			else
			    {
			    /* call raytracing keeping
				plotting list */
			    status = mb_rt(verbose, 
				    rt_svp, 0.0, 
				    angles[i], 0.5*ttimes[i],
				    ssv_mode, ssv, angles_null[i], 
				    nraypathmax, &nraypath[i], 
				    raypathx[i], raypathy[i], 
				    &acrosstrack[i], &depth[i], 
				    &ttime, &ray_stat, &error);

			    /* reset acrosstrack distances */
			    for (j=0;j<nraypath[i];j++)
				raypathx[i][j] = factor * raypathx[i][j];
			    }
				    
			/* get acrosstrack distance */
			acrosstrack[i] = factor * acrosstrack[i];

			/* add to depth if needed */
			depth[i] = depth[i] + heave[i] + draft;

			/* get min max depths */
			if (depth[i] < bath_min)
			    bath_min = depth[i];
			if (depth[i] > bath_max)
			    bath_max = depth[i];

			/* output some debug values */
			if (verbose >= 5)
			    fprintf(stderr,"dbg5       %3d %3d %6.3f %6.3f %8.2f %8.2f\n",
				k, i, 0.5*ttimes[i], angles[i], acrosstrack[i],depth[i]);

			/* get sums for linear fit */
			sx += acrosstrack[i];
			sy += depth[i];
			sxx += acrosstrack[i]*acrosstrack[i];
			sxy += acrosstrack[i]*depth[i];
			ns++;
			}
		    }

		/* reset first flag */
		if (buff->buffer_kind[k] == MB_DATA_DATA)
		    first = MB_NO;

		/* get linear fit to ping */
		if (ns > 0)
			{
			delta = ns*sxx - sx*sx;
			a = (sxx*sy - sx*sxy)/delta;
			b = (ns*sxy - sx*sy)/delta;
			}

		/* get residuals */
		if (ns > 0)
		  {
		  /* output some debug values */
		  if (verbose >= 5)
			fprintf(stderr,"dbg5       beam   xtrack   depth     fit    residual\n");

		  /* loop over all beams */
		  for (i=0;i<nbeams;i++)
		    if (mb_beam_ok(beamflag[i]))
			{
			depth_predict = a + b*acrosstrack[i];
			res = depth[i] - depth_predict;
			angle[i] += angles[i];
			residual[i] += res;
			res_sd[i] += res * res;
			nresidual[i]++;

			/* output some debug values */
			if (verbose >= 5)
		    		fprintf(stderr,"dbg5       %4d %10f %10f %10f %10f\n",
					i,acrosstrack[i],depth[i],
					depth_predict,res);
			}
		  }
		}

	/* end raytracing */
	status = mb_rt_deall(verbose, &rt_svp, &error);

	/* calculate final residuals */
	beam_first = nbeams;
	beam_last = -1;
	for (i=0;i<nbeams;i++)
		if (nresidual[i] > 0)
			{
			angle[i] = angle[i]/nresidual[i];
			residual[i] = residual[i]/nresidual[i];
			res_sd[i] = sqrt(res_sd[i]/nresidual[i]
					    - residual[i] * residual[i]);
			if (i < beam_first)
				beam_first = i;
			if (i > beam_last)
				beam_last = i;
			}

	/* output residuals and stuff */
	if (verbose >= 1)
		{
		fprintf(stderr,"\nCurrent Bathymetry Depth Range:\n");
		fprintf(stderr,"\tminimum depth: %f\n",bath_min);
		fprintf(stderr,"\tmaximum depth: %f\n",bath_max);
		fprintf(stderr,"\nSwath Bathymetry Beam Residuals:\n");
			fprintf(stderr," beam   angle   residual     sigma  calculations\n");
		for (i=0;i<nbeams;i++)
			fprintf(stderr," %4d  %7.3f  %9.3f  %9.3f  %5d\n",
				i,angle[i], residual[i], res_sd[i], nresidual[i]);
		}
		
	/* turn message off */
	do_message_off();

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	/* return */
	return(status);
}
/*--------------------------------------------------------------------*/
