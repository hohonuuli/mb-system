/*--------------------------------------------------------------------
 *    The MB-system:	mbnavedit_extrawidgets.h	8/7/95
 *    $Id$
 *
 *    Copyright (c) 1995-2015 by
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
 * MBNAVEDIT is an interactive navigation editor for swath sonar data.
 * It can work with any data format supported by the MBIO library.
 * This include file contains global widget parameters shared with
 * the Motif interface code.
 *
 * Author:	D. W. Caress
 * Date:	June 24,  1995
 *
 *
 */

/*--------------------------------------------------------------------*/

#ifndef MB_YES
#include "mb_status.h"
#endif

#ifdef MBNAVEDIT_DECLARE_GLOBALS
#define MBNAVEDIT_EXTERNAL
#else
#define MBNAVEDIT_EXTERNAL extern
#endif

/* Global Widget Declarations Added By Hand */

MBNAVEDIT_EXTERNAL Widget       fileSelectionBox_list;
MBNAVEDIT_EXTERNAL Widget       fileSelectionBox_text;
MBNAVEDIT_EXTERNAL Widget       scrolledWindow_hscrollbar;
MBNAVEDIT_EXTERNAL Widget       scrolledWindow_vscrollbar;

/*--------------------------------------------------------------------*/
