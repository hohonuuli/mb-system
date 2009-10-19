/*--------------------------------------------------------------------
 *    The MB-system:	mbneptune2esf.c	2004/11/11
 *    $Id$
 *
 *    Copyright (c) 2004-2009 by
 *    Gordon Keith
 *      CSIRO Marine Research
 *      Castray Esplanade
 *      Battery Point TAS 7000
 *      Australia
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
 * MBneptune2esf reads a binstat_?.rules file generated by manually cleaning
 * data using the correlation plot in the binstat tool of the neptune sofware
 * written by Kongsberg Simrad and writes these edits in an mbsystem esf file.
 * The purpose of this tool is to allow data cleaning done using neptune to 
 * be carried into the mbsystem environment - thus making neptune a more useful
 * tool.
 * 
 * MBneptune2esf applies all the manual edits in the rules file given to those
 * of the input files it recognises as matching the original lines.
 * A line is considered to match if the line name contained in the rules file
 * (e.g. 0002_20000408_221415) is part of the file name of the data file
 * (e.g. 0002_20000408_221415.mb57).
 *
 * Swath line files should be converted to format 57 using mbcopy before 
 * using this program, as format 56 does not have the ability to flag data
 * (although it can be zeroed).
 *
 * Data flagged by rules generated in binstat is not flagged by this program -
 * only manually flagged data is flagged.
 * 
 * Author:	G. J. Keith
 * Date:	11 November 2004
 *
 * Acknowledgments:
 * This program is based on a skeleton derived from mbclean.
 *
 * $Log: mbneptune2esf.c,v $
 * Revision 5.5  2008/09/11 20:20:14  caress
 * Checking in updates made during cruise AT15-36.
 *
 * Revision 5.4  2006/01/18 15:17:00  caress
 * Added stdlib.h include.
 *
 * Revision 5.3  2005/11/05 01:07:54  caress
 * Programs changed to register arrays through mb_register_array() rather than allocating the memory directly with mb_realloc() or mb_malloc().
 *
 * Revision 5.2  2005/03/25 04:43:02  caress
 * Standardized the string lengths used for filenames and comment data.
 *
 * Revision 5.1  2004/12/18 01:38:52  caress
 * Working towards release 5.0.6.
 *
 * Revision 5.0  2004/12/02 06:40:49  caress
 * New program submitted by Gordon Keith.
 *
 *
 */

/* standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

/* mbio include files */
#include "../../include/mb_status.h"
#include "../../include/mb_format.h"
#include "../../include/mb_define.h"
#include "../../include/mb_io.h"
#include "../../include/mb_swap.h"
#include "../../include/mb_process.h"
#include "../../include/mbsys_simrad2.h"

/* length of line name nnnn_yyyymmdd_hhmmss = 20 */
#define LINE_NAME_LENGTH 20
#define LINE_NAME_BUFFER LINE_NAME_LENGTH + 1

/* use MB-System memory allocation functions */
#define	USE_MB_MALLOC 1

/* list of flagged beams */
struct neptune_beam_list
{
  int	beam;
  struct neptune_beam_list *next;
};

/* ping tree structure */
struct neptune_ping_tree 
{
  int	ping;
  struct neptune_ping_tree *prev;
  struct neptune_ping_tree *next;
  struct neptune_beam_list *beams;
};

/* line tree structure */
struct neptune_line_tree 
{
  char	name[LINE_NAME_BUFFER];
  struct neptune_line_tree *prev;
  struct neptune_line_tree *next;
  struct neptune_ping_tree *pings;
};

/* ping structure definition */
struct mbclean_ping_struct 
	{
	int	time_i[7];
	double	time_d;
	double	navlon;
	double	navlat;
	double	speed;
	double	heading;
	int	beams_bath;
	char	*beamflag;
	char	*beamflagorg;
	double	*bath;
	double	*bathacrosstrack;
	double	*bathalongtrack;
	double	*bathx;
	double	*bathy;
	};

/* function prototypes */
int mbclean_save_edit(int verbose, FILE *sofp, double time_d, int beam, int action, int *error);
int find_line(int verbose, char* line_name, struct neptune_line_tree **node, int create, 
	      struct neptune_line_tree **result, int *nlines, int *error);
int find_ping(int verbose, int ping, struct neptune_ping_tree **node, int create, 
	      struct neptune_ping_tree **result, int *error);
int line_array(struct neptune_line_tree *line, struct neptune_line_tree ***array, int *n);
int print_pings(FILE *output, struct neptune_ping_tree *node);
int free_pings(int verbose, struct neptune_ping_tree **node, int *error);

static char rcs_id[] = "$Id$";

/*--------------------------------------------------------------------*/

