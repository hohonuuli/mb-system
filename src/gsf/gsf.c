/********************************************************************
 *
 * Module Name : GSF
 *
 * Author/Date : J. S. Byrne / 3 May 1994
 *
 * Description : This source file contains the gsf library entry point
 *  functions for accessesing multibeam sonar data in a generic byte stream
 *  format.  Each record in these binary files contains an ID and a size,
 *  and these two values are used to read and decode the rest of the data
 *  record.  Data records are read/written one at a time, in a sequential
 *  manner.  Refer to the DoDBL Generic Sensor format design documentation
 *  for a more detailed verbal description of the data format.
 *
 * Restrictions/Limitations :
 * 1) This library assumes the host computer uses the ASCII character set.
 * 2) This library assumes that the type short is 16 bits, and that the type
 *    int is 32 bits.
 ******
 * NOTE
 * Not (yet!) supported on a machine with 64 bit architecture.
 ******
 *
 * Change Descriptions :
 * who  when      what
 * ---  ----      ----
 * jsb  10-25-94  Added gsfOpenBuffered function call, and call to setvbuf
 *                in gsfOpen to deal with high data rate multibeam data.
 * jsb  08-14-95  Direct and sequential access now work through common
 *                gsfRead and gsfWrite API. All pointers to dynamically
 *                allocated memory are now maintained by the library.
 *                Call this version "GSF-v01.01".
 * jsb  11/01/95  Completed modifications to indexing to support increase in
 *                gsf file size after initial index file creation.  The size
 *                of the file is now stored in the index file header. Index
 *                files without the expected header are recreated on the first
 *                open. This is still version GSF-v01.01. Also added a unique
 *                sensor specific subrecord for Simrad em1000.
 * jsb  12/22/95  Added gsfGetMBParams, gsfPutMBParams, gsfIsStarboardPing,
 *                and gsfGetSwathBathyBeamWidths. Also added GSF_APPEND as
 *                a file access mode, and modifed GSF_CREATE access mode so
 *                that files can be updated (read and written). This is gsf
 *                library version GSF-v01.02.
 * fd   04/15/96  Corrected the internals of gsfIsStarboardPing
 * hem  08/20/96  Added support for single beam pings; added gsfStringError;
 *                fixed 4 byte boundary padding.  This is gsf library
 *                version GSF-v1.03.
 * jsb  10/04/96  Changed fopen argument from "wb" to "a+b" for the GSF_APPEND
 *                access mode.  Also added logic to set file pointer to top prior
 *                to trying to read the gsf header record in gsfOpen/gsfOpenBuffered 
 *                when the file access mode is GSF_APPEND.  Replaced use of 
 *                numOpenFiles with *handle as argument to gsfRead and gsfWrite
 *                within gsfOpen and gsfOpenBuffered.  This repairs problems which
 *                can occur when a single application is accessing multiple files.
 * jsb  04/18/97  Added gsf version dependancy on approach to padding records out
 *                to four byte boundary. This is required in order to support the
 *                update access modes for versions prior to 1.03.  Replaced use of
 *                fgetpos, fsetpos with ftell, fseek.  This was done so that we can
 *                compair the previous_record field of the file table with addresses
 *                from an index file. Modified gsfStringError so that there is a single
 *                return statement, this was done to eliminate "statement not reached"
 *                compile warnings.
 * bac  10/27/97  Added a case in gsfGetSwathBathyBeamWidths for the Sea Beam 2112/36.
 * dwc 1/9/98     Added a case in gsfGetSwathBathyBeamWidths for the Elac Bottomchart MkII.
 * bac  03/15/98  Added an array subrecord for signal to noise ratio.
 * bac  03/15/98  Added an array subrecord for beam angle forward.
 *
 * Classification : Unclassified
 *
 * References : DoDBL Generic Sensor Format Sept. 30, 1993
 *
 *
 * Copyright (C) Science Applications International Corp.
 ********************************************************************/

/* standard c library includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>

/* rely on the network type definitions of (u_short, and u_long) */
#include <sys/types.h>
#include <netinet/in.h>

/* gsf library interface description */
#include "gsf.h"

/* get the prototypes for the gsf encode and gsf decode functions */
#include "gsf_ft.h"
#include "gsf_enc.h"
#include "gsf_dec.h"
#include "gsf_indx.h"

/* Macros required for this module */
#define GSF_FILL_SIZE 8                   /* gsf packaging with no checksum */
#define GSF_FILL_SIZE_CHECKSUM 12         /* gsf packaging with checksum */
#define GSF_STREAM_BUF_SIZE 8192          /* gsf default stream buffer size */
#define GSF_UNKNOWN_PARAM_TEXT "UNKNWN"   /* Flag value for unknown parameter value */

/* Static Global data for this module */
static unsigned char streamBuff[GSF_MAX_RECORD_SIZE];
static int      numOpenFiles;
static GSF_FILE_TABLE gsfFileTable[GSF_MAX_OPEN_FILES];

/* Global external data defined in this module */
int             gsfError;       /* used to report most recent error */

/* Static functions used, but not exported from this source file */
static gsfuLong gsfChecksum(unsigned char *buff, unsigned int num_bytes);
static int      gsfSeekRecord(int handle, gsfDataID *id);
static int      gsfUnpackStream (int handle, int desiredRecord, gsfDataID *dataID, gsfRecords *rptr, unsigned char *buf, int max_size);
static int      gsfSetParam(int handle, int index, char *val, gsfRecords *rec);
static int      gsfNumberParams(char *param);

/********************************************************************
 *
 * Function Name : gsfOpen
 *
 * Description : This function attempts to open a gsf data file.  If the
 *  file exits and is opened readonly or update the gsf header is read
 *  to confirm that this is a gsf data file.  If the file is opened create,
 *  the GSF header containing the version number of the software library is
 *  written into the header.  This function passes an integer handle back to
 *  the calling application.  The handle is used for all further access to the
 *  file. gsfOpen explicitly sets stream bufferring to the value specified
 *  by GSF_STREAM_BUF_SIZE.  The internal file table is searched for an
 *  available entry whose name matches that specified in the argument list, if
 *  no match is found, then the first available entry is used.  Up to
 *  GSF_MAX_OPEN_FILES files may be open by an application at a time.
 *
 * Inputs :
 *  filename = a fully qualified path to the gsf file to open
 *  mode may have the following values:
 *     GSF_READONLY = open an existing file for read only access
 *     GSF_UPDATE   = open an existing file for reading an writing
 *     GSF_CREATE   = create a new gsf file
 *     GSF_READONLY_INDEX = open an existing file for read only access with index
 *     GSF_UPDATE_INDEX   = open an existing file for reading an writing with index
 *  handle = a pointer to an integer to be assigned a handle which will be
 *     reference for all future file access.
 *
 * Returns :
 *  This funciton returns zero if successful, or -1 if an error occured.
 *
 * Error Conditions :
 *     GSF_BAD_ACCESS_MODE
 *     GSF_TOO_MANY_OPEN_FILES
 *     GSF_FOPEN_ERROR
 *     GSF_SETVBUF_ERROR
 *     GSF_UNRECOGNIZED_FILE
 *
 ********************************************************************/
int
gsfOpen(const char *filename, const int mode, int *handle)
{
    char           *access_mode;
    int             fileTableIndex;
    int             length;
    int             headerSize;
    int             ret;
    gsfDataID       id;
    struct stat     stat_buf;
    FILE           *fp;

    /* Clear the gsfError value each time a new file is opened */
    gsfError = 0;

    /* get the desired file access mode */
    switch (mode)
    {
        case GSF_CREATE:
            access_mode = "w+b";
            break;

        case GSF_READONLY:
            access_mode = "rb";
            break;

        case GSF_UPDATE:
            access_mode = "r+b";
            break;

        case GSF_READONLY_INDEX:
            access_mode = "rb";
            break;

        case GSF_UPDATE_INDEX:
            access_mode = "r+b";
            break;

        case GSF_APPEND:
            access_mode = "a+b";
            break;

        default:
            gsfError = GSF_BAD_ACCESS_MODE;
            return (-1);
    }

    /* check the number of files currently openned */
    if (numOpenFiles >= GSF_MAX_OPEN_FILES)
    {
        gsfError = GSF_TOO_MANY_OPEN_FILES;
        return (-1);
    }

    /* Try to open this file */
    if ((fp = fopen(filename, access_mode)) == (FILE *) NULL)
    {
        gsfError = GSF_FOPEN_ERROR;
        return (-1);
    }

    /* The file was successfully opened, load the gsf file table structure by
     * searching the gsf file table for the caller's filename.  This is done
     * so that the same file table slot may be re-used.  Applications which
     * want their file closed frequently, such as real-time data collection
     * programs may do this to assure data integrity, and it makes sense
     * to resuse the file table slot they occupied from a previous call to
     * gsfOpen, so that the ping scale factors don't have to be reset except
     * when a new file is created.
     */
    numOpenFiles++;
    length = strlen (filename);
    if (length >= sizeof(gsfFileTable[0].file_name))
    {
        length = sizeof(gsfFileTable[0].file_name) - 1;
    }
    for (fileTableIndex=0; fileTableIndex<GSF_MAX_OPEN_FILES; fileTableIndex++)
    {
        if ((memcmp(gsfFileTable[fileTableIndex].file_name, filename, length) == 0) &&
            (gsfFileTable[fileTableIndex].occupied == 0))
        {
            break;
        }
    }

    /* If no filename match was found then use the first available slot */
    if (fileTableIndex == GSF_MAX_OPEN_FILES)
    {
        for (fileTableIndex=0; fileTableIndex<GSF_MAX_OPEN_FILES; fileTableIndex++)
        {
            if (gsfFileTable[fileTableIndex].occupied == 0)
            {
                strncpy (gsfFileTable[fileTableIndex].file_name, filename, sizeof(gsfFileTable[fileTableIndex].file_name));
                /* This is the first open for this file, so clear the
                 * pointers to dynamic memory.
                 */
                gsfFree (&gsfFileTable[fileTableIndex].rec);
                break;
            }
        }
    }

    gsfFileTable[fileTableIndex].fp = fp;
    gsfFileTable[fileTableIndex].buf_size = GSF_STREAM_BUF_SIZE;
    gsfFileTable[fileTableIndex].occupied = 1;
    *handle = fileTableIndex + 1;

    /* Set the desired buffer size */
    if (setvbuf(fp, NULL, _IOFBF, GSF_STREAM_BUF_SIZE))
    {
        gsfClose ((const int) *handle);
        gsfError = GSF_SETVBUF_ERROR;
        return (-1);
    }

    /* Use stat to get the size of this file. File size is used by gsfPercent */
    if (stat (filename, &stat_buf))
    {
        gsfError = GSF_READ_ERROR;
        return(-1);
    }
    gsfFileTable[fileTableIndex].file_size = (long) stat_buf.st_size;

    /* If this file was just created, (ie it has a size of 0 bytes) then
     * write the gsf file header record. Also, set a flag to indicate
     * that the ping scale factors need to be written with the next swath
     * bathymetry ping record.
     */
    if (stat_buf.st_size == 0)
    {
        gsfFileTable[fileTableIndex].scales_read = 1;

        /* write the gsf file header to the file */
        id.checksumFlag = 0;
        id.reserved = 0;
        id.recordID = GSF_RECORD_HEADER;
        strcpy(gsfFileTable[fileTableIndex].rec.header.version, GSF_VERSION);
        gsfFileTable[fileTableIndex].bufferedBytes += gsfWrite(*handle, &id, &gsfFileTable[fileTableIndex].rec);

        /* Flush this record to disk so that the file size will be non-zero
         * on the next call to gsfOpen.
         */
        if (fflush (gsfFileTable[fileTableIndex].fp))
        {
            gsfError = GSF_FLUSH_ERROR;
            return(-1);
        }
    }
    else
    {
        /* Read the GSF header, if the access mode is append, we need to
         * seek back to the top of the file.
         */
        if (mode == GSF_APPEND)
        {
            if (fseek(gsfFileTable[fileTableIndex].fp, 0, SEEK_SET))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
        }
        /* Read the GSF header */
        headerSize = gsfRead(*handle, GSF_NEXT_RECORD, &id, &gsfFileTable[fileTableIndex].rec, NULL, 0);
        if (!strstr(gsfFileTable[fileTableIndex].rec.header.version, "GSF-"))
        {
            fclose(fp);
            numOpenFiles--;
            *handle = 0;
            gsfFileTable[fileTableIndex].occupied = 0;
            memset(&gsfFileTable[fileTableIndex].rec.header, 0, sizeof(gsfFileTable[fileTableIndex].rec.header));
            gsfError = GSF_UNRECOGNIZED_FILE;
            return (-1);
        }
        /* If the mode is append seek back to the end of the file */
        if (mode == GSF_APPEND)
        {
            if (fseek(gsfFileTable[fileTableIndex].fp, 0, SEEK_END))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
        }
    }

    /* jsb 04/16/97 Save the GSF version ID into the file table */
    ret = sscanf (gsfFileTable[fileTableIndex].rec.header.version, "GSF-v%d.%d", 
        &gsfFileTable[fileTableIndex].major_version_number,
        &gsfFileTable[fileTableIndex].minor_version_number);
    if (ret != 2)
    {
        gsfError = GSF_UNRECOGNIZED_FILE;
        return (-1);
    } 

    /*  Set the update flag if needed. This is used to force a call to fflush
     *  between read an write operations, on files opened for update.
     */
    if ((mode == GSF_UPDATE) ||
        (mode == GSF_UPDATE_INDEX) ||
        (mode == GSF_CREATE))
    {
        gsfFileTable[fileTableIndex].update_flag = 1;
    }
    else
    {
        gsfFileTable[fileTableIndex].update_flag = 0;
    }

    /* Set the index flag and open the index file if needed. */
    if ((mode == GSF_READONLY_INDEX) || (mode == GSF_UPDATE_INDEX))
    {
        gsfFileTable[fileTableIndex].direct_access = 1;
        if (gsfOpenIndex (filename, *handle, &gsfFileTable[fileTableIndex]) == -1)
        {
            gsfFileTable[fileTableIndex].direct_access = 0;
            return (-1);
        }

        /* Move the file pointer back to the first record past the gsf file header. This
         * is required since we will have read the entire to create the index.
         */
        if (fseek(gsfFileTable[fileTableIndex].fp, headerSize, SEEK_SET))
        {
            gsfError = GSF_FILE_SEEK_ERROR;
            return (-1);
        }
    }
    else
    {
        gsfFileTable[fileTableIndex].direct_access = 0;
    }

    /* Save the file acess mode */
    switch (mode)
    {
        case GSF_CREATE:
            gsfFileTable[fileTableIndex].access_mode = GSF_CREATE;
            break;

        case GSF_READONLY:
            gsfFileTable[fileTableIndex].access_mode = GSF_READONLY;
            break;

        case GSF_UPDATE:
            gsfFileTable[fileTableIndex].access_mode = GSF_UPDATE;
            break;

        case GSF_READONLY_INDEX:
            gsfFileTable[fileTableIndex].access_mode = GSF_READONLY_INDEX;
            break;

        case GSF_UPDATE_INDEX:
            gsfFileTable[fileTableIndex].access_mode = GSF_UPDATE_INDEX;
            break;

        case GSF_APPEND:
            gsfFileTable[fileTableIndex].access_mode = GSF_APPEND;
            break;

        default:
            gsfError = GSF_BAD_ACCESS_MODE;
            return (-1);
    }

    return (0);
}

/********************************************************************
 *
 * Function Name : gsfOpenBuffered
 *
 * Description : This function attempts to open a gsf data file.  If the
 *  file exits and is opened readonly or update the gsf header is read
 *  to confirm that this is a gsf data file.  If the file is opened create,
 *  the GSF header containing the version number of the software library is
 *  written into the header.  This function passes an integer handle back to
 *  the calling application.  The handle is used for all further access to the
 *  file. gsfOpenBufferd explicitly sets stream bufferring to the value
 *  specified by the buf_size argument. The internal file table is searched
 *  for an available entry whose name matches that specified in the argument
 *  list, if no match is found, then the first available entry is used.  Up
 *  to GSF_MAX_OPEN_FILES files may be open by an application at a time.
 *  gsfOpenBuffered performs identical processing to gsfOpen, except here,
 *  the caller is allowed to explicitly set the standard system library level
 *  I/O buffer size.
 *
 * Inputs :
 *  filename = a fully qualified path to the gsf file to open
 *  mode may have the following values:
 *     GSF_READONLY = open an existing file for read only access
 *     GSF_UPDATE   = open an existing file for reading an writing
 *     GSF_CREATE   = create a new gsf file
 *     GSF_READONLY_INDEX = open an existing file for read only access with index
 *     GSF_UPDATE_INDEX   = open an existing file for reading an writing with index
 *  handle = a pointer to an integer to be assigned a handle which will be
 *     reference for all future file access.
 *  buf_size = an integer buffer size in bytes.
 *
 * Returns :
 *  This funciton returns zero if successful, or -1 if an error occured.
 *
 * Error Conditions :
 *     GSF_BAD_ACCESS_MODE
 *     GSF_TOO_MANY_OPEN_FILES
 *     GSF_FOPEN_ERROR
 *     GSF_SETVBUF_ERROR
 *     GSF_UNRECOGNIZED_FILE
 *
 ********************************************************************/
