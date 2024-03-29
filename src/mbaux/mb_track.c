/*--------------------------------------------------------------------
 *    The MB-system:	mb_track.c	8/15/93
 *    $Id$
 *
 *    Copyright (c) 1993-2015 by
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
 * mb_track.c plots the shiptrack of swath sonar data.
 *
 * Author:	D. W. Caress
 * Date:	August, 1993
 *
 *
 */

/* standard global include files */
#include <stdio.h>
#include <math.h>
#include <string.h>

/* mbio include files */
#include "mb_status.h"
#include "mb_define.h"
#include "mb_aux.h"

//* global defines */
#define IMOVE 	3
#define IDRAW 	2
#define ISTROKE	-2
#define IOR 	-3
#define EPS 0.0001

static char rcs_id[]="$Id$";

/*--------------------------------------------------------------------------*/
/* 	function mb_track plots the shiptrack of multibeam data. */
void mb_track(int verbose, struct swath *data, int *error)
{
	char	*function_name = "mb_track";
	int	status = MB_SUCCESS;
	int	time_tick, time_annot, date_annot;
	double	hour0, hour1;
	int	time_j[5];
	double	x, y, x1, y1, x2, y2, x3, y3, x4, y4;
	double	dx, dy;
	double	angle;
	char	label[25];
	int	i;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:              %d\n",verbose);
		fprintf(stderr,"dbg2       swath:                %p\n",data);
		fprintf(stderr,"dbg2       time tick interval:   %f\n",data->time_tick_int);
		fprintf(stderr,"dbg2       time interval:        %f\n",data->time_annot_int);
		fprintf(stderr,"dbg2       date interval:        %f\n",data->date_annot_int);
		fprintf(stderr,"dbg2       time tick length:     %f\n",data->time_tick_len);
		fprintf(stderr,"dbg2       data->npings:         %d\n",data->npings);
		for (i=0;i<data->npings;i++)
			{
			fprintf(stderr,"dbg2       i:%d time:%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d.%6.6d position: %.9f %.9f\n",
					i,data->pings[i].time_i[0],data->pings[i].time_i[1],data->pings[i].time_i[2],
					data->pings[i].time_i[3],data->pings[i].time_i[4],data->pings[i].time_i[5],data->pings[i].time_i[6],
					data->pings[i].navlon,data->pings[i].navlat);
			}
		}

	/* set line width */
	data->contour_setline(3);
	data->contour_newpen(0);

	/* draw the time ticks */
	for (i=1;i<data->npings;i++)
		{
		/* get time of day */
		hour0 = data->pings[i-1].time_i[3]
			+ data->pings[i-1].time_i[4]/60.0
			+ data->pings[i-1].time_i[5]/3600.0;
		hour1 = data->pings[i].time_i[3]
			+ data->pings[i].time_i[4]/60.0
			+ data->pings[i].time_i[5]/3600.0;

		/* check for time tick */
		time_tick = MB_NO;
		if (floor(hour0/data->time_tick_int)
			!= floor(hour1/data->time_tick_int))
			time_tick = MB_YES;

		/* check for time annotation */
		time_annot = MB_NO;
		if (floor(hour0/data->time_annot_int)
			!= floor(hour1/data->time_annot_int))
			time_annot = MB_YES;

		/* check for date annotation */
		date_annot = MB_NO;
		if (floor(hour0/data->date_annot_int)
			!= floor(hour1/data->date_annot_int))
			date_annot = MB_YES;

		/* now get azimuth and location if needed */
		if (date_annot == MB_YES || time_annot == MB_YES
			|| time_tick == MB_YES)
			{
			/* get azimuth from heading */
			angle = data->pings[i].heading + 90.0;
			if (angle > 360.0)
				angle = angle - 360.0;
			dx = sin(DTR*angle);
			dy = cos(DTR*angle);

			/* cheat and get location by averaging */
			x = 0.5*(data->pings[i-1].navlon
				+ data->pings[i].navlon);
			y = 0.5*(data->pings[i-1].navlat
				+ data->pings[i].navlat);
			}

		/* do date annotation if needed */
		if (date_annot == MB_YES)
			{
			x1 = x + 0.375*data->time_tick_len*(dx - dy);
			y1 = y + 0.375*data->time_tick_len*(dy + dx);
			x3 = x + 0.375*data->time_tick_len*(dx + dy);
			y3 = y + 0.375*data->time_tick_len*(dy - dx);
			x2 = x + 0.375*data->time_tick_len*(-dx + dy);
			y2 = y + 0.375*data->time_tick_len*(-dy - dx);
			x4 = x + 0.375*data->time_tick_len*(-dx - dy);
			y4 = y + 0.375*data->time_tick_len*(-dy + dx);
			data->contour_plot(x1,y1,IMOVE);
			data->contour_plot(x2,y2,IDRAW);
			data->contour_plot(x3,y3,IMOVE);
			data->contour_plot(x4,y4,ISTROKE);
			mb_get_jtime(verbose,data->pings[i].time_i,time_j);
			sprintf(label," %2.2d:%2.2d/%3.3d",
				data->pings[i].time_i[3],
				data->pings[i].time_i[4],
				time_j[1]);
			data->contour_plot_string(x,y,data->time_tick_len,90.0-angle,label);
			}

		/* do time annotation if needed */
		else if (time_annot == MB_YES)
			{
			x1 = x + 0.375*data->time_tick_len*(dx - dy);
			y1 = y + 0.375*data->time_tick_len*(dy + dx);
			x3 = x + 0.375*data->time_tick_len*(dx + dy);
			y3 = y + 0.375*data->time_tick_len*(dy - dx);
			x2 = x + 0.375*data->time_tick_len*(-dx + dy);
			y2 = y + 0.375*data->time_tick_len*(-dy - dx);
			x4 = x + 0.375*data->time_tick_len*(-dx - dy);
			y4 = y + 0.375*data->time_tick_len*(-dy + dx);
			data->contour_plot(x1,y1,IMOVE);
			data->contour_plot(x2,y2,IDRAW);
			data->contour_plot(x3,y3,IMOVE);
			data->contour_plot(x4,y4,ISTROKE);
			sprintf(label,"   %2.2d:%2.2d",
				data->pings[i].time_i[3],
				data->pings[i].time_i[4]);
			data->contour_plot_string(x,y,data->time_tick_len,90.0-angle,label);
			}

		/* do time tick if needed */
		else if (time_tick == MB_YES)
			{
			x1 = x + 0.25*data->time_tick_len*(dx - dy);
			y1 = y + 0.25*data->time_tick_len*(dy + dx);
			x3 = x + 0.25*data->time_tick_len*(dx + dy);
			y3 = y + 0.25*data->time_tick_len*(dy - dx);
			x2 = x + 0.25*data->time_tick_len*(-dx + dy);
			y2 = y + 0.25*data->time_tick_len*(-dy - dx);
			x4 = x + 0.25*data->time_tick_len*(-dx - dy);
			y4 = y + 0.25*data->time_tick_len*(-dy + dx);
			data->contour_plot(x1,y1,IMOVE);
			data->contour_plot(x2,y2,IDRAW);
			data->contour_plot(x3,y3,IMOVE);
			data->contour_plot(x4,y4,ISTROKE);
			}
		}

	/* draw the shiptrack */
	for (i=0;i<data->npings;i++)
		{
		if (i == 0)
			data->contour_plot(data->pings[i].navlon,data->pings[i].navlat,IMOVE);
		else if (i < data->npings - 1)
			data->contour_plot(data->pings[i].navlon,data->pings[i].navlat,IDRAW);
		else
			data->contour_plot(data->pings[i].navlon,data->pings[i].navlat,ISTROKE);
		}

	/* reset line width */
	data->contour_setline(0);

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return;
}


