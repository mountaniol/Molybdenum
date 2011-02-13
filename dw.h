#ifndef _dir_t_watch_zdfhsdfjkhsdfjasdj987987
#define _dir_t_watch_zdfhsdfjkhsdfjasdj987987

/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "obj.h"
#include "d.h"

/* Ticket: I try to protect the structure */
/* In the situation when a user added a dir_t to watched and then deallocated it without remove from the watch */
/* It can save the day by checking the ticket: */
/* If the memory already reused and the fiels of ticket changed it remove the pinter to structure from he que */

#define DIR_T_TICKET			( (int) 314159 )
#define WATCHER_STATUS_RUN_BIT	(1<<0)

#define SET_BIT(i, bit)	(i |= bit )
#define CLN_BIT(i, bit)	(i &= ~bit)

struct dir_t_holder
{
	obj_t t;
	dir_t *			ps_dir;			/* The dir_t struct */
	struct stat		stat;			/* Last known stat of the directory */
	time_t 			t_next_run;		/* When it should be checked next time */
};

typedef struct dir_t_holder dholder_t;


struct dir_t_watcher
{
	obj_t t;
	pthread_t 			pid_watch;				/* pid of watcher thread */
	pthread_t 			pid_rescan;				/* pid of rescan  thread */

	pthread_mutex_t 	lock;					/* Lock of the watcher */

	int					status;					/* Status of the watcher: run / stop*/

	pthread_mutex_t 	q_watch_lock;			/* Lock of the watcher */
	pthread_mutex_t 	q_changed_lock;			/* Lock of the watcher */
	pthread_mutex_t 	q_rescan_lock;			/* Lock of the watcher */

	que_t *				q_watch;				/* Queue of all dir_t to watch */
	que_t * 			q_changed;				/* Queue of dir_t that changed */
	que_t * 			q_rescan;				/* If dir_t has flag RESCAN set and the dir_t has been changed the dir_t added to this que for its files rescanning */
};

typedef struct dir_t_watcher dwatch_t;


dwatch_t * dwatch_create(void);
int dwatch_start(dwatch_t * ps_w);
int dwatch_stop(dwatch_t * ps_w);
int dwatch_dir(dwatch_t *ps_w, dir_t * ps_d);

#endif /* _dir_t_watch_zdfhsdfjkhsdfjasdj987987 */