int
gsfOpenBuffered(const char *filename, const int mode, int *handle, int buf_size)
{
    char           *access_mode;
    int             fileTableIndex;
    int             length;
    int             headerSize;
    int             ret;
    gsfDataID       id;
    struct stat     stat_buf;
    FILE           *fp;

    /* Clear the gsfError value each time a new file is opened */
    gsfError = 0;

    /* get the desired file access mode */
    switch (mode)
    {
        case GSF_CREATE:
            access_mode = "w+b";
            break;

        case GSF_READONLY:
            access_mode = "rb";
            break;

        case GSF_UPDATE:
            access_mode = "r+b";
            break;

        case GSF_READONLY_INDEX:
            access_mode = "rb";
            break;

        case GSF_UPDATE_INDEX:
            access_mode = "r+b";
            break;

        case GSF_APPEND:
            access_mode = "a+b";
            break;

        default:
            gsfError = GSF_BAD_ACCESS_MODE;
            return (-1);
    }

    /* check the number of files currently openned */
    if (numOpenFiles >= GSF_MAX_OPEN_FILES)
    {
        gsfError = GSF_TOO_MANY_OPEN_FILES;
        return (-1);
    }

    /* Try to open this file */
    if ((fp = fopen(filename, access_mode)) == (FILE *) NULL)
    {
        gsfError = GSF_FOPEN_ERROR;
        return (-1);
    }

    /* The file was successfully opened, load the gsf file table structure by
     * searching the gsf file table for the caller's filename.  This is done
     * so that the same file table slot may be re-used.  Applications which
     * want their file closed frequently, such as real-time data collection
     * programs may do this to assure data integrity, and it makes sense
     * to resuse the file table slot they occupied from a previous call to
     * gsfOpen, so that the ping scale factors don't have to be reset except
     * when a new file is created.
     */
    numOpenFiles++;
    length = strlen (filename);
    if (length >= sizeof(gsfFileTable[0].file_name))
    {
        length = sizeof(gsfFileTable[0].file_name) - 1;
    }
    for (fileTableIndex=0; fileTableIndex<GSF_MAX_OPEN_FILES; fileTableIndex++)
    {
        if ((memcmp(gsfFileTable[fileTableIndex].file_name, filename, length) == 0) &&
            (gsfFileTable[fileTableIndex].occupied == 0))
        {
            break;
        }
    }

    /* If no filename match was found then use the first available slot */
    if (fileTableIndex == GSF_MAX_OPEN_FILES)
    {
        for (fileTableIndex=0; fileTableIndex<GSF_MAX_OPEN_FILES; fileTableIndex++)
        {
            if (gsfFileTable[fileTableIndex].occupied == 0)
            {
                strncpy (gsfFileTable[fileTableIndex].file_name, filename, sizeof(gsfFileTable[fileTableIndex].file_name));
                /* This is the first open for this file, so clear the
                 * pointers to dynamic memory.
                 */
                gsfFree (&gsfFileTable[fileTableIndex].rec);
                break;
            }
        }
    }

    gsfFileTable[fileTableIndex].fp = fp;
    gsfFileTable[fileTableIndex].buf_size = buf_size;
    gsfFileTable[fileTableIndex].occupied = 1;
    *handle = fileTableIndex + 1;

    /* Set the desired buffer size */
    if (setvbuf(fp, NULL, _IOFBF, buf_size))
    {
        gsfClose ((const int) *handle);
        gsfError = GSF_SETVBUF_ERROR;
        return (-1);
    }

    /* Use stat to get the size of this file. File size is used by gsfPercent */
    if (stat (filename, &stat_buf))
    {
        gsfError = GSF_READ_ERROR;
        return(-1);
    }
    gsfFileTable[fileTableIndex].file_size = (long) stat_buf.st_size;

    /* If this file was just created, (ie it has a size of 0 bytes) then
     * write the gsf file header record. Also, set a flag to indicate
     * that the ping scale factors need to be written with the next swath
     * bathymetry ping record.
     */
    if (stat_buf.st_size == 0)
    {
        gsfFileTable[fileTableIndex].scales_read = 1;

        /* write the gsf file header to the file */
        id.checksumFlag = 0;
        id.reserved = 0;
        id.recordID = GSF_RECORD_HEADER;
        strcpy(gsfFileTable[fileTableIndex].rec.header.version, GSF_VERSION);
        gsfFileTable[fileTableIndex].bufferedBytes += gsfWrite(*handle, &id, &gsfFileTable[fileTableIndex].rec);

        /* Flush this record to disk so that the file size will be non-zero
         * on the next call to gsfOpen.
         */
        if (fflush (gsfFileTable[fileTableIndex].fp))
        {
            gsfError = GSF_FLUSH_ERROR;
            return(-1);
        }
    }
    else
    {
        /* Read the GSF header, if the access mode is append, we need to
         * seek back to the top of the file.
         */
        if (mode == GSF_APPEND)
        {
            if (fseek(gsfFileTable[fileTableIndex].fp, 0, SEEK_SET))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
        }
        /* Read the GSF header */
        headerSize = gsfRead(*handle, GSF_NEXT_RECORD, &id, &gsfFileTable[fileTableIndex].rec, NULL, 0);
        if (!strstr(gsfFileTable[fileTableIndex].rec.header.version, "GSF-"))
        {
            fclose(fp);
            numOpenFiles--;
            *handle = 0;
            gsfFileTable[fileTableIndex].occupied = 0;
            memset(&gsfFileTable[fileTableIndex].rec.header, 0, sizeof(gsfFileTable[fileTableIndex].rec.header));
            gsfError = GSF_UNRECOGNIZED_FILE;
            return (-1);
        }
        /* If the mode is append seek back to the end of the file */
        if (mode == GSF_APPEND)
        {
            if (fseek(gsfFileTable[fileTableIndex].fp, 0, SEEK_END))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
        }
    }

    /* jsb 04/16/97 Save the GSF version ID into the file table */
    ret = sscanf (gsfFileTable[fileTableIndex].rec.header.version, "GSF-v%d.%d", 
        &gsfFileTable[fileTableIndex].major_version_number,
        &gsfFileTable[fileTableIndex].minor_version_number);
    if (ret != 2)
    {
        gsfError = GSF_UNRECOGNIZED_FILE;
        return (-1);
    } 

    /*  Set the update flag if needed. This is used to force a call to fflush
     *  between read an write operations, on files opened for update.
     */
    if ((mode == GSF_UPDATE) ||
        (mode == GSF_UPDATE_INDEX) ||
        (mode == GSF_CREATE))
    {
        gsfFileTable[fileTableIndex].update_flag = 1;
    }
    else
    {
        gsfFileTable[fileTableIndex].update_flag = 0;
    }

    /* Set the index flag and open the index file if needed. */
    if ((mode == GSF_READONLY_INDEX) || (mode == GSF_UPDATE_INDEX))
    {
        gsfFileTable[fileTableIndex].direct_access = 1;
        if (gsfOpenIndex (filename, *handle, &gsfFileTable[fileTableIndex]) == -1)
        {
            gsfFileTable[fileTableIndex].direct_access = 0;
            return (-1);
        }

        /* Move the file pointer back to the first record past the gsf file header. This
         * is required since we will have read the entire to create the index.
         */
        if (fseek(gsfFileTable[fileTableIndex].fp, headerSize, SEEK_SET))
        {
            gsfError = GSF_FILE_SEEK_ERROR;
            return (-1);
        }
    }
    else
    {
        gsfFileTable[fileTableIndex].direct_access = 0;
    }

    /* Save the file acess mode */
    switch (mode)
    {
        case GSF_CREATE:
            gsfFileTable[fileTableIndex].access_mode = GSF_CREATE;
            break;

        case GSF_READONLY:
            gsfFileTable[fileTableIndex].access_mode = GSF_READONLY;
            break;

        case GSF_UPDATE:
            gsfFileTable[fileTableIndex].access_mode = GSF_UPDATE;
            break;

        case GSF_READONLY_INDEX:
            gsfFileTable[fileTableIndex].access_mode = GSF_READONLY_INDEX;
            break;

        case GSF_UPDATE_INDEX:
            gsfFileTable[fileTableIndex].access_mode = GSF_UPDATE_INDEX;
            break;

        case GSF_APPEND:
            gsfFileTable[fileTableIndex].access_mode = GSF_APPEND;
            break;

        default:
            gsfError = GSF_BAD_ACCESS_MODE;
            return (-1);
    }

    return (0);
}

/********************************************************************
 *
 * Function Name : gsfClose
 *
 * Description : This function closes a gsf file previously openned
 *  using gsfOpen.
 *
 * Inputs :
 *  handle = the handle of the gsf file to be closed.
 *
 * Returns :
 *  This function returns zero if successful, or -1 if an error occured.
 *
 * Error Conditions :
 *    GSF_BAD_FILE_HANDLE
 *    GSF_FILE_CLOSE_ERROR
 *
 ********************************************************************/

int
gsfClose(const int handle)
{
    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    if (gsfFileTable[handle -1].direct_access)
    {
        gsfCloseIndex (&gsfFileTable[handle - 1]);
    }

    if (fclose(gsfFileTable[handle - 1].fp))
    {
        gsfError = GSF_FILE_CLOSE_ERROR;
        return (-1);
    }

    numOpenFiles--;

    /* jsb 05/14/97 Clear the contents of the gsfFileTable fields. We don't
     * want to clear the filename, this allows a performance improvement for
     * programs which use append to log gsf files. (ie: data acquisition) 
     */
    gsfFileTable[handle-1].major_version_number = 0;
    gsfFileTable[handle-1].minor_version_number = 0;
    gsfFileTable[handle-1].file_size = 0;
    gsfFileTable[handle-1].previous_record = 0;
    gsfFileTable[handle-1].buf_size = 0;
    gsfFileTable[handle-1].bufferedBytes = 0;
    gsfFileTable[handle-1].occupied = 0;
    gsfFileTable[handle-1].update_flag = 0;
    gsfFileTable[handle-1].direct_access = 0;
    gsfFileTable[handle-1].read_write_flag = 0;
    gsfFileTable[handle-1].scales_read = 0;
    gsfFileTable[handle-1].access_mode = 0;

    /* clear the contents of the index data table */
    if (gsfFileTable[handle-1].index_data.scale_factor_addr)
    { 
	free(gsfFileTable[handle-1].index_data.scale_factor_addr);
    }
    memset (&gsfFileTable[handle-1].index_data, 0, sizeof(gsfFileTable[handle-1].index_data));

    /* Clear the necessary fields of the gsfRecords data structure */
    memset(&gsfFileTable[handle-1].rec.header, 0, sizeof(gsfHeader));

    return (0);
}

/********************************************************************
 *
 * Function Name : gsfSeek
 *
 * Description : This function may be used to move the file pointer
 *  for a previously openned gsf file.
 *
 * Inputs :
 *  handle = the integer handle returned from gsf Open
 *  option = the desired action for moving the file pointer, where:
 *    GSF_REWIND, move pointer to first record in the file.
 *    GSF_END_OF_FILE, move pointer to the end of the file.
 *    GSF_PREVIOUS_RECORD, backup to the beginning of the record just
 *     written or just read.
 *
 * Returns :
 *  This function returns zero if successful, or -1 if an error occured.
 *
 * Error Conditions :
 *    GSF_BAD_FILE_HANDLE
 *    GSF_FILE_SEEK_ERROR
 *    GSF_BAD_SEEK_OPTION
 *
 ********************************************************************/

int
gsfSeek(int handle, int option)
{
    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    switch (option)
    {
        case GSF_REWIND:
            /* If the last operation was a write then we need to fflush */
            if (gsfFileTable[handle - 1].read_write_flag == LAST_OP_WRITE)
            {
                if (fflush (gsfFileTable[handle - 1].fp))
                {
                    gsfError = GSF_FLUSH_ERROR;
                    return(-1);
                }
            }
            gsfFileTable[handle - 1].read_write_flag = LAST_OP_FLUSH;

            if (fseek(gsfFileTable[handle - 1].fp, 0, SEEK_SET))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
            break;

        case GSF_END_OF_FILE:
            /* If the last operation was a write then we need to fflush */
            if (gsfFileTable[handle - 1].read_write_flag == LAST_OP_WRITE)
            {
                if (fflush (gsfFileTable[handle - 1].fp))
                {
                    gsfError = GSF_FLUSH_ERROR;
                    return(-1);
                }
            }
            gsfFileTable[handle - 1].read_write_flag = LAST_OP_FLUSH;

            if (fseek(gsfFileTable[handle - 1].fp, 0, SEEK_END))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
            break;

        case GSF_PREVIOUS_RECORD:
            if (fseek(gsfFileTable[handle - 1].fp, gsfFileTable[handle - 1].previous_record, SEEK_SET))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
            break;

        default:
            gsfError = GSF_BAD_SEEK_OPTION;
            return (-1);
    }

    return (0);
}

/********************************************************************
 *
 * Function Name : gsfRead
 *
 * Description : gsfRead supports both direct and sequential access. If the
 *  file is opened for sequential access, this function reads the desired
 *  record from the gsf data file specified by handle.  The "desiredRecord"
 *  argument may be set to GSF_NEXT_RECORD to read the next record in the
 *  data file, or "desiredRecord" record may be set to specify the record
 *  of interest, in which case the file will be read, skipping past
 *  intermediary records until the desired record is found.  When the desired
 *  record is found, it is read and then decoded from external to internal
 *  form. If the optional checksum is found with the data it will be verified.
 *  All of the fields of the gsfDataID structure, with the exception of the
 *  record_number field will be loaded with the values contained in the GSF
 *  record byte stream.  The record_number field will be undefined.  The
 *  stream and max_size arguments are normally set to NULL, unless the
 *  calling application is interested in a copy of the GSF byte stream.
 *
 *  If the file is opened for direct access, then the combination of the
 *  recordID and the record_number fields of the dataID structure are used
 *  to uniquely identify the record of interest.  The address for this record
 *  is retrieved from the index file, which was created on a previous call
 *  to gsfOpen or gsfOpenBuffered.  If the record of interest is a ping record
 *  for which we need to retrieve new scale factors, then the ping record
 *  containing the scale factors needed is read first, and then the ping
 *  record of interest is read.  Direct access applications should set the
 *  desiredRecord argument equal to the recordID field in the gsfDataID
 *  structure.
 *
 * Inputs :
 *    handle = the handle to the file as provided by gsfOpen
 *    desiredRecord = the desired record or GSF_NEXT_RECORD
 *    dataID = a pointer to a gsfDataID structure to be populated for the
 *             input record.
 *    rptr = a pointer to a gsfRecords structure to be populated with the
 *           data from the input record in internal form.
 *    buf = an optional pointer to caller memory to be populated with a copy
 *          of the gsf byte stream for this record.
 *    max_size = an optional maximum size to copy into buf
 *
 * Returns :
 *  This function returns the number of bytes read if successful,
 *  or -1 if an error occured.
 *
 * Error Conditions :
 *    GSF_BAD_FILE_HANDLE
 *    GSF_FILE_SEEK_ERROR
 *    GSF_FLUSH_ERROR
 *    GSF_READ_TO_END_OF_FILE
 *    GSF_READ_ERROR
 *    GSF_RECORD_SIZE_ERROR
 *    GSF_INSUFFICIENT_SIZE
 *    GSF_CHECKSUM_FAILURE
 *    GSF_UNRECOGNIZED_RECORD_ID
 *    GSF_HEADER_RECORD_DECODE_FAILED
 *    GSF_SVP_RECORD_DECODE_FAILED
 *    GSF_PROCESS_PARAM_RECORD_DECODE_FAILED
 *    GSF_SENSOR_PARAM_RECORD_DECODE_FAILED
 *    GSF_COMMENT_RECORD_DECODE_FAILED
 *    GSF_HISTORY_RECORD_DECODE_FAILED
 *    GSF_NAV_ERROR_RECORD_DECODE_FAILED
 ********************************************************************/

int
gsfRead(int handle, int desiredRecord, gsfDataID *dataID, gsfRecords *rptr, unsigned char *buf, int max_size)
{
    int             ret;
    gsfDataID       tmpID;

    /* Clear gsfError before each read */
    gsfError = 0;

    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    /* If this file is opened for direct access, then move the file pointer
     * to the record which the caller wants to read. Note that there is no
     * file re-positioning to be done if the caller wants the next record.
     */
    if ((gsfFileTable[handle - 1].direct_access) &&
        (desiredRecord != GSF_NEXT_RECORD))
    {
        memset(&tmpID, 0, sizeof(tmpID));
        tmpID.recordID = desiredRecord;
        tmpID.record_number = dataID->record_number;

        ret = gsfSeekRecord(handle, &tmpID);
        if (ret < 0)
        {
            /* gsfError is set in gsfSeekRecord */
            return (-1);
        }
    }

    ret = gsfUnpackStream (handle, desiredRecord, dataID, rptr, buf, max_size);

    return (ret);
}

