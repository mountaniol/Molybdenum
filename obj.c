#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include "obj.h"
#include "d.h"
#include "f.h"
#include "e.h"
#include "dw.h"
#include "l.h"
#include "lock.h"
#include "cbs.h"

int	st_obj_free(void);

static obj_f *obj_f_array[OBJ_TYPE_END];


void destroy_objects()
{
	int i;

	for (i = 0; i < OBJ_TYPE_END; i++)
	{
		if(obj_f_array[i])
		{
			free(obj_f_array[i]);
			obj_f_array[i] = NULL;
		}
	}
}


int init_objects()
{
	memset(obj_f_array, 0, sizeof(obj_f *) * OBJ_TYPE_END);
	/* Add here init of your object */
	obj_dir_init_me();
	owatch_init_me();

	atexit(destroy_objects);
	return(0);
}


int obj_f_install(type_e t, obj_f * ps_f)
{
	obj_f_array[t] = ps_f;
	return(0);
}


static obj_e obj_init_ticket(obj_t * ps_o)
{
	struct timeval s_tv;
	gettimeofday(&s_tv, NULL);

	ps_o->ticket = ( (s_tv.tv_sec << (s_tv.tv_usec & 0xff) ) ^ ( s_tv.tv_usec) );
	return OBJ_E_OK;
}


obj_e obj_init(obj_t * ps_o, type_e t)
{
	/* If the object not registred - return error */
	if (!obj_f_array[t]) return( OBJ_E_TYPE );

	olock_init(&ps_o->lock);
	olock_lock(&ps_o->lock);
	ps_o->type = t;
	obj_init_ticket(ps_o);
	ps_o->id = cbs_get_id();
	ps_o->q_sig = que_create();
	olock_unlock(&ps_o->lock);
	if (!ps_o->id) 
	{
		ps_o->error = OBJ_E_ID;
		return(OBJ_E_ID);
	}

	/* If all right then insert the object int CBS */
	cbs_insert_obj(ps_o);
	return(OBJ_E_OK);
}


obj_e obj_finish(obj_t * ps_o)
{
	/* If the object not registred - return error */
	cbs_remove_obj(ps_o);
	olock_lock(&ps_o->lock);
	cbs_return_id(ps_o);
	ps_o->type = OBJ_TYPE_NONE;
	que_destroy((que_t *) ps_o->q_sig);
	olock_unlock(&ps_o->lock);
	olock_destroy(&ps_o->lock);
	return(OBJ_E_OK);
}


obj_e obj_err(obj_t * o)
{
	obj_e e;
	if (!o) return(OBJ_E_MEMORY);
	e = o->error;
	o->error = OBJ_E_OK;
	return(e);
}


obj_t * obj_dup(obj_t * ps_o)
{
	if (ps_o && obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->dup(ps_o));
	return(NULL);
}


obj_t * obj_new(type_e t, void * ps_v)
{
	obj_t * o = NULL;
	if (obj_f_array[t] )
	{
		o = obj_f_array[t]->new(ps_v);
		obj_init(o,t);
	}
	return(o);
}


int obj_free(obj_t * ps_o)
{
	type_e e;
	if (ps_o && obj_f_array[ps_o->type]) 
	{
		e = ps_o->type;
		obj_finish(ps_o);
		return(obj_f_array[e]->free(ps_o));
	}

	return(1);
}


obj_e obj_reset(obj_t * ps_o)
{
	if (ps_o && obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->init(ps_o));
	return(OBJ_E_UNKNOWN);
}

int obj_lock(obj_t * ps_o)
{
	if (ps_o && obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->lock(ps_o));
	return(1);
}


int obj_unlock(obj_t * ps_o)
{
	if (ps_o && obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->lock(ps_o));
	return(1);
}

size_t obj_amount(obj_t * ps_o)
{
	if (ps_o && obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->amount(ps_o));
	return(1);
}


obj_t * obj_diff(obj_t * ps_a, obj_t * ps_b)
{
	if (ps_a && ps_b && obj_f_array[ps_a->type]) return(obj_f_array[ps_a->type]->diff(ps_a, ps_b));
	return(NULL);
}


obj_t * obj_same(obj_t * ps_a, obj_t * ps_b)
{
	if (ps_a && ps_b && obj_f_array[ps_a->type]) return(obj_f_array[ps_a->type]->same(ps_a, ps_b));
	return(NULL);
}



