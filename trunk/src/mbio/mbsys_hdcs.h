/*--------------------------------------------------------------------
 *    The MB-system:	mbsys_hdcs.h	3/16/99
 *	$Id: mbsys_hdcs.h,v 5.2 2001-07-20 00:32:54 caress Exp $
 *
 *    Copyright (c) 1999, 2000 by
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
 * mbsys_hdcs.h defines the data structure used by MBIO functions
 * to store swath sonar data in the UNB OMG HDCS format:
 *      MBF_OMGHDCSJ : MBIO ID 151
 *
 * Author:	D. W. Caress
 * Date:	March 16, 1999
 *
 * $Log: not supported by cvs2svn $
 * Revision 5.1  2001/01/22  07:43:34  caress
 * Version 5.0.beta01
 *
 * Revision 5.0  2000/12/01  22:48:41  caress
 * First cut at Version 5.0.
 *
 * Revision 4.1  2000/09/30  06:31:19  caress
 * Snapshot for Dale.
 *
 * Revision 4.0  1999/03/31  18:29:20  caress
 * MB-System 4.6beta7
 *
 * Revision 1.1  1999/03/31  18:11:35  caress
 * Initial revision
 *
 *
 */
/*
 * Notes on the MBSYS_HDCS data structure:
 *   1. The OMG-HDCS format is a collection of similar data
 *      formats used by the seafloor mapping software 
 *      developed by Dr. John Hughes Clarke (Ocean Mapping 
 *      Group of the University of New Brunswick). A variety
 *      of sonars are supported in OMG-HDCS.
 *   2. OMG-HDCS files all begin with a summary header that
 *      specifies the format version, the data source
 *      (type of sonar), the number of records,  and the
 *      minimum and maximum values of position and data
 *      values.
 *   3. The summary header is followed by a set of uniformly
 *      sized data records. The data record size is determined
 *      by the format version and data source. Ecch record is
 *      is divided into a profile (header) and an array of
 *      beam structures.
 *   4. Sidescan imagery can be stored in parallel files in
 *      the same directory as the primary bathymetry and
 *      amplitude files. The sidescan files have the suffix
 *      ".ss_data" added to the end of the primary file's name.
 *   5. Comment records are encoded in MBIO by setting the first
 *      eight bytes of the data record header (profile) to '#' 
 *      values. The profile part of the comment record will have
 *      the same size as expected for data from the tool type
 *      listed in the summary. However, the data section for
 *      comment records will always be 256 bytes long regardless
 *      of the tool type. This mechanism is supported only by 
 *      MB-System.
 *
 */

/* defines sizes and maximums */
#define	MBSYS_HDCS_SUMMARY_SIZE	96
#define	MBSYS_HDCS_MAX_COMMENT	252
#define	MBSYS_HDCS_MAX_BEAMS		1440
#define	MBSYS_HDCS_MAX_PIXELS		1024

/* define tools (sonars) supported by OMG_HDCS */
#define MBSYS_HDCS_NUM_TOOLS		25   /* as of March 1998 */
#define MBSYS_HDCS_None			-1
#define MBSYS_HDCS_SingleBeam		0
#define MBSYS_HDCS_ELAC_BottomChart	1
#define MBSYS_HDCS_EM12_dual		2
#define MBSYS_HDCS_EM100_depth		3
#define MBSYS_HDCS_FanSweep		4
#define MBSYS_HDCS_SeaBeam		5
#define MBSYS_HDCS_EM3000		6
#define MBSYS_HDCS_Navitronics_Smith	7
#define MBSYS_HDCS_EM12_single		8
#define MBSYS_HDCS_EM100_depth_ss	9
#define MBSYS_HDCS_EM1000		10
#define MBSYS_HDCS_LADS_2ndary		11
#define MBSYS_HDCS_EM3000D 		12
#define MBSYS_HDCS_SB2100		13
#define MBSYS_HDCS_ISIS_Submetrix	14
#define MBSYS_HDCS_EM1000_ampl		15
#define MBSYS_HDCS_SB2K      		16
#define MBSYS_HDCS_Seabat9001		17
#define MBSYS_HDCS_FanSweep_10A		18
#define MBSYS_HDCS_FanSweep_20		19
#define MBSYS_HDCS_ISIS_SWA		20
#define MBSYS_HDCS_SeaBeam_1180_MkII	21
#define MBSYS_HDCS_SeaBat_8101		22
#define MBSYS_HDCS_EM300		23
#define MBSYS_HDCS_EM121A		24
#define MBSYS_HDCS_COMMENT		999

