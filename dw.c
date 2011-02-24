/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include <pthread.h>

#include "f.h"
#include "d.h"
#include "dw.h"

void * thread_rescan(void* pv_attr)
{
	while(1)
	{
		usleep(500000);
	}

	return(NULL);
}

void * thread_watch(void* pv_attr)
{
	int i;
	owatch_t * sp_w = ( owatch_t * ) pv_attr;
    node_t * ps_node;
    obj_holder_t * ps_holder;
    struct stat s_stat;

	while(1)
	{
		/* Lock que */
        while((sp_w->status & WATCHER_F_RUN) != WATCHER_F_RUN)
        {
            sleep(1);
        }

        /* Run on the list and add nodes that changed to scan_que */
        pthread_mutex_lock(&sp_w->q_watch_lock);
        ps_node = sp_w->q_watch->list;

        while(ps_node)
        {
            ps_holder = (obj_holder_t *) ps_node->pc_data;

			if(ps_holder->ps_obj->type == OBJ_TYPE_DIR)
			{
				dir_t * ps_dir = (dir_t *) ps_holder->ps_obj; 	
				i = stat(ps_dir->dir, &s_stat);
				/* If there bit of rescan */
				
			}

            if (i)
            {
                /* XXX analyze the error. If nosuch directory - signal + remove from list */
            }

            if (    s_stat.st_atime != ps_holder->stat.st_atime 
				||  s_stat.st_ctime != ps_holder->stat.st_ctime 
				||  s_stat.st_mtime != ps_holder->stat.st_mtime)
            {
				SET_BIT(sp_w->status, WATCHER_F_CHANGED);
				sp_w->amount++;
				memcpy(&ps_holder->stat, &s_stat, sizeof(struct stat));

				if (ps_holder->ps_obj->type == OBJ_TYPE_DIR)
					if (ps_holder->status & WATCHER_F_RESCAN) dir_t_refresh( (dir_t *) ps_holder->ps_obj);
            }

            
            ps_node = ps_node->next;
        }

        pthread_mutex_unlock(&sp_w->q_watch_lock);
        usleep(500000);

	}
	return(NULL);
}



owatch_t * owatch_create(void)
{
	int i;
	owatch_t * sp_w;
	sp_w = calloc(1, sizeof(owatch_t));

	if(!sp_w) return(NULL);

	pthread_mutex_init(&sp_w->q_changed_lock, NULL);
	pthread_mutex_init(&sp_w->q_rescan_lock, NULL);
	pthread_mutex_init(&sp_w->q_watch_lock, NULL);

	sp_w->q_watch = 	que_create();
	sp_w->q_rescan = 	que_create();
	sp_w->q_watch = 	que_create();

	if(NULL == sp_w->q_watch || NULL == sp_w->q_rescan || NULL == sp_w->q_watch)
		goto error_que;

	/* All right. Start the threads. */

	/* Watch thread */
	i = pthread_create(&sp_w->pid_watch, NULL, thread_watch, sp_w);
	i = pthread_create(&sp_w->pid_rescan, NULL, thread_rescan, sp_w);
		
	return(sp_w);

error_que:

	if (sp_w->q_watch) que_destroy(sp_w->q_watch);
	if (sp_w->q_rescan) que_destroy(sp_w->q_rescan);
	if (sp_w->q_changed) que_destroy(sp_w->q_changed);

	pthread_mutex_destroy(&sp_w->q_changed_lock);
	pthread_mutex_destroy(&sp_w->q_rescan_lock);
	pthread_mutex_destroy(&sp_w->q_watch_lock);

	CLN_BIT(sp_w->status, WATCHER_F_RUN);

	free(sp_w);
	return(NULL);

}


int owatch_destroy(owatch_t * sp_w)
{
	if (!sp_w) return(-1);

	CLN_BIT(sp_w->status, WATCHER_F_RUN);

	pthread_mutex_lock(&sp_w->lock);
	pthread_mutex_lock(&sp_w->q_changed_lock);
	pthread_mutex_lock(&sp_w->q_rescan_lock);
	pthread_mutex_lock(&sp_w->q_watch_lock);

	if(sp_w->pid_watch > 0)  pthread_cancel(sp_w->pid_watch);
	if(sp_w->pid_rescan > 0) pthread_cancel(sp_w->pid_rescan);

	if (sp_w->q_watch) 		que_destroy(sp_w->q_watch);
	if (sp_w->q_rescan) 	que_destroy(sp_w->q_rescan);
	if (sp_w->q_changed) 	que_destroy(sp_w->q_changed);

	pthread_mutex_destroy(&sp_w->q_changed_lock);
	pthread_mutex_destroy(&sp_w->q_rescan_lock);
	pthread_mutex_destroy(&sp_w->q_watch_lock);
	pthread_mutex_destroy(&sp_w->lock);

	free(sp_w);
	return(0);


}

