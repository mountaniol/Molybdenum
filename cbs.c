#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "cbs.h"


/* Pointer to CBS hash tables   */
static cbs_hash1_t 	*   	cbs_hash = NULL;
static que_t * 				q_id;
static id_t 				id_current = 1;

static que_t *				q_notification = NULL;
static pthread_cond_t 		q_notification_cv = PTHREAD_COND_INITIALIZER;
static pthread_t 			q_notification_pid;
static olock_t 				id_lock;


#define CBS_LOCK() 		olock_lock(&cbs_hash->t.lock);
#define CBS_UNLOCK() 	olock_unlock(&cbs_hash->t.lock);

/********************************** file operations ***********************************/


static cbs_hash1_t * cbs_hash1_new()
{
	cbs_hash1_t * ps_h;
	ps_h = calloc(1, sizeof(cbs_hash1_t));
	return(ps_h);
}


static int cbs_hash1_free(cbs_hash1_t * ps_h)
{
	free(ps_h);
	return(0);
}


static cbs_hash2_t * cbs_hash2_new()
{
	cbs_hash2_t * ps_h;
	ps_h = calloc(1, sizeof(cbs_hash2_t));
	return(ps_h);
}


static int cbs_hash2_free(cbs_hash2_t * ps_h)
{
	free(ps_h);
	return(0);
}


static cbs_o * cbs_o_new(obj_t * o)
{
	cbs_o * ps_o;
	ps_o = calloc(1, sizeof(cbs_o));
	ps_o->q_employee = que_create();

	if(!ps_o->q_employee) 
	{
		free(ps_o);
		return(NULL);
	}
	
	ps_o->o = o;
	return(ps_o);
}


static int cbs_o_free(cbs_o * ps_o)
{
	if(ps_o->q_employee) que_destroy(ps_o->q_employee);
	free(ps_o);
	return(0);
}


static obj_t * obj_hash1_new(void * blabla)
{
	return( (obj_t *) cbs_hash1_new());
}


static obj_t * obj_cbs_o_new(void * v)
{
	return( (obj_t *) cbs_o_new(v));
}


static obj_t * obj_hash2_new(void * blabla)
{
	return( (obj_t *) cbs_hash2_new());
}

static int obj_hash1_free(obj_t * o)
{
	return(cbs_hash1_free((cbs_hash1_t *) o) );
}


static int obj_hash2_free(obj_t * o)
{
	return(cbs_hash2_free((cbs_hash2_t *) o) );
}

static int obj_cbs_o_free(obj_t * o)
{
	return(cbs_o_free((cbs_o *) o) );
}


/********************************** end file operations ***********************************/



static void * cbs_note_send_thread(void * data)
{
	osig_t * ps_sig = (osig_t *) data;
	cbs_o * ps_cbs;

	CBS_LOCK();
	ps_cbs = CBS_ID_TO_CBS(ps_sig->dst);
	CBS_UNLOCK();

	if(!ps_cbs)
	{
		cbs_signal_free(ps_sig);
		return(NULL);
	}

	/* If the ticket is the same then send the signal. */
	if(ps_sig->ticket_dst == ps_cbs->o->ticket)
	{
		ps_cbs->o->sighandler(ps_sig);
	}
	/* Wrong ticket, free signal */
	else
	{
		printf("Wrong ticket: in signal %X, in dst object %X\n", ps_sig->ticket_dst, ps_cbs->o->ticket);	
		cbs_signal_free(ps_sig);
	}

	return(NULL);
}



static void * cbs_notification_thread(void * data)
{
	osig_t * 	ps_sig;
	cbs_o * 	ps_cbs;

	pthread_attr_t pt_attr;
	pthread_t pid;

	pthread_attr_init(&pt_attr);
	pthread_attr_setstacksize(&pt_attr, 32768);

	/* printf("Thread notification: i am alive\n"); */

	while(1)
	{
#if 1
		olock_lock(&q_notification->lock);
		printf("Thread notification: going to sleep\n");
		pthread_cond_wait(&q_notification_cv, &q_notification->lock);
		olock_unlock(&q_notification->lock);
#endif		
		
		/* printf("Thread notification: i woke up\n"); */
		do
		{
			/* This que keeps only pointers to real objects. */
			/* It doesn't allocate or free think. */

			
			ps_sig = (osig_t *) que_extract_data_r(q_notification);

			/* printf("Thread notification: extracted up\n"); */
			if (ps_sig)	 pthread_create(&pid, &pt_attr, cbs_note_send_thread, (void *) ps_sig);	
#if 0			
			CBS_LOCK();
			ps_cbs = CBS_ID_TO_CBS(ps_sig->dst);
			CBS_UNLOCK();
			/* printf("Thread notification: got cbsp\n"); */
			if ( !ps_cbs ) continue;
			/* printf("Thread notification: call handler\n"); */

			/* If the ticket is same - send the signal. */
			if ( ps_sig->ticket_dst == ps_cbs->o->ticket )
			{
				ps_cbs->o->sighandler(ps_sig);
			}
			/* Wrong ticket, free signal */
			else
			{
				printf("Wrong ticket: in signal %X, in dst object %X\n", ps_sig->ticket_dst, ps_cbs->o->ticket);    
				cbs_signal_free(ps_sig);
			}
#endif			
			/* printf("Thread notification: finished\n"); */
		} while (ps_sig);
		
		// usleep(50);		
	}

	printf("Thread notification: exit\n");
	return(NULL);
}