/********************************************************************
 *
 * Function Name : gsfUnpackStream
 *
 * Description : gsfUnpackStream is a static function (not available to
 *   application programs) which is used by gsfRead to read and decode
 *   gsf records. It performs the bulk of the processing required to read
 *   a gsf record.  This processing exists as a function seperate from
 *   gsfRead since it is required both by gsfRead and by gsfSeekRecord.
 *   gsfUnpackStream is used by gsfSeekRecord to read a ping record with
 *   scale factors, which is required to support direct access.
 *
 * Inputs :
 *    handle = the handle to the file as provided by gsfOpen
 *    desiredRecord = the desired record or GSF_NEXT_RECORD
 *    dataID = a pointer to a gsfDataID structure to be populated for the
 *             input record.
 *    rptr = a pointer to a gsfRecords structure to be populated with the
 *           data from the input record in internal form.
 *    buf = an optional pointer to caller memory to be populated with a copy
 *          of the gsf byte stream for this record.
 *    max_size = an optional maximum size to copy into buf
 *
 * Returns :
 *  This function returns the number of bytes read if successful,
 *  or -1 if an error occured.
 *
 * Returns :
 *
 * Error Conditions :
 *    GSF_FILE_SEEK_ERROR
 *    GSF_FLUSH_ERROR
 *    GSF_READ_TO_END_OF_FILE
 *    GSF_RECORD_SIZE_ERROR
 *    GSF_INSUFFICIENT_SIZE
 *    GSF_CHECKSUM_FAILURE
 *    GSF_HEADER_RECORD_DECODE_FAILED
 *    GSF_SVP_RECORD_DECODE_FAILED
 *    GSF_PROCESS_PARAM_RECORD_DECODE_FAILED
 *    GSF_SENSOR_PARAM_RECORD_DECODE_FAILED
 *    GSF_COMMENT_RECORD_DECODE_FAILED
 *    GSF_HISTORY_RECORD_DECODE_FAILED
 *    GSF_NAV_ERROR_RECORD_DECODE_FAILED
 *
 ********************************************************************/

static int
gsfUnpackStream (int handle, int desiredRecord, gsfDataID *dataID, gsfRecords *rptr, unsigned char *buf, int max_size)
{
    int             readNext = 1;
    int             ret;
    size_t          readStat;
    gsfuLong        tmpBuff[2];
    gsfuLong        dataSize;
    size_t          readSize;
    gsfuLong        did;
    gsfDataID       thisID;
    gsfuLong        temp;
    unsigned char  *dptr = streamBuff;
    gsfuLong        ckSum;

    /* This loop will read one record at a time until the record type
     * desired by the caller is found.
     */
    while (readNext)
    {
        /* Get the current record pointer */
        if ((gsfFileTable[handle - 1].previous_record = ftell(gsfFileTable[handle - 1].fp)) == -1)
        {
            gsfError = GSF_FILE_SEEK_ERROR;
            return (-1);
        }

        /* If the file is open for update and the last operation was a write,
         * flush the buffer.
         */
        if ((gsfFileTable[handle - 1].update_flag) &&
            (gsfFileTable[handle - 1].read_write_flag == LAST_OP_WRITE))
        {
            if (fflush (gsfFileTable[handle - 1].fp))
            {
                gsfError = GSF_FLUSH_ERROR;
                return(-1);
            }
        }
        gsfFileTable[handle - 1].read_write_flag = LAST_OP_READ;

        /* read the data size, and gsf ID fields */
        readStat = fread((void *) tmpBuff, GSF_LONG_SIZE, (size_t) 2, gsfFileTable[handle - 1].fp);
        if (readStat != 2)
        {
            if (feof(gsfFileTable[handle - 1].fp))
            {
                gsfError = GSF_READ_TO_END_OF_FILE;
                return (-1);
            }
            gsfError = GSF_READ_ERROR;
            return (-1);
        }

        /* convert from gsf to host byte order, gsf byte order = network byte order */
        dataSize = (gsfuLong) ntohl(tmpBuff[0]);
        readSize = dataSize;
        did = (gsfuLong) ntohl(tmpBuff[1]);

        /* convert the did value into a gsfDataID struct
         * First the check sum value
         *
         * 1098 7654 3210 9876 5432 1098 7654 3210
         * 1000 0000 0000 0000 0000 0000 0000 0000
         *    8  0    0    0    0    0    0    0
         */
        thisID.checksumFlag = (did & 0x80000000);

        /* Now the reserved field
         * 1098 7654 3210 9876 5432 1098 7654 3210
         * 0111 1111 1100 0000 0000 0000 0000 0000
         *    7  F    C    0    0    0    0    0
         */
        temp = did & 0x7FC00000;
        thisID.reserved = (temp >> 22);

        /* Now the combination of registry number and data type number
         * 1098 7654 3210 9876 5432 1098 7654 3210
         * 0000 0000 0011 1111 1111 1111 1111 1111
         *    0  0    3    F    F    F    F    F
         */
        temp = did;
        thisID.recordID = (temp & 0x003FFFFF);

        /* if there is a checksum read it, we'll read four additional bytes */
        if (thisID.checksumFlag)
        {
            readSize = dataSize + 4;
            /* jsb 01-30-95
             * dptr += 4;
             */
            dptr = streamBuff + 4;
        }
        else
        {
            dptr = streamBuff;
        }

        /* Make sure that we have a big enough buffer to fit this record,
         *  then read it out.
         */
        if (readSize > GSF_MAX_RECORD_SIZE)
        {
            gsfError = GSF_RECORD_SIZE_ERROR;
            return (-1);
        }

        /* If the caller passed GSF_NEXT_RECORD, as the desiredRecord, they
         * want the next record
         */
        if ((desiredRecord == GSF_NEXT_RECORD) || (thisID.recordID == desiredRecord))
        {
            readNext = 0;
            /* Set the caller's ID structure with those items we've read */
            dataID->checksumFlag = thisID.checksumFlag;
            dataID->reserved = thisID.reserved;
            dataID->recordID = thisID.recordID;

            readStat = fread(streamBuff, (size_t) 1, readSize, gsfFileTable[handle - 1].fp);
            if (readStat != readSize)
            {
                if (feof(gsfFileTable[handle - 1].fp))
                {
                    gsfError = GSF_READ_TO_END_OF_FILE;
                    return (-1);
                }
                gsfError = GSF_READ_ERROR;
                return (-1);
            }
        }

        /* This record is not the requested record, advance the file pointer */
        else if (thisID.recordID != desiredRecord)
        {
            readStat = fseek(gsfFileTable[handle - 1].fp, readSize, SEEK_CUR);
            if (readStat)
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }
        }
    }

    /*
    * If the caller's buffer isn't null, move this data into their buffer.
    *  Don't move the 4 byte checksum into the buffer.
    */
    if ((buf) && (dataSize <= max_size))
    {
        memcpy(buf, dptr, dataSize);
    }
    else if ((buf) && (dataSize > max_size))
    {
        gsfError = GSF_INSUFFICIENT_SIZE;
        return (-1);
    }

    /* We have the record of interest, verify the checksum if required */
    if (thisID.checksumFlag)
    {
        memcpy(&tmpBuff[0], streamBuff, GSF_LONG_SIZE);
        ckSum = (gsfuLong) ntohl(tmpBuff[0]);
        if (ckSum != (gsfChecksum(dptr, dataSize)))
        {
            gsfError = GSF_CHECKSUM_FAILURE;
            return (-1);
        }
    }

    /* Invoke the appropriate function for unpacking this record into a
    * standard gsf structure.
    */
    switch (thisID.recordID)
    {
        case (GSF_RECORD_HEADER):
            ret = gsfDecodeHeader(&rptr->header, dptr);
            if (ret < 0)
            {
                gsfError = GSF_HEADER_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_SWATH_BATHY_SUMMARY):
            ret = gsfDecodeSwathBathySummary(&rptr->summary, dptr);
            if (ret < 0)
            {
                gsfError = GSF_SUMMARY_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_SWATH_BATHYMETRY_PING):
            ret = gsfDecodeSwathBathymetryPing(&rptr->mb_ping, dptr, &gsfFileTable[handle - 1], handle, dataSize);
            if (ret < 0)
            {
                /* gsfError is set within gsfDecodeSwathBathymetryPing */
                return (-1);
            }
            break;

        case (GSF_RECORD_SOUND_VELOCITY_PROFILE):
            ret = gsfDecodeSoundVelocityProfile(&rptr->svp, &gsfFileTable[handle - 1], dptr);
            if (ret < 0)
            {
                gsfError = GSF_SVP_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_PROCESSING_PARAMETERS):
            ret = gsfDecodeProcessingParameters(&rptr->process_parameters, &gsfFileTable[handle - 1], dptr);
            if (ret < 0)
            {
                gsfError = GSF_PROCESS_PARAM_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_SENSOR_PARAMETERS):
            ret = gsfDecodeSensorParameters(&rptr->sensor_parameters, &gsfFileTable[handle - 1], dptr);
            if (ret < 0)
            {
                gsfError = GSF_SENSOR_PARAM_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_COMMENT):
            ret = gsfDecodeComment(&rptr->comment, &gsfFileTable[handle - 1], dptr);
            if (ret < 0)
            {
                gsfError = GSF_COMMENT_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_HISTORY):
            ret = gsfDecodeHistory(&rptr->history, &gsfFileTable[handle - 1], dptr);
            if (ret < 0)
            {
                gsfError = GSF_HISTORY_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_NAVIGATION_ERROR):
            ret = gsfDecodeNavigationError(&rptr->nav_error, dptr);
            if (ret < 0)
            {
                gsfError = GSF_NAV_ERROR_RECORD_DECODE_FAILED;
                return (-1);
            }
            break;

	case (GSF_RECORD_SINGLE_BEAM_PING):
	    ret = gsfDecodeSinglebeam(&rptr->sb_ping, dptr, &gsfFileTable[handle - 1], handle, dataSize);
	    if (ret < 0)
	    {
		/* gsfError is set within gsfDecodeSinglebeam */
		return (-1);
	    }
	    break;

        default:
            gsfError = GSF_UNRECOGNIZED_RECORD_ID;
            return (-1);
    }

    return (readSize + GSF_FILL_SIZE);
}

/********************************************************************
 *
 * Function Name : gsfSeekRecord
 *
 * Description : This function moves the current gsf file position to the
 *    begining of the nth record of a specific type.  The record number and
 *    type of interest are specified by id.record_number and id.recordID.
 *    The byte offset into the file for the record of interest is retreaved
 *    from the index file created by a previous call to gsfOpen with one of
 *    the supported direct access modes specified.  This function is
 *    maintained as static to the library since the functions gsfRead
 *    and gsfWrite may be called directly to access a specific record.
 *
 * Inputs :
 *  handle = the handle for this file as returned by gsfOpen
 *  id = a pointer to a gsfDataID containing the record id information for
 *       the record of interest.
 *
 * Returns :
 *  This function returns zero if successful, or -1 if an error occured.
 *
 * Error Conditions :
 *  GSF_BAD_FILE_HANDLE
 *  GSF_UNRECOGNIZED_RECORD_ID
 *  GSF_INVALID_RECORD_NUMBER
 *  GSF_RECORD_TYPE_NOT_AVAILABLE
 *  GSF_FILE_SEEK_ERROR
 *  GSF_INDEX_FILE_READ_ERROR
 *
 *******************************************************************/

static int
gsfSeekRecord(int handle, gsfDataID *id)
{
    int             ret;
    int             offset;
    int             i;
    int             scale_index;
    long            addr;
    gsfRecords      scalesRecord;
    INDEX_REC       index_rec;

    /* Clear gsfError before each seek */
    gsfError = 0;

    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    /* Make sure we have a valid recordID */
    if ((id->recordID < 1) || (id->recordID >= NUM_REC_TYPES))
    {
        gsfError = GSF_UNRECOGNIZED_RECORD_ID;
        return (-1);
    }

    /* Make sure we have a valid record_number */
    if ((id->record_number < -1) ||
        (id->record_number > gsfFileTable[handle - 1].index_data.number_of_records[id->recordID]))
    {
        gsfError = GSF_INVALID_RECORD_NUMBER;
        return (-1);
    }

    /* Check the record_types to see if the requested type is available */
    if (gsfFileTable[handle - 1].index_data.record_type[id->recordID] == -1)
    {
        /* The record type is not available. */
        gsfError = GSF_RECORD_TYPE_NOT_AVAILABLE;
        return (-1);
    }

    /* If the record number requested is -1, use the last record of
     * this type. Note that the record number counts from one.
     */
    if (id->record_number == -1)
    {
        offset = gsfFileTable[handle - 1].index_data.number_of_records[id->recordID] - 1;
    }
    else
    {
        offset = id->record_number - 1;
    }

    /* Compute the record address within the index file and read
     * the index record.
     */
    addr = gsfFileTable[handle - 1].index_data.start_addr[id->recordID] +
        (offset * sizeof(INDEX_REC));
    if (fseek(gsfFileTable[handle - 1].index_data.fp, addr, 0))
    {
        gsfError = GSF_FILE_SEEK_ERROR;
        return (-1);
    }
    ret = fread(&index_rec, sizeof(INDEX_REC), 1, gsfFileTable[handle - 1].index_data.fp);
    if (ret != 1)
    {
        gsfError = GSF_INDEX_FILE_READ_ERROR;
        return(-1);
    }
    if (gsfFileTable[handle - 1].index_data.swap)
    {
        SwapLong((unsigned long *) &index_rec.addr, 1);
    }

    /* If the record type is GSF_RECORD_SWATH_BATHYMETRY_PING then we
     * need to ensure that we have the ping record scale factors which
     * apply to the ping record of interest.  The scale factor subrecord
     * of the ping record will only be present in the file when the scale
     * factors change.
     */
    if (id->recordID == GSF_RECORD_SWATH_BATHYMETRY_PING)
    {
        /* Clear the scale index */
        scale_index = -1;
        for (i = 1; i < gsfFileTable[handle - 1].index_data.number_of_records[0]; i++)
        {
            /* When the address of the record containing scale factors is
             * greater than the address of the record of interest, get the
             * address of the record containing scale factors which is one
             * prior to this one.
             */
            if (gsfFileTable[handle - 1].index_data.scale_factor_addr[i].addr >
                index_rec.addr)
            {
                scale_index = i - 1;
                break;
            }
        }

        /* If we didn't find a record containing scale factors with an address
         * greater than the address of the record of interest, then use the
         * last record in the file with scale factors.  Note that this
         * condition is true if the file only contains one set of scale
         * factors.
         */
        if (scale_index == -1)
        {
            scale_index = gsfFileTable[handle - 1].index_data.number_of_records[0] - 1;
        }

        /* We only need to go read the ping record with scale factors if we
         * need to use a different set of scale factors than we did last time.
         */
        if (scale_index != gsfFileTable[handle - 1].index_data.last_scale_factor_index)
        {
            addr = gsfFileTable[handle - 1].index_data.scale_factor_addr[scale_index].addr;
            if (fseek(gsfFileTable[handle - 1].fp, addr, 0))
            {
                gsfError = GSF_FILE_SEEK_ERROR;
                return (-1);
            }

            memset(&scalesRecord, 0, sizeof(scalesRecord));
            ret = gsfUnpackStream (handle, GSF_NEXT_RECORD, id, &scalesRecord, NULL, 0);
            if (ret < 0)
            {
                /* gsfError will have been set in gsfUnpackStream */
                return(-1);
            }
            memcpy(&gsfFileTable[handle - 1].rec, &scalesRecord, sizeof(gsfFileTable[handle - 1].rec));

            /* We now have the scale factors we need, save this index for
             * the comparison next time.
             */
            gsfFileTable[handle - 1].index_data.last_scale_factor_index = scale_index;
        }
    }

    /* Seek to this offset in the gsf file */
    if (fseek(gsfFileTable[handle - 1].fp, index_rec.addr, SEEK_SET))
    {
        gsfError = GSF_FILE_SEEK_ERROR;
        return (-1);
    }

    return (0);
}

