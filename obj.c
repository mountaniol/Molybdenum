#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include "obj.h"
#include "d.h"
#include "f.h"
#include "e.h"
#include "dw.h"
#include "l.h"

static obj_f *obj_f_array[OBJ_TYPE_END];

int init_objects()
{
	memset(obj_f_array, 0, sizeof(obj_f ) * OBJ_TYPE_END);
	/* Add here init of your object */
	obj_dir_init_me();
	return(0);
}

int obj_f_install(type_e t, obj_f * ps_f)
{
	obj_f_array[t] = ps_f;
	return(0);
}


obj_e obj_type_validity(obj_t * ps_o, type_e e_type)
{
	if (!ps_o) return(OBJ_E_ARG);

	if (ps_o->type <= OBJ_TYPE_BEGIN || ps_o->type >= OBJ_TYPE_END)
	{
		ps_o->error = OBJ_E_TYPE;
		return(OBJ_E_TYPE);
	}

	if (e_type > 0 && ps_o->type != e_type)
	{
		ps_o->error = OBJ_E_TYPE;
		return(OBJ_E_TYPE);
	}

	return(0);
}



size_t obj_type_size(type_e t)
{
	switch(t)
	{
	case OBJ_TYPE_BEGIN:
		return(0);
		break;
	case OBJ_TYPE_CHAR:
	case OBJ_TYPE_UCHAR:
		return(sizeof(char));
		break;
	case OBJ_TYPE_SHORT:
	case OBJ_TYPE_USHORT:
		return(sizeof(short));
		break;
	case OBJ_TYPE_INT:
	case OBJ_TYPE_UINT:
		return(sizeof(int));
		break;
	case OBJ_TYPE_LONG:
	case OBJ_TYPE_ULONG:
		return(sizeof(long));
		break;
	case OBJ_TYPE_LOLGLONG:
		return(sizeof(long long));
		break;
	case OBJ_TYPE_POINTER:
		return (sizeof(void *));
		break;
	case OBJ_TYPE_ENTRY:
		return(sizeof(entry_t));
		break;
	case OBJ_TYPE_DIR:
		return(sizeof(dir_t));
		break;
	case OBJ_TYPE_FILTER:
		return(sizeof(dfilter_t));
	case OBJ_TYPE_FILE:
		break;
	case OBJ_TYPE_QUE:
		return(sizeof(que_t));
		break;
	case OBJ_TYPE_NODE:
		return(sizeof(node_t));
		break;
	case OBJ_TYPE_WATCHER:
		return(sizeof(dwatch_t));
		break;
	case OBJ_TYPE_DHOLDER:
		return(sizeof(dholder_t));
		break;
	case OBJ_TYPE_END:
		return(0);
		break;
	}

	return(-1);
}


obj_t * obj_new(type_e t, void * data)
{
	obj_t * ps_o;
	
	switch(t)
	{
	case OBJ_TYPE_BEGIN:
		return(NULL);
		break;
	case OBJ_TYPE_CHAR:
	case OBJ_TYPE_UCHAR:
	case OBJ_TYPE_SHORT:
	case OBJ_TYPE_USHORT:
	case OBJ_TYPE_INT:
	case OBJ_TYPE_UINT:
	case OBJ_TYPE_LONG:
	case OBJ_TYPE_ULONG:
	case OBJ_TYPE_LOLGLONG:
	case OBJ_TYPE_POINTER:
	case OBJ_TYPE_ENTRY:

		ps_o = calloc(1, sizeof(obj_t));
		if(!ps_o) return(NULL);
		ps_o->data = data;
		ps_o->size = obj_type_size(t);

		break;
	case OBJ_TYPE_DIR:
		return( (obj_t *) dir_t_create_empty() );
		break;
	case OBJ_TYPE_FILTER:
		return( (obj_t *) dfilter_create() );
	case OBJ_TYPE_FILE:
		break;
	case OBJ_TYPE_QUE:
		return( (obj_t *) que_create() );
		break;
	case OBJ_TYPE_NODE:
		return( (obj_t *) node_create_data(data) );
		break;
	case OBJ_TYPE_WATCHER:
		return((obj_t *) dwatch_create() );
		break;
	case OBJ_TYPE_DHOLDER:
		return((obj_t *) dholder_new() );
		break;
	case OBJ_TYPE_END:
		return(NULL);
		break;
	}

	return(NULL);
}