int cbs_init(void)
{
	/* init objects operations */
	obj_f * pf_hash1;
	obj_f * pf_hash2;
	obj_f * pf_cbs_o;

	pthread_attr_t attr;


	pf_hash1 = calloc(1, sizeof(obj_f));
	pf_hash1->new = obj_hash1_new;
	pf_hash1->free = obj_hash1_free;
	obj_f_install(OBJ_TYPE_CBS_HASH1, pf_hash1);

	pf_hash2 = calloc(1, sizeof(obj_f));
	pf_hash2->new = obj_hash2_new;
	pf_hash2->free = obj_hash2_free;
	obj_f_install(OBJ_TYPE_CBS_HASH2, pf_hash2);

	pf_cbs_o = calloc(1, sizeof(obj_f));
	pf_cbs_o->new = obj_cbs_o_new;
	pf_cbs_o->free = obj_cbs_o_free;
	obj_f_install(OBJ_TYPE_CBS_O, pf_cbs_o);


    cbs_hash = (cbs_hash1_t *) obj_new(OBJ_TYPE_CBS_HASH1, NULL);

    if (!cbs_hash) return(-1);

	q_id = que_create();

	if (!q_id)
	{
		free(cbs_hash);
		return(-1);
	}


	q_notification = que_create();

	if (!q_notification)
	{
		que_destroy(q_id);
		free(cbs_hash);
		return(-1);
	}

	olock_init(&id_lock);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if(pthread_create(&q_notification_pid, &attr, cbs_notification_thread, NULL))
		printf("Can't start notification thread\n");

	pthread_detach(q_notification_pid);
	pthread_attr_destroy(&attr);

	
    return(0);
}


static int cbs_hash_destroy()
{
	int i;
	int j;

	for (i = 0 ; i < CBS_HASH_1 ; i++)
	{	
		if(cbs_hash->o[i]) 	
		{

			for (j = 0; j < CBS_HASH_2 ; j++)
			{
				if (cbs_hash->o[i]->o[j]) obj_free((obj_t *) cbs_hash->o[i]->o[j]);
			}

			obj_free(&cbs_hash->o[i]->t);
		}
	}

	obj_free(&cbs_hash->t);
	cbs_hash = NULL;

	return(0);
}




int cbs_destroy()
{
	if(!cbs_hash) return(0);

	pthread_cancel(q_notification_pid);

	olock_lock(&id_lock);

	que_destroy_data(q_id);
	que_destroy_data(q_notification);
	cbs_hash_destroy();

	olock_destroy(&id_lock);
	pthread_cond_destroy(&q_notification_cv);
	return(0);
}



static id_t get_new_id()
{
	id_t i;
	if(!cbs_hash) return(0);

	olock_lock(&id_lock);
	i = id_current++;
	olock_unlock(&id_lock);

	return(i);
}


/* Get ID */
id_t cbs_get_id()
{
	id_t * ps_id;
	id_t i;

	if(!cbs_hash) return(0);

	ps_id = (id_t *) que_extract_data_r(q_id);

	if(ps_id)
	{
		i = *ps_id;
		free(ps_id);
	}
	else i = get_new_id();

	return(i);
}



/* Return used id to stack of IDs */

int cbs_return_id(obj_t * o)
{
	id_t * ps_id;

	if(! o || !o->id) return(OBJ_E_ID);
	if(!cbs_hash) return(0);

	ps_id = calloc(1,sizeof(id_t));
	
	if (!ps_id) 
	{
		o->error = OBJ_E_MEMORY;
		return(-1);
	}

	*ps_id = o->id;
	if(!que_add_data_to_tail_r(q_id, (char *)  ps_id))
	{
		free(ps_id);
		o->error = OBJ_W_AGAIN;
		return(-1);
	}
	return(0);
}