int main (int argc, char **argv)
{
	char program_name[] = "mbneptune2esf";
	char help_message[] =  "mbneptune2esf reads a Simrad Neptune BinStat rules files and a list of MB-Systems data files\nand applies the flags in the rules file to the esf file of the coresponding line";
	char usage_message[] = "mbneptune2esf [-Rrules -Fformat -Iinfile -Ooutfile -V -H]";
	extern char *optarg;
	int	errflg = 0;
	int	c;
	int	help = 0;
	int	flag = 0;

	/* MBIO status variables */
	int	status;
	int	verbose = 0;
	int	error = MB_ERROR_NO_ERROR;
	char	*message = NULL;

	/* MBIO read control parameters */
	int	read_datalist = MB_NO;
	char	read_file[MB_PATH_MAXLINE];
	char	swathfile[MB_PATH_MAXLINE];
	void	*datalist;
	int	look_processed = MB_DATALIST_LOOK_UNSET;
	double	file_weight;
	int	format;
	int	variable_beams;
	int	traveltime;
	int	beam_flagging; 
	int	pings;
	int	lonflip;
	double	bounds[4];
	int	btime_i[7];
	int	etime_i[7];
	double	btime_d;
	double	etime_d;
	double	speedmin;
	double	timegap;
	double	distance;
	double	altitude;
	double	sonardepth;
	int	beams_bath;
	int	beams_amp;
	int	pixels_ss;
	double	*amp;
	double	*ss;
	double	*ssacrosstrack;
	double	*ssalongtrack;

	/* mbio read and write values */
	void	*mbio_ptr = NULL;
	struct mb_io_struct	*mb_io_ptr;
	struct mbsys_simrad2_struct	*store;
	struct mbsys_simrad2_ping_struct *sim_ping;
	int	kind;
	struct mbclean_ping_struct cur_ping;
	int	pingsread;
	int	nfiletot = 0;
	int	ndatatot = 0;
	int	nflagtot = 0;
	int	nzerotot = 0;
	int	nflagesftot = 0;
	int	nunflagesftot = 0;
	int	nzeroesftot = 0;
	int	ndata = 0;
	int	nflag = 0;
	int	nzero = 0;
	int	nflagesf = 0;
	int	nunflagesf = 0;
	int	nzeroesf = 0;
	char	comment[MB_COMMENT_MAXLINE];
	int	mode = 1;
	int	action;

	/* rules file */
	char    rulesfile[MB_PATH_MAXLINE];
	struct neptune_line_tree *rule_lines = NULL;
	int     no_lines = 0;
	FILE    *rules_fp;
	int	rules_done;
	int	rule_level;
	int	usable_rule;
	int	nscan;
	int	nlines;
	int	npings;
	int	nbeams;
	int 	used;
	int 	bytes;
	char	buffer[MB_PATH_MAXLINE];
	char	word[MB_PATH_MAXLINE];
	char	buff[MB_PATH_MAXLINE];
	char	beams_buff[MB_PATH_MAXLINE];
	char	line_name[MB_PATH_MAXLINE];
	int	nlen;
	int	slen;
	int	ping_no;
	int	beam_no;
	struct neptune_line_tree **lines;
	struct neptune_line_tree *line;
	struct neptune_ping_tree *ping;
	struct neptune_beam_list *beam;
	char	*buffer_ptr;

	int     output_file_set = MB_NO;
	char    output_file[MB_PATH_MAXLINE];
	FILE	*output;
	
	/* save file control variables */
	int	esffile_open = MB_NO;
	char	esffile[MB_PATH_MAXLINE];
	struct mb_esf_struct esf;

	/* processing variables */
	int	read_data;
	int	start, done;
	int	i, j, k;

	/* get current default values */
	status = mb_defaults(verbose,&format,&pings,&lonflip,bounds,
		btime_i,etime_i,&speedmin,&timegap);

	/* reset all defaults but the format and lonflip */
	pings = 1;
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
	btime_i[6] = 0;
	etime_i[0] = 2062;
	etime_i[1] = 2;
	etime_i[2] = 21;
	etime_i[3] = 10;
	etime_i[4] = 30;
	etime_i[5] = 0;
	etime_i[6] = 0;
	speedmin = 0.0;
	timegap = 1000000000.0;
	strcpy(read_file, "datalist.mb-1");
	strcpy(rulesfile, "-");

	/* process argument list */
	while ((c = getopt(argc, argv, "VvHhF:f:I:i:M:m:O:o:R:r:")) != -1)
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
		case 'F':
		case 'f':
			sscanf (optarg,"%d", &format);
			flag++;
			break;
		case 'I':
		case 'i':
			sscanf (optarg,"%s", read_file);
			flag++;
			break;
		case 'M':
		case 'm':
			sscanf (optarg,"%d", &mode);
			flag++;
			break;
		case 'O':
		case 'o':
			sscanf (optarg,"%s", output_file);
			output_file_set = MB_YES;
			flag++;
			break;
		case 'R':
		case 'r':
			sscanf (optarg,"%s", rulesfile);
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
		fprintf(stderr,"dbg2       verbose:        %d\n",verbose);
		fprintf(stderr,"dbg2       help:           %d\n",help);
		fprintf(stderr,"dbg2       pings:          %d\n",pings);
		fprintf(stderr,"dbg2       lonflip:        %d\n",lonflip);
		fprintf(stderr,"dbg2       bounds[0]:      %f\n",bounds[0]);
		fprintf(stderr,"dbg2       bounds[1]:      %f\n",bounds[1]);
		fprintf(stderr,"dbg2       bounds[2]:      %f\n",bounds[2]);
		fprintf(stderr,"dbg2       bounds[3]:      %f\n",bounds[3]);
		fprintf(stderr,"dbg2       btime_i[0]:     %d\n",btime_i[0]);
		fprintf(stderr,"dbg2       btime_i[1]:     %d\n",btime_i[1]);
		fprintf(stderr,"dbg2       btime_i[2]:     %d\n",btime_i[2]);
		fprintf(stderr,"dbg2       btime_i[3]:     %d\n",btime_i[3]);
		fprintf(stderr,"dbg2       btime_i[4]:     %d\n",btime_i[4]);
		fprintf(stderr,"dbg2       btime_i[5]:     %d\n",btime_i[5]);
		fprintf(stderr,"dbg2       btime_i[6]:     %d\n",btime_i[6]);
		fprintf(stderr,"dbg2       etime_i[0]:     %d\n",etime_i[0]);
		fprintf(stderr,"dbg2       etime_i[1]:     %d\n",etime_i[1]);
		fprintf(stderr,"dbg2       etime_i[2]:     %d\n",etime_i[2]);
		fprintf(stderr,"dbg2       etime_i[3]:     %d\n",etime_i[3]);
		fprintf(stderr,"dbg2       etime_i[4]:     %d\n",etime_i[4]);
		fprintf(stderr,"dbg2       etime_i[5]:     %d\n",etime_i[5]);
		fprintf(stderr,"dbg2       etime_i[6]:     %d\n",etime_i[6]);
		fprintf(stderr,"dbg2       speedmin:       %f\n",speedmin);
		fprintf(stderr,"dbg2       timegap:        %f\n",timegap);
		fprintf(stderr,"dbg2       data format:    %d\n",format);
		fprintf(stderr,"dbg2       input file:     %s\n",read_file);
		fprintf(stderr,"dbg2       mode:           %d\n",mode);
		}

	/* if help desired then print it and exit */
	if (help)
		{
		fprintf(stderr,"\n%s\n",help_message);
		fprintf(stderr,"\nusage: %s\n", usage_message);
		exit(error);
		}

	/* read rules */
	if (0 == strncmp("-",rulesfile,2))
	  rules_fp =stdin;
	else
	if ((rules_fp = fopen(rulesfile,"r")) == NULL)
		{
		fprintf(stderr,"\nUnable to open rules file %s\n",rulesfile);
		fprintf(stderr,"\nusage: %s\n", usage_message);
		exit(error);
		}

	rules_done = MB_NO;
	usable_rule = MB_NO;
	rule_level = 0;
	status = MB_SUCCESS;
	while (MB_NO == rules_done)
		{
		/* skip till we find a rule */
		while (MB_NO == rules_done &&
		       MB_NO == usable_rule)
			{
			buffer_ptr = fgets(buffer,MB_PATH_MAXLINE,rules_fp);
			if (buffer_ptr != buffer)
				rules_done = MB_YES;
			else
				{
				  nscan = sscanf(buffer, "%s",word);
				  if (1 == nscan && ')' == word[0])
				  	rule_level--;
				  else if (1 == nscan && '(' == word[0] && ')' != word[1])
				    {
				      rule_level++;
				      if (2 == rule_level)
					{
					  nscan = sscanf(buffer, "%s %s",buff,word);
					  if (2 == nscan && 0 == strncmp(word,"LINES_PING_BEAM_RULE",20))
					    usable_rule = MB_YES;
					}
				    }
				}
			}

		/* read a rule */
		if (MB_NO == rules_done) 
			{
			buffer_ptr = fgets(buffer,MB_PATH_MAXLINE,rules_fp);
			if (buffer_ptr != buffer)
				{
				rules_done = MB_YES;
				status = MB_FAILURE;
				error = MB_ERROR_EOF;
				}
			else
				{

				if (MB_SUCCESS == status && 2 != sscanf(buffer,"%s %d",word,&nlines))
				  status = MB_FAILURE;
				
				if (MB_SUCCESS == status && 0 != strncmp(word,"NO_OF_LINES",11))
				  status = MB_FAILURE;

				if (MB_SUCCESS == status)
				  for(i = 0; i < nlines; i++)
				    {
				      if (buffer != fgets(buffer,MB_PATH_MAXLINE,rules_fp))
					status = MB_FAILURE;
				      
				      if (MB_SUCCESS == status && 2 != sscanf(buffer,"%s '%s'",word,line_name))
					status = MB_FAILURE;
				      
				      if (MB_SUCCESS == status && 0 != strncmp(word,"LINE_NAME",9))
					status = MB_FAILURE;
				      
				      if (MB_SUCCESS == status)
					status = find_line(verbose,line_name,&rule_lines,MB_YES,&line,&no_lines,&error);
				      
				      if (buffer != fgets(buffer,MB_PATH_MAXLINE,rules_fp))
					status = MB_FAILURE;
				      
				      if (MB_SUCCESS == status && 2 != sscanf(buffer,"%s %d",word,&npings))
					status = MB_FAILURE;
				      
				      if (MB_SUCCESS == status && 0 != strncmp(word,"NO_OF_PINGS",11))
					status = MB_FAILURE;
				      
				      if (MB_SUCCESS == status)
					for(j = 0; j < npings; j++)
					  {
					    if (buffer != fgets(buffer,MB_PATH_MAXLINE,rules_fp))
					      status = MB_FAILURE;
					    
					    if (MB_SUCCESS == status && 5 > sscanf(buffer,"%s %d %s %d %s %n",
										   word,&ping_no,buff,&nbeams,beams_buff,&used))
					      status = MB_FAILURE;
					    
					    if (MB_SUCCESS == status && 
						(0 != strncmp(word,"PING",4) || 
						 0 != strncmp(buff,"NO_OF_BEAMS",11) ||
						 0 != strncmp(beams_buff,"BEAMS",5)))
					      status = MB_FAILURE;
					    
					    if (MB_SUCCESS == status)
					      status = find_ping(verbose,ping_no,&(line->pings),MB_YES,&ping,&error);
					    
                                            beam = ping->beams;
                                            if (NULL != beam)
					      {
                                              while (NULL != beam->next)
                                                beam = beam->next;
					      }
					    if (MB_SUCCESS == status)
					      for (k = 0; k < nbeams; k++) 
						{
						  if (MB_SUCCESS == status && 1 > sscanf(&buffer[used],"%d %n",
											 &beam_no,&bytes))
						    status = MB_FAILURE;
						  used += bytes;
						  
						  if (NULL == beam)
						    {
#ifdef USE_MB_MALLOC
						      status = mb_mallocd(verbose,__FILE__,__LINE__,sizeof(struct neptune_beam_list),(void **)&(ping->beams),&error);
#else
						      ping->beams = malloc(sizeof(struct neptune_beam_list));
#endif
						      beam = ping->beams;
						    }
						  else
						    {
#ifdef USE_MB_MALLOC
						      status = mb_mallocd(verbose,__FILE__,__LINE__,sizeof(struct neptune_beam_list),(void **)&(beam->next),&error);
#else
						      beam->next = malloc(sizeof(struct neptune_beam_list));
#endif
						      beam = beam->next;
						    }
						  beam->beam = beam_no;
						  beam->next = NULL;
						}
					  }
				    }
				if (MB_FAILURE == status && 1<= verbose) 
				  {
				    fprintf(stderr,"Failed to understand line: \n%s",buffer);
				  }

				status = MB_SUCCESS;
				}
			}
		}

	/* put lines in an array */
	status = mb_mallocd(verbose,__FILE__,__LINE__,no_lines * sizeof(void*),(void **)&lines,&error);
	i = 0;
	line_array(rule_lines,&lines,&i);

	/* output rules found */
	if (MB_YES == output_file_set) 
	  {
	    if (0 == strncmp("-",output_file,2))
	      output = stdout;
	    else
	      output = fopen(output_file, "w");
	    if (output != NULL)
	      {
		for (i = 0; i < no_lines; i++)
		  {
		    fprintf(output, "%s\n", lines[i]->name);
		    print_pings(output, lines[i]->pings);
		  }
	      }
	  }

	/* get format if required */
	if (format == 0)
		mb_get_format(verbose,read_file,NULL,&format,&error);

	/* determine whether to read one file or a list of files */
	if (format < 0)
		read_datalist = MB_YES;

	/* open file list */
	if (read_datalist == MB_YES)
	    {
	    if ((status = mb_datalist_open(verbose,&datalist,
					    read_file,look_processed,&error)) != MB_SUCCESS)
		{
		error = MB_ERROR_OPEN_FAIL;
		fprintf(stderr,"\nUnable to open data list file: %s\n",
			read_file);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}
	    if ((status = mb_datalist_read(verbose,datalist,
			    swathfile,&format,&file_weight,&error))
			    == MB_SUCCESS)
		read_data = MB_YES;
	    else
		read_data = MB_NO;
	    }
	/* else copy single filename to be read */
	else
	    {
	    strcpy(swathfile, read_file);
	    read_data = MB_YES;
	    }

	/* loop over all files to be read */
	while (read_data == MB_YES)
	{

	  usable_rule = MB_NO;
	  /* check if this file matches any lines */
	  if (MBF_EM300RAW == format || MBF_EM300MBA == format)
	  for (i = 0; i < no_lines; i++)
	    {
	      strcpy(line_name,lines[i]->name);
	      nlen = strlen(line_name);
	      slen = strlen(swathfile);
	      for (j = 0 ; j <= slen-nlen; j++) 
		if (0 == strncmp(line_name,&swathfile[j],nlen))
		    {
		      usable_rule = MB_YES;
		      line = lines[i];
		      j = slen;
		      i = no_lines;
		    }
	    }
		
	  if (MB_YES == usable_rule)
	    {

	/* check format and get format flags */
	if ((status = mb_format_flags(verbose,&format,
			&variable_beams, &traveltime, &beam_flagging, 
			&error)) 
		!= MB_SUCCESS)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error returned from function <mb_format_flags> regarding input format %d:\n%s\n",format,message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}

	/* check that clean mode is allowed 
		for the specified data format */
	if (beam_flagging == MB_NO && mode <= 2)
		{
		fprintf(stderr,"\nMBIO format %d does not allow flagging of bad data \nas negative numbers (specified by cleaning mode %d).\n",format,mode);
		fprintf(stderr,"\nCopy the data to another format or set the cleaning mode to zero \nbad data values (-M3 or -M4).\n");
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}

	/* initialize reading the input swath sonar file */
	if ((status = mb_read_init(
		verbose,swathfile,format,pings,lonflip,bounds,
		btime_i,etime_i,speedmin,timegap,
		&mbio_ptr,&btime_d,&etime_d,
		&beams_bath,&beams_amp,&pixels_ss,&error)) != MB_SUCCESS)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error returned from function <mb_read_init>:\n%s\n",message);
		fprintf(stderr,"\nMultibeam File <%s> not initialized for reading\n",swathfile);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}
		
	/* initialize and increment counting variables */
	ndata = 0;
	nflag = 0;
	nzero = 0;
	nflagesf = 0;
	nunflagesf = 0;
	nzeroesf = 0;

	/* give the statistics */
	if (verbose >= 0)
		{
		fprintf(stderr,"\nProcessing %s\n",swathfile);
		}

	/* allocate memory for data arrays */
	cur_ping.beamflag = NULL;
	cur_ping.beamflagorg = NULL;
	cur_ping.bath = NULL;
	cur_ping.bathacrosstrack = NULL;
	cur_ping.bathalongtrack = NULL;
	cur_ping.bathx = NULL;
	cur_ping.bathy = NULL;
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(char), (void **)&cur_ping.beamflag, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(char), (void **)&cur_ping.beamflagorg, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&cur_ping.bath, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&cur_ping.bathacrosstrack, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&cur_ping.bathalongtrack, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&cur_ping.bathx, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&cur_ping.bathy, &error);
	
	amp = NULL;
	ss = NULL;
	ssacrosstrack = NULL;
	ssalongtrack = NULL;
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_AMPLITUDE,
						sizeof(double), (void **)&amp, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_SIDESCAN, 
						sizeof(double), (void **)&ss, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_SIDESCAN, 
						sizeof(double), (void **)&ssacrosstrack, &error);
	if (error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_SIDESCAN, 
						sizeof(double), (void **)&ssalongtrack, &error);

	/* if error initializing memory then quit */
	if (error != MB_ERROR_NO_ERROR)
		{
		mb_error(verbose,error,&message);
		fprintf(stderr,"\nMBIO Error allocating data arrays:\n%s\n",message);
		fprintf(stderr,"\nProgram <%s> Terminated\n",
			program_name);
		exit(error);
		}
		
	/* now deal with old edit save file */
	if (status == MB_SUCCESS)
	    {
	    /* reset message */
	    fprintf(stderr, "Sorting old edits...\n");

	    /* handle esf edits */
	    status = mb_esf_load(verbose, swathfile, 
				 MB_YES, MB_YES, esffile, &esf, &error); 
	    if (status == MB_SUCCESS
		    && esf.esffp != NULL)
		    esffile_open = MB_YES;
	    if (status == MB_FAILURE 
		    && error == MB_ERROR_OPEN_FAIL)
		    {
		    esffile_open = MB_NO;
		    fprintf(stderr, "\nUnable to open new edit save file %s\n", 
			esf.esffile);
		    }
	    else if (status == MB_FAILURE 
		    && error == MB_ERROR_MEMORY_FAIL)
		    {
		    esffile_open = MB_NO;
		    fprintf(stderr, "\nUnable to allocate memory for edits in esf file %s\n", esf.esffile);
		    }
	    /* reset message */
	    fprintf(stderr, "%d old edits sorted...\n",esf.nedit);
	    }
 
	/* read */
	done = MB_NO;
	start = 0;
	fprintf(stderr, "Processing data...\n");
	while (done == MB_NO)
	    {
	    if (verbose > 1) fprintf(stderr,"\n");

	    /* read next record */
	    error = MB_ERROR_NO_ERROR;
	    status = mb_get(verbose,
			    mbio_ptr,&kind,&pingsread,
			    cur_ping.time_i,&cur_ping.time_d,
			    &cur_ping.navlon,&cur_ping.navlat,
			    &cur_ping.speed,&cur_ping.heading,
			    &distance,&altitude,&sonardepth,
			    &cur_ping.beams_bath,&beams_amp,&pixels_ss,
			    cur_ping.beamflag,cur_ping.bath,amp,
			    cur_ping.bathacrosstrack,cur_ping.bathalongtrack,
			    ss,ssacrosstrack,ssalongtrack,
			    comment,
			    &error);
	    if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  current data status:\n");
		fprintf(stderr,"dbg2    kind:       %d\n",kind);
		fprintf(stderr,"dbg2    status:     %d\n",status);
		fprintf(stderr,"dbg2    ndata:      %d\n",ndata);
		fprintf(stderr,"dbg2    nflagesf:   %d\n",nflagesf);
		fprintf(stderr,"dbg2    nunflagesf: %d\n",nunflagesf);
		fprintf(stderr,"dbg2    nzeroesf:   %d\n",nzeroesf);
		fprintf(stderr,"dbg2    nflag:      %d\n",nflag);
		fprintf(stderr,"dbg2    nzero:      %d\n",nzero);
		}
	    if (status == MB_SUCCESS && kind == MB_DATA_DATA)
		{
		/* save original beamflags */
		for (i=0;i<cur_ping.beams_bath;i++)
		    {
		    cur_ping.beamflagorg[i] = cur_ping.beamflag[i];
		    }
		    
		/* apply saved edits */
		status = mb_esf_apply(verbose, &esf, 
		    		cur_ping.time_d, cur_ping.beams_bath, 
				cur_ping.beamflag, &error);

		/* update counters */
		for (i=0;i<cur_ping.beams_bath;i++)
		    {
		    if (cur_ping.beamflag[i] != cur_ping.beamflagorg[i])
		    	{
			if (mb_beam_ok(cur_ping.beamflag[i]))
			    nunflagesf++;
			else
			    nflagesf++;
			}
		    }
		ndata++;

	/* get mbio descriptor */
	mb_io_ptr = (struct mb_io_struct *) mbio_ptr;

	/* get data structure pointer */
	store = (struct mbsys_simrad2_struct *) mb_io_ptr->store_data;

	sim_ping = (struct mbsys_simrad2_ping_struct *) store->ping;

	/* get ping no */
	ping_no = sim_ping ->png_count;
	
	/* if ping in rules get it and flag beams */
	if (MB_SUCCESS == find_ping(verbose,ping_no,&line->pings,MB_NO,&ping,&error)) {
	  beam = ping->beams;
	  while (NULL != beam) {
	    beam_no = beam->beam;
			if (mb_beam_ok(cur_ping.beamflag[beam_no]))
			    {
			    if (verbose >= 1)
			    fprintf(stderr,"z: %4d %2d %2d %2.2d:%2.2d:%2.2d.%6.6d  %4d %8.2f\n",
					    cur_ping.time_i[0],
					    cur_ping.time_i[1],
					    cur_ping.time_i[2],
					    cur_ping.time_i[3],
					    cur_ping.time_i[4],
					    cur_ping.time_i[5],
					    cur_ping.time_i[6],
					    i,cur_ping.bath[beam_no]);
			    if (mode <= 2)
				{
				cur_ping.beamflag[beam_no] 
					    = MB_FLAG_FLAG + MB_FLAG_FILTER;
				nflag++;
				mb_ess_save(verbose, &esf, cur_ping.time_d, beam_no, MBP_EDIT_FILTER, &error);
				}
			    else
				{
				cur_ping.beamflag[beam_no] = MB_FLAG_NULL;
				nzero++;
				mb_ess_save(verbose, &esf, cur_ping.time_d, beam_no, MBP_EDIT_ZERO, &error);
				}
			    }

	    beam = beam->next;
	  }

	    /* write out edits from completed pings */
	    if (status == MB_SUCCESS
		|| done == MB_YES)
		{
		    for (i=0;i<cur_ping.beams_bath;i++)
			{
			if (cur_ping.beamflag[i] != 
				cur_ping.beamflagorg[i])
			    {
			    if (mb_beam_ok(cur_ping.beamflag[i]))
				    action = MBP_EDIT_UNFLAG;
			    else if (mb_beam_check_flag_filter2(cur_ping.beamflag[i]))
				    action = MBP_EDIT_FILTER;
			    else if (mb_beam_check_flag_filter(cur_ping.beamflag[i]))
				    action = MBP_EDIT_FILTER;
			    else if (cur_ping.beamflag[i] != MB_FLAG_NULL)
				    action = MBP_EDIT_FLAG;
			    else
				    action = MBP_EDIT_ZERO;
			    mb_esf_save(verbose, &esf,
					    cur_ping.time_d, i,
					    action, &error);
			    }
			}
		}
	}
		}
	    else if (error > MB_ERROR_NO_ERROR)
		{
		done = MB_YES;
		}
	    }

	/* close the files */
	status = mb_close(verbose,&mbio_ptr,&error);

	/* close edit save file */
	status = mb_esf_close(verbose, &esf, &error);
	
	/* update mbprocess parameter file */
	if (esffile_open == MB_YES)
	    {
	    /* update mbprocess parameter file */
	    status = mb_pr_update_format(verbose, swathfile, 
			MB_YES, format, 
			&error);
	    status = mb_pr_update_edit(verbose, swathfile, 
			MBP_EDIT_ON, esffile, 
			&error);
	    }

	/* check memory */
	if (verbose >= 4)
		status = mb_memory_list(verbose,&error);

	/* increment the total counting variables */
	nfiletot++;
	ndatatot += ndata;
	nflagesftot += nflagesf;
	nunflagesftot += nunflagesf;
	nzeroesftot += nzeroesf;
	nflagtot += nflag;
	nzerotot += nzero;

	/* give the statistics */
	if (verbose >= 0)
		{
		fprintf(stderr,"%d bathymetry data records processed\n",ndata);
		if (esf.nedit > 0)
			{
			fprintf(stderr,"%d beams flagged in old esf file\n",nflagesf);
			fprintf(stderr,"%d beams unflagged in old esf file\n",nunflagesf);
			fprintf(stderr,"%d beams zeroed in old esf file\n",nzeroesf);
			}
		fprintf(stderr,"%d beams flagged\n",nflag);
		fprintf(stderr,"%d beams zeroed\n",nzero);
		}

	    } /* if usable_rule */

	/* figure out whether and what to read next */
        if (read_datalist == MB_YES)
                {
		if ((status = mb_datalist_read(verbose,datalist,
			    swathfile,&format,&file_weight,&error))
			    == MB_SUCCESS)
                        read_data = MB_YES;
                else
                        read_data = MB_NO;
                }
        else
                {
                read_data = MB_NO;
                }

	/* end loop over files in list */
	}
        if (read_datalist == MB_YES)
		mb_datalist_close(verbose,&datalist,&error);

	/* give the total statistics */
	if (verbose >= 0)
		{
		fprintf(stderr,"\nMBclean Processing Totals:\n");
		fprintf(stderr,"-------------------------\n");
		fprintf(stderr,"%d total swath data files processed\n",nfiletot);
		fprintf(stderr,"%d total bathymetry data records processed\n",ndatatot);
		fprintf(stderr,"%d total beams flagged in old esf files\n",nflagesftot);
		fprintf(stderr,"%d total beams unflagged in old esf files\n",nunflagesftot);
		fprintf(stderr,"%d total beams zeroed in old esf files\n",nzeroesftot);
		fprintf(stderr,"%d total beams flagged\n",nflagtot);
		fprintf(stderr,"%d total beams zeroed\n",nzerotot);
		}

	/* set program status */
	status = MB_SUCCESS;

	/* free memory associated with lines. */
	for (i=0; i < no_lines; i++)
	      {
	      free_pings(verbose, &lines[i]->pings, &error);
#ifdef USE_MB_MALLOC
	      mb_freed(verbose,__FILE__,__LINE__,(void **) &lines[i], &error);
#else
	      free(lines[i]);
#endif
	      }

	/* check memory */
	if (verbose >= 4)
		status = mb_memory_list(verbose,&error);

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  Program <%s> completed\n",
			program_name);
		fprintf(stderr,"dbg2  Ending status:\n");
		fprintf(stderr,"dbg2       status:  %d\n",status);
		}

	/* end it all */
	exit(error);
}
/*--------------------------------------------------------------------*/
int mbclean_save_edit(int verbose, FILE *sofp, double time_d, int beam, int action, int *error)
{
	/* local variables */
	char	*function_name = "mbclean_save_edit";
	int	status = MB_SUCCESS;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> called\n",
			function_name);
		fprintf(stderr,"dbg2  Input arguments:\n");
	
		fprintf(stderr,"dbg2       sofp:            %ld\n",(long)sofp);
		fprintf(stderr,"dbg2       time_d:          %f\n",time_d);
		fprintf(stderr,"dbg2       beam:            %d\n",beam);
		fprintf(stderr,"dbg2       action:          %d\n",action);
		}
	/* write out the edit */