int obj_free(obj_t * ps_o)
{
	if (obj_type_validity(ps_o, 0)) return(0);

	switch(ps_o->type)
	{
	case OBJ_TYPE_BEGIN:
		return(-1);
		break;
	case OBJ_TYPE_CHAR:
	case OBJ_TYPE_UCHAR:
	case OBJ_TYPE_SHORT:
	case OBJ_TYPE_USHORT:
	case OBJ_TYPE_INT:
	case OBJ_TYPE_UINT:
	case OBJ_TYPE_LONG:
	case OBJ_TYPE_ULONG:
	case OBJ_TYPE_LOLGLONG:
	case OBJ_TYPE_POINTER:
	case OBJ_TYPE_ENTRY:
		free(ps_o);
		return(0);
		break;
	case OBJ_TYPE_DIR:
		return(  dir_t_free((dir_t *)ps_o) );
		break;
	case OBJ_TYPE_FILTER:
		return( dfilter_free((dfilter_t *) ps_o ));
	case OBJ_TYPE_FILE:
		break;
	case OBJ_TYPE_QUE:
		return( que_destroy((que_t *) ps_o) );
		break;
	case OBJ_TYPE_NODE:
		free( ps_o );
		return( 0 );
		break;
	case OBJ_TYPE_WATCHER:
		return( dwatch_destroy((dwatch_t *) ps_o ) );
		break;
	case OBJ_TYPE_DHOLDER:
		return( dholder_free((dholder_t *) ps_o) );
		break;
	case OBJ_TYPE_END:
		return(-1);
		break;
	}

	return(-1);

}



obj_e obj_init(obj_t * ps_o, type_e t)
{

	ps_o->size = obj_type_size(t);
	if (ps_o->size < 0 )
	{
		return(OBJ_E_TYPE);
	}

	ps_o->type = t;

	return(OBJ_E_OK);
}


static obj_t * obj_dup_simple(obj_t * ps_o)
{
	obj_t * ps_new; 

	if(obj_type_validity(ps_o, 0))	return(NULL);

	ps_new = malloc(sizeof(obj_t));
	memcpy(ps_new, ps_o, sizeof(obj_t));
	ps_new->data = malloc(ps_o->size);
	memcpy(ps_new->data, ps_o->data, ps_o->size);
	return(ps_new);
}


static obj_t * obj_copy_filter_t(obj_t * ps_o)
{
	dfilter_t * ps_f_d 		= NULL;
	dfilter_t * ps_f_tmp 	= NULL;
	dfilter_t * ps_f_s = (dfilter_t *) ps_o;

	if (!ps_o || obj_type_validity(ps_o, OBJ_TYPE_FILTER)) return(NULL);
	
	while(ps_f_s) 
	{
		ps_f_tmp = dfilter_dup(ps_f_s);
		if (ps_f_d) dfilter_add_to_dfilter(ps_f_d, ps_f_tmp);
		else ps_f_d = ps_f_tmp;
	}

	return((obj_t *)ps_f_d);
}





obj_t * obj_next(obj_t * ps_o)
{
	switch(ps_o->type)
	{
	case OBJ_TYPE_BEGIN:
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
	case OBJ_TYPE_ENTRY:
	case OBJ_TYPE_DIR:
	case OBJ_TYPE_FILTER:
		return( (obj_t *) ((dfilter_t *)ps_o)->next );
		break;
	case OBJ_TYPE_FILE:
	case OBJ_TYPE_QUE:
		return(NULL);
		break;
	case OBJ_TYPE_NODE:
		return((obj_t *) ((node_t *)ps_o)->next );
	case OBJ_TYPE_WATCHER:
	case OBJ_TYPE_DHOLDER:
	case OBJ_TYPE_END:
		return(NULL);
		break;
	}

	return(NULL);
}