/********************************************************************
 *
 * Function Name : gsfWrite
 *
 * Description : gsfWrite encodes the data from internal to external form,
 *  and then writes the requested record into the file specified by handle,
 *  where handle is the value retured by gsfOpen.  The record is written to
 *  the current file pointer for handle.  An optional checksum may be computed
 *  and encoded with the data.
 *
 *  If the file is opened for sequential access (GSF_CREATE, or GSF_UPDATE)
 *  then the recordID field of the gsfDataID structure is used to specify
 *  the record to be written.  The record is written at the current location
 *  in the file.
 *
 *  If the file is opened for direct access (GSF_UPDATE_INDEX), then the
 *  combination of the recordID and the record_number fields of the gsfDataID
 *  structure are used to uniquely identify the record to be written.  The
 *  address of the record of interest is read from the index file and the file
 *  pointer is moved to this offset before the record is encoded and written
 *  to disk.
 *
 * Inputs :
 *  handle = the handle for this file as returned by gsfOpen
 *  id = a pointer to a gsfDataID containing the record id information for
 *       the record to write.
 *  rptr = a pointer to a gsfRecords structure from which to get the internal
 *         form of the record to be written to the file.
 *
 * Returns :
 *  This function returns the number of bytes written if successful, or
 *  -1 if an error occured.
 *
 * Error Conditions :
 *    GSF_BAD_FILE_HANDLE
 *    GSF_UNRECOGNIZED_RECORD_ID
 *    GSF_FILE_SEEK_ERROR
 *    GSF_WRITE_ERROR
 *    GSF_HEADER_RECORD_ENCODE_FAILED
 *    GSF_SVP_RECORD_ENCODE_FAILED
 *    GSF_PROCESS_PARAM_RECORD_ENCODE_FAILED
 *    GSF_SENSOR_PARAM_RECORD_ENCODE_FAILED
 *    GSF_COMMENT_RECORD_ENCODE_FAILED
 *    GSF_HISTORY_RECORD_ENCODE_FAILED
 *    GSF_NAV_ERROR_RECORD_ENCODE_FAILED
 *    GSF_FLUSH_ERROR
 *    GSF_SINGLE_BEAM_ENCODE_FAILED
 *
 ********************************************************************/

int
gsfWrite(int handle, gsfDataID *id, gsfRecords *rptr)
{
    unsigned char  *ucptr;
    gsfuLong        tmpBuff[3] =
    {0, 0, 0};
    gsfuLong        gsfBuff[3];
    gsfuLong        temp;
    gsfuLong        dataSize;
    size_t          writeStat;
    int             i;
    int             pad;
    int             ret;

    /* Clear gsfError before each write */
    gsfError = 0;

    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    /* See if we need to make room for the optional checksum */
    if (id->checksumFlag)
    {
        /* four byte size, four byte id, and four byte checksum */
        ucptr = streamBuff + GSF_FILL_SIZE_CHECKSUM;
    }
    else
    {
        /* four byte size and four byte id */
        ucptr = streamBuff + GSF_FILL_SIZE;
    }

    /* Invoke the appropriate function for packing this record into a
    * byte stream.
    */
    switch (id->recordID)
    {
        case (GSF_RECORD_HEADER):
            ret = gsfEncodeHeader(ucptr, &rptr->header);
            if (ret < 0)
            {
                gsfError = GSF_HEADER_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_SWATH_BATHY_SUMMARY):
            ret = gsfEncodeSwathBathySummary(ucptr, &rptr->summary);
            if (ret < 0)
            {
                gsfError = GSF_SUMMARY_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_SWATH_BATHYMETRY_PING):
            ret = gsfEncodeSwathBathymetryPing(ucptr, &rptr->mb_ping, &gsfFileTable[handle - 1], handle);
            if (ret < 0)
            {
                /* gsfError is set within gsfEncodeSwathBathymetryPing */
                return (-1);
            }
            break;

        case (GSF_RECORD_SOUND_VELOCITY_PROFILE):
            ret = gsfEncodeSoundVelocityProfile(ucptr, &rptr->svp);
            if (ret < 0)
            {
                gsfError = GSF_SVP_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_PROCESSING_PARAMETERS):
            ret = gsfEncodeProcessingParameters(ucptr, &rptr->process_parameters);
            if (ret < 0)
            {
                gsfError = GSF_PROCESS_PARAM_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_SENSOR_PARAMETERS):
            ret = gsfEncodeSensorParameters(ucptr, &rptr->sensor_parameters);
            if (ret < 0)
            {
                gsfError = GSF_SENSOR_PARAM_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_COMMENT):
            ret = gsfEncodeComment(ucptr, &rptr->comment);
            if (ret < 0)
            {
                gsfError = GSF_COMMENT_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_HISTORY):
            ret = gsfEncodeHistory(ucptr, &rptr->history);
            if (ret < 0)
            {
                gsfError = GSF_HISTORY_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

        case (GSF_RECORD_NAVIGATION_ERROR):
            ret = gsfEncodeNavigationError(ucptr, &rptr->nav_error);
            if (ret < 0)
            {
                gsfError = GSF_NAV_ERROR_RECORD_ENCODE_FAILED;
                return (-1);
            }
            break;

	case (GSF_RECORD_SINGLE_BEAM_PING):
	    ret = gsfEncodeSinglebeam(ucptr, &rptr->sb_ping);
	    if (ret < 0)
	    {
		gsfError = GSF_SINGLE_BEAM_ENCODE_FAILED;
		return (-1);
	    }
	    break;

        default:
            gsfError = GSF_UNRECOGNIZED_RECORD_ID;
            return (-1);
    }

    /* GSF specification requires all records to be a whole multiple of 4 bytes */
    dataSize = (gsfuLong) ret;
    pad = dataSize % 4;
    if (pad)
    {
        /* jsb 04/18/97 A bug was fixed here in version 1.03, if this file was
         * created with a version of gsf prior to 1.03 we need to support it the
         * old way.
         */
        if ((gsfFileTable[handle - 1].major_version_number == 1) && 
            (gsfFileTable[handle - 1].minor_version_number <= 2))
        {
            memset(ucptr + dataSize, 0, pad);
            dataSize += pad;
        }
        else
        {
            memset(ucptr + dataSize, 0, (4 - pad));
            dataSize += (4 - pad);
        }
    }

    /* load the data identifier for this gsf record, first the checksum flag */
    if (id->checksumFlag)
    {
        /* set the checksum bit */
        tmpBuff[1] |= 0x80000000;

        /* compute the checksum */
        tmpBuff[2] = gsfChecksum(ucptr, dataSize);
    }

    /* now the reserved field */
    temp = (gsfuLong) id->reserved;
    tmpBuff[1] |= (temp << 22);

    /* now the recordID, goes in bits 00-21 */
    tmpBuff[1] |= (gsfuLong) id->recordID;

    /* load the size of the data for this gsf record */
    tmpBuff[0] = dataSize;

    /* Now load the gsf packaging words into gsf byte order */
    for (i = 0; i < 3; i++)
    {
        gsfBuff[i] = htonl(tmpBuff[i]);
    }

    /* Set the buffer pointer back to the first byte */
    ucptr = streamBuff;

    /* Add the gsf packaging words to the gsf stream */
    if (id->checksumFlag)
    {
        memcpy(ucptr, gsfBuff, GSF_FILL_SIZE_CHECKSUM);
        dataSize += GSF_FILL_SIZE_CHECKSUM;
    }
    else
    {
        memcpy(ucptr, gsfBuff, GSF_FILL_SIZE);
        dataSize += GSF_FILL_SIZE;
    }

    /* Save the current record pointer */
    if ((gsfFileTable[handle - 1].previous_record = ftell(gsfFileTable[handle - 1].fp)) == -1)
    {
        gsfError = GSF_FILE_SEEK_ERROR;
        return (-1);
    }

    /*  If the file is open for update and the last operation was a read,
     *  flush the buffer.
     */
    if ((gsfFileTable[handle - 1].update_flag) &&
        (gsfFileTable[handle - 1].read_write_flag == LAST_OP_READ))
    {
        if (fflush (gsfFileTable[handle - 1].fp))
        {
            gsfError = GSF_FLUSH_ERROR;
            return(-1);
        }
        gsfFileTable[handle - 1].bufferedBytes = 0;
    }
    gsfFileTable[handle - 1].read_write_flag = LAST_OP_WRITE;

    /* Check to see if this record will fit into the current buffer, if not
     * force a flush of the stream before writting. This is done to ensure
     * that an output file will always contain whole gsf records.
     */
    gsfFileTable[handle-1].bufferedBytes += dataSize;
    if (gsfFileTable[handle-1].bufferedBytes >= gsfFileTable[handle-1].buf_size)
    {
        if (fflush(gsfFileTable[handle-1].fp))
        {
            gsfError = GSF_FLUSH_ERROR;
            return(-1);
        }
        gsfFileTable[handle-1].bufferedBytes = 0;
    }

    /* If this file is opened for direct access, then move the file pointer
     * to the record which the caller wants to write.
     */
    if (gsfFileTable[handle - 1].direct_access)
    {
        ret = gsfSeekRecord(handle, id);
        if (ret < 0)
        {
            /* gsfError is set in gsfSeekRecord */
            return (-1);
        }
    }

    /* Now write the data to the disk */
    writeStat = fwrite(ucptr, (size_t) 1, (size_t) dataSize, gsfFileTable[handle - 1].fp);
    if (writeStat != dataSize)
    {
        gsfError = GSF_WRITE_ERROR;
        return (-1);
    }

    /* return the number of bytes written */
    return (dataSize);
}

/********************************************************************
 *
 * Function Name : gsfLoadScaleFactors
 *
 * Description : gsfLoadScaleFactors should be used to load the swath
 *  bathymetry ping record scale factor structure.  This function assures
 *  that the multiplier and offset fields of the scale factor structure
 *  have a precision equal to that which will be stored in the gsf data file.
 *  This function should be called once for each beam array data type
 *  contained in your data.
 *
 * Inputs :
 *  sf = a pointer to the gsfScaleFactors structure to be loaded
 *  subrecordID = the subrecord id for the beam array data
 *  c_flag = the compression flag for the beam array
 *  precision = the presision to which the beam array data are to be stored
 *              (a value of 0.1 would indicate decimeter precision for depth)
 *  offset = the "DC" offset to scale the data by.
 *
 * Returns :
 *  This function returns zero if successful, or -1 if an error occured.
 *
 * Error Conditions :
 *    GSF_TOO_MANY_ARRAY_SUBRECORDS
 *
 ********************************************************************/

int
gsfLoadScaleFactor(gsfScaleFactors *sf, int subrecordID, char c_flag, double precision, int offset)
{
    unsigned long   itemp;
    double          mult;

    /* If we're adding a new subrecord, bump counter and check bounds */
    if (sf->scaleTable[subrecordID - 1].multiplier == 0.0)
    {
        sf->numArraySubrecords++;
        if (sf->numArraySubrecords > GSF_MAX_PING_ARRAY_SUBRECORDS)
        {
            sf->numArraySubrecords--;
            gsfError = GSF_TOO_MANY_ARRAY_SUBRECORDS;
            return (-1);
        }
    }

    /* Compute the multiplier as one over the requested precision */
    mult = 1.0 / precision;

    /* Test to see if integer truncation will cause an error */
    if (mult > ULONG_MAX)
    {
        gsfError = GSF_CANNOT_REPRESENT_PRECISION;
        return (-1);
    }

    /* In order to assure the same multiplier is used throughout, truncate
    *  to an integer.  This is the value which is stored with the data.
    */
    itemp = (int) mult;

    /* The multiplier to be applied to the data is converted back to a
    *  double here, for floating point performance.
    */
    sf->scaleTable[subrecordID - 1].compressionFlag = c_flag;
    sf->scaleTable[subrecordID - 1].multiplier = ((double) itemp);
    sf->scaleTable[subrecordID - 1].offset = (double) offset;

    return (0);
}

/********************************************************************
 *
 * Function Name : gsfGetScaleFactors
 *
 * Description : gsfGetScaleFactors may be used to obtain the multiplier
 *  and DC offset values by which each swath bathymetry ping array subrecord
 *  is be scaled. gsfGetScalesFactors must be called once for each array
 *  subrecord of interest.  At leat one swath bathymetry ping record
 *  must have been read from, or written to the file specified by handle.
 *
 * Inputs :
 *  handle = the integer value set by a call to gsfOpen.
 *  subrecordID = an integer value containing the subrecord id of the requested scale factors
 *  c_flag = the address of an unsigned character to contain the the compression flag
 *  multiplier = the address of a double to contain the scaling multiplier
 *  offset = the address of a double to contain the scaling DC offset.
 *
 * Returns :
 *  This function returns zero if successful, or -1 if an error occured.
 *
 * Error Conditions :
 *    GSF_TOO_MANY_ARRAY_SUBRECORDS
 *    GSF_ILLEGAL_SCALE_FACTOR_MULTIPLIER
 *
 ********************************************************************/

int
gsfGetScaleFactor(int handle, int subrecordID, unsigned char *c_flag, double *multiplier, double *offset)
{
    unsigned long   itemp;
    double          mult;

    if ((subrecordID < 1) || (subrecordID > GSF_MAX_PING_ARRAY_SUBRECORDS))
    {
        gsfError = GSF_TOO_MANY_ARRAY_SUBRECORDS;
        return(-1);
    }

    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    /* Make sure the multiplier is not zero */
    if (gsfFileTable[handle-1].rec.mb_ping.scaleFactors.scaleTable[subrecordID-1].multiplier == 0.0)
    {
        gsfError = GSF_ILLEGAL_SCALE_FACTOR_MULTIPLIER;
        return (-1);
    }

    /* Set the compression flag */
    *c_flag = gsfFileTable[handle-1].rec.mb_ping.scaleFactors.scaleTable[subrecordID-1].compressionFlag;

    /* Set the multiplier */
    *multiplier = gsfFileTable[handle-1].rec.mb_ping.scaleFactors.scaleTable[subrecordID-1].multiplier;

    /* Set the offset */
    *offset = gsfFileTable[handle-1].rec.mb_ping.scaleFactors.scaleTable[subrecordID-1].offset;

    return (0);
}

/********************************************************************
 *
 * Function Name : gsfFree
 *
 * Description : This function frees all dynamically allocated memory
 *    from a gsfRecords data structure, and it then clears all the
 *    data elements in the structure.
 *
 * Inputs :
 *    gsfRecords *rec = a pointer to ta gsfRecords data structure
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/

void
gsfFree (gsfRecords *rec)
{
    int i;

    /* Free the array subrecords in ping data structure */
    if (rec->mb_ping.depth != (double *) NULL)
    {
        free (rec->mb_ping.depth);
        rec->mb_ping.depth = (double *) NULL;
    }

    if (rec->mb_ping.across_track != (double *) NULL)
    {
        free (rec->mb_ping.across_track);
        rec->mb_ping.across_track = (double *) NULL;
    }

    if (rec->mb_ping.along_track != (double *) NULL)
    {
        free (rec->mb_ping.along_track);
        rec->mb_ping.along_track = (double *) NULL;
    }

    if (rec->mb_ping.travel_time != (double *) NULL)
    {
        free (rec->mb_ping.travel_time);
        rec->mb_ping.travel_time = (double *) NULL;
    }

    if (rec->mb_ping.beam_angle != (double *) NULL)
    {
        free (rec->mb_ping.beam_angle);
        rec->mb_ping.beam_angle = (double *) NULL;
    }

    if (rec->mb_ping.mc_amplitude != (double *) NULL)
    {
        free (rec->mb_ping.mc_amplitude);
        rec->mb_ping.mc_amplitude = (double *) NULL;
    }

    if (rec->mb_ping.mr_amplitude != (double *) NULL)
    {
        free (rec->mb_ping.mr_amplitude);
        rec->mb_ping.mr_amplitude = (double *) NULL;
    }

    if (rec->mb_ping.echo_width != (double *) NULL)
    {
        free (rec->mb_ping.echo_width);
        rec->mb_ping.echo_width = (double *) NULL;
    }

    if (rec->mb_ping.quality_factor != (double *) NULL)
    {
        free (rec->mb_ping.quality_factor);
        rec->mb_ping.quality_factor = (double *) NULL;
    }

    if (rec->mb_ping.receive_heave != (double *) NULL)
    {
        free (rec->mb_ping.receive_heave);
        rec->mb_ping.receive_heave = (double *) NULL;
    }
    if (rec->mb_ping.depth_error != (double *) NULL)
    {
        free (rec->mb_ping.depth_error);
        rec->mb_ping.depth_error = (double *) NULL;
    }

    if (rec->mb_ping.across_track_error != (double *) NULL)
    {
        free (rec->mb_ping.across_track_error);
        rec->mb_ping.across_track_error = (double *) NULL;
    }

    if (rec->mb_ping.along_track_error != (double *) NULL)
    {
        free (rec->mb_ping.along_track_error);
        rec->mb_ping.along_track_error = (double *) NULL;
    }

    if (rec->mb_ping.nominal_depth != (double *) NULL)
    {
        free (rec->mb_ping.nominal_depth);
        rec->mb_ping.nominal_depth = (double *) NULL;
    }

    if (rec->mb_ping.beam_flags != (unsigned char *) NULL)
    {
        free (rec->mb_ping.beam_flags);
        rec->mb_ping.beam_flags = (unsigned char *) NULL;
    }

    if (rec->mb_ping.signal_to_noise != (double *) NULL)
    {
        free (rec->mb_ping.signal_to_noise);
        rec->mb_ping.signal_to_noise = (double *) NULL;
    }

    if (rec->mb_ping.beam_angle_forward != (double *) NULL)
    {
        free (rec->mb_ping.beam_angle_forward);
        rec->mb_ping.beam_angle_forward = (double *) NULL;
    }

    /* Free the dynamically allocated memory from the svp record */
    if (rec->svp.depth != (double *) NULL)
    {
        free (rec->svp.depth);
        rec->svp.depth = (double *) NULL;
    }

    if (rec->svp.sound_speed != (double *) NULL)
    {
        free (rec->svp.sound_speed);
        rec->svp.sound_speed = (double *) NULL;
    }

    /* Free the dynamically allocated memory from the processing parameters */
    for (i=0; i<rec->process_parameters.number_parameters; i++)
    {
        if (rec->process_parameters.param[i] != (char *) NULL)
        {
            free (rec->process_parameters.param[i]);
            rec->process_parameters.param[i] = (char *) NULL;
        }
    }
    rec->process_parameters.number_parameters = 0;

    /* Free the dynamically allocated memory from the sensor parameters */
    for (i=0; i<rec->sensor_parameters.number_parameters; i++)
    {
        if (rec->sensor_parameters.param[i] != (char *) NULL)
        {
            free (rec->sensor_parameters.param[i]);
            rec->sensor_parameters.param[i] = (char *) NULL;
        }
    }
    rec->sensor_parameters.number_parameters = 0;

    /* Free the dynamically allocated memory from the comment record */
    if (rec->comment.comment != (char *) NULL)
    {
        free (rec->comment.comment);
        rec->comment.comment = (char *) NULL;
    }

    /* Free the dynamically allocated memory from the history record */
    if (rec->history.command_line != (char *) NULL)
    {
        free (rec->history.command_line);
        rec->history.command_line = (char *) NULL;
    }

    if (rec->history.comment != (char *) NULL)
    {
        free (rec->history.comment);
        rec->history.comment = (char *) NULL;
    }

    /* Now clear all the data from the gsf Records structure */
    memset (rec, 0, sizeof(gsfRecords));

    return;
}

