#ifndef _owatch_zdfhsdfjkhsdfjasdj987987
#define _owatch_zdfhsdfjkhsdfjasdj987987

/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "obj.h"
#include "d.h"

/* Ticket: I try to protect the structure */
/* In the situation when a user added a dir_t to watched and then deallocated it without remove from the watch */
/* It can save the day by checking the ticket: */
/* If the memory already reused and the fiels of ticket changed it remove the pinter to structure from he que */

#define DIR_T_TICKET			( (int) 314159 )
#define WATCHER_F_RUN		(1<<0)
#define WATCHER_F_CHANGED	(1<<1)
#define WATCHER_F_RESCAN	(1<<2)

#define SET_BIT(i, bit)	(i |= bit )
#define CLN_BIT(i, bit)	(i &= ~bit)

struct watcher_obj_holder
{
	obj_t t;
	obj_t *			ps_obj;			/* The dir_t struct */
	struct stat		stat;			/* Last known stat of the directory */
	time_t 			t_next_run;		/* When it should be checked next time */
    
    int				status;
};

typedef struct watcher_obj_holder obj_holder_t;


struct obj_watcher
{
	obj_t t;
	pthread_t 			pid_watch;				/* pid of watcher thread */
	pthread_t 			pid_rescan;				/* pid of rescan  thread */

	pthread_mutex_t 	lock;					/* Lock of the watcher */

	int					status;					/* Status of the watcher: run / stop / changes */

	pthread_mutex_t 	q_watch_lock;			/* Lock of the watcher */
	pthread_mutex_t 	q_changed_lock;			/* Lock of the watcher */
	pthread_mutex_t 	q_rescan_lock;			/* Lock of the watcher */

	que_t *				q_watch;				/* Queue of all dir_t to watch */
	que_t * 			q_changed;				/* Queue of dir_t that changed */
	que_t * 			q_rescan;				/* If dir_t has flag RESCAN set and the dir_t has been changed the dir_t added to this que for its files rescanning */
	size_t 				amount;					/* Amount is the amount of changed items */
    void (*callback)(obj_t*);               /* Send changed object to this func */
};

typedef struct obj_watcher owatch_t;

owatch_t * owatch_create(void);
int owatch_destroy(owatch_t * sp_w);
int owatch_star(owatch_t * ps_w);
int owatch_stop(owatch_t * ps_w);
int watch_obj(owatch_t *ps_w, obj_t * ps_o, int i_rescan);

obj_holder_t * 		dholder_new();
int 				oholder_free(obj_holder_t * ps_holder);
int 				oholder_set_obj_t(obj_holder_t * ps_h,  obj_t * ps_o);
obj_holder_t * 		objholder_new_from_obj_t(obj_t * ps_o);

void owatch_init_me(void);


#endif /* _owatch_zdfhsdfjkhsdfjasdj987987 */