int owatch_star(owatch_t * ps_w)
{
	pthread_mutex_lock(&ps_w->lock);
	SET_BIT(ps_w->status, WATCHER_F_RUN);	
	pthread_mutex_unlock(&ps_w->lock);
	return(0);
}


int owatch_stop(owatch_t * ps_w)
{

	pthread_mutex_lock(&ps_w->lock);
	CLN_BIT(ps_w->status, WATCHER_F_RUN);	
	pthread_mutex_unlock(&ps_w->lock);
	return(0);
}

obj_holder_t * dholder_new()
{
    obj_holder_t * ps_h = calloc(1, sizeof(obj_holder_t));
    return(ps_h);
}

int oholder_free(obj_holder_t * ps_holder)
{
    /* XXX Remove holder from all  queues it it is there */
    free(ps_holder);
    return(0);
}

int oholder_set_obj_t(obj_holder_t * ps_h,  obj_t * ps_o)
{
	if (ps_o->type != OBJ_TYPE_DIR) return(-1);

	if (ps_o->type == OBJ_TYPE_DIR)
	{
		dir_t * ps_dir = (dir_t *) ps_o;

		if( stat(ps_dir->dir, &ps_h->stat)) return(-1);
		ps_dir->ticket = DIR_T_TICKET;
	}

	ps_h->ps_obj = ps_o;
    return(0);
}

obj_holder_t * objholder_new_from_obj_t(obj_t * ps_o)
{
    obj_holder_t * ps_holder = dholder_new();
    if(!ps_holder) return (NULL);

    if (oholder_set_obj_t(ps_holder, ps_o))
    {
        oholder_free(ps_holder);
        return(NULL);
    }

    return (ps_holder);
}


/* Adding an object to watch. If i_rescan != 0 it will rescan the object on change (if it an directory it rescan the directory) */
int watch_obj(owatch_t *ps_w, obj_t * ps_o, int i_rescan)
{
    obj_holder_t * ps_holder = objholder_new_from_obj_t(ps_o);

    que_add_data(ps_w->q_watch, (char *) ps_holder);
    return(0);
}





/********************************************************/
/* Interface for obj_t abstraction */
/********************************************************/


static int obj_watch_free(obj_t * ps_o)
{
	return( owatch_destroy( (owatch_t *)ps_o ) );
}


static int obj_watch_lock(obj_t * ps_o)
{
	owatch_t * ps_w = (owatch_t *) (ps_o);
	return pthread_mutex_lock(&ps_w->lock);
}

static int obj_watch_unlock(obj_t * ps_o)
{
	owatch_t * ps_w = (owatch_t *) (ps_o);
	return pthread_mutex_unlock(&ps_w->lock);
}


static obj_t * obj_watch_new(void * blabla)
{
	return ((obj_t *) owatch_create());
}

static int obj_watch_valid(obj_t * ps_o)
{
	if (ps_o->type == OBJ_TYPE_WATCHER) return(OBJ_E_OK);
	return(OBJ_E_TYPE);
}


static size_t obj_watch_amount(obj_t * ps_o)
{
	return( ((owatch_t *) ps_o)->q_watch->amount);
	return(OBJ_E_TYPE);
}


void owatch_init_me()
{
	obj_f * pf = calloc(1, sizeof(obj_f));
	pf->dup = NULL;
	pf->free = obj_watch_free;
	pf->lock = obj_watch_lock;
	pf->unlock = obj_watch_unlock;
	pf->new = obj_watch_new;
	pf->valid = obj_watch_valid;

	/* pf->init = obj_dir_t_init;
	pf->diff = obj_dir_t_diff;
	pf->same = obj_dir_t_diff; */
	pf->amount = obj_watch_amount;

	obj_f_install(OBJ_TYPE_WATCHER, pf);
}

