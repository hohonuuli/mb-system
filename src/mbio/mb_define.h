/*--------------------------------------------------------------------
 *    The MB-system:	mb_define.h	4/21/96
 *    $Id$
 *
 *    Copyright (c) 1996-2015 by
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
 * mb_define.h defines macros used by MB-System programs and functions
 * for degree/radian conversions and min/max calculations.
 *
 * Author:	D. W. Caress
 * Date:	April 21, 1996
 *
 * $Log: mb_define.h,v $
 *
 */

/* include this code only once */
#ifndef MB_DEFINE_DEF
#define MB_DEFINE_DEF

#include "mb_config.h"

/* include for mb_s_char types */
#if HAVE_STDINT_H
# include <stdint.h>
#endif

/* For XDR/RPC */
#ifdef HAVE_RPC_RPC_H
# include <rpc/rpc.h>
#endif
#ifdef HAVE_RPC_TYPES_H
# include <rpc/types.h>
# include <rpc/xdr.h>
#endif

/* for Windows */
#if defined(_WIN32) && (_MSC_VER < 1800)
#	if !defined(copysign)
#		define copysign(x,y) _copysign(x,y)
#	endif
#	if !defined(log2)
#		define log2(x) (log(x) / log(2))
#	endif
#	if !defined(rint)
#		define rint(x) (floor((x)+0.5))
#	endif
#endif

#ifdef _WIN32
#define sleep Sleep
#define popen _popen
#define pclose _pclose
#define ftello ftell
#define fseeko fseek
#	ifndef isnan
#		define isnan(x) _isnan(x)
#	endif
#	ifndef inline
#		define inline __inline
#	endif
#endif

/* MB-system version id */
#define	MB_VERSION	VERSION
#define	MB_BUILD_DATE	VERSION_DATE
#define	MB_SVN		"$Id$"

/* type definitions of signed and unsigned char */
typedef unsigned char	mb_u_char;

/* From stdint.h if available */
#if defined INT8_MAX || defined int8_t
typedef int8_t mb_s_char;
#else
typedef signed char	mb_s_char;
#endif

/* type definitions of signed and unsigned long int (64 bit integer) */
typedef unsigned long long	mb_u_long;
typedef long long	mb_s_long;

/* type definitions for structures used in beam angle calculations */
typedef struct {
    double x;
    double y;
    double z;
} mb_3D_vector;

typedef struct {
    double roll;
    double pitch;
    double heading;
} mb_3D_orientation;

/* declare buffer maximum */
#define	MB_BUFFER_MAX	5000

/* maximum path length in characters */
#define MB_PATH_MAXLINE 1024

/* maximum comment length in characters */
#define MB_COMMENT_MAXLINE 1944

/* other string length defines */
#define MB_NAME_LENGTH		32
#define MB_LONGNAME_LENGTH	128
#define MB_DESCRIPTION_LENGTH	2048

/* typedef for path string */
typedef char mb_path[MB_PATH_MAXLINE];
typedef char mb_name[MB_NAME_LENGTH];
typedef char mb_longname[MB_LONGNAME_LENGTH];

/* maximum number of asynchronous data saved */
#define MB_ASYNCH_SAVE_MAX 10000

/* maximum size of SVP profiles */
#define MB_SVP_MAX 1024

/* maximum number of CTD samples per record */
#define MB_CTD_MAX 256

/* maximum number of asynchronous nav samples per record */
#define MB_NAV_MAX 256

/* file mode (read or write) */
#define	MB_FILEMODE_READ	0
#define	MB_FILEMODE_WRITE	1

/* types of  files used by swath sonar data formats */
#define	MB_FILETYPE_NORMAL	1
#define	MB_FILETYPE_SINGLE	2
#define	MB_FILETYPE_XDR		3
#define	MB_FILETYPE_GSF		4
#define	MB_FILETYPE_NETCDF	5
#define MB_FILETYPE_SURF	6
#define MB_FILETYPE_SEGY	7

/* settings for recursive datalist reading functions */
#define MB_DATALIST_LOOK_UNSET		0
#define MB_DATALIST_LOOK_NO 		1
#define MB_DATALIST_LOOK_YES		2