int obj_lock(obj_t * ps_o)
{
	switch(ps_o->type)
	{
	case OBJ_TYPE_BEGIN:
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
	case OBJ_TYPE_ENTRY:
		return(0);

	case OBJ_TYPE_DIR:
		return ( pthread_mutex_lock( & ((dir_t * )ps_o)->lock) );
	case OBJ_TYPE_FILTER:
		return(0);
		break;
	case OBJ_TYPE_FILE:
		return(0);
	case OBJ_TYPE_QUE:
		return ( pthread_mutex_lock( & ((que_t * )ps_o)->lock) );
		break;
	case OBJ_TYPE_NODE:
		return(0);
	case OBJ_TYPE_WATCHER:
		return ( pthread_mutex_lock( & ((dwatch_t * )ps_o)->lock ) );
		break;
	case OBJ_TYPE_DHOLDER:
		return(0);
		break;
	case OBJ_TYPE_END:
		return(0);
		break;
	}

	return(-1);
}


int obj_unlock(obj_t * ps_o)
{
	switch(ps_o->type)
	{
	case OBJ_TYPE_BEGIN:
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
	case OBJ_TYPE_ENTRY:
		return(0);

	case OBJ_TYPE_DIR:
		return ( pthread_mutex_unlock( & ((dir_t * )ps_o)->lock) );
	case OBJ_TYPE_FILTER:
		return(0);
		break;
	case OBJ_TYPE_FILE:
		return(0);
	case OBJ_TYPE_QUE:
		return ( pthread_mutex_unlock( & ((que_t * )ps_o)->lock) );
		break;
	case OBJ_TYPE_NODE:
		return(0);
	case OBJ_TYPE_WATCHER:
		return ( pthread_mutex_unlock( & ((dwatch_t * )ps_o)->lock ) );
		break;
	case OBJ_TYPE_DHOLDER:
		return(0);
		break;
	case OBJ_TYPE_END:
		return(0);
		break;
	}

	return(-1);
}


/* It created new que_t and then copy chain of data */
static obj_t * obj_dup_que_t(obj_t * ps_o)
{
	que_t * ps_dst = NULL;
	que_t * ps_src = (que_t *) ps_o;
	node_t * ps_node = NULL;

	if(obj_type_validity(ps_o, OBJ_TYPE_QUE)) return(NULL);

	ps_dst = que_create();
	if(ps_src->amount > 0)
	{
		ps_node = ps_src->list;
		while(ps_node)
		{
			que_add_node(ps_dst, (node_t *) obj_dup( (obj_t *) ps_node));
			ps_node = ps_node->next;
		}
	}
	return((obj_t *)ps_dst);
}



obj_t * obj_dup(obj_t * ps_o)
{
	if (obj_f_array[ps_o->type]) return(obj_f_array[ps_o->type]->obj_dup(ps_o));

	return(NULL);
}

obj_t * obj_dup_orig(obj_t * ps_o)
{
	obj_t * ps_new = NULL;

	switch(ps_o->type)
	{
	case OBJ_TYPE_BEGIN:
		break;
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
		ps_new = obj_copy_filter_t(ps_o);
		break;
	case OBJ_TYPE_FILE:
		ps_new =  obj_dup_que_t(ps_o);
		break;
	case OBJ_TYPE_QUE:
		break;
	case OBJ_TYPE_NODE:
		break;
	case OBJ_TYPE_WATCHER:
		break;
	case OBJ_TYPE_DHOLDER:
		break;
	case OBJ_TYPE_END:
		break;
	}

	return(ps_new);
}