fprintf(stderr,"OUTPUT EDIT: %f %d %d\n",time_d,beam,action);
	if (sofp != NULL)
	    {		
#ifdef BYTESWAPPED
	    mb_swap_double(&time_d);
	    beam = mb_swap_int(beam);
	    action = mb_swap_int(action);
#endif
	    if (fwrite(&time_d, sizeof(double), 1, sofp) != 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_WRITE_FAIL;
		}
	    if (status == MB_SUCCESS
		&& fwrite(&beam, sizeof(int), 1, sofp) != 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_WRITE_FAIL;
		}
	    if (status == MB_SUCCESS
		&& fwrite(&action, sizeof(int), 1, sofp) != 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_WRITE_FAIL;
		}
	    }

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",
			function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:       %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:      %d\n",status);
		}

	/* return */
	return(status);
}

/*--------------------------------------------------------------------*/
int find_line(int verbose, char* line_name, struct neptune_line_tree **node, int create, 
	      struct neptune_line_tree **result, int *nlines, int *error)
{
	int	status = MB_SUCCESS;
	int	comp;

	if (NULL == *node) {
	  if (MB_NO == create)
	    return MB_FAILURE;

#ifdef USE_MB_MALLOC
	    status = mb_mallocd(verbose,__FILE__,__LINE__,sizeof(struct neptune_line_tree),
			(void **)result,error);
#else
	    *result = malloc(sizeof(struct neptune_line_tree));
#endif
	    if (MB_SUCCESS == status) 
	      {
		strncpy((*result)->name,line_name,LINE_NAME_LENGTH);
		(*result)->name[LINE_NAME_LENGTH] =0;
		(*result)->prev = NULL;
		(*result)->next = NULL;
		(*result)->pings = NULL;
		(*nlines)++;
		*node = *result;
	      }
	    return status;
	}