/*--------------------------------------------------------------------------*/
/* 	function mb_trackpingnumber annotates pingnumbers */
void mb_trackpingnumber(int verbose, struct swath *data, int *error)
{
	char	*function_name = "mb_trackpingnumber";
	int	status = MB_SUCCESS;
	int	pingnumber_tick, pingnumber_annot;
	double	x, y, x1, y1, x2, y2;
	double	dx, dy;
	double	angle;
	char	label[25];
	double	justify[4];
	int	i;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:              %d\n",verbose);
		fprintf(stderr,"dbg2       swath:                %p\n",data);
		fprintf(stderr,"dbg2       pingnumber tick int:  %d\n",data->pingnumber_tick_int);
		fprintf(stderr,"dbg2       pingnumber annot int: %d\n",data->pingnumber_annot_int);
		fprintf(stderr,"dbg2       pingnumber tick len:  %f\n",data->pingnumber_tick_len);
		}

	/* set line width */
	data->contour_setline(3);
	data->contour_newpen(0);

	/* draw the pingnumber ticks and annotations */
	for (i=0;i<data->npings;i++)
		{
		/* check for pingnumber tick */
		pingnumber_tick = MB_NO;
		if (data->pings[i].pingnumber % data->pingnumber_tick_int == 0)
			pingnumber_tick = MB_YES;

		/* check for pingnumber annotation */
		pingnumber_annot = MB_NO;
		if (data->pings[i].pingnumber % data->pingnumber_annot_int == 0 )
			pingnumber_annot = MB_YES;

		/* now get azimuth and location if needed */
		if (pingnumber_tick == MB_YES || pingnumber_annot == MB_YES)
			{
			/* get azimuth from heading */
			angle = data->pings[i].heading + 90.0;
			if (angle > 360.0)
				angle = angle - 360.0;
			dx = sin(DTR*angle);
			dy = cos(DTR*angle);

			/* get location */
			x = data->pings[i].navlon;
			y = data->pings[i].navlat;
			}

		/* do pingnumber annotation if needed */
		if (pingnumber_annot == MB_YES)
			{
			sprintf(label,"%d ", data->pings[i].pingnumber);
			data->contour_justify_string(data->pingnumber_tick_len, label, justify);
			x1 = x - 0.375*data->pingnumber_tick_len*dx;
			y1 = y - 0.375*data->pingnumber_tick_len*dy;
			x2 = x - 1.5*justify[2]*dx;
			y2 = y - 1.5*justify[2]*dy;
			data->contour_plot(x1,y1,IMOVE);
			data->contour_plot(x,y,IDRAW);
			data->contour_plot_string(x2,y2,data->pingnumber_tick_len,90.0-angle,label);
			}

		/* do time tick if needed */
		else if (pingnumber_tick == MB_YES)
			{
			x1 = x - 0.25*data->pingnumber_tick_len*dx;
			y1 = y - 0.25*data->pingnumber_tick_len*dy;
			x2 = x + 0.25*data->pingnumber_tick_len*dx;
			y2 = y + 0.25*data->pingnumber_tick_len*dy;
			data->contour_plot(x1,y1,IMOVE);
			data->contour_plot(x,y,IDRAW);
			}
		}

	/* reset line width */
	data->contour_setline(0);

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return;
}

/*--------------------------------------------------------------------------*/
/* 	function mb_trackname plots the filename on the shiptrack.
	 - contributed by Gordon Keith, CSIRO, December 2004 */
void mb_trackname(int verbose, int perpendicular, struct swath *data, char *file, int *error)
{
	char	*function_name = "mb_trackname";
	int	status = MB_SUCCESS;
	double	angle;
	char	label[MB_PATH_MAXLINE];

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:            %d\n",verbose);
		fprintf(stderr,"dbg2       perpendicular:      %d\n",perpendicular);
		fprintf(stderr,"dbg2       swath:              %p\n",data);
		fprintf(stderr,"dbg2       file:               %s\n",file);
		}

	strncpy(label,file,MB_PATH_MAXLINE);
	mb_get_basename(verbose,label,error);
	if (perpendicular == MB_YES)
		angle = 0.0 - data->pings[0].heading;
	else
		angle = 90.0 - data->pings[0].heading;
	if (angle < 0.0)
		angle += 360.0;
	if (angle > 360.0)
		angle -= 360.0;
	data->contour_plot_string(data->pings[0].navlon,data->pings[0].navlat,
		    data->name_hgt,
		    angle,
		    label);

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return;
}

/*--------------------------------------------------------------------------*/