/********************************************************************
 *
 * Function Name : gsfPrintError
 *
 * Description : This function is used to print a short message describing
 *  the most recent error encountered.  This function need only be called if
 *  a -1 is returned from one of the gsf functions.
 *
 * Inputs :
 *  fp = a pointer to a FILE to which to write the message.
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/

void
gsfPrintError(FILE * fp)
{
    switch (gsfError)
    {
        case GSF_FOPEN_ERROR:
            fprintf(fp, "GSF Unable to open requested file\n");
            break;

        case GSF_UNRECOGNIZED_FILE:
            fprintf(fp, "GSF Error unrecognized file\n");
            break;

        case GSF_BAD_ACCESS_MODE:
            fprintf(fp, "GSF Error illegal access mode\n");
            break;

        case GSF_READ_ERROR:
            fprintf(fp, "GSF Error reading input data\n");
            break;

        case GSF_WRITE_ERROR:
            fprintf(fp, "GSF Error writing output data\n");
            break;

        case GSF_INSUFFICIENT_SIZE:
            fprintf(fp, "GSF Error insufficient size specified\n");
            break;

        case GSF_RECORD_SIZE_ERROR:
            fprintf(fp, "GSF Error record size is out of bounds\n");
            break;

        case GSF_CHECKSUM_FAILURE:
            fprintf(fp, "GSF Error data checksum failure\n");
            break;

        case GSF_FILE_CLOSE_ERROR:
            fprintf(fp, "GSF Error closing gsf file\n");
            break;

        case GSF_TOO_MANY_ARRAY_SUBRECORDS:
            fprintf(fp, "GSF Error too many array subrecords\n");
            break;

        case GSF_TOO_MANY_OPEN_FILES:
            fprintf(fp, "GSF Error too many open files\n");
            break;

        case GSF_MEMORY_ALLOCATION_FAILED:
            fprintf(fp, "GSF Error memory allocation failure\n");
            break;

        case GSF_STREAM_DECODE_FAILURE:
            fprintf(fp, "GSF Error stream decode failure\n");
            break;

        case GSF_UNRECOGNIZED_RECORD_ID:
            fprintf(fp, "GSF Error unrecognized record id\n");
            break;

        case GSF_BAD_SEEK_OPTION:
            fprintf(fp, "GSF Error unrecognized file seek option\n");
            break;

        case GSF_FILE_SEEK_ERROR:
            fprintf(fp, "GSF Error file seek failed\n");
            break;

        case GSF_UNRECOGNIZED_SENSOR_ID:
            fprintf(fp, "GSF Error unrecognized sensor specific subrecord id\n");
            break;

        case GSF_UNRECOGNIZED_ARRAY_SUBRECORD_ID:
            fprintf(fp, "GSF Error unrecognized array subrecord id \n");
            break;

        case GSF_UNRECOGNIZED_SUBRECORD_ID:
            fprintf(fp, "GSF Error unrecognized subrecord id\n");
            break;

        case GSF_ILLEGAL_SCALE_FACTOR_MULTIPLIER:
            fprintf(fp, "GSF Error illegal scale factor multiplier specified\n");
            break;

        case GSF_FILE_TELL_ERROR:
            fprintf(fp, "GSF Error file tell failed\n");
            break;

        case GSF_INDEX_FILE_OPEN_ERROR:
            fprintf(fp, "GSF Error open of index file failed\n");
            fprintf(fp, "%s\n", strerror(errno));
            break;

        case GSF_CORRUPT_INDEX_FILE_ERROR:
            fprintf(fp, "GSF Error index file is corrupted, delete index file\n");
            break;

        case GSF_SCALE_INDEX_CALLOC_ERROR:
            fprintf(fp, "GSF Error calloc of scale factor index memory failed\n");
            break;

        case GSF_RECORD_TYPE_NOT_AVAILABLE:
            fprintf(fp, "GSF Error requested indexed record type not in gsf file\n");
            break;

	case GSF_SINGLE_BEAM_ENCODE_FAILED:
	    fprintf(fp, "GSF Error stream encode failure\n");
	    break;

        case GSF_READ_TO_END_OF_FILE:
            break;

        default:
            fprintf(fp, "GSF unknown error\n");
            break;
    }
    return;
}

/********************************************************************
 *
 * Function Name : gsfStringError
 *
 * Description : This function is used to return a string with  
 *  a short message describing the most recent error encountered.
 *  This function need only be called if
 *  a -1 is returned from one of the gsf functions.
 *
 * Inputs :
 *  error_string = a pointer to a character string.
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/

char *
gsfStringError(void)
{
    char             *ptr;

    switch (gsfError)
    {
	case GSF_FOPEN_ERROR:
	    ptr = "GSF Unable to open requested file";
	    break;

	case GSF_UNRECOGNIZED_FILE:
	    ptr = "GSF Error unrecognized file";
	    break;

	case GSF_BAD_ACCESS_MODE:
	    ptr = "GSF Error illegal access mode";
	    break;

	case GSF_READ_ERROR:
	    ptr = "GSF Error reading input data";
	    break;

	case GSF_WRITE_ERROR:
	    ptr = "GSF Error writing output data";
	    break;

	case GSF_INSUFFICIENT_SIZE:
	    ptr = "GSF Error insufficient size specified";
	    break;

	case GSF_RECORD_SIZE_ERROR:
	    ptr = "GSF Error record size is out of bounds";
	    break;

	case GSF_CHECKSUM_FAILURE:
	    ptr = "GSF Error data checksum failure";
	    break;

	case GSF_FILE_CLOSE_ERROR:
	    ptr = "GSF Error closing gsf file";
	    break;

	case GSF_TOO_MANY_ARRAY_SUBRECORDS:
	    ptr = "GSF Error too many array subrecords";
	    break;

	case GSF_TOO_MANY_OPEN_FILES:
	    ptr = "GSF Error too many open files";
	    break;

	case GSF_MEMORY_ALLOCATION_FAILED:
	    ptr = "GSF Error memory allocation failure";
	    break;

	case GSF_STREAM_DECODE_FAILURE:
	    ptr = "GSF Error stream decode failure";
	    break;

	case GSF_UNRECOGNIZED_RECORD_ID:
	    ptr = "GSF Error unrecognized record id";
	    break;

	case GSF_BAD_SEEK_OPTION:
	    ptr = "GSF Error unrecognized file seek option";
	    break;

	case GSF_FILE_SEEK_ERROR:
	    ptr = "GSF Error file seek failed";
	    break;

	case GSF_UNRECOGNIZED_SENSOR_ID:
	    ptr = "GSF Error unrecognized sensor specific subrecord id";
	    break;

	case GSF_UNRECOGNIZED_ARRAY_SUBRECORD_ID:
	    ptr = "GSF Error unrecognized array subrecord id";
	    break;

	case GSF_UNRECOGNIZED_SUBRECORD_ID:
	    ptr = "GSF Error unrecognized subrecord id";
	    break;

	case GSF_ILLEGAL_SCALE_FACTOR_MULTIPLIER:
	    ptr = "GSF Error illegal scale factor multiplier specified";
	    break;

	case GSF_FILE_TELL_ERROR:
	    ptr = "GSF Error file tell failed";
	    break;

	case GSF_INDEX_FILE_OPEN_ERROR:
	    ptr = "GSF Error open of index file failed";
	    break;

	case GSF_CORRUPT_INDEX_FILE_ERROR:
	    ptr = "GSF Error index file is corrupted, delete index file";
	    break;

	case GSF_SCALE_INDEX_CALLOC_ERROR:
	    ptr = "GSF Error calloc of scale factor index memory failed";
	    break;

	case GSF_RECORD_TYPE_NOT_AVAILABLE:
	    ptr = "GSF Error requested indexed record type not in gsf file";
	    break;

	case GSF_SINGLE_BEAM_ENCODE_FAILED:
	    ptr = "GSF Error single beam encode failure";
	    break;

	case GSF_READ_TO_END_OF_FILE:
            ptr = "GSF End of File Encountered";
	    break;

	default:
	    ptr = "GSF unknown error";
	    break;
    }

    return (ptr);
}

/********************************************************************
 *
 * Function Name : gsfIndexTime
 *
 * Description : This function returns the time (Posix.4) associated with
 *  a specified record number and type.  It also returns the record number
 *  that was read.
 *
 * Inputs :
 *  handle = gsf file handle assigned by gsfOpen or gsfOpenBuffered
 *  record_type = record type to be retrieved
 *  record_number = record number to be retrieved (-1 will get the time
 *                  and record number of the last record of this type)
 *  sec = Posix.4 seconds
 *  nsec = Posix.4 nanoseconds
 *
 * Returns :
 *  This function returns the record number if successful, or -1 if an
 *  error occured.
 *
 * Error Conditions :
 *    GSF_RECORD_TYPE_NOT_AVAILABLE
 *
 ********************************************************************/

int
gsfIndexTime(int handle, int record_type, int record_number, time_t * sec, long *nsec)
{
    long            addr;
    int             offset;
    INDEX_REC       index_rec;

    /* Check the record_types to see if the requested type is available */
    if (gsfFileTable[handle - 1].index_data.record_type[record_type] == -1)
    {
        gsfError = GSF_RECORD_TYPE_NOT_AVAILABLE;
        return (-1);
    }

    /*  If the record number requested is -1, use the last record of
     *  this type.
     */
    if (record_number == -1)
    {
        offset = gsfFileTable[handle - 1].index_data.number_of_records[record_type] - 1;
    }
    else
    {
        offset = record_number - 1;
    }

    /*  Compute the record address within the index file and read the
     *  index record.
     */
    addr = gsfFileTable[handle - 1].index_data.start_addr[record_type] +
        (offset * sizeof(INDEX_REC));
    if (fseek(gsfFileTable[handle - 1].index_data.fp, addr, 0))
    {
        gsfError = GSF_FILE_SEEK_ERROR;
        return (-1);
    }
    if (fread(&index_rec, sizeof(INDEX_REC), 1, gsfFileTable[handle - 1].index_data.fp) != 1)
    {
        gsfError = GSF_INDEX_FILE_READ_ERROR;
        return (-1);
    }
    if (gsfFileTable[handle - 1].index_data.swap)
    {
        SwapLong((unsigned long *) &index_rec, 3);
    }

    /*  Store the time and return the record number.    */
    *sec = index_rec.sec;
    *nsec = index_rec.nsec;

    return (offset+1);
}

/********************************************************************
 *
 * Function Name : gsfCheckSum
 *
 * Description :  This function computes and returns the modulo-32 form
 *                 byte-wise sum of the num_bytes starting at buff.
 *
 * Inputs :
 *  buff = a pointer to an unsigned char buffer containing the data
 *  num_bytes = a integer containing the number of bytes of data
 *
 * Returns : a gsfuLong (u_long) data type containing the computed checksum.
 *
 * Error Conditions :
 *
 ********************************************************************/

static          gsfuLong
gsfChecksum(unsigned char *buff, unsigned int num_bytes)
{
    unsigned char  *ptr;
    gsfuLong        checkSum = 0;

    /*
    * Compute the checksum as the modulo-32 sum of all bytes
    * between the checksum value and the end of the record.
    */
    for (ptr = buff; ptr < buff + num_bytes; ptr++)
    {
        checkSum += *ptr;
    }
    return checkSum;
}

/********************************************************************
 *
 * Function Name : gsfPercent
 *
 * Description : This function returns an integer value representing
 *  the location of the file pointer as a percentage of the total file
 *  size.  It may be used to obtain an indication of how far along a
 *  program is in reading a gsf data file.  The file size is obtained
 *  when the file is opened.
 *
 * Inputs :
 *  handle = gsf file handle assigned by gsfOpen or gsfOpenBuffered
 *
 * Returns :
 *  This function returns the current file position as a percentage of
 *  the file size, or -1 if an error occurred. gsfError will be set to
 *  indicate the error.
 *
 * Error Conditions :
 *  GSF_BAD_FILE_HANDLE
 *  GSF_FILE_TELL_ERROR
 *
 ********************************************************************/

int
gsfPercent (int handle)
{
    int             percent;
    long            addr;

    /* Clear gsfError each time down */
    gsfError = 0;

    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    /* Retreive the current file position */
    addr = ftell (gsfFileTable[handle - 1].fp);
    if (addr == -1)
    {
        gsfError = GSF_FILE_TELL_ERROR;
        return (-1);
    }

    percent = 0;
    if (gsfFileTable[handle - 1].file_size > 0)
    {
        percent = 100.0 * (double)addr / (double)gsfFileTable[handle - 1].file_size;
    }
    return(percent);
}

/********************************************************************
 *
 * Function Name : gsfGetNumberRecods
 *
 * Description : This function will return the number of records of a
 *  given type to the caller. The number of records is retreived from
 *  the index file, so the file must have been opened for direct
 *  access (GSF_READONLY_INDEX, or GSF_UPDATE_INDEX).
 *
 * Inputs :
 *    handle = the handle to the file as provided by gsfOpen
 *    desiredRecord = the desired record or GSF_NEXT_RECORD
 *
 * Returns :
 *  This function returns the number of records of type desiredRecord
 *  contained in the GSF file designated by handle, or -1 if an error
 *  occured.
 *
 * Error Conditions :
 *  GSF_BAD_FILE_HANDLE
 *  GSF_BAD_ACCESS_MODE
 *
 ********************************************************************/

int
gsfGetNumberRecords (int handle, int desiredRecord)
{
    /* Clear gsfError each time down */
    gsfError = 0;

    if ((handle < 1) || (handle >= GSF_MAX_OPEN_FILES))
    {
        gsfError = GSF_BAD_FILE_HANDLE;
        return (-1);
    }

    if ((desiredRecord < 0) || (desiredRecord > NUM_REC_TYPES))
    {
        gsfError = GSF_UNRECOGNIZED_RECORD_ID;
        return (-1);
    }

    if (gsfFileTable[handle - 1].direct_access == 0)
    {
        gsfError = GSF_BAD_ACCESS_MODE;
        return(-1);
    }

    return (gsfFileTable[handle - 1].index_data.number_of_records[desiredRecord]);
}

/********************************************************************
 *
 * Function Name : gsfCopyRecords
 *
 * Description : This function will copy all of the data contained in the
 *  source gsfRecords data structure to the target gsfRecords data
 *  structure. The target MUST be memset to zero before the first call to
 *  gsfCopyRecords.  This function allocates dynmanic memory which is NOT
 *  maintained by the library.  It is up to the calling application to
 *  release the memory allocated.  This may be done by maintaining the
 *  target data structure as static data, or by using gsfFree to release
 *  the memory.
 *
 * Inputs :
 *  target = a pointer to a gsfRecords data structure allocated by the
 *      calling application, into which the source data is to be copied.
 *  source = a pointer to a gsfRecords data structure allocated by the
 *      calling application, from which data is to be copied.
 *
 * Returns : This function returns zero if successful, or -1 if an error
 *  occurs.
 *
 * Error Conditions :
 *  GSF_MEMORY_ALLOCATION_FAILED
 *
 ********************************************************************/