/* settings of i/o array dimension types */
#define MB_MEM_TYPE_NONE		0
#define MB_MEM_TYPE_BATHYMETRY		1
#define MB_MEM_TYPE_AMPLITUDE		2
#define MB_MEM_TYPE_SIDESCAN		3

/* declare PI if needed */
#ifndef M_PI
#define	M_PI	3.14159265358979323846
#endif

/* the natural log of 2 is always useful */
#define MB_LN_2        0.69314718056

/* multiply this by degrees to get radians */
#define DTR	0.01745329251994329500

/* multiply this by radians to get degrees */
#define RTD	57.2957795130823230000

/* min max round define */
#ifndef MIN
#define	MIN(A, B)	((A) < (B) ? (A) : (B))
#endif
#ifndef MAX
#define	MAX(A, B)	((A) > (B) ? (A) : (B))
#endif
#ifndef ROUND
#define	ROUND(X)	X < 0.0 ? ceil(X - 0.5) : floor(X + 0.5)
#endif

/* NaN defines */
#ifdef NO_IEEE
#define MB_MAKE_FNAN(x) (x = FLT_MAX)
#define MB_MAKE_DNAN(x) (x = DBL_MAX)
#define MB_IS_FNAN(x) ((x) == FLT_MAX)
#define MB_IS_DNAN(x) ((x) == DBL_MAX)
#else
#define MB_MAKE_FNAN(x) (x = (float) NAN)
#define MB_MAKE_DNAN(x) (x = NAN)
#define MB_IS_FNAN isnan
#define MB_IS_DNAN isnan
#endif

/* default grid no data value define */
#define	MB_DEFAULT_GRID_NODATA		-9999999.9

/* safe square root define - sets argument to zero if negative */
#define SAFESQRT(X) sqrt(MAX(0.0, X))

/* position projection flag (0 = longitude latitude, 1 = projected eastings northings) */
#define	MB_PROJECTION_GEOGRAPHIC	0
#define	MB_PROJECTION_PROJECTED		1

/* MBIO core function prototypes */
int mb_defaults(int verbose, int *format, int *pings,
		int *lonflip, double bounds[4],
		int *btime_i, int *etime_i,
		double *speedmin, double *timegap);
int mb_env(int verbose, char *psdisplay, char *imgdisplay, char *mbproject);
int mb_lonflip(int verbose, int *lonflip);
int mb_mbview_defaults(int verbose,
			int *primary_colortable,
			int *primary_colortable_mode,
			int *primary_shade_mode,
			int *slope_colortable,
			int *slope_colortable_mode,
			int *secondary_colortable,
			int *secondary_colortable_mode,
			double *illuminate_magnitude,
			double *illuminate_elevation,
			double *illuminate_azimuth,
			double *slope_magnitude);
int mb_fbtversion(int verbose, int *fbtversion);
int mb_uselockfiles(int verbose, int *uselockfiles);
int mb_fileiobuffer(int verbose, int *fileiobuffer);
int mb_format_register(int verbose, int *format, void *mbio_ptr, int *error);
int mb_format_info(int verbose, int *format, int *system,
		int *beams_bath_max, int *beams_amp_max, int *pixels_ss_max,
		char *format_name, char *system_name, char *format_description,
		int *numfile, int *filetype, int *variable_beams,
		int *traveltime, int *beam_flagging,
		int *nav_source, int *heading_source, int *vru_source, int *svp_source,
		double *beamwidth_xtrack, double *beamwidth_ltrack,
		int *error);
int mb_format(int verbose, int *format, int *error);
int mb_format_system(int verbose, int *format, int *system, int *error);
int mb_format_description(int verbose, int *format,
		char *description, int *error);
int mb_format_dimensions(int verbose, int *format,
		int *beams_bath_max, int *beams_amp_max, int *pixels_ss_max,
		int *error);
int mb_format_flags(int verbose, int *format,
		int *variable_beams, int *traveltime, int *beam_flagging,
		int *error);
int mb_format_source(int verbose, int *format,
		int *nav_source, int *heading_source,
		int *vru_source, int *svp_source,
		int *error);
int mb_format_beamwidth(int verbose, int *format,
		double *beamwidth_xtrack, double *beamwidth_ltrack,
		int *error);
int mb_get_format(int verbose, char *filename, char *fileroot,
		    int *format, int *error);