/* define beam record structure */
struct mbsys_hdcs_beam_struct 
	{
        int	status;           /* status is either OK (0) or bad (other) */
        int	observedDepth;    /* Depth (mm)                            */
        int	acrossTrack;      /* Across track position of depth (mm)   */
        int	alongTrack;       /* Along track position of depth (mm)    */
        int	latOffset;        /* Latitude offset wrt. profile          */
        int	longOffset;       /* Longitude offset wrt. profile         */
        int	processedDepth;    /* Depth (mm)                            */
	int	timeOffset;
        int	depthAccuracy;    /* Depth accuracy (mm)                   */
	mb_u_char reflectivity;  
	char	Q_factor;  /* phase or amplitude detection */ 
	char	beam_no;  
	char	freq;   /* 12.7, 13.0, 13.3, 95.0, Smii, GLORIA */

	char	calibratedBackscatter; /* effects of power/TVG and atten. 
					    removed*/
	char	mindB;		
	char	maxdB;
	mb_u_char	pseudoAngleIndependentBackscatter;		
					/* corrected for mean angular dependence
					    for geological visualisation */
	int	range;   /* other option on EM 12 */
	int	no_samples;
	int	offset;
	int	centre_no;
	char	sample_unit; /* whether in time or distance */
	char	sample_interval; /* seconds or metres */
	char	dummy[2];
	mb_u_char	samp_win_length;
	short	beam_depress_angle;
	unsigned short	beam_heading_angle;
	};

/* structure to hold everything */
struct mbsys_hdcs_struct
	{
	int    kind;
	int    read_summary;
	int    profile_size;
	int    num_beam;
	int    beam_size;
	int    data_size;
	int    image_size;
        int    sensorNumber;	/*  1 = depth file */
        int    subFileID;	/*  1 = data (as opposed to index) */
        int    fileVersion;	/* 1 = original format */
				/* 2 = packed format for EM1000 and others */
				/* 3 = packed format for EM300 */
        int    toolType;	/* Tool Type implies Profile Record Size
					and Depth Record Size
					and Image Record Size */
        int    numProfiles;	/* # of profiles in the file             */
        int    numDepths_sum;	/* # of depths in the file             */
        int    timeScale;	/* time scale (# of uSec. units) */
        int    refTime;		/* Reference time (100 sec. units)       */
        int    minTime;		/* Minimum time (offset wrt. ref.)       */
        int    maxTime;		/* Maximum time (offset wrt. ref.)       */
        int    positionType;	/* Geographic(1)/ UTM(2) */
        int    positionScale;	/* Position scale (# of nRad. units)     */
        int    refLat;		/* Reference latitude (100 nRadians)     */
        int    minLat;		/* Minimum latitude (offset wrt. ref.)   */
        int    maxLat;		/* Maximum latitude (offset wrt. ref.)   */
        int    refLong;		/* Reference longitude (100 nRadians)    */
        int    minLong;		/* Minimum longitude (offset wrt. ref.)  */
        int    maxLong;		/* Maximum longitude (offset wrt. ref.)  */
        int    minObsDepth;	/* Minimum depth (mm)                    */
        int    maxObsDepth;	/* Maximum depth (mm)                    */
        int    minProcDepth;	/* Minimum depth (mm)                    */
        int    maxProcDepth;	/* Maximum depth (mm)                    */
        int    status_sum;	/* status not actually used at all ....  */

        int    status_pro;	/* status is either OK (0) 
					or no nav (1)
					or unwanted for gridding (2) 
					or comment record (999) (MB-System only) */
        int    numDepths_pro;	/* Number of depths in profile        */
        int    numSamples;	/* Number of sidescan samples in parallel file        */
        int    timeOffset;	/* Time offset  wrt. header           */
        int    vesselLatOffset;	/* Latitude offset wrt. header        */
        int    vesselLongOffset;/* Longitude offset wrt. header       */
        int    vesselHeading;	/* Heading (100 nRadians)             */
        int    vesselHeave;	/* Heave (mm)                         */
        int    vesselPitch;	/* Vessel pitch (100 nRadians)        */
        int    vesselRoll;	/* Vessel roll (100 nRadians)         */
        int    tide;		/* Tide (mm)                          */
        int    vesselVelocity;	/* Vessel Velocity (mm/s)
					note - transducer pitch is 
					generally tucked into the vel field     */
	char    power;
	char    TVG;
	char    attenuation;
	char    edflag;
        int	soundVelocity; 	/* mm/s */
	int	lengthImageDataField;
        int	pingNo;
	char	mode;  
	char	Q_factor;  
	char	pulseLength;   /* centisecs*/
	mb_u_char unassigned;  
	unsigned short td_sound_speed;
	unsigned short samp_rate;
	mb_u_char z_res_cm;
	mb_u_char xy_res_cm;
	mb_u_char ssp_source;
	mb_u_char filter_ID;
	unsigned short absorp_coeff;
	unsigned short tx_pulse_len;
	unsigned short tx_beam_width;
	unsigned short max_swath_width;
	mb_u_char tx_power_reduction;
	mb_u_char rx_beam_width;
	mb_u_char rx_bandwidth;
	mb_u_char rx_gain_reduction;
	mb_u_char tvg_crossover;
	mb_u_char beam_spacing;
	mb_u_char coverage_sector;
	mb_u_char yaw_stab_mode;
	struct mbsys_hdcs_beam_struct *beams;
	mb_s_char	*ss_raw;
	int	pixel_size;	/* processed sidescan pixel size in mm */
	int	pixels_ss;	/* number of processed sidescan pixels stored */
	short	ss_proc[MBSYS_HDCS_MAX_PIXELS];
				/* the processed sidescan ordered port to starboard */
	int	ssalongtrack[MBSYS_HDCS_MAX_PIXELS];
				/* the processed sidescan alongtrack distances 
					in mm */
	char	comment[MBSYS_HDCS_MAX_COMMENT];
	};
	
