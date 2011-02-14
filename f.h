#ifndef _filter_h_shgdfasdhfgasdkhfgasdhjgyt876876876876
#define _filter_h_shgdfasdhfgasdkhfgasdhjgyt876876876876

/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

/* Filter: here defined structure of filter, used to define search on directory */

/* The idea of filter: user can create as many filters as he wants */
/* The filters added to filters' chain */
/* When the user created all filters he applyes them to a directory (already scanned directory dir_t) */
/* As a result he gets a dir_t structure which contains only items that suits to all filters in the filter chain */
/* In the future the filters should be a part of dir_t */

#include "d.h"


/* This enum defines what the filter does */

typedef enum efilter_type
{
	FILTER_SIZE = 1,				/* The file tested for its size. Only files with exactly the same size as given pass the filter */
	FILTER_SIZE_MORE,				/* The file tested for its size. Only files which size is more then given pass the filter */
	FILTER_SIZE_LESS,				/* The file tested for its size. Only files which size is less then given pass the filter */
	FILTER_FULL_NAME, 				/* Test full name of the file, if it exactly the same the filtering  */
	FILTER_PART_NAME,				/* If given substring is found anywhere in file name it passes the filtering */
	FILTER_PART_INSENSITIVE_NAME,	/* If given substring is found anywhere in file name it passes; search is insensitive to register  */
	FILTER_HAS_NOT,					/* If given substring is NOT found in file name it passes; search IS sensitive to register  */
	FILTER_HAS_NOT_INSENSITIVE,		/* If given substring is NOT found in file name it passes; search is insensitive to register  */
	FILTER_EXTENTION,				/* Test extention of file; given pattern compired to extention ( of the file */
	FILTER_TYPE,					/* Test type of file; given data extracted as mode_t and compired to the fiven pointer by macro S_*  */
	FILTER_MODE,					/* Test permission of file, find all files / dirs with exactly the given permissions */

} efilter_t;


typedef enum efilter_file_type
{
	FILTER_TYPE_REG = S_IFREG,
	FILTER_TYPE_SCK = S_IFSOCK,
	FILTER_TYPE_LNK = S_IFLNK,
	FILTER_TYPE_BLK = S_IFBLK,
	FILTER_TYPE_DIR = S_IFDIR,
	FILTER_TYPE_CHR = S_IFCHR,
	FILTER_TYPE_FIF = S_IFIFO,

} etype_t;


struct dir_filter
{
	obj_t t;
	struct dir_filter * next;
	efilter_t e_ftype;	/* Type of the filter */
	char * pc_data;		/* Pointer to data, the data depends on e_ftype. So for FILTER_NAME it expect to find there char*, but for FILTER_SIZE - off_t / off64_t */
};

typedef struct dir_filter dfilter_t;

dfilter_t * dfilter_create(void);
dfilter_t * dfilter_dup(dfilter_t * ps_f_src);
int 		dfilter_destroy(dfilter_t * sp_filter);
dfilter_t * dfilter_create_with_params(efilter_t e, char * pc_data);
int 		dfilter_add_data(dfilter_t * sp_f, char * pc_data);
int 		dfilter_set_type(dfilter_t * sp_f,  efilter_t e);
int 		dfilter_add_to_dfilter(dfilter_t * sp_f, dfilter_t * sp_f_next);
int 		dfilter_add(dfilter_t * sp_f, efilter_t e, char * pc_data);
int 		dfilter_by_copy(dir_t * sp_dir_in,  dir_t * sp_dir_out, dfilter_t *sp_f);
int 		dfilter_by_shrink(dir_t * sp_dir, dfilter_t *sp_f);
int 		dfilter_free(dfilter_t * sp_f);
int 		dfilter_test_entry(dfilter_t * ps_f, entry_t * ps_entry);

#endif /* _filter_h_shgdfasdhfgasdkhfgasdhjgyt876876876876 */
