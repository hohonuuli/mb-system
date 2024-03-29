/*
 * $Id$
 *
 *  Copyright (c) 1991-2015 by P. Wessel, W. H. F. Smith, R. Scharroo,
 *  F. Wobbe, and J. Luis
 *
 * See LICENSE.TXT file for copying and redistribution conditions.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3 or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * Contact info: gmt.soest.hawaii.edu
 */

#pragma once
#ifndef GMT_VERSION_H_
#define GMT_VERSION_H_

/* for manpages only */
#define MANDATE "15 May 2015"

#define GMT_MAJOR_VERSION 5
#define GMT_MINOR_VERSION 1
#define GMT_RELEASE_VERSION 2
#define GMT_VERSION_YEAR 2015

#define GMT_SOURCE_CODE_CONTROL_VERSION_STRING "14256"
#ifdef GMT_SOURCE_CODE_CONTROL_VERSION_STRING
#	define GMT_STRING "5.1.2 (r14256)"
#else /* if HAVE_SVN_VERSION or if unversioned */
#	define GMT_STRING "5.1.2"
#endif

#ifdef __x86_64__
#	define GMT_VER_64 " [64-bit]"
#else
#	define GMT_VER_64 ""
#endif

#ifdef _OPENMP /* Open MP Parallelization is on */
#	define GMT_MP " [MP]"
#else
#	define GMT_MP ""
#endif

#ifndef GMT_MANPAGE
#	define GMT_VERSION GMT_version()
	static inline char *GMT_version () {
		/* final string returned by GMT_version() */
		static char version[] = GMT_STRING GMT_VER_64 GMT_MP;
		return version;
	}
#endif /* !GMT_MANPAGE */

#endif /* !GMT_VERSION_H_ */

/* vim: set ft=c: */
