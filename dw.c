/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include <pthread.h>

#include "f.h"
#include "d.h"
#include "dw.h"

void * thread_rescan(void* pv_attr)
{
	return(NULL);
}

void * thread_watch(void* pv_attr)
{
	int i;
	dwatch_t * sp_w = ( dwatch_t * ) pv_attr;
    node_t * ps_node;
    dholder_t * ps_holder;
    struct stat s_stat;

	while(1)
	{
		/* Lock que */
        while((sp_w->status & WATCHER_STATUS_RUN_BIT) != WATCHER_STATUS_RUN_BIT)
        {
            sleep(1);
        }

        /* Run on the list and add nodes that changed to scan_que */
        pthread_mutex_lock(&sp_w->q_watch_lock);
        ps_node = sp_w->q_watch->list;

        while(ps_node)
        {
            ps_holder = (dholder_t *) ps_node->pc_data;
            i = stat(ps_holder->ps_dir->dir, &s_stat);
            if (i)
            {
                /* XXX analyze the error. If nosuch directory - signal + remove from list */
            }

            if (s_stat.st_atime != ps_holder->stat.st_atime)
            {
                printf("%s : atime changed\n", ps_holder->ps_dir->dir);
            }

            if (s_stat.st_ctime != ps_holder->stat.st_ctime)
            {
                printf("%s : ctime changed\n", ps_holder->ps_dir->dir);
            }

            if (s_stat.st_mtime != ps_holder->stat.st_mtime)
            {
                printf("%s : mtime changed\n", ps_holder->ps_dir->dir);
            }

            memcpy(&ps_holder->stat, &s_stat, sizeof(struct stat)); 
            ps_node = ps_node->next;
        }

        pthread_mutex_unlock(&sp_w->q_watch_lock);
        usleep(500000);

	}
	return(NULL);
}



dwatch_t * dwatch_create(void)
{
	int i;
	dwatch_t * sp_w;
	sp_w = calloc(1, sizeof(dwatch_t));

	if(!sp_w) return(NULL);

	obj_init(&sp_w->t, OBJ_TYPE_WATCHER);

	pthread_mutex_init(&sp_w->lock, NULL);
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
	pthread_mutex_destroy(&sp_w->lock);

	CLN_BIT(sp_w->status, WATCHER_STATUS_RUN_BIT);

	free(sp_w);
	return(NULL);

}



int dwatch_destroy(dwatch_t * sp_w)
{
	if (!sp_w) return(-1);

	CLN_BIT(sp_w->status, WATCHER_STATUS_RUN_BIT);

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

int dwatch_start(dwatch_t * ps_w)
{
	pthread_mutex_lock(&ps_w->lock);
	SET_BIT(ps_w->status, WATCHER_STATUS_RUN_BIT);	
	pthread_mutex_unlock(&ps_w->lock);
	return(0);
}


int dwatch_stop(dwatch_t * ps_w)
{

	pthread_mutex_lock(&ps_w->lock);
	CLN_BIT(ps_w->status, WATCHER_STATUS_RUN_BIT);	
	pthread_mutex_unlock(&ps_w->lock);
	return(0);
}

dholder_t * dholder_new()
{
    dholder_t * ps_h = calloc(1, sizeof(dholder_t));
	if(ps_h) obj_init(&ps_h->t, OBJ_TYPE_DHOLDER);
    return(ps_h);
}

int dholder_free(dholder_t * ps_holder)
{
    /* XXX Remove holder from all  queues it it is there */
    free(ps_holder);
    return(0);
}

int dholder_set_dir_t(dholder_t * ps_h,  dir_t * ps_dir)
{
    if( stat(ps_dir->dir, &ps_h->stat)) return(-1);
    ps_dir->ticket = DIR_T_TICKET;
    ps_h->ps_dir = ps_dir;
    return(0);
}

dholder_t * dholder_new_from_dir_t(dir_t * ps_d)
{
    dholder_t * ps_holder = dholder_new();
    if(!ps_holder) return (NULL);

    if (dholder_set_dir_t(ps_holder, ps_d))
    {
        dholder_free(ps_holder);
        return(NULL);
    }
    return (ps_holder);
}


int dwatch_dir(dwatch_t *ps_w, dir_t * ps_d)
{
    dholder_t * ps_holder = dholder_new_from_dir_t(ps_d);

    que_add_data(ps_w->q_watch, (char *) ps_holder);
    return(0);
}





/********************************************************/
/* Interface for obj_t abstraction */
/********************************************************/


int obj_watch_free(obj_t * ps_o)
{
	return(int dwatch_destroy( (dwatch_t *)ps_o) );
}


int obj_dir_t_lock(obj_t * ps_o)
{
	dir_t * ps_d = (dir_t *) (ps_o);
	return pthread_mutex_lock(&ps_d->lock);
}

int obj_dir_t_unlock(obj_t * ps_o)
{
	dir_t * ps_d = (dir_t *) (ps_o);
	return pthread_mutex_unlock(&ps_d->lock);
}

obj_t * obj_dir_t_new(void * blabla)
{
	return ((obj_t *)dir_t_create_empty());
}

int obj_watch_valid(obj_t * ps_o)
{
	if (ps_o->type == OBJ_TYPE_WATCHER) return(OBJ_E_OK);
	return(OBJ_E_TYPE);
}


void obj_dir_init_me()
{
	obj_f * pf = calloc(1, sizeof(obj_f));
	pf->obj_dup = obj_copy_dir_t;
	pf->obj_free = obj_dir_t_free;
	pf->obj_lock = obj_dir_t_lock;
	pf->obj_unlock = obj_dir_t_unlock;
	pf->obj_new = obj_dir_t_new;
	pf->obj_valid = obj_dir_t_valid;

	obj_f_install(OBJ_TYPE_DIR, pf);
}