int mb_datalist_open(int verbose,
		void **datalist,
		char *path,
		int look_processed,
		int *error);
int mb_datalist_read(int verbose,
		void *datalist,
		char *path, int *format, double *weight,
		int *error);
int mb_datalist_read2(int verbose,
		void *datalist,
		int *pstatus, char *path, char *ppath, int *format, double *weight,
		int *error);
int mb_datalist_close(int verbose,
		void **datalist, int *error);
int mb_get_relative_path(int verbose,
		char *path,
		char *pwd,
		int *error);
int mb_get_shortest_path(int verbose,
		char *path,
		int *error);
int mb_get_basename(int verbose,
		char *path,
		int *error);
int mb_check_info(int verbose, char *file, int lonflip,
		    double bounds[4], int *file_in_bounds,
		    int *error);
int mb_make_info(int verbose, int force,
		    char *file, int format, int *error);
int mb_get_fbt(int verbose, char *file, int *format, int *error);
int mb_get_fnv(int verbose, char *file, int *format, int *error);
int mb_get_ffa(int verbose, char *file, int *format, int *error);
int mb_get_ffs(int verbose, char *file, int *format, int *error);
int mb_swathbounds(int verbose, int checkgood,
		double navlon, double navlat, double heading,
		int nbath, int nss,
		char *beamflag, double *bath,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		int *ibeamport,
		int *ibeamcntr,
		int *ibeamstbd,
		int *ipixelport,
		int *ipixelcntr,
		int *ipixelstbd,
		int *error);
int mb_read_init(int verbose, char *file,
		int format, int pings, int lonflip, double bounds[4],
		int btime_i[7], int etime_i[7],
		double speedmin, double timegap,
		void **mbio_ptr, double *btime_d, double *etime_d,
		int *beams_bath, int *beams_amp, int *pixels_ss,
		int *error);
int mb_write_init(int verbose,
		char *file, int format, void **mbio_ptr,
		int *beams_bath, int *beams_amp, int *pixels_ss,
		int *error);
int mb_close(int verbose, void **mbio_ptr, int *error);
int mb_read_ping(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *error);
int mb_get_all(int verbose, void *mbio_ptr, void **store_ptr, int *kind,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading,
		double *distance, double *altitude, double *sonardepth,
		int *nbath, int *namp, int *nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		char *comment, int *error);
int mb_get(int verbose, void *mbio_ptr, int *kind, int *pings,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading,
		double *distance, double *altitude, double *sonardepth,
		int *nbath, int *namp, int *nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		char *comment, int *error);
int mb_read(int verbose, void *mbio_ptr,
		int *kind, int *pings,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading,
		double *distance, double *altitude, double *sonardepth,
		int *nbath, int *namp, int *nss,
		char *beamflag, double *bath, double *amp,
		double *bathlon, double *bathlat,
		double *ss, double *sslon, double *sslat,
		char *comment, int *error);
int mb_write_ping(int verbose, void *mbio_ptr, void *store_ptr,
		int *error);
int mb_put_all(int verbose, void *mbio_ptr, void *store_ptr,
		int usevalues, int kind,
		int time_i[7], double time_d,
		double navlon, double navlat,
		double speed, double heading,
		int nbath, int namp, int nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		char *comment, int *error);
int mb_put_comment(int verbose, void *mbio_ptr, char *comment,
		int *error);
int mb_fileio_open(int verbose, void *mbio_ptr, int *error);
int mb_fileio_close(int verbose, void *mbio_ptr, int *error);
int mb_fileio_get(int verbose, void *mbio_ptr, char *buffer, size_t *size, int *error);
int mb_fileio_put(int verbose, void *mbio_ptr, char *buffer, size_t *size, int *error);
int mb_alloc(int verbose, void *mbio_ptr,
		void **store_ptr, int *error);
int mb_deall(int verbose, void *mbio_ptr,
		void **store_ptr, int *error);
int mb_get_store(int verbose, void *mbio_ptr,
		    void **store_ptr, int *error);
int mb_dimensions(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *nbath, int *namp, int *nss, int *error);
int mb_pingnumber(int verbose, void *mbio_ptr,
		int *pingnumber, int *error);