int cbs_insert_obj(obj_t * o)
{
	cbs_o * ps_cbs_o;

	if(CBS_OBJ_WRONG(o) ) return(OBJ_E_ID);

	if(!cbs_hash) return(-1);

/* 	printf("Inserting: o->%i\n", o->id); */
	CBS_LOCK();

	if(! cbs_hash->o[ CBSH1(o->id) ])
		cbs_hash->o[ CBSH1(o->id) ] = (cbs_hash2_t *) obj_new(OBJ_TYPE_CBS_HASH2, NULL);

	/* Level 2 array allocation */

	ps_cbs_o = (cbs_o *) obj_new(OBJ_TYPE_CBS_O, o);

	if ( !ps_cbs_o )
	{
		if ( cbs_hash->o[ CBSH1(o->id) ]->amount == 0 )
		{

			obj_free(&cbs_hash->o[ CBSH1(o->id) ]->t);
			CBS_UNLOCK();
		}
		return(-1);
	}


	olock_lock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);

	cbs_hash->o[ CBSH1(o->id) ]->o[ CBSH2(o->id) ] = ps_cbs_o;
	cbs_hash->o[ CBSH1(o->id) ]->amount++;
	cbs_hash->amount++;

/* 	printf("Inserted: obj->%i\n", cbs_hash->o[ CBSH1(o->id) ]->o[ CBSH2(o->id) ]->o->id); */
	olock_unlock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);
	CBS_UNLOCK();
	
	return(0);
}



/* Internal function. Called under lock */
static int remove_from_manager(cbs_o * manager, cbs_o * employee)
{
	return( NULL == que_remove_node_by_data(manager->q_employee, (char *) employee) ? 1 : 0 );
}



int cbs_remove_obj(obj_t * o)
{
	cbs_o * ps_cbs_rm;
	cbs_o * ps_cbs_tmp;

	if(CBS_OBJ_WRONG(o) ) return(OBJ_E_ID);
	if (!cbs_hash) return(OBJ_E_INDEX);

	CBS_LOCK();

	if (!cbs_hash->o[ CBSH1(o->id) ]) 
	{
		CBS_UNLOCK();
		return(OBJ_E_INDEX);
	}

	if (!cbs_hash->o[ CBSH1(o->id) ]->o[CBSH2(o->id)]) 
	{
		CBS_UNLOCK();
		return(OBJ_E_INDEX);
	}

	/* Get node of this object */
	ps_cbs_rm = (cbs_o *) CBS_OBJ_TO_CBS(o);

	olock_lock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);

	/* Remove it from all employees */
	do
	{
		/* */
		ps_cbs_tmp = (cbs_o *) que_extract_data_r(ps_cbs_rm->q_employee);
		if (ps_cbs_tmp)	ps_cbs_tmp->manager = NULL;

	} while (ps_cbs_tmp);

	/* Remove it from its manager */
	if (ps_cbs_rm->manager)
		remove_from_manager(ps_cbs_rm->manager, ps_cbs_rm);

	/* Free the cbs object */
	obj_free( (obj_t *) CBS_OBJ_TO_CBS(o));
	CBS_OBJ_TO_CBS(o) = NULL;

	/* Decrease the amount of objects the hashes hold */
	cbs_hash->amount--;
	cbs_hash->o[ CBSH1(o->id) ]->amount--;
	
	/* Now test the amount of objects in 2 level hash. If it 0 then free the memory */	
	if(cbs_hash->o[ CBSH1(o->id) ]->amount == 0)
	{	
		olock_unlock( &((obj_t *) cbs_hash->o[ CBSH1(o->id) ])->lock);
		obj_free( ( obj_t *) cbs_hash->o[ CBSH1(o->id) ]) ;
		cbs_hash->o[ CBSH1(o->id) ] = NULL;
	}
		
	if (cbs_hash->o[ CBSH1(o->id) ]) olock_unlock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);
	CBS_UNLOCK();
	
	return(0);
}


long cbs_hash_amount()
{
	return(cbs_hash->amount);
}


/* Set m as the manager of e (if no other manager) */
/* Set e as employee of m */

int cbs_set_employee(obj_t * m, obj_t * e)
{
	cbs_o * ps_cbs_m;
	cbs_o * ps_cbs_e;

	if (!m || !e) return(OBJ_E_ARG);
	if (CBS_WRONG_ID(m) || CBS_WRONG_ID(e) ) return(OBJ_E_ID);

	ps_cbs_e = CBS_OBJ_TO_CBS(e);
	ps_cbs_m = CBS_OBJ_TO_CBS(m);

	if (!ps_cbs_e || !ps_cbs_m) return(OBJ_E_INDEX);

	/* Set m as a manager */
	ps_cbs_e->manager = ps_cbs_m;

	/* Set e as an employee */
	que_add_data_to_tail_r(ps_cbs_m->q_employee, (char *) ps_cbs_e);

	return(0);
}