int
gsfCopyRecords (gsfRecords *target, gsfRecords *source)
{
    int             i;

    gsfError = 0;

    /* Copy the gsf header over to the  target */
    memcpy(&target->header, &source->header, sizeof(target->header));

    /* Copy the ping summary record over to the target */
    memcpy(&target->summary, &source->summary, sizeof(target->summary));

    /* Decide which arrays we need to allocate memory for */
    if (source->mb_ping.depth != (double *) NULL)
    {
        if (target->mb_ping.depth == (double *) NULL)
        {
            target->mb_ping.depth = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.depth == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.depth = (double *) realloc (target->mb_ping.depth, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.depth == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.depth, source->mb_ping.depth, sizeof(double) * source->mb_ping.number_beams);
    }

    /* Decide which arrays we need to allocate memory for */
    if (source->mb_ping.nominal_depth != (double *) NULL)
    {
        if (target->mb_ping.nominal_depth == (double *) NULL)
        {
            target->mb_ping.nominal_depth = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.nominal_depth == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.nominal_depth = (double *) realloc (target->mb_ping.nominal_depth, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.nominal_depth == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.nominal_depth, source->mb_ping.nominal_depth, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.across_track != (double *) NULL)
    {
        if (target->mb_ping.across_track == (double *) NULL)
        {
            target->mb_ping.across_track = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.across_track == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.across_track = (double *) realloc (target->mb_ping.across_track, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.across_track == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.across_track, source->mb_ping.across_track, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.along_track != (double *) NULL)
    {
        if (target->mb_ping.along_track == (double *) NULL)
        {
            target->mb_ping.along_track = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.along_track == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.along_track = (double *) realloc (target->mb_ping.along_track, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.along_track == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.along_track, source->mb_ping.along_track, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.travel_time != (double *) NULL)
    {
        if (target->mb_ping.travel_time == (double *) NULL)
        {
            target->mb_ping.travel_time = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.travel_time == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.travel_time = (double *) realloc (target->mb_ping.travel_time, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.travel_time == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.travel_time, source->mb_ping.travel_time, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.beam_angle != (double *) NULL)
    {
        if (target->mb_ping.beam_angle == (double *) NULL)
        {
            target->mb_ping.beam_angle = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.beam_angle == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.beam_angle = (double *) realloc (target->mb_ping.beam_angle, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.beam_angle == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.beam_angle, source->mb_ping.beam_angle, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.mc_amplitude != (double *) NULL)
    {
        if (target->mb_ping.mc_amplitude == (double *) NULL)
        {
            target->mb_ping.mc_amplitude = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.mc_amplitude == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.mc_amplitude = (double *) realloc (target->mb_ping.mc_amplitude, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.mc_amplitude == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.mc_amplitude, source->mb_ping.mc_amplitude, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.mr_amplitude != (double *) NULL)
    {
        if (target->mb_ping.mr_amplitude == (double *) NULL)
        {
            target->mb_ping.mr_amplitude = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.mr_amplitude == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.mr_amplitude = (double *) realloc (target->mb_ping.mr_amplitude, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.mr_amplitude == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.mr_amplitude, source->mb_ping.mr_amplitude, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.echo_width != (double *) NULL)
    {
        if (target->mb_ping.echo_width == (double *) NULL)
        {
            target->mb_ping.echo_width = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.echo_width == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.echo_width = (double *) realloc (target->mb_ping.echo_width, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.echo_width == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.echo_width, source->mb_ping.echo_width, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.quality_factor != (double *) NULL)
    {
        if (target->mb_ping.quality_factor == (double *) NULL)
        {
            target->mb_ping.quality_factor = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.quality_factor == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.quality_factor = (double *) realloc (target->mb_ping.quality_factor, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.quality_factor == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.quality_factor, source->mb_ping.quality_factor, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.receive_heave != (double *) NULL)
    {
        if (target->mb_ping.receive_heave == (double *) NULL)
        {
            target->mb_ping.receive_heave = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.receive_heave == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.receive_heave = (double *) realloc (target->mb_ping.receive_heave, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.receive_heave == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.receive_heave, source->mb_ping.receive_heave, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.depth_error != (double *) NULL)
    {
        if (target->mb_ping.depth_error == (double *) NULL)
        {
            target->mb_ping.depth_error = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.depth_error == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.depth_error = (double *) realloc (target->mb_ping.depth_error, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.depth_error == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.depth_error, source->mb_ping.depth_error, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.across_track_error != (double *) NULL)
    {
        if (target->mb_ping.across_track_error == (double *) NULL)
        {
            target->mb_ping.across_track_error = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.across_track_error == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.across_track_error = (double *) realloc (target->mb_ping.across_track_error, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.across_track_error == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.across_track_error, source->mb_ping.across_track_error, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.along_track_error != (double *) NULL)
    {
        if (target->mb_ping.along_track_error == (double *) NULL)
        {
            target->mb_ping.along_track_error = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.along_track_error == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.along_track_error = (double *) realloc (target->mb_ping.along_track_error, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.along_track_error == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.along_track_error, source->mb_ping.along_track_error, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.quality_flags != (unsigned char *) NULL)
    {
        if (target->mb_ping.quality_flags == (unsigned char *) NULL)
        {
            target->mb_ping.quality_flags = (unsigned char *) calloc (sizeof(unsigned char), source->mb_ping.number_beams);
            if (target->mb_ping.quality_flags == (unsigned char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.quality_flags = (unsigned char *) realloc (target->mb_ping.quality_flags, sizeof(unsigned char) * source->mb_ping.number_beams);
            if (target->mb_ping.quality_flags == (unsigned char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.quality_flags, source->mb_ping.quality_flags, sizeof(unsigned char) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.beam_flags != (unsigned char *) NULL)
    {
        if (target->mb_ping.beam_flags == (unsigned char *) NULL)
        {
            target->mb_ping.beam_flags = (unsigned char *) calloc (sizeof(unsigned char), source->mb_ping.number_beams);
            if (target->mb_ping.beam_flags == (unsigned char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.beam_flags = (unsigned char *) realloc (target->mb_ping.beam_flags, sizeof(unsigned char) * source->mb_ping.number_beams);
            if (target->mb_ping.beam_flags == (unsigned char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.beam_flags, source->mb_ping.beam_flags, sizeof(unsigned char) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.signal_to_noise != (double *) NULL)
    {
        if (target->mb_ping.signal_to_noise == (double *) NULL)
        {
            target->mb_ping.signal_to_noise = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.signal_to_noise == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.signal_to_noise = (double *) realloc (target->mb_ping.signal_to_noise, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.signal_to_noise == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.signal_to_noise, source->mb_ping.signal_to_noise, sizeof(double) * source->mb_ping.number_beams);
    }

    if (source->mb_ping.beam_angle_forward != (double *) NULL)
    {
        if (target->mb_ping.beam_angle_forward == (double *) NULL)
        {
            target->mb_ping.beam_angle_forward = (double *) calloc (sizeof(double), source->mb_ping.number_beams);
            if (target->mb_ping.beam_angle_forward == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        else if (target->mb_ping.number_beams < source->mb_ping.number_beams)
        {
            target->mb_ping.beam_angle_forward = (double *) realloc (target->mb_ping.beam_angle_forward, sizeof(double) * source->mb_ping.number_beams);
            if (target->mb_ping.beam_angle_forward == (double *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
        }
        memcpy (target->mb_ping.beam_angle_forward, source->mb_ping.beam_angle_forward, sizeof(double) * source->mb_ping.number_beams);
    }

    /* Copy the swath bathymetry ping record over to the target by moving
     * the data over one item at a time so we don't overwrite the arrays.
     */
    target->mb_ping.ping_time       = source->mb_ping.ping_time;
    target->mb_ping.latitude        = source->mb_ping.latitude;
    target->mb_ping.longitude       = source->mb_ping.longitude;
    target->mb_ping.number_beams    = source->mb_ping.number_beams;
    target->mb_ping.center_beam     = source->mb_ping.center_beam;
    target->mb_ping.ping_flags      = source->mb_ping.ping_flags;
    target->mb_ping.reserved        = source->mb_ping.reserved;
    target->mb_ping.tide_corrector  = source->mb_ping.tide_corrector;
    target->mb_ping.depth_corrector = source->mb_ping.depth_corrector;
    target->mb_ping.heading         = source->mb_ping.heading;
    target->mb_ping.pitch           = source->mb_ping.pitch;
    target->mb_ping.roll            = source->mb_ping.roll;
    target->mb_ping.heave           = source->mb_ping.heave;
    target->mb_ping.course          = source->mb_ping.course;
    target->mb_ping.speed           = source->mb_ping.speed;
    target->mb_ping.scaleFactors    = source->mb_ping.scaleFactors;
    target->mb_ping.sensor_id       = source->mb_ping.sensor_id;
    target->mb_ping.sensor_data     = source->mb_ping.sensor_data;

    /* Now hande the sound velocity profile dynamic memory */
    if (target->svp.depth == (double *) NULL)
    {
        target->svp.depth = (double *) calloc (sizeof(double), source->svp.number_points);
        if (target->svp.depth == (double *) NULL)
        {
            gsfError = GSF_MEMORY_ALLOCATION_FAILED;
            return(-1);
        }
        memcpy (target->svp.depth, source->svp.depth, sizeof(double) * source->svp.number_points);
    }
    else if (target->svp.number_points < source->svp.number_points)
    {
        target->svp.depth = (double *) realloc (target->svp.depth, sizeof(double) * source->svp.number_points);
        if (target->svp.depth == (double *) NULL)
        {
            gsfError = GSF_MEMORY_ALLOCATION_FAILED;
            return(-1);
        }
        memcpy (target->svp.depth, source->svp.depth, sizeof(double) * source->svp.number_points);
    }

    if (target->svp.sound_speed == (double *) NULL)
    {
        target->svp.sound_speed = (double *) calloc (sizeof(double), source->svp.number_points);
        if (target->svp.sound_speed == (double *) NULL)
        {
            gsfError = GSF_MEMORY_ALLOCATION_FAILED;
            return(-1);
        }
        memcpy (target->svp.sound_speed, source->svp.sound_speed, sizeof(double) * source->svp.number_points);
    }
    else if (target->svp.number_points < source->svp.number_points)
    {
        target->svp.sound_speed = (double *) realloc (target->svp.sound_speed, sizeof(double) * source->svp.number_points);
        if (target->svp.sound_speed == (double *) NULL)
        {
            gsfError = GSF_MEMORY_ALLOCATION_FAILED;
            return(-1);
        }
        memcpy (target->svp.sound_speed, source->svp.sound_speed, sizeof(double) * source->svp.number_points);
    }

    /* Copy the sound velocity profile record from the source to the target */
    target->svp.observation_time = source->svp.observation_time;
    target->svp.application_time = source->svp.application_time;
    target->svp.latitude         = source->svp.latitude;
    target->svp.longitude        = source->svp.longitude;
    target->svp.number_points    = source->svp.number_points;

    /* Copy the processing parameters from the source to the target */
    target->process_parameters.param_time        = source->process_parameters.param_time;
    target->process_parameters.number_parameters = source->process_parameters.number_parameters;
    for (i=0; i<source->process_parameters.number_parameters; i++)
    {
        if (target->process_parameters.param[i] == (char *) NULL)
        {
            target->process_parameters.param[i] = (char *) calloc (sizeof(char), source->process_parameters.param_size[i] + 1);
            if (target->process_parameters.param[i] == (char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
            strncpy (target->process_parameters.param[i], source->process_parameters.param[i], source->process_parameters.param_size[i]);
            target->process_parameters.param_size[i] = source->process_parameters.param_size[i];
        }
        else if (target->process_parameters.param_size[i] < source->process_parameters.param_size[i])
        {
            target->process_parameters.param[i] = (char *) realloc (target->process_parameters.param[i], source->process_parameters.param_size[i] + 1);
            if (target->process_parameters.param[i] == (char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
            strncpy (target->process_parameters.param[i], source->process_parameters.param[i], source->process_parameters.param_size[i]);
            target->process_parameters.param_size[i] = source->process_parameters.param_size[i];
        }
    }

    /* Copy the sensor parameters from the source to the target */
    target->sensor_parameters.param_time        = source->sensor_parameters.param_time;
    target->sensor_parameters.number_parameters = source->sensor_parameters.number_parameters;
    for (i=0; i<source->sensor_parameters.number_parameters; i++)
    {
        if (target->sensor_parameters.param[i] == (char *) NULL)
        {
            target->sensor_parameters.param[i] = (char *) calloc (sizeof(char), source->sensor_parameters.param_size[i] + 1);
            if (target->sensor_parameters.param[i] == (char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
            strncpy (target->sensor_parameters.param[i], source->sensor_parameters.param[i], source->sensor_parameters.param_size[i]);
            target->sensor_parameters.param_size[i] = source->sensor_parameters.param_size[i];
        }
        else if (target->sensor_parameters.param_size[i] < source->sensor_parameters.param_size[i])
        {
            target->sensor_parameters.param[i] = (char *) realloc (target->sensor_parameters.param[i], source->sensor_parameters.param_size[i] + 1);
            if (target->sensor_parameters.param[i] == (char *) NULL)
            {
                gsfError = GSF_MEMORY_ALLOCATION_FAILED;
                return(-1);
            }
            strncpy (target->sensor_parameters.param[i], source->sensor_parameters.param[i], source->sensor_parameters.param_size[i]);
            target->sensor_parameters.param_size[i] = source->sensor_parameters.param_size[i];
        }
    }

    /* Copy the comment from the source to the target */
    target->comment.comment_time = source->comment.comment_time;
    target->comment.comment_length = source->comment.comment_length;
    if (source->comment.comment_length > 0)
    {
	if (target->comment.comment == (char *) NULL)
	{
	    target->comment.comment = (char *) calloc (sizeof(char), source->comment.comment_length + 1);
	    if (target->comment.comment == (char *) NULL)
	    {
		gsfError = GSF_MEMORY_ALLOCATION_FAILED;
		return(-1);
	    }
	    strncpy (target->comment.comment, source->comment.comment, source->comment.comment_length);
	    target->comment.comment_length = source->comment.comment_length;
	}
	else if (target->comment.comment_length < source->comment.comment_length)
	{
	    target->comment.comment = (char *) realloc (target->comment.comment, source->comment.comment_length + 1);
	    if (target->comment.comment == (char *) NULL)
	    {
		gsfError = GSF_MEMORY_ALLOCATION_FAILED;
		return(-1);
	    }
	    strncpy (target->comment.comment, source->comment.comment, source->comment.comment_length);
	    target->comment.comment_length = source->comment.comment_length;
	}
    }

    /* Copy the history record from the source to the target */
    target->history.history_time = source->history.history_time;
    strncpy(target->history.host_name, source->history.host_name, GSF_HOST_NAME_LENGTH);
    strncpy(target->history.operator, source->history.host_name, GSF_OPERATOR_LENGTH);

    if (target->history.command_line != (char *) NULL)
    {
        free(target->history.command_line);
    }
    if (source->history.command_line != (char *) NULL)
    {
	target->history.command_line = (char *) calloc (sizeof(char), strlen(source->history.command_line) + 1);
	if (target->history.command_line == (char *) NULL)
	{
	    gsfError = GSF_MEMORY_ALLOCATION_FAILED;
	    return(-1);
	}
    }

    if (target->history.comment != (char *) NULL)
    {
        free(target->history.comment);
    }
    if (source->history.comment != (char *) NULL)
    {
	target->history.comment = (char *) calloc (sizeof(char), strlen(source->history.comment) + 1);
	if (target->history.comment == (char *) NULL)
	{
	    gsfError = GSF_MEMORY_ALLOCATION_FAILED;
	    return(-1);
	}
    }

    /* Copy the navigation error record from the source to the target */
    target->nav_error = source->nav_error;

    return(0);
}

/********************************************************************
 *
 * Function Name : gsfSetParam
 *
 * Description : This function allocates memory for the keyword=value
 *     style parameter and copies the parameter into the allocated space.
 *
 * Inputs :
 *     handle = an integer value containing the file handle, set by gsfOpen
 *     index  = an integer value specifing the index into the processing
 *         parameters array into which the value is to be written.
 *     val = a pointer to the character string containing the parameter.
 *     rec = a pointer to the gsfRecords data structure into which the
 *         parameter is written
 *
 * Returns : This function returns zero if successful, or -1 if an error
 *  occurs.
 *
 * Error Conditions :
 *     GSF_MEMORY_ALLOCATION_FAILED
 *     GSF_PARAM_SIZE_FIXED
 *
 ********************************************************************/

static int
gsfSetParam(int handle, int index, char *val, gsfRecords *rec)
{
    int             len;
    char           *ptr;

    len = strlen (val);
    ptr = gsfFileTable[handle-1].rec.process_parameters.param[index];
    if (ptr == (char *) NULL)
    {
        ptr = (char *) calloc (len+1, sizeof(char));
        if (ptr == (char *) NULL)
        {
            gsfError = GSF_MEMORY_ALLOCATION_FAILED;
            return (-1);
        }
    }
    /* If memory has already been allocated, make sure we have enough space */
    else if (gsfFileTable[handle-1].rec.process_parameters.param_size[index] < len)
    {
        /* If the output file is open update, we cannot write a parameter
         * bigger in size than the one that exists on the disk now.
         */
        if ((gsfFileTable[handle-1].access_mode == GSF_UPDATE) ||
            (gsfFileTable[handle-1].access_mode == GSF_UPDATE_INDEX))
        {
            gsfError = GSF_PARAM_SIZE_FIXED;
            return(-1);
        }
        ptr = (char *) realloc((void *) ptr, len + 1);
        if (ptr == (char *) NULL)
        {
            gsfError = GSF_MEMORY_ALLOCATION_FAILED;
            return (-1);
        }
    }
    gsfFileTable[handle-1].rec.process_parameters.param[index] = ptr;
    gsfFileTable[handle-1].rec.process_parameters.param_size[index] = len;
    rec->process_parameters.param[index] = ptr;
    rec->process_parameters.param_size[index] = len;
    strncpy(rec->process_parameters.param[index], val, len);

    return(0);
}

/********************************************************************
 *
 * Function Name : gsfPutMBParams
 *
 * Description : This function moves swath bathymetry sonar processing
 *    parameters from internal form to "KEYWORD=VALUE" form.  The internal
 *    form parameters are read from an MB_PARAMETERS data structure maintained
 *    by the caller.  The "KEYWORD=VALUE" form parameters are written into the
 *    processing_parameters structure of the gsfRecords data structure
 *    maitained by the caller. Parameters for up to two pairs of
 *    transmit/receive arrays are supported, for systems such as Reson SeaBat
 *    9002.
 *
 * Inputs :
 *     p = a pointer to the gsfMBParams data structure which contains
 *         the parameters in internal form.
 *     rec = a pointer to the gsfRecords data structure into which the
 *         parameters are to be written in the "KEYWORK=VALUE" form.
 *     handle = the integer handle to the file set by gsfOpen.
 *     numArrays = the integer value specifying the number of pairs of
 *         arrays which need to have seperate parameters tracked.
 *
 * Returns : This function returns zero if successful, or -1 if an error
 *  occurs.
 *
 * Error Conditions :
 *     GSF_MEMORY_ALLOCATION_FAILED
 *     GSF_PARAM_SIZE_FIXED
 *
 ********************************************************************/
int
gsfPutMBParams(gsfMBParams *p, gsfRecords *rec, int handle, int numArrays)
{
    char            temp[256];
    char            temp2[64];
    int             ret;

    /* Load the text descriptor for the start of time epoch */
    sprintf(temp, "REFERENCE TIME=1970/001 00:00:00");
    ret = gsfSetParam(handle, 0, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* This parameter indicates whether the depth data has been roll compensated */
    if (p->roll_compensated == GSF_COMPENSATED)
    {
        sprintf(temp, "ROLL_COMPENSATED=YES");
    }
    else
    {
        sprintf(temp, "ROLL_COMPENSATED=NO ");
    }
    ret = gsfSetParam(handle, 1, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* This parameter indicates whether the depth data has been pitch compensated */
    if (p->pitch_compensated == GSF_COMPENSATED)
    {
        sprintf(temp, "PITCH_COMPENSATED=YES");
    }
    else
    {
        sprintf(temp, "PITCH_COMPENSATED=NO ");
    }
    ret = gsfSetParam(handle, 2, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* This parameter indicates whether the depth has been heave compensated */
    if (p->heave_compensated == GSF_COMPENSATED)
    {
        sprintf(temp, "HEAVE_COMPENSATED=YES");
    }
    else
    {
        sprintf(temp, "HEAVE_COMPENSATED=NO ");
    }
    ret = gsfSetParam(handle, 3, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* This parameter indicates whether the depth has been tide compensated */
    if (p->tide_compensated == GSF_COMPENSATED)
    {
        sprintf(temp, "TIDE_COMPENSATED=YES");
    }
    else
    {
        sprintf(temp, "TIDE_COMPENSATED=NO ");
    }
    ret = gsfSetParam(handle, 4, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* If the depth field of the swath bathy ping data structure is true depth,
     * meaning depth is computed by indegrating travel time through the sound
     * speed profile, then this parameter is set as DEPTH_CALCULATION=CORRECTED.
     * If the depth field of the swath bathymetry ping data structure is
     * relative to 1500 meters per second, then this parameter is set as
     * DEPTH_CALCULATION=DEPTHS_RE_1500_MS.
     */
    if (p->depth_calculation == GSF_TRUE_DEPTHS)
    {
        sprintf(temp, "DEPTH_CALCULATION=CORRECTED");
    }
    else if (p->depth_calculation == GSF_DEPTHS_RE_1500_MS)
    {
        sprintf(temp, "DEPTH_CALCULATION=RELATIVE_TO_1500_MS");
    }
    else
    {
        sprintf(temp, "DEPTH_CALCULATION=UNKNOWN");
    }
    ret = gsfSetParam(handle, 5, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* This parameter indicates whether the angle travel time pairs
     * have been corrected for ray tracing.
     */
    if (p->ray_tracing == GSF_COMPENSATED)
    {
        sprintf(temp, "RAY_TRACING=YES");
    }
    else
    {
        sprintf(temp, "RAY_TRACING=NO");
    }
    ret = gsfSetParam(handle, 6, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The DRAFT_TO_APPLY parameter is a place holder for a new draft
     * value which is known, but not yet applied.
     */
    if (numArrays == 1)
    {
        if (p->to_apply.draft[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "DRAFT_TO_APPLY=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "DRAFT_TO_APPLY=%+06.2f",
                p->to_apply.draft[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->to_apply.draft[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "DRAFT_TO_APPLY=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "DRAFT_TO_APPLY=%+06.2f,%+06.2f",
                p->to_apply.draft[0],
                p->to_apply.draft[1]);
        }
    }
    ret = gsfSetParam(handle, 7, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The PITCH_BIAS_TO_APPLY parameter is place holder for a pitch bias
     * value which is known but not yet applied.
     */
    if (numArrays == 1)
    {
        if (p->to_apply.pitch_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "PITCH_TO_APPLY=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "PITCH_TO_APPLY=%+06.2f",
                p->to_apply.pitch_bias[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->to_apply.pitch_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "PITCH_TO_APPLY=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "PITCH_TO_APPLY=%+06.2f,%+06.2f",
                p->to_apply.pitch_bias[0],
                p->to_apply.pitch_bias[1]);
        }
    }
    ret = gsfSetParam(handle, 8, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The ROLL_BIAS_TO_APPLY parameter is place holder for a roll bias value
     * which is known, but not yet applied.
     */
    if (numArrays == 1)
    {
        if (p->to_apply.roll_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "ROLL_TO_APPLY=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "ROLL_TO_APPLY=%+06.2f",
                p->to_apply.roll_bias[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->to_apply.roll_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "ROLL_TO_APPLY=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "ROLL_TO_APPLY=%+06.2f,%+06.2f",
                p->to_apply.roll_bias[0],
                p->to_apply.roll_bias[1]);
        }
    }
    ret = gsfSetParam(handle, 9, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The GYRO_BIAS_TO_APPLY parameter is place holder for a gyro bias value
     * which is known, but not yet applied.
     */
    if (numArrays == 1)
    {
        if (p->to_apply.gyro_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "GYRO_TO_APPLY=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "GYRO_TO_APPLY=%+06.2f",
                p->to_apply.gyro_bias[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->to_apply.gyro_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "GYRO_TO_APPLY=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "GYRO_TO_APPLY=%+06.2f,%+06.2f",
                p->to_apply.gyro_bias[0],
                p->to_apply.gyro_bias[1]);
        }
    }
    ret = gsfSetParam(handle, 10, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The POSITION_OFFSET_TO_APPLY parameter is place holder for a position
     *  offset which is known, but not yet applied.
     */
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "POSITION_OFFSET_TO_APPLY=");
    if (p->to_apply.position_x_offset == GSF_UNKNOWN_PARAM_VALUE)
    {
        sprintf(temp2, "%s,",
            GSF_UNKNOWN_PARAM_TEXT);
    }
    else
    {
        sprintf(temp2, "%+06.2f,",
            p->to_apply.position_x_offset);
    }
    strcat(temp, temp2);
    if (p->to_apply.position_y_offset == GSF_UNKNOWN_PARAM_VALUE)
    {
        sprintf(temp2, "%s,", GSF_UNKNOWN_PARAM_TEXT);
    }
    else
    {
        sprintf(temp2, "%+06.2f,",
            p->to_apply.position_y_offset);
    }
    strcat(temp, temp2);
    if (p->to_apply.position_z_offset == GSF_UNKNOWN_PARAM_VALUE)
    {
        sprintf(temp2, "%s", GSF_UNKNOWN_PARAM_TEXT);
    }
    else
    {
        sprintf(temp2, "%+06.2f",
            p->to_apply.position_z_offset);
    }
    strcat(temp, temp2);
    ret = gsfSetParam(handle, 11, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The TRANSDUCER_OFFSET_TO_APPLY parameter is place holder for a
     * transducer position offset which is known, but not yet applied.
     */
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "TRANSDUCER_OFFSET_TO_APPLY=");
    if (numArrays == 1)
    {
        if (p->to_apply.transducer_x_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->to_apply.transducer_x_offset[0]);
        }
        strcat(temp, temp2);
        if (p->to_apply.transducer_y_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->to_apply.transducer_y_offset[0]);
        }
        strcat(temp, temp2);
        if (p->to_apply.transducer_z_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s", GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f",
                p->to_apply.transducer_z_offset[0]);
        }
        strcat(temp, temp2);
    }
    else if (numArrays == 2)
    {
        if (p->to_apply.transducer_x_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->to_apply.transducer_x_offset[0]);
        }
        strcat(temp, temp2);
        if (p->to_apply.transducer_y_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->to_apply.transducer_y_offset[0]);
        }
        strcat(temp, temp2);
        if (p->to_apply.transducer_z_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->to_apply.transducer_z_offset[0]);
        }
        strcat(temp, temp2);
        if (p->to_apply.transducer_x_offset[1] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->to_apply.transducer_x_offset[1]);
        }
        strcat(temp, temp2);
        if (p->to_apply.transducer_y_offset[1] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->to_apply.transducer_y_offset[1]);
        }
        strcat(temp, temp2);
        if (p->to_apply.transducer_z_offset[1] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f",
                p->to_apply.transducer_z_offset[1]);
        }
        strcat(temp, temp2);
    }
    ret = gsfSetParam(handle, 12, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The APPLIED_DRAFT parameter defines the transducer draft value
     * previously applied to the depths.
     */
    if (numArrays == 1)
    {
        if (p->applied.draft[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_DRAFT=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_DRAFT=%+06.2f",
                p->applied.draft[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->applied.draft[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_DRAFT=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_DRAFT=%+06.2f,%+06.2f",
                p->applied.draft[0],
                p->applied.draft[1]);
        }
    }
    ret = gsfSetParam(handle, 13, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The APPLIED_PITCH_BIAS parameter defines the pitch bias previously
     * applied.
     */
    if (numArrays == 1)
    {
        if (p->applied.pitch_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_PITCH_BIAS=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_PITCH_BIAS=%+06.2f",
                p->applied.pitch_bias[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->applied.pitch_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_PITCH_BIAS=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_PITCH_BIAS=%+06.2f,%+06.2f",
                p->applied.pitch_bias[0],
                p->applied.pitch_bias[1]);
        }
    }
    ret = gsfSetParam(handle, 14, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The APPLIED_ROLL_BIAS parameter defines the roll bias previously
     * applied to the data.
     */
    if (numArrays == 1)
    {
        if (p->applied.roll_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_ROLL_BIAS=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_ROLL_BIAS=%+06.2f",
                p->applied.roll_bias[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->applied.roll_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_ROLL_BIAS=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_ROLL_BIAS=%+06.2f,%+06.2f",
                p->applied.roll_bias[0],
                p->applied.roll_bias[1]);
        }
    }
    ret = gsfSetParam(handle, 15, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The APPLIED_GYRO_BIAS parameter defines the gyro bias previously
     * applied to the data.
     */
    if (numArrays == 1)
    {
        if (p->applied.gyro_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_GYRO_BIAS=%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_GYRO_BIAS=%+06.2f",
                p->applied.gyro_bias[0]);
        }
    }
    else if (numArrays == 2)
    {
        if (p->applied.gyro_bias[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp, "APPLIED_GYRO_BIAS=%s,%s",
                GSF_UNKNOWN_PARAM_TEXT,
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp, "APPLIED_GYRO_BIAS=%+06.2f,%+06.2f",
                p->applied.gyro_bias[0],
                p->applied.gyro_bias[1]);
        }
    }
    ret = gsfSetParam(handle, 16, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The APPLIED_POSITION_OFFSET parameter defines the x,y,z position in
     * ship coordinates to which the lat lons are relative.
     */
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "APPLIED_POSITION_OFFSET=");
    if (p->applied.position_x_offset == GSF_UNKNOWN_PARAM_VALUE)
    {
        sprintf(temp2, "%s,",
            GSF_UNKNOWN_PARAM_TEXT);
    }
    else
    {
        sprintf(temp2, "%+06.2f,",
            p->applied.position_x_offset);
    }
    strcat(temp, temp2);
    if (p->applied.position_y_offset == GSF_UNKNOWN_PARAM_VALUE)
    {
        sprintf(temp2, "%s,",
            GSF_UNKNOWN_PARAM_TEXT);
    }
    else
    {
        sprintf(temp2, "%+06.2f,",
            p->applied.position_y_offset);
    }
    strcat(temp, temp2);
    if (p->applied.position_z_offset == GSF_UNKNOWN_PARAM_VALUE)
    {
        sprintf(temp2, "%s",
            GSF_UNKNOWN_PARAM_TEXT);
    }
    else
    {
        sprintf(temp2, "%+06.2f",
            p->applied.position_z_offset);
    }
    strcat(temp, temp2);
    ret = gsfSetParam(handle, 17, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The APPLIED_TRANSDUCER_OFFSET parameter defines the x,y,z offsets
     * in ship coordinates to which have been applied to refer the x,y,z
     * beam values to the ship reference point.
     */
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "APPLIED_TRANSDUCER_OFFSET=");
    if (numArrays == 1)
    {
        if (p->applied.transducer_x_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->applied.transducer_x_offset[0]);
        }
        strcat(temp, temp2);
        if (p->applied.transducer_y_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->applied.transducer_y_offset[0]);
        }
        strcat(temp, temp2);
        if (p->applied.transducer_z_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f",
                p->applied.transducer_z_offset[0]);
        }
        strcat(temp, temp2);
    }
    else if (numArrays == 2)
    {
        if (p->applied.transducer_x_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->applied.transducer_x_offset[0]);
        }
        strcat(temp, temp2);
        if (p->applied.transducer_y_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->applied.transducer_y_offset[0]);
        }
        strcat(temp, temp2);
        if (p->applied.transducer_z_offset[0] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->applied.transducer_z_offset[0]);
        }
        strcat(temp, temp2);
        if (p->applied.transducer_x_offset[1] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->applied.transducer_x_offset[1]);
        }
        strcat(temp, temp2);
        if (p->applied.transducer_y_offset[1] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s,",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f,",
                p->applied.transducer_y_offset[1]);
        }
        strcat(temp, temp2);
        if (p->applied.transducer_z_offset[1] == GSF_UNKNOWN_PARAM_VALUE)
        {
            sprintf(temp2, "%s",
                GSF_UNKNOWN_PARAM_TEXT);
        }
        else
        {
            sprintf(temp2, "%+06.2f",
                p->applied.transducer_z_offset[1]);
        }
        strcat(temp, temp2);
    }
    ret = gsfSetParam(handle, 18, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The horizontal datum parameter defines the elipsoid to which the
     * latitude longitude values are referenced.
     */
    switch (p->horizontal_datum)
    {
        case (GSF_H_DATUM_WGE):
            sprintf(temp, "GEOID=WGS-84");
            break;

        default:
            sprintf(temp, "GEOID=UNKNWN");
            break;

    }
    ret = gsfSetParam(handle, 19, temp, rec);
    if (ret)
    {
        return(-1);
    }

    /* The TIDAL_DATUM paremeter defines the reference datum for tide
     * corrections
     */
    switch (p->horizontal_datum)
    {
        case (GSF_V_DATUM_MLLW):
            sprintf(temp, "TIDAL_DATUM=MLLW   ");
            break;

        case (GSF_V_DATUM_MLW):
            sprintf(temp, "TIDAL_DATUM=MLW    ");
            break;

        default:
            sprintf(temp, "TIDAL_DATUM=UNKNOWN");
            break;
    }
    ret = gsfSetParam(handle, 20, temp, rec);
    if (ret)
    {
        return(-1);
    }

    rec->process_parameters.number_parameters = 21;

    return(0);
}

/********************************************************************
 *
 * Function Name : gsfGetMBParams
 *
 * Description : This function moves swath bathymetry sonar processing
 *    parameters from external, form to internal form.  The external
 *    "KEYWORD=VALUE" format parameters are read from a processing_params
 *    structure of a gsfRecords data structure maintained by the caller.
 *    The internal form parameters are written into a gsfMBParams data
 *    structure maintained by the caller. Parameters for up to two pairs of
 *    transmit/receive arrays are supported, for systems such as Reson SeaBat
 *    9002.
 *
 * Inputs :
 *     rec = a pointer to the gsfRecords data structure from which the
 *         parameters in "KEYWORK=VALUE" form are to be read.
 *     p = a pointer to the gsfMBParams data structure which will be populated.
 *     numArrays = the integer value specifying the number of pairs of
 *         arrays which need to have seperate parameters tracked.
 *
 * Returns : This function returns zero if successful, or -1 if an error
 *  occurs.
 *
 * Error Conditions :
 *  none.
 *
 ********************************************************************/
int
gsfGetMBParams(gsfRecords *rec, gsfMBParams *p, int *numArrays)
{
    int i;
    char str[64];

    /* Set this value to zero in case we can't determine it */
    *numArrays = 0;

    for (i=0; i<rec->process_parameters.number_parameters; i++)
    {
        if (strstr(rec->process_parameters.param[i], "REFERENCE TIME"))
        {
            memset(p->start_of_epoch, 0, sizeof(p->start_of_epoch));
            strncpy(p->start_of_epoch, rec->process_parameters.param[i], sizeof(p->start_of_epoch));
        }
        else if (strstr(rec->process_parameters.param[i], "ROLL_COMPENSATED"))
        {
            if (strstr(rec->process_parameters.param[i], "YES"))
            {
                p->roll_compensated = GSF_COMPENSATED;
            }
            else
            {
                p->roll_compensated = GSF_UNCOMPENSATED;
            }
        }
        else if (strstr(rec->process_parameters.param[i], "PITCH_COMPENSATED"))
        {
            if (strstr(rec->process_parameters.param[i], "YES"))
            {
                p->pitch_compensated = GSF_COMPENSATED;
            }
            else
            {
                p->pitch_compensated = GSF_UNCOMPENSATED;
            }
        }
        else if (strstr(rec->process_parameters.param[i], "HEAVE_COMPENSATED"))
        {
            if (strstr(rec->process_parameters.param[i], "YES"))
            {
                p->heave_compensated = GSF_COMPENSATED;
            }
            else
            {
                p->heave_compensated = GSF_UNCOMPENSATED;
            }
        }
        else if (strstr(rec->process_parameters.param[i], "TIDE_COMPENSATED"))
        {
            if (strstr(rec->process_parameters.param[i], "YES"))
            {
                p->tide_compensated = GSF_COMPENSATED;
            }
            else
            {
                p->tide_compensated = GSF_UNCOMPENSATED;
            }
        }
        else if (strstr(rec->process_parameters.param[i], "DEPTH_CALCULATION"))
        {
            if (strstr(rec->process_parameters.param[i], "CORRECTED"))
            {
                p->depth_calculation = GSF_TRUE_DEPTHS;
            }
            else if (strstr(rec->process_parameters.param[i], "CALCULATED_RE_1500_MS"))
            {
                p->depth_calculation = GSF_DEPTHS_RE_1500_MS;
            }
            else
            {
                p->depth_calculation = GSF_DEPTH_CALC_UNKNOWN;
            }
        }

        /* This parameter indicates whether the angle travel time
         * pairs have been corrected for ray tracing.
         */
        else if (strstr(rec->process_parameters.param[i], "RAY_TRACING"))
        {
            if (strstr(rec->process_parameters.param[i], "YES"))
            {
                p->ray_tracing = GSF_COMPENSATED;
            }
            else
            {
                p->ray_tracing = GSF_UNCOMPENSATED;
            }
        }
        else if (strstr(rec->process_parameters.param[i], "DRAFT_TO_APPLY"))
        {
            p->to_apply.draft[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.draft[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "DRAFT_TO_APPLY=%lf,%lf",
                    &p->to_apply.draft[0],
                    &p->to_apply.draft[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        else if (strstr(rec->process_parameters.param[i], "PITCH_TO_APPLY"))
        {
            p->to_apply.pitch_bias[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.pitch_bias[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "PITCH_TO_APPLY=%lf,%lf",
                    &p->to_apply.pitch_bias[0],
                    &p->to_apply.pitch_bias[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        else if (strstr(rec->process_parameters.param[i], "ROLL_TO_APPLY"))
        {
            p->to_apply.roll_bias[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.roll_bias[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "ROLL_TO_APPLY=%lf,%lf",
                    &p->to_apply.roll_bias[0],
                    &p->to_apply.roll_bias[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        else if (strstr(rec->process_parameters.param[i], "GYRO_TO_APPLY"))
        {
            p->to_apply.gyro_bias[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.gyro_bias[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "GYRO_TO_APPLY=%lf,%lf",
                    &p->to_apply.gyro_bias[0],
                    &p->to_apply.gyro_bias[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        /* The POSITION_OFFSET_TO_APPLY parameter is place holder for a known,
         * but presently uncorrected position reference offset. The string
         * contains an x,y,z triplet which defines the location on the
         * vessel to which the latitude and longitude are relative.
         */
        else if (strstr(rec->process_parameters.param[i], "POSITION_OFFSET_TO_APPLY"))
        {
            p->to_apply.position_x_offset = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.position_y_offset = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.position_z_offset = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "POSITION_OFFSET_TO_APPLY=%lf,%lf,%lf",
                    &p->to_apply.position_x_offset,
                    &p->to_apply.position_y_offset,
                    &p->to_apply.position_z_offset);
            }
        }
        else if (strstr(rec->process_parameters.param[i], "TRANSDUCER_OFFSET_TO_APPLY"))
        {
            p->to_apply.transducer_x_offset[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.transducer_y_offset[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.transducer_z_offset[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.transducer_x_offset[1] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.transducer_y_offset[1] = GSF_UNKNOWN_PARAM_VALUE;
            p->to_apply.transducer_z_offset[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "TRANSDUCER_OFFSET_TO_APPLY=%lf,%lf,%lf,%lf,%lf,%lf",
                    &p->to_apply.transducer_x_offset[0],
                    &p->to_apply.transducer_y_offset[0],
                    &p->to_apply.transducer_z_offset[0],
                    &p->to_apply.transducer_x_offset[1],
                    &p->to_apply.transducer_y_offset[1],
                    &p->to_apply.transducer_z_offset[1]);
            }
        }
        else if (strstr(rec->process_parameters.param[i], "APPLIED_DRAFT"))
        {
            p->applied.draft[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.draft[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "APPLIED_DRAFT=%lf,%lf",
                    &p->applied.draft[0],
                    &p->applied.draft[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        else if (strstr(rec->process_parameters.param[i], "APPLIED_PITCH_BIAS"))
        {
            p->applied.pitch_bias[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.pitch_bias[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "APPLIED_PITCH_BIAS=%lf,%lf",
                    &p->applied.pitch_bias[0],
                    &p->applied.pitch_bias[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        else if (strstr(rec->process_parameters.param[i], "APPLIED_ROLL_BIAS"))
        {
            p->applied.roll_bias[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.roll_bias[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "APPLIED_ROLL_BIAS=%lf,%lf",
                    &p->applied.roll_bias[0],
                    &p->applied.roll_bias[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        else if (strstr(rec->process_parameters.param[i], "APPLIED_GYRO_BIAS"))
        {
            p->applied.gyro_bias[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.gyro_bias[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "APPLIED_GYRO_BIAS=%lf,%lf",
                    &p->applied.gyro_bias[0],
                    &p->applied.gyro_bias[1]);
            }
            /* Get the number of array pairs from each sonar alignment parameter */
            *numArrays = gsfNumberParams(rec->process_parameters.param[i]);
        }
        /* The APPLIED_POSITION_OFFSET parameter defines the x,y,z position in
         * ship coordinates to which the lat lons are relative.
         */
        else if (strstr(rec->process_parameters.param[i], "APPLIED_POSITION_OFFSET"))
        {
            p->applied.position_x_offset = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.position_y_offset = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.position_z_offset = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "APPLIED_POSITION_OFFSET=%lf,%lf,%lf",
                    &p->applied.position_x_offset,
                    &p->applied.position_y_offset,
                    &p->applied.position_z_offset);
            }
        }
        /* The APPLIED_TRANSDUCER_OFFSET parameter defines the x,y,z offsets
         * in ship coordinates to which have been applied to refer the x,y,z
         * beam values to the ship reference point.
         */
        else if (strstr(rec->process_parameters.param[i], "APPLIED_TRANSDUCER_OFFSET"))
        {
            p->applied.transducer_x_offset[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.transducer_y_offset[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.transducer_z_offset[0] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.transducer_x_offset[1] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.transducer_y_offset[1] = GSF_UNKNOWN_PARAM_VALUE;
            p->applied.transducer_z_offset[1] = GSF_UNKNOWN_PARAM_VALUE;
            if (!strstr(rec->process_parameters.param[i], GSF_UNKNOWN_PARAM_TEXT))
            {
                sscanf (rec->process_parameters.param[i], "APPLIED_TRANSDUCER_OFFSET=%lf,%lf,%lf,%lf,%lf,%lf",
                    &p->applied.transducer_x_offset[0],
                    &p->applied.transducer_y_offset[0],
                    &p->applied.transducer_z_offset[0],
                    &p->applied.transducer_x_offset[1],
                    &p->applied.transducer_y_offset[1],
                    &p->applied.transducer_z_offset[1]);
            }
        }
        /* The horizontal datum parameter defines the elipsoid to which
         * the latitude and longitude values are referenced.
         */
        else if (strstr(rec->process_parameters.param[i], "GEOID"))
        {
            sscanf (rec->process_parameters.param[i], "GEOID=%s", str);
            if (strstr(str, "WGS-84"))
            {
                p->horizontal_datum = GSF_H_DATUM_WGE;
            }
            else
            {
                p->horizontal_datum = GSF_H_DATUM_UND;
            }
        }

        /* The TIDAL_DATUM paremeter defines the reference datum for tide
         * corrections
         */
        else if (strstr(rec->process_parameters.param[i], "TIDAL_DATUM"))
        {
            sscanf (rec->process_parameters.param[i], "TIDAL_DATUM=%s",
                str);

            if (strstr(str, "MLLW"))
            {
                p->vertical_datum = GSF_V_DATUM_MLLW;
            }
            else if (strstr(str, "MLW"))
            {
                p->vertical_datum = GSF_V_DATUM_MLW;
            }
            else
            {
                p->vertical_datum = GSF_V_DATUM_UNKNOWN;
            }
        }
    }

    return(0);
}

/********************************************************************
 *
 * Function Name : numberParams
 *
 * Description : This function parses a KEYWORD=VALUE style parameter
 *   and returns the number of comma delimited values which follow the
 *   equal sign.
 *
 * Inputs :
 *    param = a pointer to a character string containing a KEYWORD=VALUE
 *    style parameter.
 *
 * Returns : This function returns the number of comma delimited values
 *    which follow the equal sign.
 *
 * Error Conditions : none
 *
 ********************************************************************/

static int
gsfNumberParams(char *param)
{
    int number;
    char *p;
    char tmp[128];

    strncpy (tmp, param, sizeof(tmp));
    p = strtok (tmp, ",");

    if (p == NULL)
    {
        return (0);
    }
    else
    {
        number = 1;
    }

    while (p=strtok(NULL, ","))
    {
        number++;
    }

    return (number);
}

/********************************************************************
 *
 * Function Name : gsfGetSwathBathyBeamWidths
 *
 * Description : This function returns to the caller the fore-aft and
 *    the port-starboard beam widths in degrees for a swath bathymetry
 *    multibeam sonar, given a gsfRecords data structure which contains
 *    a populated gsfSwathBathyPing structure.
 *
 * Inputs :
 *     data = The address of a gsfRecords data structure maintained by the
 *         caller which contains a populated gsfSwathBathyPing substructure.
 *     fore_aft = The address of a double allocated by the caller which will
 *         be loaded with the sonar's fore/aft beam width in degrees.
 *     athwartship = The address of a double allocated by the caller which will
 *         be loaded with the sonar's athwartship beam width in degrees.
 *
 * Returns : This function returns zero if successful, or -1 if an error
 *     occured.
 *
 * Error Conditions : unrecognized sonar id or mode.
 *
 ********************************************************************/
int
gsfGetSwathBathyBeamWidths(gsfRecords *data, double *fore_aft, double *athwartship)
{
    int             ret=0;   /* Assume that we will be successful. */

    /* Switch on the type of sonar this data came from */
    switch (data->mb_ping.sensor_id)
    {
        case GSF_SWATH_BATHY_SUBRECORD_SEABEAM_SPECIFIC:
            *fore_aft = 2.666666666667;
            *athwartship = 2.666666666667;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_EM12_SPECIFIC:
            *fore_aft = 1.7;
            *athwartship = 4.4;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_EM100_SPECIFIC:
            switch (data->mb_ping.sensor_data.gsfEM100Specific.mode)
            {
                case(1):  /* wide */
                   *athwartship = 2.5;
                   break;

                case(2):  /* ulta-wide */
                   *athwartship = 5.5;
                   break;

                case(3):  /* narrow */
                   *athwartship = 2.0;
                   break;

                default:  /* Unrecognized sonar mode */
                   *athwartship = 0.0;
                   ret = -1;
                   break;
            }
            *fore_aft = 3.0;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_EM950_SPECIFIC:
        case GSF_SWATH_BATHY_SUBRECORD_EM1000_SPECIFIC:
            *fore_aft = 3.3;
            *athwartship = 3.3;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_EM121A_SPECIFIC:
            *fore_aft = data->mb_ping.sensor_data.gsfEM121ASpecific.beam_width;
            *athwartship = data->mb_ping.sensor_data.gsfEM121ASpecific.beam_width;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_EM121_SPECIFIC:
            *fore_aft = data->mb_ping.sensor_data.gsfEM121Specific.beam_width;
            *athwartship = data->mb_ping.sensor_data.gsfEM121Specific.beam_width;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_SASS_SPECIFIC:
            ret = -1;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_SEAMAP_SPECIFIC:
            ret = -1;
            break;

        case GSF_SWATH_BATHY_SUBRECORD_SEABAT_SPECIFIC:
            if (data->mb_ping.sensor_data.gsfSeaBatSpecific.mode & GSF_SEABAT_WIDE_MODE)
            {
                *fore_aft = 10.0;
            }
            else
            {
                /* Set the F/A beam width to 2.4 here, but also set the return code to 
                 * indicate failure.  This sonar supports multiple beam widths, and
                 * this information is NOT provided in the data stream from the sonar.
                 */
                *fore_aft = 1.5;
                ret = -1;
            }
	    if (data->mb_ping.sensor_data.gsfSeaBatSpecific.mode & GSF_SEABAT_9003)
	    {
		*athwartship = 3.0;
            }
	    else 
	    {
                *athwartship = 1.5;
            }
            break;

        case (GSF_SWATH_BATHY_SUBRECORD_SEABAT_II_SPECIFIC):
	    *fore_aft = data->mb_ping.sensor_data.gsfSeaBatIISpecific.fore_aft_bw;
	    *athwartship = data->mb_ping.sensor_data.gsfSeaBatIISpecific.athwart_bw;
	    break;

        case (GSF_SWATH_BATHY_SUBRECORD_SEABAT_8101_SPECIFIC):
	    *fore_aft = data->mb_ping.sensor_data.gsfSeaBat8101Specific.fore_aft_bw;
	    *athwartship = data->mb_ping.sensor_data.gsfSeaBat8101Specific.athwart_bw;
	    break;

        case (GSF_SWATH_BATHY_SUBRECORD_SEABEAM_2112_SPECIFIC):
	    *fore_aft = 2.0;      
	    *athwartship = 2.0;
	    break;

        case (GSF_SWATH_BATHY_SUBRECORD_ELAC_MKII_SPECIFIC):
	    *fore_aft = 2.0;      
	    *athwartship = 2.0;
	    break;

        default:
        {
            ret -1;
        }
    }
    return(ret);
}

/********************************************************************
 *
 * Function Name : gsfIsStarboardPing
 *
 * Description : This function uses the sonar specific data union
 *     of a gsfSwathBathymetry ping structure to determine if the ping
 *     is from the starboard arrays of a multibeam installation with
 *     dual transmit receive sonar arrays.
 *
 * Inputs :
 *     data = The address of a gsfRecords data structure maintained by the
 *         caller which contains a populated gsfSwathBathyPing substructure.
 *
 * Returns : This function returns non-zero if the ping contained in the
 *     passed data represents a starboard looking ping from a dual headed
 *     sonar installation.  Otherwise, zero is returned.
 *
 * Error Conditions : none
 *
 ********************************************************************/
int
gsfIsStarboardPing(gsfRecords *data)
{
    int ret;


    /* Switch on the type of sonar this data came from */
    switch (data->mb_ping.sensor_id)
    {
        case GSF_SWATH_BATHY_SUBRECORD_SEABAT_SPECIFIC:
/* zzz_ */
/*          if (data->mb_ping.sensor_data.gsfSeaBatSpecific.mode &   */
/*               (GSF_SEABAT_9002 | GSF_SEABAT_STBD_HEAD))           */
        if ( data->mb_ping.sensor_data.gsfSeaBatSpecific.mode  &  GSF_SEABAT_STBD_HEAD )
/* zzz_ */
        {
           ret = 1;
        }
        break;

        default:
            ret = 0;
            break;
    }


    return(ret);
}