int mb_segynumber(int verbose, void *mbio_ptr,
		int *line, int *shot, int *cdp, int *error);
int mb_beamwidths(int verbose, void *mbio_ptr,
		double *beamwidth_xtrack, double *beamwidth_ltrack, int *error);
int mb_sonartype(int verbose, void *mbio_ptr, void *store_ptr,
		int *sonartype, int *error);
int mb_sidescantype(int verbose, void *mbio_ptr, void *store_ptr,
		int *ss_type, int *error);
int mb_preprocess(int verbose, void *mbio_ptr, void *store_ptr,
		double time_d, double navlon, double navlat,
		double speed, double heading, double sonardepth,
		double roll, double pitch, double heave,
		int *error);
int mb_extract(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading,
		int *nbath, int *namp, int *nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		char *comment, int *error);
int mb_insert(int verbose, void *mbio_ptr, void *store_ptr,
		int kind, int time_i[7], double time_d,
		double navlon, double navlat,
		double speed, double heading,
		int nbath, int namp, int nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		char *comment, int *error);
int mb_extract_nav(int verbose, void *mbio_ptr, void *store_ptr, int *kind,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading, double *draft,
		double *roll, double *pitch, double *heave,
		int *error);
int mb_extract_nnav(int verbose, void *mbio_ptr, void *store_ptr,
		int nmax, int *kind, int *n,
		int *time_i, double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading, double *draft,
		double *roll, double *pitch, double *heave,
		int *error);
int mb_insert_nav(int verbose, void *mbio_ptr, void *store_ptr,
		int time_i[7], double time_d,
		double navlon, double navlat,
		double speed, double heading, double draft,
		double roll, double pitch, double heave,
		int *error);
int mb_extract_altitude(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind,
		double *transducer_depth, double *altitude,
		int *error);
int mb_insert_altitude(int verbose, void *mbio_ptr, void *store_ptr,
		double transducer_depth, double altitude,
		int *error);
int mb_extract_svp(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind,
		int *nsvp,
		double *depth, double *velocity,
		int *error);
int mb_insert_svp(int verbose, void *mbio_ptr, void *store_ptr,
		int nsvp,
		double *depth, double *velocity,
		int *error);
int mb_ttimes(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *nbeams,
		double *ttimes, double	*angles,
		double *angles_forward, double *angles_null,
		double *heave, double *alongtrack_offset,
		double *draft, double *ssv, int *error);
int mb_detects(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *nbeams, int *detects, int *error);
int mb_pulses(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *nbeams, int *pulses, int *error);
int mb_gains(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, double *transmit_gain, double *pulse_length,
		double *receive_gain, int *error);
int mb_extract_rawssdimensions(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, double *sample_interval,
		int *num_samples_port, int *num_samples_stbd, int *error);
int mb_extract_rawss(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *sidescan_type, double *sample_interval,
		double *beamwidth_xtrack, double *beamwidth_ltrack,
		int *num_samples_port, double *rawss_port, 
		int *num_samples_stbd, double *rawss_stbd, int *error);
int mb_insert_rawss(int verbose, void *mbio_ptr, void *store_ptr,
		int kind, int sidescan_type, double sample_interval,
		double beamwidth_xtrack, double beamwidth_ltrack,
		int num_samples_port, double *rawss_port,
		int num_samples_stbd, double *rawss_stbd, int *error);
int mb_extract_segytraceheader(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind,
		void *segytraceheader_ptr,
		int *error);
int mb_extract_segy(int verbose, void *mbio_ptr, void *store_ptr,
		int *sampleformat,
		int *kind,
		void *segyheader_ptr,
		float *segydata,
		int *error);
int mb_insert_segy(int verbose, void *mbio_ptr, void *store_ptr,
		int kind,
		void *segyheader_ptr,
		float *segydata,
		int *error);
int mb_ctd(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *nctd, double *time_d,
		double *conductivity, double *temperature,
		double *depth, double *salinity, double *soundspeed, int *error);
int mb_ancilliarysensor(int verbose, void *mbio_ptr, void *store_ptr,
		int *kind, int *nsensor, double *time_d,
		double *sensor1, double *sensor2, double *sensor3,
		double *sensor4, double *sensor5, double *sensor6,
		double *sensor7, double *sensor8, int *error);