	comp = strncmp(line_name,(*node)->name,LINE_NAME_LENGTH);
	if (0 == comp)
	  {
	    *result = *node;
	    return status;
	  }
	if (0 > comp)
	  return find_line(verbose, line_name, &((*node)->prev), create, result, nlines, error);
	  return find_line(verbose, line_name, &((*node)->next), create, result, nlines, error);

}
/*--------------------------------------------------------------------*/
int find_ping(int verbose, int ping, struct neptune_ping_tree **node, int create, 
	      struct neptune_ping_tree **result, int *error)
{
	int	status = MB_SUCCESS;

	if (NULL == *node) {
	  if (MB_NO == create)
	    return MB_FAILURE;

#ifdef USE_MB_MALLOC
	    status = mb_mallocd(verbose,__FILE__,__LINE__,sizeof(struct neptune_ping_tree),
			(void **)result,error);
#else
	    *result = malloc(sizeof(struct neptune_ping_tree));
#endif
	    if (MB_SUCCESS == status) 
	      {
		(*result)->ping = ping;
		(*result)->prev = NULL;
		(*result)->next = NULL;
		(*result)->beams = NULL;
		*node = *result;
	      }
	    return status;
	}

	if (ping == (*node)->ping)
	  {
	    *result = *node;
	    return status;
	  }
	if ((*node)->ping > ping)
	  return find_ping(verbose, ping, &((*node)->prev), create, result, error);
	  return find_ping(verbose, ping, &((*node)->next), create, result, error);

}
/*--------------------------------------------------------------------*/