/* system specific function prototypes */
int mbsys_hdcs_alloc(int verbose, void *mbio_ptr, void **store_ptr, 
			int *error);
int mbsys_hdcs_deall(int verbose, void *mbio_ptr, void **store_ptr, 
			int *error);
int mbsys_hdcs_extract(int verbose, void *mbio_ptr, void *store_ptr, 
			int *kind, int time_i[7], double *time_d,
			double *navlon, double *navlat,
			double *speed, double *heading,
			int *nbath, int *namp, int *nss,
			char *beamflag, double *bath, double *amp, 
			double *bathacrosstrack, double *bathalongtrack,
			double *ss, double *ssacrosstrack, double *ssalongtrack,
			char *comment, int *error);
int mbsys_hdcs_insert(int verbose, void *mbio_ptr, void *store_ptr, 
			int kind, int time_i[7], double time_d,
			double navlon, double navlat,
			double speed, double heading,
			int nbath, int namp, int nss,
			char *beamflag, double *bath, double *amp, 
			double *bathacrosstrack, double *bathalongtrack,
			double *ss, double *ssacrosstrack, double *ssalongtrack,
			char *comment, int *error);
int mbsys_hdcs_ttimes(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, int *nbeams,
			double *ttimes, double *angles, 
			double *angles_forward, double *angles_null,
			double *heave, double *alongtrack_offset, 
			double *draft, double *ssv, int *error);
int mbsys_hdcs_extract_altitude(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, double *transducer_depth, double *altitude, 
			int *error);
int mbsys_hdcs_insert_altitude(int verbose, void *mbio_ptr, void *store_ptr,
			double transducer_depth, double altitude, 
			int *error);
int mbsys_hdcs_extract_nav(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, int time_i[7], double *time_d,
			double *navlon, double *navlat,
			double *speed, double *heading, double *draft, 
			double *roll, double *pitch, double *heave, 
			int *error);
int mbsys_hdcs_insert_nav(int verbose, void *mbio_ptr, void *store_ptr,
			int time_i[7], double time_d,
			double navlon, double navlat,
			double speed, double heading, double draft, 
			double roll, double pitch, double heave,
			int *error);
int mbsys_hdcs_copy(int verbose, void *mbio_ptr, 
			void *store_ptr, void *copy_ptr,
			int *error);