int mb_copyrecord(int verbose, void *mbio_ptr,
		void *store_ptr, void *copy_ptr, int *error);

int mb_buffer_init(int verbose, void **buff_ptr, int *error);
int mb_buffer_close(int verbose, void **buff_ptr, void *mbio_ptr,
		int *error);
int mb_buffer_load(int verbose, void *buff_ptr,void *mbio_ptr,
		int nwant, int *nload, int *nbuff, int *error);
int mb_buffer_dump(int verbose, void *buff_ptr, void *mbio_ptr, void *ombio_ptr,
		int nhold, int *ndump, int *nbuff, int *error);
int mb_buffer_clear(int verbose, void *buff_ptr, void *mbio_ptr,
		int nhold, int *ndump, int *nbuff, int *error);
int mb_buffer_info(int verbose, void *buff_ptr, void *mbio_ptr,
		int id, int *system, int *kind, int *error);
int mb_buffer_get_next_data(int verbose, void *buff_ptr, void *mbio_ptr,
		int start, int *id,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading,
		int *nbath, int *namp, int *nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		int *error);
int mb_buffer_get_next_nav(int verbose, void *buff_ptr, void *mbio_ptr,
		int start, int *id,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading, double *draft,
		double *roll, double *pitch, double *heave,
		int *error);
int mb_buffer_extract(int verbose, void *buff_ptr, void *mbio_ptr,
		int id, int *kind,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading,
		int *nbath, int *namp, int *nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		char *comment,
		int *error);
int mb_buffer_extract_nav(int verbose, void *buff_ptr, void *mbio_ptr,
		int id, int *kind,
		int time_i[7], double *time_d,
		double *navlon, double *navlat,
		double *speed, double *heading, double *draft,
		double *roll, double *pitch, double *heave,
		int *error);
int mb_buffer_insert(int verbose, void *buff_ptr, void *mbio_ptr,
		int id, int time_i[7], double time_d,
		double navlon, double navlat,
		double speed, double heading,
		int nbath, int namp, int nss,
		char *beamflag, double *bath, double *amp,
		double *bathacrosstrack, double *bathalongtrack,
		double *ss, double *ssacrosstrack, double *ssalongtrack,
		char *comment,
		int *error);
int mb_buffer_insert_nav(int verbose, void *buff_ptr, void *mbio_ptr,
		int id, int time_i[7], double time_d,
		double navlon, double navlat,
		double speed, double heading, double draft,
		double roll, double pitch, double heave,
		int *error);
int mb_buffer_get_kind(int verbose, void *buff_ptr, void *mbio_ptr,
		int id, int *kind,
		int *error);
int mb_buffer_get_ptr(int verbose, void *buff_ptr, void *mbio_ptr,
		int id, void **store_ptr,
		int *error);

int mb_coor_scale(int verbose, double latitude,
		double *mtodeglon, double *mtodeglat);
int mb_apply_lonflip(int verbose, int lonflip, double *longitude);

int mb_error(int, int, char **);
int mb_notice_log_datatype(int verbose, void *mbio_ptr, int data_id);
int mb_notice_log_error(int verbose, void *mbio_ptr, int error_id);
int mb_notice_log_problem(int verbose, void *mbio_ptr, int problem_id);
int mb_notice_get_list(int verbose, void *mbio_ptr, int *notice_list);
int mb_notice_message(int verbose, int notice, char **message);
int mb_navint_add(int verbose, void *mbio_ptr,
		double time_d, double lon_easting, double lat_northing, int *error);
int mb_navint_interp(int verbose, void *mbio_ptr,
		double time_d, double heading, double rawspeed,
		double *lon, double *lat, double *speed,
		int *error);
int mb_navint_prjinterp(int verbose, void *mbio_ptr,
		double time_d, double heading, double rawspeed,
		double *easting, double *northing, double *speed,
		int *error);
int mb_attint_add(int verbose, void *mbio_ptr,
		double time_d, double heave,
		double roll, double pitch, int *error);
int mb_attint_nadd(int verbose, void *mbio_ptr,
		int nsamples, double *time_d, double *heave,
		double *roll, double *pitch, int *error);
int mb_attint_interp(int verbose, void *mbio_ptr,
		double time_d, double *heave,
		double *roll, double *pitch,
		int *error);
