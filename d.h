#ifndef __sdflkjfsdflkHKHLKJHLKJhkjhkjh6876876876
#define __sdflkjfsdflkHKHLKJHLKJhkjhkjh6876876876

/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <dirent.h>

#include "obj.h"
#include "l.h"

#define DIR_T_RECORDS 4096
#define DIR_TYPE_FILE 8
#define DIR_TYPE_DIR 4

struct entry_struct
{
	obj_t t;
	char 	name[FILENAME_MAX]; /* Name of file without path */
	short 	len;				/* Len of file name */
	char 	type;				/* Type of the file. Unused for the moment */
	struct 	stat s_st;			/* struct stat */
};

typedef struct entry_struct entry_t;

struct dir_filter;

/* rescan flag: used by watcher. If set the watched rescan all files on the directory change */
#define DIR_T_FLAG_RESCAN	(1<<0)

struct directory
{
	obj_t t;
	int 				ticket;								/* Used in watch */
	int 				amount;								/* Amount of files in the dir_t: it not REAL size of array, it is number of entries in array */
	char 				dir[FILENAME_MAX];					/* Directory name */
	entry_t * 			entry;								/* Pointer to entries array */
	size_t 				entry_allocated;					/* size of entries array: how much memory allocated */
	struct dir_filter * filter;								/* Prefilter: this filter used on the scan time */
	pthread_mutex_t 	lock;								/* Lock */
	struct stat 		stat;								/* Stat of the directory itself */
	unsigned int		interval;							/* Watch related: how often it should be checked by watcher thread */
	unsigned int 		flags;								/* Misc flags of the dir_t */
};


typedef struct directory dir_t;

dir_t * dir_t_create(void);
dir_t * dir_t_create_empty(void);

int 	dir_t_allocate_entry(dir_t * ps_d, size_t l_num);
int 	dir_t_stat(dir_t * ps_dir);

int 	dir_t_amount(dir_t *ps_dir);

/* 		dir_t entries capacity manipulations */

int 	dir_t_resize(dir_t *ps_dir, size_t i_new_size);
int 	dir_t_insrease(dir_t *ps_dir, size_t i_new_size);
int 	dir_t_shrink(dir_t *ps_dir, size_t i_new_size);
int 	dir_t_reset(dir_t *ps_dir);

int 	dir_t_clean(dir_t * ps_dir);
int 	dir_t_free(dir_t * d);

dir_t * dir_t_scan(char *dir_name);
dir_t * dir_t_scan2(char *dir_name);
dir_t *	dir_t_scan_files_only(char *dir_name);
dir_t *	dir_t_scan_dirs_only(char *dir_name);

dir_t * dir_t_extract_dirs_by_filter(dir_t *d, char * filter);
dir_t * dir_t_extract_files_by_filter(dir_t *d, char * filter);
dir_t * dir_t_extract_regular_files(dir_t *d);
que_t * dir_t_scan_to_que(char *dir_name);
que_t * dir_t_scan_files_only_to_que(char *dir_name);

struct dir_filter;

dir_t * dir_t_scan_filter(char *dir_name, struct dir_filter * ps_filter);

int 	dir_t_del_dfilter(dir_t * ps_dir);
int 	dir_t_add_dfilter(dir_t * ps_dir, struct dir_filter * ps_f);
dir_t  * dir_t_diff(dir_t * sp_a, dir_t * sp_b);

#endif /* __sdflkjfsdflkHKHLKJHLKJhkjhkjh6876876876 */
