#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>

#include "obj.h"
#include "d.h"
#include "f.h"
#include "e.h"
#include "dw.h"
#include "l.h"
#include "lock.h"

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


obj_e obj_init(obj_t * ps_o, type_e t)
{
	/* If the object not registred - return error */
	if (!obj_f_array[t]) return( OBJ_E_TYPE );

	ps_o->type = t;
	ps_o->f = obj_f_array[t];

	return(OBJ_E_OK);
}


obj_t * obj_dup(obj_t * ps_o)
{
	if (ps_o && obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->dup(ps_o));
	return(NULL);
}


obj_t * obj_new(type_e t, void * ps_v)
{
	if (obj_f_array[t] ) return(obj_f_array[t]->new(ps_v));
	return(NULL);
}


int obj_free(obj_t * ps_o)
{
	if (ps_o && obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->free(ps_o));
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