int mb_hedint_add(int verbose, void *mbio_ptr,
		double time_d, double heading, int *error);
int mb_hedint_nadd(int verbose, void *mbio_ptr,
		int nsamples, double *time_d, double *heading, int *error);
int mb_hedint_interp(int verbose, void *mbio_ptr,
		double time_d, double *heading,
		int *error);
int mb_depint_add(int verbose, void *mbio_ptr, double time_d, double sonardepth, int *error);
int mb_depint_interp(int verbose, void *mbio_ptr,
		double time_d, double *sonardepth,
		int *error);
int mb_altint_add(int verbose, void *mbio_ptr, double time_d, double altitude, int *error);
int mb_altint_interp(int verbose, void *mbio_ptr,
		double time_d, double *altitude,
		int *error);
int mb_loadnavdata(int verbose, char *merge_nav_file, int merge_nav_format, int merge_nav_lonflip,
                int *merge_nav_num, int *merge_nav_alloc,
                double **merge_nav_time_d, double **merge_nav_lon,
                double **merge_nav_lat, double **merge_nav_speed, int *error);
int mb_loadsensordepthdata(int verbose, char *merge_sensordepth_file, int merge_sensordepth_format,
                int *merge_sensordepth_num, int *merge_sensordepth_alloc,
                double **merge_sensordepth_time_d, double **merge_sensordepth_sensordepth,
                int *error);
int mb_loadaltitudedata(int verbose, char *merge_altitude_file, int merge_altitude_format,
                int *merge_altitude_num, int *merge_altitude_alloc,
                double **merge_altitude_time_d, double **merge_altitude_altitude,
		int *error);
int mb_loadheadingdata(int verbose, char *merge_heading_file, int merge_heading_format,
                int *merge_heading_num, int *merge_heading_alloc,
                double **merge_heading_time_d, double **merge_heading_heading,
                int *error);
int mb_loadattitudedata(int verbose, char *merge_attitude_file, int merge_attitude_format,
                int *merge_attitude_num, int *merge_attitude_alloc,
                double **merge_attitude_time_d, double **merge_attitude_roll,
                double **merge_attitude_pitch, double **merge_attitude_heave,
                int *error);
int mb_loadsoundspeeddata(int verbose, char *merge_soundspeed_file, int merge_soundspeed_format,
                int *merge_soundspeed_num, int *merge_soundspeed_alloc,
                double **merge_soundspeed_time_d, double **merge_soundspeed_soundspeed,
		int *error);
int mb_loadtimeshiftdata(int verbose, char *merge_timeshift_file, int merge_timeshift_format,
                int *merge_timeshift_num, int *merge_timeshift_alloc,
                double **merge_timeshift_time_d, double **merge_timeshift_timeshift,
                int *error);

int mb_swap_check(void);
int mb_get_double(double *, char *, int);
int mb_get_int(int *, char *, int);
int mb_get_binary_short(int, void *, void *);
int mb_get_binary_int(int, void *, void *);
int mb_get_binary_float(int, void *, void *);
int mb_get_binary_double(int, void *, void *);
int mb_get_binary_long(int, void *, void *);
int mb_put_binary_short(int, short, void *);
int mb_put_binary_int(int, int, void *);
int mb_put_binary_float(int, float, void *);
int mb_put_binary_double(int, double, void *);
int mb_put_binary_long(int, mb_s_long, void *);
int mb_get_bounds (char *text, double *bounds);
double mb_ddmmss_to_degree (char *text);
int mb_takeoff_to_rollpitch(int verbose,
		double theta, double phi,
		double *alpha, double *beta,
		int *error);
int mb_rollpitch_to_takeoff(int verbose,
		double alpha, double beta,
		double *theta, double *phi,
		int *error);
int mb_xyz_to_takeoff(int verbose,
		double x, double y, double z,
		double *theta, double *phi,
		int *error);
int mb_lever(int verbose,
		double sonar_offset_x,
		double sonar_offset_y,
		double sonar_offset_z,
		double nav_offset_x,
		double nav_offset_y,
		double nav_offset_z,
		double vru_offset_x,
		double vru_offset_y,
		double vru_offset_z,
		double vru_pitch,
		double vru_roll,
		double *lever_x,
		double *lever_y,
		double *lever_z,
		int *error);
