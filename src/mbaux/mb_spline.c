/*--------------------------------------------------------------------
 *    The MB-system:	mb_spline.c	10/11/00
 *    $Id$
 *
 *    Copyright (c) 2000-2015 by
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
 * This source file includes 1D interpolation routines.
 *
 * The 1D spline interpolation routines are modified from spline()
 * and splint() in the book:
 *   Press, W. H., S. A. Teukolsky, W. T. Vetterling, B. P. Flannery,
 *   Numerical Recipies in C: the Art of Scientific Computing,
 *   Cambridge University Press, 1988.
 * The 1D linear interpolation routine is homegrown, but mimics the
 * spline routines in usage.
 *
 * Author:	D. W. Caress
 * Date:	October 11, 2000
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

static char rcs_id[]="$Id$";

/*--------------------------------------------------------------------------*/
int mb_spline_init(int verbose, double *x, double *y,
	int n, double yp1, double ypn, double *y2, int *error)
{
	char	*function_name = "mb_spline_init";
	int	status = MB_SUCCESS;
	int	i, k;
	double	p, qn, sig, un, *u;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:          %d\n",verbose);
		fprintf(stderr,"dbg2       x:                %p\n",x);
		fprintf(stderr,"dbg2       y:                %p\n",y);
		fprintf(stderr,"dbg2       n:                %d\n",n);
		fprintf(stderr,"dbg2       yp1:              %f\n",yp1);
		fprintf(stderr,"dbg2       ypn:              %f\n",ypn);
		fprintf(stderr,"dbg2       y2:               %p\n",y2);
		}

	/* check for n > 2 */
	if (n < 3)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_NOT_ENOUGH_DATA;
		}

	/* allocate memory for working vector */
	if (status == MB_SUCCESS)
		status = mb_mallocd(verbose,__FILE__,__LINE__, n * sizeof(double), (void **)&u, error);

	/* set up spline interpolation coefficients */
	if (status == MB_SUCCESS)
		{
		if (yp1 > 0.99e30)
			y2[1]=u[1]=0.0;
		else
			{
			y2[1] = -0.5;
			u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
			}
		for (i=2;i<=n-1;i++)
			{
			sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
			p=sig*y2[i-1]+2.0;
			y2[i]=(sig-1.0)/p;
			u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
			u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
			}
		if (ypn > 0.99e30)
			qn=un=0.0;
		else
			{
			qn=0.5;
			un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
			}
		y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
		for (k=n-1;k>=1;k--)
			y2[k]=y2[k]*y2[k+1]+u[k];

		/* deallocate memory for vector */
		status = mb_freed(verbose,__FILE__, __LINE__, (void **)&u, error);
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
int mb_spline_interp(int verbose, double *xa, double *ya, double *y2a,
	int n, double x, double *y, int *i, int *error)
{
	char	*function_name = "mb_spline_interp";
	int	status = MB_SUCCESS;
	int	klo, khi, k;
	double	h, b, a;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:          %d\n",verbose);
		fprintf(stderr,"dbg2       xa:               %p\n",xa);
		fprintf(stderr,"dbg2       ya:               %p\n",ya);
		fprintf(stderr,"dbg2       y2a:              %p\n",y2a);
		fprintf(stderr,"dbg2       n:                %d\n",n);
		fprintf(stderr,"dbg2       x:                %f\n",x);
		}

	/* check for n >= 1 */
	if (n < 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_NOT_ENOUGH_DATA;
		}

	/* perform interpolation */
	if (status == MB_SUCCESS)
		{
		klo=1;
		khi=n;
		while (khi-klo > 1)
			{
			k=(khi+klo) >> 1;
			if (xa[k] > x) khi=k;
			else klo=k;
			}
		if (khi == 1) khi = 2;
		if (klo == n) klo = n - 1;
		h=xa[khi]-xa[klo];
		a=(xa[khi]-x)/h;
		b=(x-xa[klo])/h;
		*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]
			+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
		*i=klo;
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       y:          %f\n",*y);
		fprintf(stderr,"dbg2       i:          %d\n",*i);
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
int mb_linear_interp(int verbose, double *xa, double *ya,
		int n, double x, double *y, int *i, int *error)
{
	char	*function_name = "mb_linear_interp";
	int	status = MB_SUCCESS;
	int	klo, khi, k;
	double	h, b;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:          %d\n",verbose);
		fprintf(stderr,"dbg2       xa:               %p\n",xa);
		fprintf(stderr,"dbg2       ya:               %p\n",ya);
		fprintf(stderr,"dbg2       n:                %d\n",n);
		fprintf(stderr,"dbg2       x:                %f\n",x);
		}

	/* check for n >= 1 */
	if (n < 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_NOT_ENOUGH_DATA;
		}

	/* perform interpolation */
	if (status == MB_SUCCESS)
		{
		/* do not extrapolate before or after the model, just use the
		 * first or last values */
		if (x <= xa[1])
			{
			*y = ya[1];
			*i = 1;
			}
		else if (x >= xa[n])
			{
			*y = ya[n];
			*i = n;
			}
		/* in range of model so linearly interpolate */
		else
			{
			klo=1;
			khi=n;
			while (khi-klo > 1)
				{
				k=(khi+klo) >> 1;
				if (xa[k] > x) khi=k;
				else klo=k;
				}
			if (khi == 1) khi = 2;
			if (klo == n) klo = n - 1;
			h=xa[khi]-xa[klo];
			b = (ya[khi] - ya[klo]) / h;
			*y = ya[klo] + b * (x - xa[klo]);
			*i=klo;
			}
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       y:          %f\n",*y);
		fprintf(stderr,"dbg2       i:          %d\n",*i);
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
int mb_linear_interp_longitude(int verbose, double *xa, double *ya,
		int n, double x, double *y, int *i, int *error)
{
	char	*function_name = "mb_linear_interp_longitude";
	int	status = MB_SUCCESS;
	int	klo, khi, k;
	double	h, b;
	double	yahi, yalo;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:          %d\n",verbose);
		fprintf(stderr,"dbg2       xa:               %p\n",xa);
		fprintf(stderr,"dbg2       ya:               %p\n",ya);
		fprintf(stderr,"dbg2       n:                %d\n",n);
		fprintf(stderr,"dbg2       x:                %f\n",x);
		}

	/* check for n >= 1 */
	if (n < 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_NOT_ENOUGH_DATA;
		}

	/* perform interpolation */
	if (status == MB_SUCCESS)
		{
		/* do not extrapolate before or after the model, just use the
		 * first or last values */
		if (x <= xa[1])
			{
			*y = ya[1];
			*i = 1;
			}
		else if (x >= xa[n])
			{
			*y = ya[n];
			*i = n;
			}
		/* in range of model so linearly interpolate */
		else
			{
			klo=1;
			khi=n;
			while (khi-klo > 1)
				{
				k=(khi+klo) >> 1;
				if (xa[k] > x) khi=k;
				else klo=k;
				}
			if (khi == 1) khi = 2;
			if (klo == n) klo = n - 1;
			h=xa[khi]-xa[klo];
			yahi = ya[khi];
			yalo = ya[klo];
			if (yahi - yalo > 180.0)
				yahi -= 360.0;
			else if (yahi - yalo < -180.0)
				yahi += 360.0;
			b = (yahi - yalo) / h;
			*y = ya[klo] + b * (x - xa[klo]);
			if (*y >= 180.0)
				*y -= 360.0;
			else if (*y < -180.0)
				*y += 360.0;
			*i=klo;
			}
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       y:          %f\n",*y);
		fprintf(stderr,"dbg2       i:          %d\n",*i);
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
int mb_linear_interp_latitude(int verbose, double *xa, double *ya,
		int n, double x, double *y, int *i, int *error)
{
	char	*function_name = "mb_linear_interp_latitude";
	int	status = MB_SUCCESS;
	int	klo, khi, k;
	double	h, b;
	double	yahi, yalo;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:          %d\n",verbose);
		fprintf(stderr,"dbg2       xa:               %p\n",xa);
		fprintf(stderr,"dbg2       ya:               %p\n",ya);
		fprintf(stderr,"dbg2       n:                %d\n",n);
		fprintf(stderr,"dbg2       x:                %f\n",x);
		}

	/* check for n >= 1 */
	if (n < 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_NOT_ENOUGH_DATA;
		}

	/* perform interpolation */
	if (status == MB_SUCCESS)
		{
		/* do not extrapolate before or after the model, just use the
		 * first or last values */
		if (x <= xa[1])
			{
			*y = ya[1];
			*i = 1;
			}
		else if (x >= xa[n])
			{
			*y = ya[n];
			*i = n;
			}
		/* in range of model so linearly interpolate */
		else
			{
			klo=1;
			khi=n;
			while (khi-klo > 1)
				{
				k=(khi+klo) >> 1;
				if (xa[k] > x) khi=k;
				else klo=k;
				}
			if (khi == 1) khi = 2;
			if (klo == n) klo = n - 1;
			h=xa[khi]-xa[klo];
			yahi = ya[khi];
			yalo = ya[klo];
			b = (yahi - yalo) / h;
			*y = ya[klo] + b * (x - xa[klo]);
			if (*y > 90.0)
				*y = 90.0;
			else if (*y < -90.0)
				*y = -90.0;
			*i=klo;
			}
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       y:          %f\n",*y);
		fprintf(stderr,"dbg2       i:          %d\n",*i);
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
int mb_linear_interp_heading(int verbose, double *xa, double *ya,
		int n, double x, double *y, int *i, int *error)
{
	char	*function_name = "mb_linear_interp_heading";
	int	status = MB_SUCCESS;
	int	klo, khi, k;
	double	h, b;
	double	yahi, yalo;

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBBA function <%s> called\n",function_name);
		fprintf(stderr,"dbg2  Revision id: %s\n",rcs_id);
		fprintf(stderr,"dbg2  Input arguments:\n");
		fprintf(stderr,"dbg2       verbose:          %d\n",verbose);
		fprintf(stderr,"dbg2       xa:               %p\n",xa);
		fprintf(stderr,"dbg2       ya:               %p\n",ya);
		fprintf(stderr,"dbg2       n:                %d\n",n);
		fprintf(stderr,"dbg2       x:                %f\n",x);
		}

	/* check for n >= 1 */
	if (n < 1)
		{
		status = MB_FAILURE;
		*error = MB_ERROR_NOT_ENOUGH_DATA;
		}

	/* perform interpolation */
	if (status == MB_SUCCESS)
		{
		/* do not extrapolate before or after the model, just use the
		 * first or last values */
		if (x <= xa[1])
			{
			*y = ya[1];
			*i = 1;
			}
		else if (x >= xa[n])
			{
			*y = ya[n];
			*i = n;
			}
		/* in range of model so linearly interpolate */
		else
			{
			klo=1;
			khi=n;
			while (khi-klo > 1)
				{
				k=(khi+klo) >> 1;
				if (xa[k] > x) khi=k;
				else klo=k;
				}
			if (khi == 1) khi = 2;
			if (klo == n) klo = n - 1;
			h=xa[khi]-xa[klo];
			yahi = ya[khi];
			yalo = ya[klo];
			if (yahi - yalo > 180.0)
				yahi -= 360.0;
			else if (yahi - yalo < -180.0)
				yahi += 360.0;
			b = (yahi - yalo) / h;
			*y = ya[klo] + b * (x - xa[klo]);
			if (*y >= 360.0)
				*y -= 360.0;
			else if (*y < 0.0)
				*y += 360.0;
			*i=klo;
			}
		}

	/* print output debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr,"\ndbg2  MBIO function <%s> completed\n",function_name);
		fprintf(stderr,"dbg2  Return values:\n");
		fprintf(stderr,"dbg2       y:          %f\n",*y);
		fprintf(stderr,"dbg2       i:          %d\n",*i);
		fprintf(stderr,"dbg2       error:      %d\n",*error);
		fprintf(stderr,"dbg2  Return status:\n");
		fprintf(stderr,"dbg2       status:     %d\n",status);
		}

	return(status);
}
/*--------------------------------------------------------------------*/