osig_t * cbs_sig_new(id_t dst, id_t src, signum_e signum, void * data)
{
	osig_t * ps_sig;
	cbs_o * ps_cbs;

	static int count = 0;

	/* Create signal */
	ps_sig = calloc(1, sizeof(osig_t));
	if (!ps_sig) return(NULL);
	olock_init(&ps_sig->lock);
	
	ps_sig->dst = dst;
	ps_sig->src = src;
	ps_sig->signum = signum;
	ps_sig->data = data;

	/* Tickets. I need them to validate that the src / dst object is unchanged */
	/* I use the ID again after the object with this ID is deallocated */
	/* Such way I minimize CBS hash size */
	/* But what happens if the signal send to an object with ID 25, but the object is already died and there now is another object with ID 25? */
	/* So id + ticket is an unic identificator, where ID used for fast search an in the cotexts where thr object can't be freed. */

	CBS_LOCK();
	ps_cbs = CBS_ID_TO_CBS(dst);
	CBS_UNLOCK();
	if (!ps_cbs) goto cbs_sig_new_error;
	ps_sig->ticket_dst = ps_cbs->o->ticket;

	CBS_LOCK();
	ps_cbs = CBS_ID_TO_CBS(src);
	CBS_UNLOCK();
	if (!ps_cbs) goto cbs_sig_new_error;
	ps_sig->ticket_src = ps_cbs->o->ticket;

	count++;
	printf("Allocated (%p): emmited by %i count: %d\n", ps_sig, src, count);

	return(ps_sig);

cbs_sig_new_error:
	/* printf("Error of signal creation\n"); */
	free(ps_sig);
	return(NULL);
}


obj_e cbs_signal_free(osig_t * ps_sig)
{
	cbs_o * ps_cbs;
	static int count = 0;
	if(!ps_sig) return(OBJ_E_ARG);

	olock_lock(&ps_sig->lock);

	/* Try to find and delete if from que of dst */

	CBS_LOCK();
	ps_cbs = CBS_ID_TO_CBS(ps_sig->dst);
	

	if(ps_cbs && ps_cbs->o->q_sig->amount > 0 )
		que_remove_node_by_data( (que_t *) ps_cbs->o->q_sig, (char *) ps_sig);

	olock_destroy(&ps_sig->lock);
	CBS_UNLOCK();

	count++;
	printf("Freeng (%p): sig emitted by %d count %d\n", ps_sig, ps_sig->src, count);
	free(ps_sig);

	return(OBJ_E_OK);
}



obj_e cbs_place_sig(osig_t * ps_s)
{
	cbs_o * ps_cbs_dst;

	CBS_LOCK();
	ps_cbs_dst = CBS_ID_TO_CBS(ps_s->dst);
	CBS_UNLOCK();

	if(!ps_cbs_dst) return(OBJ_E_INDEX);

	if ( NULL == que_add_data_to_tail_r( (que_t *) ps_cbs_dst->o->q_sig, (char *) ps_s) )
	{
		free(ps_s);
		return(OBJ_E_UNKNOWN);
	}

	que_add_data_to_tail_r(q_notification, (char * ) ps_s);
	pthread_cond_signal(&q_notification_cv);

	return(0);
}


obj_e cbs_send_sig_id(id_t src, id_t dst, signum_e signum, void * data)
{
	osig_t * ps_sig;

	ps_sig = cbs_sig_new(dst, src, signum, data);
	if (!ps_sig) return(OBJ_E_MEMORY);
	return(cbs_place_sig(ps_sig));
}


obj_e cbs_signal_reply(osig_t * ps_orig_sig, signum_e signum, void * data)
{
	cbs_o * ps_cbs;
	id_t tmp;
	ticket_t tmp_ticket;

	/* Reuse the same signal structure. Exchange src <-> dst */
	tmp 				= ps_orig_sig->dst;
	tmp_ticket			= ps_orig_sig->ticket_dst;

	ps_orig_sig->dst 			= ps_orig_sig->src;
	ps_orig_sig->ticket_dst 	= ps_orig_sig->ticket_src;

	ps_orig_sig->src 			= tmp;
	ps_orig_sig->ticket_src 	= tmp_ticket;

	ps_orig_sig->signum 		= signum;
	ps_orig_sig->data 			= data;
	
	/* Remove it from object's que if it is there */	
	CBS_LOCK();
	ps_cbs = CBS_ID_TO_CBS(tmp);
	CBS_UNLOCK();

	que_remove_node_by_data(ps_cbs->o->q_sig, (char *) ps_orig_sig);

	return(cbs_place_sig(ps_orig_sig));
}


osig_t * cbs_get_signal(obj_t * o)
{
	osig_t * ps_sig;

	ps_sig = (osig_t *) que_extract_data_r((que_t *) o->q_sig);
	return(ps_sig);
}


obj_e cbs_sig_to_manager(obj_t * src, signum_e signum, void * data)
{
	return(0);
}			