int mb_mergesort(void *base, size_t nmemb,register size_t size, int (*cmp) (const void *, const void *));
int mb_double_compare(const void *a, const void *b);
int mb_int_compare(const void *a, const void *b);
int mb_edit_compare(const void *a, const void *b);
void hilbert(int n, double delta[], double kappa[]);
void hilbert2(int n, double data[]);

int mb_absorption(int verbose,
		double frequency, double temperature,double salinity,
		double depth, double ph, double soundspeed,
		double *absorption, int *error);

int mb_mem_debug_on(int verbose, int *error);
int mb_mem_debug_off(int verbose, int *error);
int mb_malloc(int verbose, size_t size, void **ptr, int *error);
int mb_realloc(int verbose, size_t size, void **ptr, int *error);
int mb_free(int verbose, void **ptr, int *error);
int mb_mallocd(int verbose, const char *sourcefile, int sourceline, size_t size, void **ptr, int *error);
int mb_reallocd(int verbose, const char *sourcefile, int sourceline, size_t size, void **ptr, int *error);
int mb_freed(int verbose, const char *sourcefile, int sourceline, void **ptr, int *error);
int mb_memory_clear(int verbose, int *error);
int mb_memory_status(int verbose, int *nalloc, int *nallocmax,
			int *overflow, size_t *allocsize, int *error);
int mb_memory_list(int verbose, int *error);
int mb_register_array(int verbose, void *mbio_ptr,
		int type, size_t size, void **handle, int *error);
int mb_update_arrays(int verbose, void *mbio_ptr,
		int nbath, int namp, int nss, int *error);
int mb_update_arrayptr(int verbose, void *mbio_ptr,
		void **handle, int *error);
int mb_list_arrays(int verbose, void *mbio_ptr, int *error);
int mb_deall_ioarrays(int verbose, void *mbio_ptr, int *error);

int mb_get_time(int verbose, int time_i[7], double *time_d);
int mb_get_date(int verbose, double time_d, int time_i[7]);
int mb_get_date_string(int verbose, double time_d, char *string);
int mb_get_jtime(int verbose, int time_i[7], int time_j[5]);
int mb_get_itime(int verbose, int time_j[5], int time_i[7]);
int mb_fix_y2k(int verbose, int year_short, int *year_long);
int mb_unfix_y2k(int verbose, int year_long, int *year_short);

int mb_proj_init(int verbose,
		char *projection,
		void **pjptr,
		int *error);
int mb_proj_free(int verbose,
		void **pjptr,
		int *error);
int mb_proj_forward(int verbose,
		void *pjptr,
		double lon, double lat,
		double *easting, double *northing,
		int *error);
int mb_proj_inverse(int verbose,
		void *pjptr,
		double easting, double northing,
		double *lon, double *lat,
		int *error);
int mb_proj_transform(int verbose,
		void *pjsrcptr,
		void *pjdstptr,
		int npoint,
		double *x, double *y, double *z,
		int *error);

int mb_swap_check();
int mb_swap_float(float *a);
int mb_swap_double(double *a);
int mb_swap_long(mb_s_long *a);

int mb_beaudoin(int verbose,
		mb_3D_orientation tx_align,
		mb_3D_orientation tx_orientation,
		double tx_steer,
		mb_3D_orientation rx_align,
		mb_3D_orientation rx_orientation,
		double rx_steer,
		double reference_heading,
		double *beamAzimuth,
		double *beamDepression,
		int	*error);
int mb_beaudoin_unrotate (int verbose,
                            mb_3D_vector orig,
                            mb_3D_orientation rotate,
                            mb_3D_vector *final,
                            int *error);

/* mb_rt function prototypes */
int mb_rt_init(int verbose, int number_node,
		double *depth, double *velocity,
		void **modelptr, int *error);
int mb_rt_deall(int verbose, void **modelptr, int *error);
int mb_rt(int verbose, void *modelptr,
	double source_depth, double source_angle, double end_time,
	int ssv_mode, double surface_vel, double null_angle,
	int nplot_max, int *nplot, double *xplot, double *zplot,
	double *x, double *z, double *travel_time, int *ray_stat, int *error);

/* end conditional include */
#endif
