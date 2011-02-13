#include <string.h>
#include <malloc.h>
#include "obj.h"
#include "d.h"
#include "f.h"

static obj_t * obj_dup_simple(obj_t * ps_o)
{
	obj_t * ps_new; 
	if (!ps_o) return(NULL);
	ps_new = malloc(sizeof(obj_t));
	memcpy(ps_new, ps_o, sizeof(obj_t));
	ps_new->data = malloc(ps_o->size);
	memcpy(ps_new->data, ps_o->data, ps_o->size);
	return(ps_new);
}


static obj_t * obj_copy_filter(obj_t * ps_o)
{
	dfilter_t * ps_f_d;
	dfilter_t * ps_f_tmp;
	dfilter_t * ps_f_s = (dfilter_t *) ps_o;

	while(ps_f_s)
	{
		ps_f_tmp = dfilter_create();

	}

	return(NULL);
}



static obj_t * obj_copy_dir_t(obj_t * ps_o)
{
	int i;
	dir_t * ps_dir_s = (dir_t *) ps_o;
	dir_t * ps_dir_d = NULL;

	if(!ps_o) return(NULL);

	ps_dir_d = dir_t_create_empty();
	if (!ps_dir_d) return(NULL);
	if (ps_dir_s->amount) 
	{	
		i = dir_t_allocate_entry(ps_dir_d, ps_dir_s->amount);
		if (i) goto obj_copy_dir_error;
	}

	memcpy(ps_dir_d->entry, ps_dir_s->entry, sizeof(entry_t) * ps_dir_s->amount);
	memcpy(ps_dir_d->dir, ps_dir_s->dir, FILENAME_MAX);
	
	ps_dir_d->amount = ps_dir_s->amount;
	ps_dir_d->entry_allocated = ps_dir_s->amount;

	if(ps_dir_s->filter) 
	{	
		ps_dir_d->filter = (dfilter_t *) obj_copy_filter( (obj_t * ) ps_dir_s->filter);
		if(!ps_dir_d->filter) goto obj_copy_dir_error;
	}
	
	ps_dir_d->flags = ps_dir_s->flags;
	ps_dir_d->interval = ps_dir_s->interval;
	memcpy(&ps_dir_d->stat, &ps_dir_s->stat, sizeof(struct stat));
	ps_dir_d->ticket = ps_dir_s->ticket;

	return( (obj_t * ) ps_dir_d );

obj_copy_dir_error:
	if(ps_dir_d) dir_t_free(ps_dir_d);
	return(NULL);

}


obj_t * obj_dup(obj_t * ps_o)
{
	obj_t * ps_new = NULL;

	switch(ps_o->type)
	{
	case OBJ_TYPE_CHAR:
	case OBJ_TYPE_SHORT:
	case OBJ_TYPE_INT:
	case OBJ_TYPE_LONG:
	case OBJ_TYPE_LOLGLONG:
	case OBJ_TYPE_UCHAR:
	case OBJ_TYPE_USHORT:
	case OBJ_TYPE_UINT:
	case OBJ_TYPE_ULONG:
	case OBJ_TYPE_POINTER:
		ps_new = obj_dup_simple(ps_o);
		break;
	case OBJ_TYPE_ENTRY:
		break;
	case OBJ_TYPE_DIR:
		ps_new = obj_copy_dir_t(ps_o);
		break;
	case OBJ_TYPE_FILTER:
		break;
	case OBJ_TYPE_FILE:
		break;
	case OBJ_TYPE_QUE:
		break;
	case OBJ_TYPE_WATCHER:
		break;
	case OBK_TYPE_DHOLDER:
		break;
	}

	return(ps_new);
}