int line_array(struct neptune_line_tree *line, struct neptune_line_tree ***array, int *n) 
{
  if (NULL == line)
    return MB_SUCCESS;

  line_array(line->prev,array,n);
  (*array)[(*n)++] = line;
  line_array(line->next,array,n);
  return MB_SUCCESS;
}

/*--------------------------------------------------------------------*/

int print_pings(FILE *output, struct neptune_ping_tree *node)
{
  struct neptune_beam_list *beam;

  if (NULL == node)
    return MB_SUCCESS;

  print_pings(output, node->prev);

  fprintf(output, "\tPing %d beams: ", node->ping);
  beam = node->beams;
  while (NULL != beam) 
    {
      fprintf(output, " %d", beam->beam);
      beam = beam->next;
    }
  fprintf(output, "\n");  

  print_pings(output, node->next);
  return MB_SUCCESS;
}

/*--------------------------------------------------------------------*/

int free_pings(int verbose, struct neptune_ping_tree **node, int *error)
{
  struct neptune_beam_list *beam;
  struct neptune_beam_list *nextbeam;

  if (NULL == *node)
    return MB_SUCCESS;
  
  free_pings(verbose,&(*node)->prev,error);
  free_pings(verbose,&(*node)->next,error);

  beam = (*node)->beams;
  while (NULL != beam) 
    {
      nextbeam = beam->next;
#ifdef USE_MB_MALLOC
      mb_freed(verbose,__FILE__,__LINE__,(void **) &beam, error);
#else
      free(beam);
#endif
      beam = nextbeam;
    }
#ifdef USE_MB_MALLOC
  mb_freed(verbose,__FILE__,__LINE__,(void **) node, error);
#else
  free(*node);
#endif

  return MB_SUCCESS;
}


/*--------------------------------------------------------------------*/

  
